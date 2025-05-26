#include <Arduino.h>
#include "Config.h"

// Flag to track which pin to pulse next
bool pulseSecPin1 = true;
bool pulseMinPin1 = true;

void setup() {
  // Initialize serial for debugging
  Serial.begin(115200);
  Serial.println("DRV8833 Solenoid Test");
  
  // Initialize solenoid control pins as outputs
  pinMode(Pins::SOLENOID_SEC_1, OUTPUT);
  pinMode(Pins::SOLENOID_SEC_2, OUTPUT);
  pinMode(Pins::SOLENOID_MIN_1, OUTPUT);
  pinMode(Pins::SOLENOID_MIN_2, OUTPUT);
  Serial.println("Pins initialized.");
  
  // Ensure both pins are LOW at startup
  digitalWrite(Pins::SOLENOID_SEC_1, LOW);
  digitalWrite(Pins::SOLENOID_SEC_2, LOW);
  digitalWrite(Pins::SOLENOID_MIN_1, LOW);
  digitalWrite(Pins::SOLENOID_MIN_2, LOW);
}

void loop() {
  // Determine which pin to pulse based on the flag
  uint8_t pulseSecPin = pulseSecPin1 ? Pins::SOLENOID_SEC_1 : Pins::SOLENOID_SEC_2;
  uint8_t pulseMinPin = pulseMinPin1 ? Pins::SOLENOID_MIN_1 : Pins::SOLENOID_MIN_2;
  
  // Debug output
  Serial.print("Pulsing ");
  Serial.print(pulseSecPin1 ? "SEC_1" : "SEC_2");
  Serial.print(" ( ");
  Serial.print(Timing::SOLENOID_PULSE_WIDTH_MS);
  Serial.println(" ms)");
  
  // Send the pulse
  digitalWrite(pulseSecPin, HIGH);
  delay(Timing::SOLENOID_PULSE_WIDTH_MS);
  digitalWrite(pulseSecPin, LOW);
  delay(Timing::SOLENOID_PULSE_WIDTH_MS);
  digitalWrite(pulseMinPin, HIGH);
  delay(Timing::SOLENOID_PULSE_WIDTH_MS);
  digitalWrite(pulseMinPin, LOW);
  
  // Toggle the flag for next pulse
  pulseSecPin1 = !pulseSecPin1;
  pulseMinPin1 = !pulseMinPin1;
  
  // Wait for remainder of the second
  delay(Timing::SECOND_MS - (3 * Timing::SOLENOID_PULSE_WIDTH_MS));
}