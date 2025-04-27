#include "StatusIndicator.h"
#include <Arduino.h>

void StatusIndicator::begin() {
    // Set up LED pins as outputs
    pinMode(Pins::LED_R, OUTPUT);
    pinMode(Pins::LED_G, OUTPUT);
    pinMode(Pins::LED_B, OUTPUT);
    
    // Initialize with OK status (green)
    setStatus(STATUS_OK);
}

void StatusIndicator::update() {
    // Handle blinking for PANIC status
    if (currentStatus == STATUS_PANIC) {
        unsigned long now = millis();
        if (now - lastBlinkTime >= 500) { // 500ms blink rate
            lastBlinkTime = now;
            blinkState = !blinkState;
            
            if (blinkState) {
                setColor(255, 0, 0); // Red
            } else {
                setColor(0, 0, 0); // Off
            }
        }
    }
}

void StatusIndicator::setStatus(StatusCode status) {
    currentStatus = status;
    
    // Reset blink state
    lastBlinkTime = millis();
    blinkState = true;
    
    // Set appropriate LED color based on status
    switch (status) {
        case STATUS_OK:
            setColor(0, 255, 0); // Green
            break;
        case STATUS_SD_MISSING:
            setColor(255, 0, 255); // Purple (Red + Blue)
            break;
        case STATUS_BUFFER_OVERFLOW:
            setColor(255, 0, 0); // Red
            break;
        case STATUS_PANIC:
            setColor(255, 0, 0); // Start with Red, will blink in update()
            break;
        default:
            setColor(0, 0, 255); // Blue (unexpected status)
            break;
    }
}

StatusCode StatusIndicator::getStatus() const {
    return currentStatus;
}

void StatusIndicator::setColor(uint8_t red, uint8_t green, uint8_t blue) {
    // ESP32 typically uses active-high logic for pins
    // Adjust values if your RGB LED is common anode (active low)
    analogWrite(Pins::LED_R, red);
    analogWrite(Pins::LED_G, green);
    analogWrite(Pins::LED_B, blue);
}