#include "UnoJoy.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

const int ledPin = 13;
const int blinkDelay = 50;
int drift = 0;
dataForController_t controllerData = getBlankDataForController();
sensors_event_t Accelerometer, Gyroscope, Temperature;
Adafruit_MPU6050 mpu;
float smoothedRoll = 0.0; // Store the smoothed roll value
const float smoothingFactor = 0.1; // Adjust from 0.0 (no smoothing) to 1.0 (max smoothing)

int getComputedRoll(){
  // Read sensor data
  mpu.getEvent(&Accelerometer, &Gyroscope, &Temperature);

  // Get raw roll data directly (from Adafruit Unified Sensor)
  float rollData = Accelerometer.acceleration.roll;

  // Apply exponential smoothing
  smoothedRoll = smoothedRoll * (1.0 - smoothingFactor) + rollData * smoothingFactor;

  // Map from -9 to +9 → 0 to 255
  int computedRoll = map(smoothedRoll * 100, -900, 900, 0, 255);
  computedRoll = constrain(computedRoll, 0, 255) - drift;

  // Debug output
  // Serial.println(computedRoll);

  return computedRoll;
}

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);

  // Initialize MPU6050
  if (!mpu.begin()) {
    // Serial.println("MPU6050 not found!");
    delay(blinkDelay);
    digitalWrite(ledPin, HIGH);
    delay(blinkDelay);
    digitalWrite(ledPin, LOW);    
  } else {
    // Serial.println("MPU6050 connected!");
    drift = 127 - getComputedRoll();
    // Serial.print("drift: ");
    // Serial.println(drift);
  }
  // comment this when debugging
  // corrupts serial output
  setupUnoJoy();

  digitalWrite(ledPin, LOW);
}

void loop() {
  controllerData.triangleOn = 0;
  controllerData.circleOn = 0;
  controllerData.squareOn = 0;
  controllerData.crossOn = 0;
  controllerData.dpadUpOn = 0;
  controllerData.dpadDownOn = 0;
  controllerData.dpadLeftOn = 0;
  controllerData.dpadRightOn = 0;
  controllerData.l1On = 0;
  controllerData.r1On = 0;
  controllerData.selectOn = 0;
  controllerData.startOn = 0;
  controllerData.homeOn = 0;
  controllerData.leftStickY = 0;
  controllerData.rightStickX = 0;
  controllerData.rightStickY = 0;
  controllerData.leftStickX = getComputedRoll();
  setControllerData(controllerData);
}
