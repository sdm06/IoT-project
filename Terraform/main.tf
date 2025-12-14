terraform {
  required_providers {
    azurerm = {
      source  = "hashicorp/azurerm"
      version = "~> 3.0"
    }
    random = {
      source  = "hashicorp/random"
      version = "~> 3.0"
    }
  }
}

provider "azurerm" {
  features {}
  subscription_id = var.subscription_id
}

resource "random_id" "suffix" {
  byte_length = 4
}

resource "azurerm_resource_group" "rg" {
  name     = "${var.project_prefix}-rg"
  location = var.location
  tags = {
    env = var.environment
  }
}

# --- IoT Hub ---
resource "azurerm_iothub" "iot_hub" {
  name                = "${var.project_prefix}-hub-${random_id.suffix.hex}"
  resource_group_name = azurerm_resource_group.rg.name
  location            = azurerm_resource_group.rg.location

  sku {
    name     = "S1"
    capacity = 1
  }
}

# [FIX] Data Source to retrieve Keys securely
# This reads the "iothubowner" policy from the hub we just created
data "azurerm_iothub_shared_access_policy" "iothubowner" {
  name                = "iothubowner"
  resource_group_name = azurerm_resource_group.rg.name
  iothub_name         = azurerm_iothub.iot_hub.name
  depends_on          = [azurerm_iothub.iot_hub]
}

# --- Cosmos DB ---
resource "azurerm_cosmosdb_account" "db_acc" {
  name                = "${var.project_prefix}-cosmos-${random_id.suffix.hex}"
  location            = azurerm_resource_group.rg.location
  resource_group_name = azurerm_resource_group.rg.name
  offer_type          = "Standard"
  kind                = "GlobalDocumentDB"

  capabilities {
    name = "EnableServerless"
  }

  consistency_policy {
    consistency_level = "Session"
  }

  geo_location {
    location          = azurerm_resource_group.rg.location
    failover_priority = 0
  }
}

resource "azurerm_cosmosdb_sql_database" "db" {
  name                = "IoTData"
  resource_group_name = azurerm_resource_group.rg.name
  account_name        = azurerm_cosmosdb_account.db_acc.name
}

resource "azurerm_cosmosdb_sql_container" "container" {
  name                = "Telemetry"
  resource_group_name = azurerm_resource_group.rg.name
  account_name        = azurerm_cosmosdb_account.db_acc.name
  database_name       = azurerm_cosmosdb_sql_database.db.name
  
  # [FIX] Changed from 'partition_key_path' to 'partition_key_paths' (plural list)
  partition_key_paths = ["/deviceId"]
}

# --- Azure Functions ---
resource "azurerm_storage_account" "func_storage" {
  name                     = "func${random_id.suffix.hex}"
  resource_group_name      = azurerm_resource_group.rg.name
  location                 = azurerm_resource_group.rg.location
  account_tier             = "Standard"
  account_replication_type = "LRS"
}

resource "azurerm_service_plan" "app_plan" {
  name                = "${var.project_prefix}-plan"
  resource_group_name = azurerm_resource_group.rg.name
  location            = azurerm_resource_group.rg.location
  os_type             = "Linux"
  sku_name            = "Y1"
}

resource "azurerm_linux_function_app" "function_app" {
  name                = "${var.project_prefix}-api-${random_id.suffix.hex}"
  resource_group_name = azurerm_resource_group.rg.name
  location            = azurerm_resource_group.rg.location

  storage_account_name       = azurerm_storage_account.func_storage.name
  storage_account_access_key = azurerm_storage_account.func_storage.primary_access_key
  service_plan_id            = azurerm_service_plan.app_plan.id

  site_config {
    application_stack {
      python_version = "3.9"
    }
    cors {
      allowed_origins = ["*"]
    }
  }

  app_settings = {
    "COSMOS_DB_CONNECTION_STRING" = azurerm_cosmosdb_account.db_acc.primary_sql_connection_string
    # [FIX] Use the data source to get the connection string
    "IOT_HUB_CONNECTION_STRING"   = data.azurerm_iothub_shared_access_policy.iothubowner.primary_connection_string
  }
}
