#include <LiquidCrystal_I2C.h>
#include <Stepper.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define in1 D3
#define in2 D4
#define in3 D5
#define in4 D6

// Replace with receiver's MAC Address
uint8_t receiverAddress[] = {0xAB, 0xCD, 0xEF, 0xGH, 0xIJ, 0xKL}; //mac address here

#define coinPin D7

// Pin definitions
volatile int pulseCount = 0; // Count the number of pulses

const int stepsPerRevolution = 300; 
// const int stepsPerRevolution2 = 200; 

Stepper myStepper(stepsPerRevolution, in1, in2, in3, in4);
// Stepper myStepperTwo(stepsPerRevolution, in11, in22, in33, in44);

// Timing definitions
unsigned long lastPulseTime = 0;
const unsigned long timeout = 1000; // Time to wait for the next pulse in milliseconds

// Coin values in pulses
const int coin1Pulses = 1; // e.g., 1 pulse for $0.10
const int coin2Pulses = 2; // e.g., 2 pulses for $0.25
// const int coin3Pulses = 3; // e.g., 3 pulses for $1.00

float totalAmount = 0.0;
int stock = 5;

 void showWelcomeMessage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome To");
  lcd.setCursor(0, 1);
  lcd.print("Sanitary Pad");
}

void ICACHE_RAM_ATTR coinInserted() {

    // Debounce logic: Ignore pulses that come too quickly after each other
  if (millis() - lastPulseTime > 1000) { // 50ms debounce time
    pulseCount++; // Increment pulse count on each pulse
    lastPulseTime = millis(); // Reset the timer on each pulse
  }
}

void onSent(uint8_t *macAddr, uint8_t sendStatus) {
  Serial.print("Send Status: ");
  Serial.println(sendStatus == 0 ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);

  myStepper.setSpeed(100);

  pinMode(coinPin, INPUT_PULLUP); // Set the coin pin as input with pull-up resistor

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Register send callback
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(onSent);

  // Add peer (receiver) address
  esp_now_add_peer(receiverAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  lcd.init();
  lcd.backlight();
  showWelcomeMessage();
  noInterrupts();
  attachInterrupt(digitalPinToInterrupt(coinPin), coinInserted, FALLING); // Attach interrupt for pulse counting
  interrupts();

  pulseCount = 0;
  totalAmount = 0.0; 
}

void loop() {
  // Data to send (can be any data structure)
  // String message = "Hello  Sender";
  // esp_now_send(receiverAddress, (uint8_t *)message.c_str(), message.length());

  delay(2000);  // Send data every 2 seconds
  if (millis() - lastPulseTime > timeout && pulseCount > 0) {
    // lastPulseTime = millis(); // Reset the timer on each pulse
    identifyCoin(pulseCount); // Identify the coin based on the pulse count
    pulseCount = 0; // Reset the pulse count after identifying the coin
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Total amount:");
    lcd.setCursor(0, 1);
    lcd.print(totalAmount);
    
    if (totalAmount >= 3.0) {
      totalAmount = 0.0; // Reset the total amount after vending
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Pad vending");
      Serial.println("Pad vending");

      if (stock <= 3){
          String message = "StockLow";
          esp_now_send(receiverAddress, (uint8_t *)message.c_str(), message.length());
          stock = stock - 1;

      } else {
         Serial.println("counterclockwise");
          myStepper.step(stepsPerRevolution);
          stock = stock -1;
          
      }
     

      // Serial.println("counterclockwise");
      // myStepperTwo.step(stepsPerRevolution);
      // delay(500);

      delay(2000); // Wait for 5 seconds

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Thank you!");
      delay(2000); // Display "Thank you" for 2 seconds

      showWelcomeMessage(); 
    }

  }
}

void identifyCoin(int count) {
  Serial.println(count);
 
  switch (count) {
    case coin1Pulses:
      Serial.println("Coin detected: 1");
      totalAmount += 1.0;
      break;
    case coin2Pulses:
      Serial.println("Coin detected: 2");
      totalAmount += 2.0;
      break;
    default:
      Serial.println("Unknown coin detected");
      break;
  }
  
}