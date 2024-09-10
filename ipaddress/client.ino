#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "******";      // Replace with your WiFi SSID
const char* password = "*******"; // Replace with your WiFi password
const char* serverIP = "192.1.1.1";  // Replace with the IP address of the server ESP

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
     WiFiClient client; 
    http.begin(client, "http://" + String(serverIP) + "/signal"); 
    int httpCode = http.GET(); // Send the request

    if (httpCode > 0) {
      String payload = http.getString(); // Get the response payload
      Serial.println(payload); // Print the response
    } else {
      Serial.println("Error sending signal");
    }
    
    http.end(); // Close the connection
  }

  delay(5000); // Send signal every 5 seconds
}
