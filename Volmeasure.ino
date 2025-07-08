#define BUZZER_PIN 2
#define LED_PIN 3
#define DISPLAY_TX_PIN 5 // Corrected to D1 based on the schematic

#include <SoftwareSerial.h>

// UART software serial: TX to display RXD
SoftwareSerial lcdSerial(DISPLAY_TX_PIN, -1); // TX only

// Voltage divider: R1 = 22k, R2 = 8.2k → Divider ratio = Vout * (R1 + R2) / R2
const float VOLTAGE_DIVIDER_RATIO = 12.0 / 3.3;
const float LOW_VOLTAGE_THRESHOLD = 10.0; // Adjust as per your system warning level

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  lcdSerial.begin(9600);     // UART baud rate for MILFORD display (VERIFY THIS!)
  Serial.begin(9600);       // Debugging

  Serial.println("Voltage Monitor Started"); // Added startup message for serial

  lcdSerial.println("Voltage Monitor");
  delay(1000);
}

void loop() {
  int rawADC = analogRead(A0);         // ADC value (0–1023)
  float vout = (rawADC / 1023.0) * 3.3; // NodeMCU has internal scaling to 3.3V
  float actualVoltage = vout * VOLTAGE_DIVIDER_RATIO;

  // Debug Serial
  Serial.print("Voltage: ");
  Serial.println(actualVoltage);

  // Display on MILFORD UART LCD
  lcdSerial.print("Voltage: ");
  lcdSerial.print(actualVoltage);
  lcdSerial.println(" V");

  // LED and Buzzer Warning
  if (actualVoltage < LOW_VOLTAGE_THRESHOLD) {
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
  }

  delay(1000); // Update every second
}

