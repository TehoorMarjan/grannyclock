#ifndef SIGNAL_LOGGER_H
#define SIGNAL_LOGGER_H

#include "Config.h"
#include "RingBuffer.h"

class SignalLogger {
public:
    /**
     * Constructor
     * @param buffer Reference to the ring buffer
     */
    SignalLogger(RingBuffer& buffer);
    
    /**
     * Initialize the signal logger
     */
    void begin();
    
    /**
     * Update RF signal logging state based on timing
     * Call this regularly in the main loop
     */
    void updateRFLogging();
    
    /**
     * Check if RF logging is currently active
     * @return true if RF logging is active
     */
    bool isRFActive() const;
    
    /**
     * Get time until RF logging state changes (in ms)
     * @return time until next state change
     */
    unsigned long timeUntilRFStateChange() const;

private:
    RingBuffer& eventBuffer;
    bool rfActive = false;
    unsigned long rfStateChangeTime = 0;
    
    // Signal validation variables
    static const uint8_t NUM_SIGNALS = 4;
    unsigned long lastInterruptTime[NUM_SIGNALS] = {0, 0, 0, 0};
    uint8_t lastPinState[NUM_SIGNALS] = {HIGH, HIGH, HIGH, HIGH};
    static const unsigned long MIN_PULSE_WIDTH = 1; // 1ms minimum detection time (system resolution limit)
    
    // ISR handlers for each pin and edge
    static void IRAM_ATTR handleRF();
    static void IRAM_ATTR handleMU();
    static void IRAM_ATTR handleBR();
    static void IRAM_ATTR handleBA();
    
    // Enable or disable interrupts for a specific pin
    void enableInterrupt(uint8_t pin);
    void disableInterrupt(uint8_t pin);
    
    // Helper to process interrupt
    void processInterrupt(SignalType signal, EdgeType edge);
    
    // Pointer to the current instance (for ISR)
    static SignalLogger* instance;
};

#endif // SIGNAL_LOGGER_H