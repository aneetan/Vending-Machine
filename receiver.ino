#include <ESP8266WiFi.h>
#include <Stepper.h>
#include <espnow.h>

#define in1 D3
#define in2 D4
#define in3 D5
#define in4 D6

const int stepsPerRevolution = 300; 
Stepper myStepperTwo(stepsPerRevolution, in1, in2, in3, in4);


void onReceive(uint8_t *macAddr, uint8_t *data, uint8_t len) {
  String message = String((char *)data);

  Serial.print("Received from: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(macAddr[i], HEX);
    if (i < 5) Serial.print(":");
  }
  Serial.print(" - Data: ");
  Serial.println(message);

  if (message == "StockLow") {
    // Run the motor if the stock is low
    Serial.println("Running motor due to low stock");
    myStepperTwo.step(stepsPerRevolution);
  }
}


void setup() {
  Serial.begin(9600);

  myStepperTwo.setSpeed(100);

  WiFi.mode(WIFI_STA);


  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Set device as a slave
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);

  // Register receive callback
  esp_now_register_recv_cb(onReceive);
}

void loop() {
  // Do nothing, just wait for incoming data

          // myStepperTwo.step(stepsPerRevolution);

}