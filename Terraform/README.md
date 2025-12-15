-----

# Azure IoT Infrastructure for ESP32

This Terraform project provisions a complete serverless backend on Azure designed to support IoT devices (specifically ESP32). It sets up the ingestion, storage, and compute layers required to handle telemetry data.

## 🏗 Architecture Overview

This configuration deploys the following resources:

  * **Azure IoT Hub (S1):** The entry point for bi-directional communication with your ESP32 devices using MQTT.
  * **Azure Cosmos DB (Serverless):** A NoSQL database (SQL API) to store device telemetry (e.g., temperature, humidity). Configured with Serverless capacity mode for cost efficiency during development.
  * **Azure Function App (Python/Linux):** A serverless compute resource to process incoming data or expose APIs. It is pre-configured with environment variables to connect to both the IoT Hub and Cosmos DB.
  * **Resource Group:** A container for all related resources.

## 🚀 Prerequisites

  * [Terraform CLI](https://developer.hashicorp.com/terraform/downloads) (v1.0+)
  * [Azure CLI](https://docs.microsoft.com/en-us/cli/azure/install-azure-cli) installed and authenticated (`az login`)
  * An active Azure Subscription

## 🛠 Project Structure

  * **`azurerm_iothub`**: Configured with the `S1` SKU. It uses a `data` source to securely retrieve the shared access policy for connection strings.
  * **`azurerm_cosmosdb_account`**: Sets up a "GlobalDocumentDB" with the `EnableServerless` capability to minimize costs.
  * **`azurerm_linux_function_app`**: Deploys a Python 3.9 function app on a Consumption plan (Y1). App settings are automatically populated with connection strings.

## ⚙️ Configuration & Usage

### 1\. Initialize

Initialize the directory to download the `azurerm` and `random` providers.

```bash
terraform init
```

### 2\. Configure Variables

Create a `terraform.tfvars` file to set your subscription ID and customize the deployment.

**`terraform.tfvars` example:**

```hcl
subscription_id = "YOUR_SUBSCRIPTION_GUID"
location        = "francecentral"
project_prefix  = "esp32-weather"
environment     = "dev"
```

### 3\. Deploy

Review the plan and apply the configuration.

```bash
terraform plan
terraform apply
```

## 🔌 Outputs & Connection Info

After a successful deployment, Terraform will output the critical information you need to configure your ESP32 firmware:

| Output Name | Description |
| :--- | :--- |
| **`iot_hub_hostname`** | The endpoint URL for your devices (e.g., `iot-esp32-hub-xyz.azure-devices.net`). **Put this in your ESP32 MQTT Code.** |
| **`iot_hub_connection_string`** | The full connection string with `iothubowner` permissions. Useful for backend testing or the Azure IoT Explorer tool. |
| **`iot_hub_name`** | The generated name of the IoT Hub resource. |

*Note: The connection string is marked as sensitive. To view it after deployment, run:*

```bash
terraform output iot_hub_connection_string
```

## 📝 Key Design Decisions

1.  **Cosmos DB Partitioning:** The container is created with a partition key of `/deviceId`. This optimizes queries that filter by a specific device, which is the most common access pattern for IoT dashboards.
2.  **Security:** The Function App connects to other services via App Settings (`COSMOS_DB_CONNECTION_STRING`, `IOT_HUB_CONNECTION_STRING`), keeping credentials out of the application code.
3.  **Random Suffix:** A `random_id` resource is used to generate unique names for globally unique Azure resources (like Storage Accounts and IoT Hub DNS), preventing naming conflicts.

## 🧹 Cleanup

To remove all resources and avoid costs:

```bash
terraform destroy
```

-----

### Would you like me to generate the Python code for the Azure Function to actually move the data from IoT Hub into that Cosmos DB container?
