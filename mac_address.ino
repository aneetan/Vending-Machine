#include <ESP8266WiFi.h>

void setup() {
  // Start the serial communication
  Serial.begin(115200);

  // Give the ESP some time to initialize
  delay(1000);

  // Get and print the MAC address
  Serial.println("ESP8266 MAC Address: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
  // Nothing to do here
}
