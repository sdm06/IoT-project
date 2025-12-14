output "iot_hub_hostname" {
  description = "The Hostname to use in your ESP32 MQTT config"
  value       = azurerm_iothub.iot_hub.hostname
}

output "iot_hub_connection_string" {
  description = "Full connection string (useful for device simulator testing)"
  value       = data.azurerm_iothub_shared_access_policy.iothubowner.primary_connection_string
  sensitive   = true
}

output "iot_hub_name" {
  value = azurerm_iothub.iot_hub.name
}
