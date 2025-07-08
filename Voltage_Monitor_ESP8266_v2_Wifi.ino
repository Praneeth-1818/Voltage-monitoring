#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "VoltageMonitor";  // Your ESP Wi-Fi name
const char* password = "12345678";    // Password (min 8 characters)

ESP8266WebServer server(80);

// Voltage Divider Configuration
const int analogInput = A0;
const float R1 = 300000.0;
const float R2 = 3000.0;
const float referenceVoltage = 1.0;  // ESP8266 ADC is 0-1V
const float calibrationFactor = 0.978;  // Adjust this based on multimeter

// HTML Template
String htmlPage(float vin, float vout, int adc) {
  String page = "<!DOCTYPE html><html><head><meta charset='utf-8'>";
  page += "<meta http-equiv='refresh' content='1'>";
  page += "<title>Voltage Monitor</title></head><body style='font-family:sans-serif;text-align:center'>";
  page += "<h1>ESP8266 Voltage Monitor</h1>";
  page += "<p><b>ADC Value:</b> " + String(adc) + "</p>";
  page += "<p><b>Measured Voltage (A0):</b> " + String(vout, 3) + " V</p>";
  page += "<p><b>Calculated Input Voltage:</b> <span style='font-size: 1.5em; color: green;'>" + String(vin, 2) + " V</span></p>";
  page += "</body></html>";
  return page;
}

void handleRoot() {
  int adcValue = analogRead(analogInput);
  float vout = (adcValue * referenceVoltage) / 1024.0;
  float vin = vout / (R2 / (R1 + R2));
  vin *= calibrationFactor;

  if (vin < 0.1) vin = 0.0;

  server.send(200, "text/html", htmlPage(vin, vout, adcValue));
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Setting up Access Point...");

  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("Access Point IP address: ");
  Serial.println(IP);

  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started.");
}

void loop() {
  server.handleClient();
}
