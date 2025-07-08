// ESP32 Voltage Monitor with OLED + Wi-Fi Web Server using AJAX (8 clients)

#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WebServer.h>

// OLED settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin Definitions
#define ANALOG_PIN 34
#define BUZZER_PIN 2
#define LED_PIN 4

// Voltage Divider Resistors (in ohms)
float R1 = 295000.0;
float R2 = 10000.0;

// ESP32 ADC Reference and correction
float ref_voltage = 3.30;
float correction_factor = 0.985;

// Voltage limits
float over_voltage_limit = 101.0;
float under_voltage_limit = 88.0;

// Averaging and smoothing
int NUM_SAMPLES = 20;
float adc_voltage = 0.0;
float in_voltage = 0.0;
float smooth_voltage = 0.0;
float alpha = 0.1;

// Wi-Fi credentials
const char* ssid = "ESP32_Voltage";
const char* password = "12345678";

WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", R"rawliteral(
    <!DOCTYPE html><html><head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body { font-family: Arial; text-align: center; }
      .voltage { font-size: 2em; color: #333; margin-top: 20px; }
      .status { font-size: 1.5em; color: green; }
      .alert { color: red; }
    </style>
    </head><body>
    <h2>ESP32 Voltage Monitor</h2>
    <div class="voltage" id="voltage">-- V</div>
    <div class="status" id="status">Loading...</div>
    <script>
      setInterval(() => {
        fetch("/voltage").then(r => r.json()).then(data => {
          document.getElementById("voltage").innerHTML = data.v + " V";
          const status = document.getElementById("status");
          if (data.alert) {
            status.className = 'status alert';
            status.innerHTML = "!! Voltage Alarm !!";
          } else {
            status.className = 'status';
            status.innerHTML = "Voltage Normal";
          }
        });
      }, 1000);
    </script>
    </body></html>
  )rawliteral");
}

void handleVoltage() {
  char json[100];
  snprintf(json, sizeof(json), "{\"v\":%.2f,\"alert\":%s}", smooth_voltage, (smooth_voltage > over_voltage_limit || smooth_voltage < under_voltage_limit) ? "true" : "false");
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  Wire.begin(21, 22); // SDA, SCL
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Voltage Monitor");
  display.display();
  delay(1500);

  WiFi.softAP(ssid, password, 1, 0, 8); // Up to 8 clients
  Serial.println("Wi-Fi Access Point started");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/voltage", handleVoltage);
  server.begin();
}

void loop() {
  long total_adc = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    total_adc += analogRead(ANALOG_PIN);
    delay(2);
  }
  float avg_adc = total_adc / (float)NUM_SAMPLES;
  adc_voltage = (avg_adc * ref_voltage) / 4095.0;
  in_voltage = adc_voltage * ((R1 + R2) / R2);
  in_voltage *= correction_factor;
  smooth_voltage = (alpha * in_voltage) + ((1 - alpha) * smooth_voltage);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Voltage: ");
  display.print(smooth_voltage, 2);
  display.println(" V");

  if (smooth_voltage > over_voltage_limit || smooth_voltage < under_voltage_limit) {
    display.println("!! Voltage Alarm !!");
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
  } else {
    display.println("Voltage Normal");
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
  }
  display.display();

  server.handleClient();
  delay(200);
}
