#include <PS2Keyboard.h>
#include <SoftwareSerial.h>

const int DataPin = 2;
const int IRQpin = 3;
PS2Keyboard keyboard;

SoftwareSerial esp8266(4, 5);  // RX, TX for ESP8266

void setup() {
  Serial.begin(115200);
  esp8266.begin(115200);  // Initialize SoftwareSerial for ESP8266 communication

  keyboard.begin(DataPin, IRQpin);
  Serial.println("Keyboard Ready.");
  esp8266.println("Keyboard Ready.");  // Send a message to ESP8266
}

void loop() {
  if (keyboard.available()) {
    char c = keyboard.read();
    Serial.print(c);  // Print to Serial for debugging
    esp8266.print(c);  // Send keystrokes to ESP8266
  }
}
