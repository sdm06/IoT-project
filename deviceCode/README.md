-----

# ESP32 Network Monitor (Azure IoT Hub)

This project turns an **Ideaspark ESP32 with built-in OLED** into a network diagnostic tool. It continuously monitors network performance metrics (Ping, Jitter, Packet Loss, and Download Speed) and transmits the telemetry to **Azure IoT Hub** via MQTT. Real-time statistics are also displayed on the device's OLED screen.

## 📋 Features

  * **Network Diagnostics:** Calculates Average Ping, Jitter, Packet Loss, and Download Speed (Mbps).
  * **Cloud Connectivity:** Securely connects to Azure IoT Hub using SAS Token authentication (generated on-device).
  * **Local Display:** Shows real-time status on the integrated SSD1306 OLED screen.
  * **JSON Telemetry:** Formats data into standard JSON for easy cloud processing.

## 🛠 Hardware Requirements

  * **Ideaspark ESP32 Board** (with integrated 0.96" OLED Display).
  * Micro-USB cable for programming.

## ⚙️ Software Prerequisites

### 1\. Install ESP32 Board Manager

To compile code for the ESP32, you must add the board definitions to the Arduino IDE.

1.  Open Arduino IDE.
2.  Go to **File** -\> **Preferences**.
3.  In the "Additional Boards Manager URLs" field, paste the following link:
    ```text
    https://dl.espressif.com/dl/package_esp32_index.json
    ```
4.  Click **OK**.
5.  Go to **Tools** -\> **Board** -\> **Boards Manager**.
6.  Search for **esp32** (by Espressif Systems) and click **Install**.

### 2\. Install Required Libraries

This project relies on several external libraries. Install them via **Tools** -\> **Manage Libraries** (Ctrl+Shift+I):

| Library Name | Search Term | Description |
| :--- | :--- | :--- |
| **Adafruit GFX Library** | `Adafruit GFX` | Core graphics library for displays. |
| **Adafruit SSD1306** | `Adafruit SSD1306` | Driver for the OLED display. |
| **PubSubClient** | `PubSubClient` | MQTT client for connecting to Azure. |
| **ArduinoJson** | `ArduinoJson` | Used to create the JSON telemetry payload. |
| **ESPping** | `ESPping` | Used for ICMP ping and jitter calculations. |

*(Note: `WiFi`, `HTTPClient`, `WiFiClientSecure`, and `Wire` are built-in libraries and do not need installation.)*

## 🚀 Setup & Configuration

### 1\. Board Selection

For the Ideaspark board, these settings usually work best:

  * **Board:** `ESP32 Dev Module`
  * **Upload Speed:** `921600`
  * **Port:** Select the COM port your device is connected to.

### 2\. Configure Credentials

Open the `.ino` file and update the `CONFIGURATION` section at the top:

```cpp
// Wi-Fi Credentials
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// Azure IoT Hub Settings
const char* mqtt_broker = "YOUR_IOTHUB_NAME.azure-devices.net";
const char* device_id = "YOUR_DEVICE_ID";
const char* device_key = "YOUR_PRIMARY_KEY"; // From Azure Portal -> Device -> Shared Access Keys
```

> **⚠️ SECURITY WARNING:** Never commit your `device_key` to public repositories (like GitHub). If you do, regenerate the key in the Azure Portal immediately.

## 🖥 usage

1.  Connect the ESP32 to your computer.
2.  Press the **Upload** button (Right Arrow icon) in Arduino IDE.
3.  Once uploaded, open the **Serial Monitor** (set baud rate to **115200**).
4.  The device will:
      * Connect to Wi-Fi.
      * Sync time (NTP) for secure Azure authentication.
      * Connect to Azure IoT Hub.
      * Run network tests and display results on the OLED.

## 🔍 Troubleshooting

  * **OLED not working?**
    The Ideaspark board usually uses the default I2C pins, but if the screen remains black, check if your specific board revision requires re-mapping pins in the `Wire.begin()` line (e.g., `Wire.begin(5, 4)` for SDA/SCL).
  * **Azure Connection Fails (RC=-2 or -4)?**
      * Check your `device_key`.
      * Ensure the device name in the code matches exactly with the Device ID in Azure.
      * Ensure the device has internet access.
  * **"Stray" characters error?**
    Ensure you are using the "Cleaned" version of the code provided in the conversation history, as copy-pasting from some web sources can introduce invisible formatting characters.
