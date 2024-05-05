#include <WiFi.h>
#include <WebSocketsClient.h>
#include <MPU6050_light.h>
#include <Wire.h>
#include <Arduino.h>
#include <time.h>

// CONSTANTS
// hotspot SSID to connect
const char *ssid = "MOTIONDRIVE-AP";
// predefined password shared between server and client
const char *password = "sBP9654=)Zj|";
// predefined port and IP for server on hotspot
const char *serverIP = "192.168.4.1";
const int serverPort = 80;

// GLOBALS
// initializing socket and sensor
MPU6050 mpu(Wire);
WebSocketsClient webSocket;

// FUNCTIONS
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      Serial.println(F("WebSocket connected"));
      break;
    case WStype_DISCONNECTED:
      Serial.println(F("WebSocket disconnected"));
      break;
  }
}
int processMPUData() {
  int tiltValue = round(mpu.getAccX() * 100);
  // Serial.print("\nTilt Value: " + String(tiltValue));
  int computedTiltValue = tiltValue;
  if (tiltValue > 0) {
    computedTiltValue = map(tiltValue, 1, 110, -128, -1);
  } else if (tiltValue < 0) {
    computedTiltValue = map(tiltValue, 1, -110, 128, 1);
  } else if (tiltValue == 0) {
    computedTiltValue = 128;
  } else {
    if (tiltValue > 0) {
      computedTiltValue = 1;
    } else {
      computedTiltValue = -1;
    }
  }
  return computedTiltValue;
}

// SETUP
void setup() {
  Serial.begin(115200);
  Serial.println(F("\nWelcome to MotionDrive Client\n"));
  Wire.begin();
  setCpuFrequencyMhz(80);

  // MPU SETUP
  Serial.println(F("\nMPU initiating"));
  const byte status = mpu.begin();
  while (status != 0) {
    Serial.println(F("MPU setup retrying..."));
    delay(1000);
  }
  mpu.calcOffsets(true, true);
  Serial.println(F("MPU initiated and calibrated"));


  // WIFI SETUP
  WiFi.begin(ssid, password);
  unsigned long wifiStartTime = millis();
  Serial.print(F("WiFi connecting"));
  // Timeout after 10 seconds
  while (WiFi.status() != WL_CONNECTED && (millis() - wifiStartTime < 10000)) {
    Serial.print(F("."));
    delay(1000);
  }
  Serial.println(F("\nWiFi connection established"));

  // WEB SOCKET SETUP
  Serial.println(F("\nWebSocket initiating"));
  webSocket.begin(serverIP, serverPort, "/ws");
  webSocket.onEvent(webSocketEvent);
  unsigned long startTime = millis();
  // Wait for the WebSocket connection to be established
  // Timeout after 10 seconds
  while (!webSocket.isConnected() && (millis() - startTime < 10000)) {
    webSocket.loop();
    delay(10);
  }
}

void loop() {
  delay(1000);
  // Handle WebSocket events
  webSocket.loop();

  // Fetch updated values from MPU
  mpu.update();

  // Fetch updated values from MPU6050
  // and convert to string for transmission to server
  String tiltStringValue = String(processMPUData());

  if(webSocket.isConnected()){
  // Send the string over WebSocket
  Serial.println("Transmitted Value: " + tiltStringValue);
  webSocket.sendTXT(tiltStringValue);
  }
}
