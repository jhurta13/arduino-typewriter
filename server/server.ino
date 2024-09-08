#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <AsyncWebSocket.h>

// WiFi credentials
const char* ssid = "HurtadoWiFi";
const char* password = "32141516";

// Initialize the server and WebSocket on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");  // WebSocket endpoint at /ws

// Global variable to store typed text
String typedText = "";

// Handle incoming serial data from the Arduino
void handleSerial() {
  while (Serial.available()) {
    char c = Serial.read();
    typedText += c;  // Append character to typedText
    if (typedText.length() > 1000) {
      typedText = typedText.substring(500);  // Keep only the last 500 characters
    }

    // Send the new typed data to all connected WebSocket clients
    ws.textAll(typedText);
  }
}

// Handle WebSocket events
void onWebSocketEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t * data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.println("Client connected.");
    client->text(typedText);  // Send the current typed text to the new client
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.println("Client disconnected.");
  }
}

void setup() {
  Serial.begin(115200);  // Serial communication with Arduino

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("WiFi connected.");
  Serial.println(WiFi.localIP());

  // WebSocket event handler
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);

  // Serve a simple webpage
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "<html><body><h1>Typed Data:</h1><p id='typedData'></p><script>"
                                    "var socket = new WebSocket('ws://' + window.location.host + '/ws');"
                                    "socket.onmessage = function(event) { document.getElementById('typedData').innerText = event.data; };"
                                    "</script></body></html>");
  });

  // Start the server
  server.begin();
}

void loop() {
  handleSerial();  // Continuously check for new serial data from Arduino
  ws.cleanupClients();  // Keep WebSocket connections alive
}
