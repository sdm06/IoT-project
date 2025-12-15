-----

# 📡 Low-Cost RF & Wi-Fi Environment Monitoring System

## 📖 Project Overview

This project implements a scalable, low-cost network of distributed sensors using ESP32 microcontrollers to actively monitor the Radio Frequency (RF) environment.

Reliable Wi-Fi is a critical utility for modern business productivity, yet traditional troubleshooting is often reactive and expensive. This solution replaces costly spectrum analyzers with affordable IoT hardware to provide continuous, real-time data on **channel congestion, signal strength (RSSI), and interference**.

The system empowers IT and NetOps teams to move from reactive troubleshooting to proactive optimization, detecting "dead zones" and unauthorized "rogue" access points before they impact users.

**Note:** This specific implementation is configured to monitor the **2.4GHz Wi-Fi band** only, optimizing for the specific hardware capabilities of the deployed ESP32 sensors.

-----

## 📂 Repository Navigation

Access the specific documentation and code for each module below:

| Folder | Module | Description |
| :--- | :--- | :--- |
| **[📂 Terraform](https://github.com/sdm06/IoT-project/tree/master/Terraform)** | **Infrastructure** | IaC scripts to provision the Azure Cloud backend (IoT Hub, Cosmos DB, Functions). |
| **[📂 deviceCode](https://github.com/sdm06/IoT-project/tree/master/deviceCode)** | **Firmware** | C++ code for ESP32 sensors tuned specifically for **2.4GHz** band scanning. |
| **[📂 FunctionApp](https://github.com/sdm06/IoT-project/tree/master/FunctionApp)** | **Backend** | Python Azure Functions for data ingestion, processing, and API access. |
| **[📂 Client](https://github.com/sdm06/IoT-project/tree/master/Client)** | **Frontend** | Dashboard application to visualize signal health maps and alerts. |
| **[📂 Docs](https://github.com/sdm06/IoT-project/tree/master/Docs)** | **Documentation** | Contains the **Business Context PDF**, **C4 Architecture Diagram**, and **Cost Calculations**. |


-----

## 🎯 Key Business Objectives

  * **Reduce Operational Costs:** Replace multi-thousand-dollar analysis tools with low-cost ESP32 devices and reduce manual troubleshooting time.
  * **Improve Productivity:** Proactively identify high congestion and signal degradation to prevent downtime.
  * **Enhance Security:** Instantly flag unauthorized "Rogue Access Points" that bypass corporate firewalls.
  * **Optimize Infrastructure:** Provide data-driven justification for network upgrades and Access Point placement.

-----
