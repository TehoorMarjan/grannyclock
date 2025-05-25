#include "SignalLogger.h"

// Initialize static instance pointer
SignalLogger* SignalLogger::instance = nullptr;

SignalLogger::SignalLogger(RingBuffer& buffer) : eventBuffer(buffer) {
    // Store instance pointer for ISR access
    instance = this;
}

void SignalLogger::begin() {
    // Set up pins for input
    pinMode(Pins::sigRF, INPUT_PULLUP);
    pinMode(Pins::sigMU, INPUT_PULLUP);
    pinMode(Pins::sigPON, INPUT_PULLUP);
    pinMode(Pins::sigBA, INPUT_PULLUP);
    
    // Initialize last pin states
    lastPinState[RF_SIGNAL] = digitalRead(Pins::sigRF);
    lastPinState[MU_SIGNAL] = digitalRead(Pins::sigMU);
    lastPinState[PON_SIGNAL] = digitalRead(Pins::sigPON);
    lastPinState[BA_SIGNAL] = digitalRead(Pins::sigBA);
    
    // Set up interrupts for all signals (always active)
    setupInterrupts();
}

void SignalLogger::setupInterrupts() {
    // Set up interrupts for all signals
    enableInterrupt(Pins::sigRF);
    enableInterrupt(Pins::sigMU);
    enableInterrupt(Pins::sigPON);
    enableInterrupt(Pins::sigBA);
}

void SignalLogger::enableInterrupt(uint8_t pin) {
    // Attach interrupt for both rising and falling edges
    attachInterrupt(digitalPinToInterrupt(pin), 
        pin == Pins::sigRF ? handleRF : 
        pin == Pins::sigMU ? handleMU :
        pin == Pins::sigPON ? handlePON : 
        handleBA, CHANGE);
}

void SignalLogger::disableInterrupt(uint8_t pin) {
    detachInterrupt(digitalPinToInterrupt(pin));
}

void IRAM_ATTR SignalLogger::handleRF() {
    if (instance) {
        instance->processInterrupt(RF_SIGNAL, digitalRead(Pins::sigRF) ? EDGE_RISING : EDGE_FALLING);
    }
}

void IRAM_ATTR SignalLogger::handleMU() {
    if (instance) {
        instance->processInterrupt(MU_SIGNAL, digitalRead(Pins::sigMU) ? EDGE_RISING : EDGE_FALLING);
    }
}

void IRAM_ATTR SignalLogger::handlePON() {
    if (instance) {
        instance->processInterrupt(PON_SIGNAL, digitalRead(Pins::sigPON) ? EDGE_RISING : EDGE_FALLING);
    }
}

void IRAM_ATTR SignalLogger::handleBA() {
    if (instance) {
        instance->processInterrupt(BA_SIGNAL, digitalRead(Pins::sigBA) ? EDGE_RISING : EDGE_FALLING);
    }
}

void SignalLogger::processInterrupt(SignalType signal, EdgeType edge) {
    // Get current timestamp
    uint32_t timestamp = millis();
    
    // Get the current digital state
    bool currentState = (edge == EDGE_RISING) ? HIGH : LOW;
    
    // Validate that this is actually a state change
    if (currentState == lastPinState[signal]) {
        // Current state is the same as last recorded state
        // This is likely a spurious interrupt, so ignore it
        return;
    }
    
    // Check for glitches (transitions too close together)
    if ((timestamp - lastInterruptTime[signal]) <= MIN_PULSE_WIDTH && lastInterruptTime[signal] > 0) {
        // This transition happened very quickly after the last one
        // Only filter if it's unreasonably fast (below system resolution)
        return;
    }
    
    // This appears to be a legitimate state change
    // Update last state and time
    lastPinState[signal] = currentState;
    lastInterruptTime[signal] = timestamp;
    
    // Write to buffer
    eventBuffer.write(signal, edge, timestamp);
}