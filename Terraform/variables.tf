variable "subscription_id" {
  description = "The Azure Subscription ID where resources will be deployed."
  type        = string
}

variable "location" {
  description = "The Azure Region to deploy resources."
  type        = string
  default     = "francecentral"
}

variable "project_prefix" {
  description = "A prefix used for all resources to ensure unique names (e.g., 'iot-demo')."
  type        = string
  default     = "iot-esp32"
}

variable "environment" {
  description = "Environment tag (e.g., dev, test, prod)."
  type        = string
  default     = "dev"
}
