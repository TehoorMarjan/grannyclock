#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Pin definitions
namespace Pins {
    // Clock signals
    constexpr uint8_t sigRF = 36; // Radio Frequency signal
    constexpr uint8_t sigMU = 39; // Minute Unit signal
    constexpr uint8_t sigBR = 34; // BR signal
    constexpr uint8_t sigBA = 35; // BA signal

    // SD Card
    constexpr uint8_t SD_CS = 17;   // Chip select
    constexpr uint8_t SD_MOSI = 23; // MOSI
    constexpr uint8_t SD_CLK = 18;  // Clock
    constexpr uint8_t SD_MISO = 19; // MISO

    // RGB LED pins
    constexpr uint8_t LED_R = 32; // Red
    constexpr uint8_t LED_G = 33; // Green
    constexpr uint8_t LED_B = 25; // Blue
}

// Signal types
enum SignalType : uint8_t {
    RF_SIGNAL = 0,
    MU_SIGNAL = 1,
    BR_SIGNAL = 2,
    BA_SIGNAL = 3
};

// Edge types
enum EdgeType : uint8_t {
    EDGE_RISING = 0,
    EDGE_FALLING = 1
};

// Event entry structure (8 bytes total)
struct EventEntry {
    uint8_t signalType;  // RF, MU, BR, BA
    uint8_t edgeType;    // RISING or FALLING
    uint16_t reserved;   // For future use and alignment
    uint32_t timestamp;  // millis() value
};

// Timing constants
namespace Timing {
    // RF signal timing (in milliseconds)
    constexpr unsigned long RF_ACTIVE_PERIOD = 5 * 60 * 1000UL;      // 5 minutes
    constexpr unsigned long RF_INACTIVE_PERIOD = 25 * 60 * 1000UL;   // 25 minutes 
    constexpr unsigned long RF_CYCLE_PERIOD = RF_ACTIVE_PERIOD + RF_INACTIVE_PERIOD; // 30 minutes total

    // SD Card commit interval
    constexpr unsigned long SD_COMMIT_INTERVAL = 60 * 1000UL; // 1 minute
}

// Buffer configuration
namespace BufferConfig {
    // Using about 32KB of memory for the ring buffer (4096 events)
    constexpr size_t BUFFER_SIZE = 4096; 
    
    // File path for data storage
    const char* const DATA_FILE_PATH = "/data.csv";
    
    // CSV header
    const char* const CSV_HEADER = "Signal,Edge,Timestamp";
}

// Status codes for LED
enum StatusCode {
    STATUS_OK,              // Green: everything normal
    STATUS_SD_MISSING,      // Purple: SD card missing, data pending
    STATUS_BUFFER_OVERFLOW, // Red: buffer overflow, logging stopped
    STATUS_PANIC            // Blinking Red: fatal error
};

// Signal name mapping
inline const char* signalTypeToString(SignalType type) {
    switch (type) {
        case RF_SIGNAL: return "RF";
        case MU_SIGNAL: return "MU";
        case BR_SIGNAL: return "BR";
        case BA_SIGNAL: return "BA";
        default: return "UN"; // Unknown
    }
}

// Edge type mapping
inline const char* edgeTypeToString(EdgeType type) {
    switch (type) {
        case EDGE_RISING: return "R";
        case EDGE_FALLING: return "F";
        default: return "U"; // Unknown
    }
}

#endif // CONFIG_H