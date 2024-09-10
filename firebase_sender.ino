// BC:DD:C2:7E:83:02
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>
#include <ESP8266WiFi.h>
// #include <espnow.h>
#include <FirebaseESP8266.h>

// Firebase credentials
FirebaseConfig config;
FirebaseAuth auth;

LiquidCrystal_I2C lcd(0x27, 20, 4);

// WiFi credentials
const char* ssid = "*****";
const char* password = "*****";

#define in1 D3
#define in2 D4
#define in3 D5
#define in4 D6

FirebaseData firebaseData;
int stock = 0;

// Replace with receiver's MAC Address

#define coinPin D7

// Pin definitions
volatile int pulseCount = 0; // Count the number of pulses

const int stepsPerRevolution = 300; 

Stepper myStepper(stepsPerRevolution, in1, in2, in3, in4);

// Timing definitions
unsigned long lastPulseTime = 0;
const unsigned long timeout = 1000; // Time to wait for the next pulse in milliseconds

// Coin values in pulses
const int coin1Pulses = 1; // e.g., 1 pulse for $0.10
const int coin2Pulses = 2; // e.g., 2 pulses for $0.25
// const int coin3Pulses = 3; // e.g., 3 pulses for $1.00

float totalAmount = 0.0;
// int stock = 5;

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

// void onSent(uint8_t *macAddr, uint8_t sendStatus) {
//   Serial.print("Send Status: ");
//   Serial.println(sendStatus == 0 ? "Delivery Success" : "Delivery Fail");
// }

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  // WiFi.mode(WIFI_STA);

   // Connecting to WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  myStepper.setSpeed(100);


  // Configure Firebase
  config.host = "vending-machine-da946-default-rtdb.firebaseio.com";
  config.signer.tokens.legacy_token = "asdfgjhkjlk;l';";
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);


  pinMode(coinPin, INPUT_PULLUP); // Set the coin pin as input with pull-up resistor

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
  // Reading the stock value from Firebase
  if (Firebase.getInt(firebaseData, "/stock")) {
    if (firebaseData.dataType() == "int") {
      stock = firebaseData.intData();
    }
  } else {
    Serial.println("Failed to get stock value from Firebase");
    Serial.println(firebaseData.errorReason());
  }

  // stock logic
  if (stock <= 0){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("No pads available!!");

  } else {
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

      if (stock > 3){
        Serial.println("counterclockwise");
        myStepper.step(stepsPerRevolution);
          // String message = "StockLow";
          // esp_now_send(receiverAddress, (uint8_t *)message.c_str(), message.length());
          // stock = stock - 1;
      }   
      stock = stock - 1;

      if (Firebase.setInt(firebaseData, "/stock", stock)) {  // Update stock at root
        Serial.println("Stock updated successfully!");
      }

      delay(2000); // Wait for 5 seconds

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Thank you!");
      delay(2000); // Display "Thank you" for 2 seconds

      showWelcomeMessage(); 
    }

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

