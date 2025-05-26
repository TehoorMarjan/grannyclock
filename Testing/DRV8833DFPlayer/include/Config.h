#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Pin definitions
namespace Pins {
    // DRV8833 control pins for solenoids
    constexpr uint8_t SOLENOID_SEC_1 = 27;  // First seconds solenoid control pin
    constexpr uint8_t SOLENOID_SEC_2 = 26;  // Second seconds solenoid control pin
    constexpr uint8_t SOLENOID_MIN_1 = 33;  // First minutes solenoid control pin
    constexpr uint8_t SOLENOID_MIN_2 = 32;  // Second minutes solenoid control pin
}

// Timing configurations
namespace Timing {
    constexpr uint16_t SOLENOID_PULSE_WIDTH_MS = 40;  // Duration of solenoid pulse in milliseconds
    constexpr uint16_t SECOND_MS = 1000;              // Milliseconds in a second
}

#endif // CONFIG_H