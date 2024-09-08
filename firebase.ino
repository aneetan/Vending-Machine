#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

// Firebase credentials
FirebaseConfig config;
FirebaseAuth auth;

// WiFi credentials
const char* ssid = "Virinchi College";
const char* password = "virinchi@2024";

FirebaseData firebaseData;
int stock = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  // Connecting to WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Configure Firebase
  config.host = "vending-machine-da946-default-rtdb.firebaseio.com";
  config.signer.tokens.legacy_token = "bi3gfaM4HODAfopo4mZakNPNwOjVVOVmDycwku5N";

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Reading the stock value from Firebase
  if (Firebase.getInt(firebaseData, "/stock")) {
    if (firebaseData.dataType() == "int") {
      stock = firebaseData.intData();

      if (stock > 10) {
        stock = 10;
        Serial.println("Stock is greater than 10. Sending stock = 10 to the user.");
          if (Firebase.setInt(firebaseData, "/stock", stock)) {  // Update stock at root
            Serial.println("Stock updated successfully!");
          }
      } else {
        Serial.print("Stock value from Firebase: ");
        Serial.println(stock);
      }
    }
  } else {
    Serial.println("Failed to get stock value from Firebase");
    Serial.println(firebaseData.errorReason());
  }

  delay(5000);  // Wait for 5 seconds before the next update
}