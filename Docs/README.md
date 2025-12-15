# 📘 Project Documentation & Architecture

Welcome to the central documentation hub for the **Low-Cost RF & Wi-Fi Environment Monitoring System**. This directory contains the architectural blueprints, business requirements, and financial estimates required to understand the scope and technical design of the system.

## 📂 Directory Contents

This folder is organized into three core pillars:

1.  **Business Context:** The problem space, user stories, and requirements.
2.  **Architecture (C4 Model):** Technical design diagrams (Context & Containers).
3.  **Cost Analysis:** Azure infrastructure estimations and hardware savings.

---

## 1. 📢 Business Context
**File:** `BusinessContext.pdf`

This document outlines the "Why" and "What" of the project. It serves as the primary reference for non-technical stakeholders and developers seeking domain knowledge.

**Executive Summary:**
[cite_start]The project proposes a scalable network of **ESP32 microcontrollers** to actively monitor the Radio Frequency (RF) environment[cite: 1, 4]. [cite_start]This replaces expensive, reactive troubleshooting with a proactive system that identifies "dead zones," channel congestion, and security threats in real-time[cite: 7, 11].

**Key Objectives:**
* [cite_start]**Reduce OpEx & CapEx:** Replace professional-grade spectrum analyzers (costing thousands) with sub-$20 ESP32 units[cite: 32].
* [cite_start]**Enhance Security:** Detect unauthorized "Rogue Access Points" that bypass corporate firewalls[cite: 16].
* [cite_start]**Proactive Optimization:** Enable IT teams to fix congestion issues before users file complaints[cite: 35].

**Target Stakeholders:**
* [cite_start]**IT Administrators:** For diagnosing specific user complaints[cite: 43].
* [cite_start]**NetOps:** For capacity planning and identifying dead zones[cite: 46].
* [cite_start]**Cybersecurity:** For receiving alerts on unauthorized SSIDs[cite: 50].

---

## 2. 🏛️ Architecture: C4 Model
**Location:** `SystemContext.drawio`

We utilize the **C4 Model** to visualize the software architecture. The system is designed as a cloud-native IoT solution using Azure Serverless technologies.

### 🟢 Level 1: System Context
**Purpose:** Shows how the monitoring system interacts with the Enterprise environment.
* **Users:** IT Admin, NetOps, Facility Managers.
* **System:** The RF Monitoring Platform.
* **External Systems:** The Corporate Wi-Fi Network (target of monitoring).

### 🔵 Level 2: Container Diagram
**Purpose:** illustrates the high-level technical building blocks and their responsibilities.

| Container | Technology | Responsibility |
| :--- | :--- | :--- |
| **IoT Device** | **ESP32 (C++)** | [cite_start]Scans 2.4GHz Wi-Fi channels for RSSI and SSID data[cite: 23]. Publishes telemetry via **MQTTS**. |
| **Cloud Gateway** | **Azure IoT Hub** | Acts as the secure MQTT message broker, handling device provisioning and bi-directional communication. |
| **Backend API** | **Azure Functions (Python)** | Serverless compute that processes telemetry and exposes a **REST API** secured via **JWT** for the frontend. |
| **Database** | **Cosmos DB** | Stores historical signal data, user profiles, and alert logs. |
| **Client App** | **React** | A local web dashboard for visualizing heatmaps, signal graphs. |
| **Infrastructure** | **Terraform** | IaC scripts ensuring reproducible deployments of the Azure resources. |

---

## 3. 💰 Cost Analysis
**File:** `CostCalculator.pdf` and `CostCalculatorImage.jpg`

This section details the estimated Operational Expenditure (OpEx) for hosting the backend on Microsoft Azure, alongside the hardware savings.

**Financial Highlights:**
* [cite_start]**Hardware Savings:** Transitioning from dedicated spectrum analyzers to ESP32s significantly lowers the barrier to entry for building-wide monitoring[cite: 32].
* **Cloud Strategy:** Usage of **Azure Functions (Consumption Plan)** and **Cosmos DB (Serverless)** ensures costs scale linearly with the number of active sensors, keeping idle costs near zero.

> **Note:** Costs in the spreadsheet are estimates based on the Azure Pricing Calculator. Actual costs may vary based on data egress (telemetry frequency) and active storage requirements.

---