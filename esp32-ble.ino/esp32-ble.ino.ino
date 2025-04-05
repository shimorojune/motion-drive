#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <BleGamepad.h>
#include "esp_bt.h"
#include "esp_bt_device.h"
#include "esp_gap_ble_api.h"
#include "esp_gatt_common_api.h"
#include "esp_err.h"

const int ledPin = 2;
const int blinkDelay = 50;
sensors_event_t Accelerometer, Gyroscope, Temperature;
Adafruit_MPU6050 mpu;
BleGamepad bleGamepad("MotionDrive", "ESP32 Dev", 100);

// -------------------------- CONFIG
int rollMultiplier = 2;

// -------------------------- Reduce Bluetooth Latency
void setBLEParams() {
  esp_ble_conn_update_params_t ble_params = {0};
  ble_params.min_int = 6;  // 7.5ms minimum interval (6 * 1.25ms)
  ble_params.max_int = 12; // 15ms maximum interval (12 * 1.25ms)
  ble_params.latency = 0;
  ble_params.timeout = 400; // Supervision timeout (recommended: 400ms)
  
  esp_ble_gap_update_conn_params(&ble_params);
  esp_ble_gatt_set_local_mtu(512); // Increase MTU for faster data transfer
}

// -------------------------- LOGGING FUNCTION
void logValue(String title, int value) {
  Serial.print(title);
  Serial.print(": ");
  Serial.println(value);
}

// -------------------------- SETUP
void setup() {
  Serial.begin(115200);
  
  // Initialize BLE Gamepad
  bleGamepad.begin();
  setBLEParams(); // Apply BLE optimizations

  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("MPU6050 not found!");
    while (1) {
      delay(blinkDelay);
      digitalWrite(ledPin, HIGH);
      delay(blinkDelay);
      digitalWrite(ledPin, LOW);
    }
  } else {
    Serial.println("MPU6050 connected!");
  }

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

// -------------------------- MAIN LOOP
void loop() {
  if (bleGamepad.isConnected()) {
    digitalWrite(ledPin, HIGH);
    
    // Read sensor data
    mpu.getEvent(&Accelerometer, &Gyroscope, &Temperature);
    
    // Compute Roll
    int computedRoll = Accelerometer.acceleration.roll * rollMultiplier;
    logValue("ComputedRoll", computedRoll);
    
    // Send data via BLE
    bleGamepad.setX(computedRoll);
  } else {
    digitalWrite(ledPin, LOW);
  }

  delay(2); // Small delay to reduce CPU load while keeping low latency
}