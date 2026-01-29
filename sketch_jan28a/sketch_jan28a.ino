#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Access Point credentials
const char* ssid = "ESP8266-WebServer";
const char* password = "12345678";  // Must be at least 8 characters

// Create web server on port 80
ESP8266WebServer server(80);

// HTML content for the main page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP8266 Web Server</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 600px;
            margin: 50px auto;
            padding: 20px;
            background-color: #f0f0f0;
        }
        .container {
            background-color: white;
            padding: 30px;
            border-radius: 10px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        h1 {
            color: #333;
            text-align: center;
        }
        .status {
            background-color: #4CAF50;
            color: white;
            padding: 15px;
            border-radius: 5px;
            margin: 20px 0;
            text-align: center;
        }
        .info {
            background-color: #2196F3;
            color: white;
            padding: 10px;
            border-radius: 5px;
            margin: 10px 0;
        }
        button {
            background-color: #008CBA;
            color: white;
            padding: 15px 32px;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            font-size: 16px;
            width: 100%;
            margin: 10px 0;
        }
        button:hover {
            background-color: #007399;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🌐 ESP8266 Web Server</h1>
        <div class="status">
            ✓ Server is Running!
        </div>
        <div class="info">
            <strong>Uptime:</strong> <span id="uptime">Loading...</span>
        </div>
        <button onclick="location.reload()">Refresh Page</button>
        <button onclick="window.location.href='/status'">Check Status</button>
    </div>
    <script>
        // Update uptime
        fetch('/uptime')
            .then(response => response.text())
            .then(data => {
                document.getElementById('uptime').textContent = data;
            });
    </script>
</body>
</html>
)rawliteral";

// Handle root URL
void handleRoot() {
    server.send(200, "text/html", index_html);
}

// Handle status page
void handleStatus() {
    String statusPage = "<html><head><meta charset='UTF-8'><style>";
    statusPage += "body { font-family: Arial; max-width: 600px; margin: 50px auto; padding: 20px; background: #f0f0f0; }";
    statusPage += ".container { background: white; padding: 30px; border-radius: 10px; }";
    statusPage += "h2 { color: #333; } .info { margin: 10px 0; padding: 10px; background: #e3f2fd; border-radius: 5px; }";
    statusPage += "</style></head><body><div class='container'>";
    statusPage += "<h2>📊 System Status</h2>";
    statusPage += "<div class='info'><strong>SSID:</strong> " + String(ssid) + "</div>";
    statusPage += "<div class='info'><strong>IP Address:</strong> " + WiFi.softAPIP().toString() + "</div>";
    statusPage += "<div class='info'><strong>MAC Address:</strong> " + WiFi.softAPmacAddress() + "</div>";
    statusPage += "<div class='info'><strong>Connected Clients:</strong> " + String(WiFi.softAPgetStationNum()) + "</div>";
    statusPage += "<div class='info'><strong>Uptime:</strong> " + String(millis() / 1000) + " seconds</div>";
    statusPage += "<div class='info'><strong>Free Heap:</strong> " + String(ESP.getFreeHeap()) + " bytes</div>";
    statusPage += "<br><button onclick='history.back()' style='padding: 10px 20px; background: #4CAF50; color: white; border: none; border-radius: 5px; cursor: pointer;'>← Back</button>";
    statusPage += "</div></body></html>";
    
    server.send(200, "text/html", statusPage);
}

// Handle uptime request
void handleUptime() {
    unsigned long uptime = millis() / 1000;
    unsigned long hours = uptime / 3600;
    unsigned long minutes = (uptime % 3600) / 60;
    unsigned long seconds = uptime % 60;
    
    String uptimeStr = String(hours) + "h " + String(minutes) + "m " + String(seconds) + "s";
    server.send(200, "text/plain", uptimeStr);
}

// Handle 404 errors
void handleNotFound() {
    String message = "404 - Page Not Found\n\n";
    message += "URI: " + server.uri() + "\n";
    message += "Method: " + String((server.method() == HTTP_GET) ? "GET" : "POST") + "\n";
    server.send(404, "text/plain", message);
}

void setup() {
    // Start Serial communication
    Serial.begin(115200);
    delay(100);
    
    Serial.println("\n\n=== ESP8266 Web Server ===");
    Serial.println("Starting Access Point...");
    
    // Configure Access Point
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    
    // Get AP IP address
    IPAddress IP = WiFi.softAPIP();
    
    Serial.println("\n✓ Access Point Started!");
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("Password: ");
    Serial.println(password);
    Serial.print("IP Address: ");
    Serial.println(IP);
    Serial.println("\nConnect to the WiFi network and open:");
    Serial.print("http://");
    Serial.println(IP);
    Serial.println("========================\n");
    
    // Configure web server routes
    server.on("/", handleRoot);
    server.on("/status", handleStatus);
    server.on("/uptime", handleUptime);
    server.onNotFound(handleNotFound);
    
    // Start web server
    server.begin();
    Serial.println("Web server started!");
}

void loop() {
    // Handle client requests
    server.handleClient();
}
