#include <LiquidCrystal.h> // Include LCD library

// Initialize the LCD library with the numbers of the interface pins
LiquidCrystal lcd(7, 6, 5, 2, 8, 9); 
// RS -> 7, E -> 6, D4 -> 5, D5 -> 2, D6 -> 8, D7 -> 9

// Define analog input
#define ANALOG_IN_PIN A0
#define BUZZER_PIN 3 // Buzzer connected to digital pin 3
#define LED_PIN 4    // LED connected to digital pin 4

// Floats for ADC voltage & Input voltage
float adc_voltage = 0.0;
float in_voltage = 0.0;

// Floats for resistor values in divider (in ohms)
float R1 = 191600.0;
float R2 = 10000.0;

// Float for reference voltage of Arduino (UNO is 5V)
float ref_voltage = 5.0;

// Integer for storing ADC value
int adc_value = 0;

// Voltage limits (adjust as per your battery spec)
float over_voltage_limit = 101.0; // Max safe battery voltage
float under_voltage_limit = 88.0; // Minimum safe voltage

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize LCD
  lcd.begin(16, 2); // 16 columns, 2 rows
  lcd.print("Voltage Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000); // Wait for 2 seconds

  // Initialize buzzer and LED pins as output
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // Ensure buzzer and LED are off at start
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  // Initial serial message
  Serial.println("DC Voltage Measurement System with Buzzer and LED Alarm");
  Serial.println("------------------------------------------------------");
  delay(10);

  // Clear LCD for fresh display
  lcd.clear();
}

void loop() {
  // Read ADC value
  adc_value = analogRead(ANALOG_IN_PIN);

  // Convert ADC value to voltage
  adc_voltage = (adc_value * ref_voltage) / 1023.0;

  // Calculate input voltage based on divider formula
  in_voltage = adc_voltage * ((R1 + R2) / R2);

  // Display voltage on Serial Monitor
  Serial.print("Battery Input Voltage: ");
  Serial.print(in_voltage, 2);
  Serial.println(" V");

  // Display voltage on LCD
  lcd.setCursor(0, 0); // First line
  lcd.print("Voltage: ");
  lcd.print(in_voltage, 2);
  lcd.print("V ");

  // Alarm conditions
  if (in_voltage > over_voltage_limit || in_voltage < under_voltage_limit) {
    Serial.println("⚠️ Voltage abnormal! Buzzer & LED ON");
    digitalWrite(BUZZER_PIN, HIGH); // Turn ON buzzer
    digitalWrite(LED_PIN, HIGH);    // Turn ON LED

    // Display warning on LCD
    lcd.setCursor(0, 1); // Second line
    lcd.print("!! Voltage Alarm !!");
  } else {
    digitalWrite(BUZZER_PIN, LOW);  // Turn OFF buzzer
    digitalWrite(LED_PIN, LOW);     // Turn OFF LED

    // Display status on LCD
    lcd.setCursor(0, 1); // Second line
    lcd.print("Voltage Normal  ");
  }

  // Delay for stability
  delay(200);
}






