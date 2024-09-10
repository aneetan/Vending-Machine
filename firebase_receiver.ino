
#include <Stepper.h>
#include <ESP8266WiFi.h>
// #include <espnow.h>
#include <FirebaseESP8266.h>

// Firebase credentials for the first
FirebaseConfig config1;
FirebaseAuth auth1;

FirebaseData firebaseData1;

// Firebase credentials for the second project
FirebaseConfig config2;
FirebaseAuth auth2;
FirebaseData firebaseData2;

int stock = 0;
int pre_stock = 0;

#define in1 D3
#define in2 D4
#define in3 D5
#define in4 D6


const char* ssid = "**";
const char* password = "*****";

const int stepsPerRevolution = 300; 
Stepper myStepperTwo(stepsPerRevolution, in1, in2, in3, in4);


void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  myStepperTwo.setSpeed(100);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");


  // Firebase 1 setup

  config1.host = "vending-machine-da946-default-rtdb.firebaseio.com";
  config1.signer.tokens.legacy_token = "asdfghjkl;"; // secret key of database
  Firebase.begin(&config1, &auth1);
  Firebase.reconnectWiFi(true);

  // Firebase 2 setup
  config2.host = "pre-stock-f249d-default-rtdb.asia-southeast1.firebasedatabase.app";
  config2.signer.tokens.legacy_token = "qwertyuiop"; // secret key of database
  
}

int firebaseSwitch = 0;
void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);

  // firebase1
  // Reading the stock value from Firebase
  if (Firebase.getInt(firebaseData1, "/stock")) {
    if (firebaseData1.dataType() == "int") {
      stock = firebaseData1.intData();
      Serial.print("Stock from Firebase 1: ");
      Serial.println(stock);
    }
  } else {
    Serial.println("Failed to get stock value from Firebase 1");
    Serial.println(firebaseData1.errorReason());
  }

  Firebase.begin(&config2, &auth2);
  Firebase.reconnectWiFi(true);
  delay(1000);

  // Firebase 2 - Reading stock value
  if (Firebase.getInt(firebaseData2, "/pre_stock")) {
    if (firebaseData2.dataType() == "int") {
      pre_stock = firebaseData2.intData();
      Serial.print("Stock from Firebase 2: ");
      Serial.println(pre_stock);
    }
  } else {
    Serial.println("Failed to get stock from Firebase 2");
    Serial.println(firebaseData2.errorReason());
  }

  delay(1000);


  // pre_stock = stock;

  if(stock < 3 && (pre_stock != stock)){
     Serial.println("Running motor due to low stock");
    myStepperTwo.step(stepsPerRevolution);
    // stock = stock - 1;
  }

   if (Firebase.setInt(firebaseData2, "/pre_stock", stock)) {  // Update stock at roo
        Serial.println("Stock updated successfully!");
      }


   Firebase.begin(&config1, &auth1);
  Firebase.reconnectWiFi(true);


  

}
