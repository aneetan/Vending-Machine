#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "****";      // Replace with your WiFi SSID
const char* password = "****"; // Replace with your WiFi password

ESP8266WebServer server(80); // HTTP server on port 80

void handleSignal() {
  Serial.println("Signal received from client");
  server.send(200, "text/plain", "Stock = 50");
}

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");

  // Define HTTP route
  server.on("/signal", handleSignal);

  // Start the server
  server.begin();
  Serial.println("Server started");

  Serial.println(WiFi.localIP());

}

void loop() {
  server.handleClient(); // Handle incoming clients
}
