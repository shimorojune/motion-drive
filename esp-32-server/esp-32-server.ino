#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <AsyncTCP.h>
#include <Wire.h>

// Set up WiFi credentials for the Access Point
const char *ssid = "MOTIONDRIVE-AP";
const char *password = "sBP9654=)Zj|";

// Create an AsyncWebServer instance on port 80
AsyncWebServer server(80);

// Create an AsyncWebSocket instance with the path "/ws"
AsyncWebSocket ws("/ws");

String extractAndPrintNumbers(String data) {
  String character = "";
  for (size_t i = 0; i < data.length(); i++) {
    if (isdigit(data[i]) && character || data[i] == '-') {
      character.concat(data[i]);
    }
  }
  return character;
}

// Callback function to handle WebSocket events
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.println("WebSocket client connected");
      break;
    case WS_EVT_DISCONNECT:
      Serial.println("WebSocket client disconnected");
      break;
    case WS_EVT_DATA:
      String dataString = extractAndPrintNumbers((char *)data);
      // Convert String to char array (C-string)
      const char *dataCharArray = dataString.c_str();
      long dataNumber = strtol(dataCharArray, NULL, 10);
      
      // Guard cases
      if(dataNumber < -128 || dataNumber > 128){        
        break;
      }

      Serial.println("Recieved data: " + dataString);

      // Send data using Wire.write
      Wire.beginTransmission(8);
      Wire.write(dataCharArray);  // Data to be sent
      Wire.endTransmission();
      break;
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  IPAddress local_IP(192, 168, 4, 1);  // Set the desired IP address
  IPAddress gateway(192, 168, 4, 1);   // Set the gateway (usually same as IP address)
  IPAddress subnet(255, 255, 255, 0);  // Set the subnet mask

  WiFi.softAPConfig(local_IP, gateway, subnet);  // Manually configure the soft AP

  // Set up the ESP32 as a soft Access Point
  WiFi.softAP(ssid, password);

  // Obtain and print the IP address of the Access Point
  IPAddress ip = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(ip);

  // Register the WebSocket event handler
  ws.onEvent(onWebSocketEvent);
  // Add the WebSocket handler to the server
  server.addHandler(&ws);

  // Define a handler for the root ("/") route that responds with a simple message
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hello, ESP32 AP!");
  });

  // Start the server
  server.begin();
}

void loop() {
  // Handle WebSocket events and clean up disconnected clients
  ws.cleanupClients();
}
