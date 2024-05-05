#include "UnoJoy.h"
#include <Wire.h>

String extractAndPrintNumbers(String data) {
  String character = "";
  for (size_t i = 0; i < data.length(); i++) {
    if (isdigit(data[i]) || data[i] == '-') {
      character.concat(data[i]);
    }
  }
  return character;
}

void receiveEvent(int byteCount) {
  dataForController_t controllerData = getBlankDataForController();
  String receivedData = "";
  while (Wire.available() > 0) {
    char c = Wire.read();
    receivedData += c;
  }

  // Now you can process the received data as needed
  // For example, you can extract and print numbers as you did in the loop function:
  String extractedValue = extractAndPrintNumbers(receivedData);
  uint8_t uintValue = extractedValue.toInt();
  Serial.println("Recieved data: " + extractedValue);


  controllerData.leftStickX = uintValue;
  setControllerData(controllerData);
}

void setup() {
  Serial.begin(115200);
  // setupUnoJoy();
  Wire.begin(8);  // Set the Arduino's address as 8
  Wire.onReceive(receiveEvent);
}

void loop() {}
