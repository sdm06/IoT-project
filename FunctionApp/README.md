-----

# Azure Functions IoT Backend (Python)

This repository contains the serverless backend code for an IoT Telemetry system. It uses **Azure Functions (Python V2 Model)** to handle device data ingestion, user authentication, and data retrieval for frontend dashboards.

## ⚡ System Architecture

1.  **Ingestion:** IoT devices send MQTT messages to Azure IoT Hub. This function triggers on those events and saves them to Cosmos DB.
2.  **API:** A REST API provides JWT-based authentication and secure access to the telemetry data.

## 📂 Repository Structure

  * `function_app.py`: Main application logic containing all 3 functions (Login, Ingest, Get).
  * `shell.nix`: NixOS configuration for a reproducible FHS environment.
  * `requirements.txt`: Python dependencies.
  * `host.json`: Global configuration options for the function host.

-----

## 🛠 Setup & Installation

### Option A: Standard Setup (Windows/Mac/Linux)

**Prerequisites:**

  * Python 3.9+
  * [Azure Functions Core Tools](https://learn.microsoft.com/en-us/azure/azure-functions/functions-run-local)
  * Azure CLI

<!-- end list -->

1.  **Create Virtual Environment:**

    ```bash
    python -m venv .venv
    source .venv/bin/activate  # or .venv\Scripts\activate on Windows
    ```

2.  **Install Dependencies:**

    ```bash
    pip install -r requirements.txt
    ```

### Option B: NixOS Setup (Recommended for Nix Users)

This project includes a `shell.nix` file that uses `buildFHSUserEnv`. This creates a File Hierarchy Standard (FHS) bubble, which is required to run `azure-functions-core-tools` and `azurite` binaries on NixOS.

1.  **Enter the Shell:**

    ```bash
    nix-shell
    ```

    *This will download `python3`, `azure-functions-core-tools`, `azurite`, and required system libraries (openssl, zlib, etc.) and drop you into a bash shell where these tools work natively.*

2.  **Install Python Libs (inside the shell):**

    ```bash
    # You are now inside the FHS bubble
    python -m venv .venv
    source .venv/bin/activate
    pip install -r requirements.txt
    ```

-----

## ⚙️ Configuration

To run locally, you must create a `local.settings.json` file in the root directory. This file is git-ignored to protect secrets.

**`local.settings.json`**

```json
{
  "IsEncrypted": false,
  "Values": {
    "AzureWebJobsStorage": "UseDevelopmentStorage=true",
    "FUNCTIONS_WORKER_RUNTIME": "python",
    "IOT_HUB_CONNECTION_STRING": "<YOUR_IOT_HUB_EVENT_ENDPOINT_STRING>",
    "COSMOS_DB_CONNECTION_STRING": "<YOUR_COSMOS_DB_CONNECTION_STRING>",
    "SECRET_KEY": "super_secret_key"
  }
}
```

> **Note:** For `IOT_HUB_CONNECTION_STRING`, ensure you use the **Event Hub compatible endpoint** (built-in endpoint) of your IoT Hub, not the device connection string.

-----

## 🚀 Running the App

Once configured, start the local function host:

```bash
func start
```

## 📡 API Endpoints

### 1\. Login

Generates a JWT token for access to protected endpoints.

  * **Route:** `POST /api/login`
  * **Body:**
    ```json
    { "username": "admin", "password": "admin" }
    ```
  * **Response:** `{ "token": "eyJ0eX...", "message": "Login successful" }`

### 2\. IoT Ingestion (Trigger)

  * **Type:** Event Hub Trigger (Automatic)
  * **Description:** Automatically triggers when your ESP32 device sends data to IoT Hub. It enriches the data (adds ID, ISO timestamp) and saves it to the `IoTData` database in Cosmos DB.

### 3\. Get Telemetry

Retrieves the last 10 telemetry records. Requires Authentication.

  * **Route:** `GET /api/get_telemetry`
  * **Headers:** `Authorization: Bearer <YOUR_TOKEN>`
  * **Response:**
    ```json
    [
      {
        "id": "abc-123",
        "timestamp": "2023-10-27T10:00:00Z",
        "deviceId": "ESP32_01",
        "ssid": "HomeWiFi",
        "rssi": -45,
        "uptime_seconds": 120,
        ...
      }
    ]
    ```

-----

## 🚢 Deployment to Azure

You can deploy this project directly using the Azure CLI or VS Code.

```bash
# Log in to Azure
az login

# Deploy to your Function App (replace with your app name)
func azure functionapp publish <YOUR_FUNCTION_APP_NAME>
```

*Ensure you have also updated the **App Settings** (Environment Variables) in the Azure Portal to match your `local.settings.json` values.*

-----

