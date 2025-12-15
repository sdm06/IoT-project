import azure.functions as func
import logging
import json
import time
import uuid
import jwt
import datetime

app = func.FunctionApp()

# --- CONFIGURATION ---
SECRET_KEY = "super_secret_key"

# =================================================================================
# 1. LOGIN (Get Token)
# =================================================================================
@app.route(route="login", auth_level=func.AuthLevel.ANONYMOUS, methods=["POST"])
def login(req: func.HttpRequest) -> func.HttpResponse:
    try:
        body = req.get_json()
        username = body.get('username')
        password = body.get('password')

        if username == "admin" and password == "admin":
            payload = {
                "user": "admin",
                "exp": datetime.datetime.utcnow() + datetime.timedelta(hours=1)
            }
            token = jwt.encode(payload, SECRET_KEY, algorithm="HS256")
            
            return func.HttpResponse(
                json.dumps({"token": token, "message": "Login successful"}),
                mimetype="application/json",
                status_code=200
            )
        else:
            return func.HttpResponse(
                json.dumps({"error": "Invalid credentials"}),
                status_code=401,
                mimetype="application/json"
            )

    except Exception as e:
        return func.HttpResponse(f"Error: {str(e)}", status_code=500)

# =================================================================================
# 2. IOT INGESTION (Azure Event Hub -> Cosmos DB)
# =================================================================================
@app.event_hub_message_trigger(arg_name="event", 
                               event_hub_name="messages/events",
                               connection="IOT_HUB_CONNECTION_STRING",
                               consumer_group="%IOT_HUB_CONSUMER_GROUP%")
@app.cosmos_db_output(arg_name="outputDocument", 
                      database_name="IoTData", 
                      container_name="Telemetry", 
                      connection="COSMOS_DB_CONNECTION_STRING")
def iot_ingest(event: func.EventHubEvent, outputDocument: func.Out[func.Document]):   
    try:
        body = event.get_body().decode('utf-8')
        logging.info(f"[Device] Data Received: {body}")

        data = json.loads(body)
        
        # Ensure ID exists
        if "id" not in data: data["id"] = str(uuid.uuid4())
        
        # Ensure Timestamp exists
        if event.enqueued_time:
            data["timestamp"] = event.enqueued_time.isoformat()
        else:
            data["timestamp"] = time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())
        
        outputDocument.set(func.Document.from_json(json.dumps(data)))
        logging.info("Saved to Cosmos DB.")

    except Exception as e:
        logging.error(f"Error: {e}")

# =================================================================================
# 3. GET TELEMETRY (API -> Frontend)
# =================================================================================
@app.route(route="get_telemetry", auth_level=func.AuthLevel.ANONYMOUS, methods=["GET"])
@app.cosmos_db_input(arg_name="documents", 
                     database_name="IoTData", 
                     container_name="Telemetry",
                     sql_query="SELECT TOP 10 * FROM c WHERE c.type = 'TELEMETRY' ORDER BY c._ts DESC",
                     connection="COSMOS_DB_CONNECTION_STRING")
def get_telemetry(req: func.HttpRequest, documents: func.DocumentList) -> func.HttpResponse:
    
    # --- AUTHENTICATION ---
    auth_header = req.headers.get('Authorization')
    if not auth_header or not auth_header.startswith("Bearer "):
        return func.HttpResponse(json.dumps({"error": "Unauthorized"}), status_code=401)
    try:
        token = auth_header.split(" ")[1]
        jwt.decode(token, SECRET_KEY, algorithms=["HS256"])
    except:
        return func.HttpResponse(json.dumps({"error": "Invalid Token"}), status_code=401)

    results = []
    if documents:
        for doc in documents:
            # Extra Safety: Skip if for some reason SSID is missing
            if not doc.get('ssid') and not doc.get('SSID'):
                continue

            # --- RICH DATA MAPPING ---
            data_point = {
                "id": doc.get('id'),
                "timestamp": doc.get('timestamp'),
                "deviceId": doc.get('deviceId', 'Unknown'),
                "type": "TELEMETRY",
                
                # Network Identity
                "ssid": doc.get('ssid') or doc.get('SSID'),
                "bssid": doc.get('bssid') or doc.get('BSSID'),
                "channel": doc.get('channel') or doc.get('Channel'),
                "local_ip": doc.get('local_ip'),
                "rssi": doc.get('rssi') or doc.get('RSSI'),

                # Network Performance (Default to 0 instead of null if missing)
                "ping_avg": doc.get('ping_avg', 0),
                "ping_jitter": doc.get('ping_jitter', 0),
                "packet_loss": doc.get('packet_loss', 0),
                "download_speed": doc.get('speed_mbps', 0.0),

                # Device Health
                "uptime_seconds": doc.get('uptime_s', 0),
                "system_ram_free": doc.get('ram_free', 0)
            }
            results.append(data_point)

    return func.HttpResponse(
        json.dumps(results),
        status_code=200,
        mimetype="application/json",
        headers={
            "Access-Control-Allow-Origin": "*",
            "Access-Control-Allow-Headers": "Content-Type, Authorization",
            "Access-Control-Allow-Methods": "GET, POST, OPTIONS"
        }
    )
