#include <Arduino.h>

#define LED_PIN PIN_PA1

#define BLINK_INTERVAL 500

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_PIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_PIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(BLINK_INTERVAL);        // wait interval
  digitalWrite(LED_PIN, LOW);   // turn the LED off by making the voltage LOW
  delay(BLINK_INTERVAL);        // wait interval
}
