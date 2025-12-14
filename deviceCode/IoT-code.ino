#include <dummy.h>

/*
 * ESP32 WiFi & Azure IoT Hub Monitor
 *
 * 1. Connects to WiFi & Syncs Time (Required for Azure).
 * 2. Generates a SAS Token dynamically for authentication.
 * 3. Connects to Azure IoT Hub MQTT (Port 8883).
 * 4. Scans WiFi environment.
 * 5. Publishes JSON data to Azure.
 */

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

// --- Screen Configuration ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define I2C_ADDRESS 0x3C
#define I2C_SDA 21
#define I2C_SCL 22

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- WiFi Configuration ---
const char* ssid = "iPhone 11";
const char* password = "12345678";

// --- Azure IoT Hub Configuration ---
const char* mqtt_broker   = "iot-esp32-hub-23ffc2af.azure-devices.net";
const char* device_id     = "esp32-01";
const char* device_key    = "S6bkD4tN3J6E+zV9SR5swLIct04XGfaOw4qoOkgujUE="; // Your SharedAccessKey

// Azure specific topic
const char* publish_topic = "devices/esp32-01/messages/events/";

// --- MQTT & WiFi Client Objects ---
WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

// --- Helper to UrlEncode ---
String urlEncode(const char* msg) {
  const char *hex = "0123456789abcdef";
  String encodedMsg = "";
  while (*msg != '\0') {
    if (('a' <= *msg && *msg <= 'z') ||
        ('A' <= *msg && *msg <= 'Z') ||
        ('0' <= *msg && *msg <= '9')) {
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

// --- Helper to Generate Azure SAS Token ---
String generateSasToken(char* host, char* deviceId, char* deviceKey) {
  // Set expiry to 1 hour from now
  time_t now;
  time(&now);
  size_t expiry = now + 3600; 

  String uri = urlEncode(host) + "%2Fdevices%2F" + urlEncode(deviceId);
  String stringToSign = uri + "\n" + String(expiry);

  // Compute HMAC-SHA256
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

  // Base64 encode the signature
  char encodedSign[64];
  size_t encodedLen;
  mbedtls_base64_encode((unsigned char*)encodedSign, 64, &encodedLen, signature, 32);
  encodedSign[encodedLen] = '\0';

  return "SharedAccessSignature sr=" + uri + "&sig=" + urlEncode(encodedSign) + "&se=" + String(expiry);
}

void setup() {
  Serial.begin(115200);

  // --- Initialize Display ---
  Wire.begin(I2C_SDA, I2C_SCL); 
  if(!display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("WiFi..."));
  display.display(); 

  // --- Connect to WiFi ---
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected");

  // --- Sync Time (REQUIRED for Azure SAS Tokens) ---
  display.println(F("Syncing Time..."));
  display.display();
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  time_t now = time(nullptr);
  while (now < 100000) { // Wait for time to sync
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("\nTime synced");

  // --- Setup MQTT Client ---
  espClient.setInsecure(); // Bypass CA cert for simplicity
  mqttClient.setServer(mqtt_broker, 8883);
  mqttClient.setBufferSize(512); // Increase buffer for long SAS tokens
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting Azure MQTT connection...");
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(F("Azure Connecting..."));
    display.display();

    // --- Generate Username & Password for Azure ---
    String username = String(mqtt_broker) + "/" + String(device_id) + "/?api-version=2021-04-12";
    String sasToken = generateSasToken((char*)mqtt_broker, (char*)device_id, (char*)device_key);

    // Connect using generated credentials
    if (mqttClient.connect(device_id, username.c_str(), sasToken.c_str())) {
      Serial.println("connected");
      display.println(F("Connected!"));
      display.display();
      delay(1000);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();

  if (WiFi.status() == WL_CONNECTED) {
    long rssi = WiFi.RSSI();
    long channel = WiFi.channel();
    long freq_mhz = 2407 + (channel * 5);

    // --- Display ---
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(F("--- Azure IoT ---"));
    display.print(F("Sig: ")); display.println(rssi);
    display.print(F("Ch:  ")); display.println(channel);
    display.println(F("Publishing..."));
    display.display();
    
    // --- JSON Payload ---
    StaticJsonDocument<256> doc; 
    doc["deviceId"] = device_id;
    doc["rssi"] = rssi;
    doc["channel"] = channel;
    doc["frequency"] = freq_mhz;
    doc["ip"] = WiFi.localIP().toString(); 

    char jsonBuffer[256];
    serializeJson(doc, jsonBuffer);

    // --- Publish ---
    if (mqttClient.publish(publish_topic, jsonBuffer)) {
      Serial.println("Sent to Azure!");
    } else {
      Serial.println("Publish FAILED");
    }

  } else {
    WiFi.begin(ssid, password);
  }
  delay(5000); 
}
