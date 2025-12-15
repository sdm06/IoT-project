#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include "time.h"
#include "mbedtls/base64.h"
#include "mbedtls/md.h"
#include <ESPping.h>
#include <HTTPClient.h>

// ==========================================
// 1. CONFIGURATION
// ==========================================
const char* ssid = "KOMPTELNET_8BDA";
const char* password = "26641799";

// Azure IoT Hub Settings
const char* mqtt_broker = "iot-esp32-hub-23ffc2af.azure-devices.net";
const char* device_id = "esp32-01";
const char* device_key = "S6bkD4tN3J6E+zV9SR5swLIct04XGfaOw4qoOkgujUE="; 

const char* publish_topic = "devices/esp32-01/messages/events/";

// ==========================================
// 2. HARDWARE SETUP
// ==========================================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define I2C_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

// Global Stats
int currentPingAvg = 0;
int currentJitter = 0;
int currentPacketLoss = 0;
float currentSpeed = 0.0;

// ==========================================
// 3. SECURITY & NETWORK HELPERS
// ==========================================
String urlEncode(const char* msg) {
  const char *hex = "0123456789abcdef";
  String encodedMsg = "";
  while (*msg != '\0') {
    if (('a' <= *msg && *msg <= 'z') || ('A' <= *msg && *msg <= 'Z') || ('0' <= *msg && *msg <= '9')) {
      encodedMsg += *msg;
    } else {
      encodedMsg += '%';
      encodedMsg += hex[*msg >> 4];
      encodedMsg += hex[*msg & 15];
    }
    msg++;
  }
  return encodedMsg;
}

String generateSasToken(char* host, char* deviceId, char* deviceKey) {
  time_t now;
  time(&now);
  size_t expiry = now + 3600; 
  String uri = urlEncode(host) + "%2Fdevices%2F" + urlEncode(deviceId);
  String stringToSign = uri + "\n" + String(expiry);

  char key[64];
  size_t keyLen = 0;
  mbedtls_base64_decode((unsigned char*)key, 64, &keyLen, (unsigned char*)deviceKey, strlen(deviceKey));

  byte signature[32];
  mbedtls_md_context_t ctx;
  mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;
  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 1);
  mbedtls_md_hmac_starts(&ctx, (const unsigned char*)key, keyLen);
  mbedtls_md_hmac_update(&ctx, (const unsigned char*)stringToSign.c_str(), stringToSign.length());
  mbedtls_md_hmac_finish(&ctx, signature);
  mbedtls_md_free(&ctx);

  char encodedSign[64];
  size_t encodedLen;
  mbedtls_base64_encode((unsigned char*)encodedSign, 64, &encodedLen, signature, 32);
  encodedSign[encodedLen] = '\0';

  return "SharedAccessSignature sr=" + uri + "&sig=" + urlEncode(encodedSign) + "&se=" + String(expiry);
}

// --- ADVANCED PING (JITTER/LOSS) ---
int getAdvancedPing() {
  IPAddress ip(8, 8, 8, 8); // Google DNS
  int successful = 0;
  int total_time = 0;
  int min_time = 10000;
  int max_time = 0;
  int attempts = 5; 

  for (int i = 0; i < attempts; i++) {
    if (Ping.ping(ip, 1)) {
      int t = Ping.averageTime();
      total_time += t;
      successful++;
      if (t < min_time) min_time = t;
      if (t > max_time) max_time = t;
    }
    delay(50);
  }

  // Calc Loss
  if (attempts > 0) {
    currentPacketLoss = ((attempts - successful) * 100) / attempts;
  } else {
    currentPacketLoss = 100;
  }

  // Calc Avg & Jitter
  int avg = 0;
  if (successful > 0) {
    avg = total_time / successful;
    currentJitter = max_time - min_time;
  } else {
    currentJitter = 0;
    return -1;
  }
  return avg;
}

// --- SPEED TEST (10KB File) ---
float getDownloadSpeed() {
  HTTPClient http;
  http.begin("http://www.google.com/robots.txt"); 
  unsigned long start = millis();
  int httpCode = http.GET();
  if (httpCode > 0) {
    int len = http.getSize();
    WiFiClient * stream = http.getStreamPtr();
    uint8_t buff[128]; 
    int totalBytes = 0;
    while(http.connected() && (len > 0 || len == -1)) {
      size_t size = stream->available();
      if(size) {
        int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
        totalBytes += c;
        if(len > 0) len -= c;
      }
      if(millis() - start > 5000) break; 
    }
    http.end();
    unsigned long duration = millis() - start;
    if (duration == 0) duration = 1;
    return ((totalBytes * 8.0) / (duration / 1000.0)) / 1000000.0;
  }
  http.end();
  return 0.0;
}

// ==========================================
// 4. SETUP
// ==========================================
void setup() {
  Serial.begin(115200);
  Wire.begin();
  if(!display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDRESS)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); 
  }
  
  display.clearDisplay(); 
  display.setTextColor(WHITE); 
  display.setCursor(0,0);
  display.println("Connecting..."); 
  display.display();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
    Serial.print("."); 
  }
  Serial.println("\nConnected!");

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  time_t now = time(nullptr);
  while (now < 100000) { 
    delay(500); 
    now = time(nullptr); 
  }

  espClient.setInsecure();
  mqttClient.setServer(mqtt_broker, 8883);
  mqttClient.setBufferSize(2048);
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting Azure...");
    String username = String(mqtt_broker) + "/" + String(device_id) + "/?api-version=2021-04-12";
    String sasToken = generateSasToken((char*)mqtt_broker, (char*)device_id, (char*)device_key);
    if (mqttClient.connect(device_id, username.c_str(), sasToken.c_str())) {
      Serial.println("Connected!");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// ==========================================
// 5. MAIN LOOP
// ==========================================
void loop() {
  if (!mqttClient.connected()) reconnect();
  mqttClient.loop();

  Serial.println("Running Network Diagnostics...");
  display.clearDisplay(); 
  display.setCursor(0,0);
  display.println("Testing Link..."); 
  display.display();

  // 1. Get Performance Metrics
  currentPingAvg = getAdvancedPing();
  currentSpeed = getDownloadSpeed();

  // 2. Prepare JSON Payload
  StaticJsonDocument<512> doc;

  doc["deviceId"] = device_id;
  doc["type"] = "TELEMETRY"; 

  // A. IDENTITY
  doc["ssid"] = WiFi.SSID();       
  doc["bssid"] = WiFi.BSSIDstr();   
  doc["channel"] = WiFi.channel();
  doc["rssi"] = WiFi.RSSI();       
  doc["local_ip"] = WiFi.localIP().toString();

  // B. PERFORMANCE
  doc["ping_avg"] = currentPingAvg;
  doc["ping_jitter"] = currentJitter;
  doc["packet_loss"] = currentPacketLoss;
  doc["speed_mbps"] = currentSpeed;

  // C. DEVICE HEALTH
  doc["uptime_s"] = millis() / 1000;
  doc["ram_free"] = ESP.getFreeHeap();

  // 3. Send to Azure
  char buffer[512];
  serializeJson(doc, buffer);
  mqttClient.publish(publish_topic, buffer);
  Serial.println("Telemetry Sent:");
  Serial.println(buffer);

  // 4. Update OLED
  display.clearDisplay(); 
  display.setCursor(0,0);
  display.print("SSID: "); display.println(WiFi.SSID());
  display.print("Ping: "); display.print(currentPingAvg); display.println(" ms");
  display.print("Speed: "); display.print(currentSpeed); display.println(" Mbps");
  display.print("Signal: "); display.print(WiFi.RSSI()); display.println(" dBm");
  display.display();

  // 5. Wait 30 seconds before next check
  delay(60000); 
}