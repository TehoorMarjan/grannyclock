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
    pinMode(Pins::sigBR, INPUT_PULLUP);
    pinMode(Pins::sigBA, INPUT_PULLUP);
    
    // Initialize last pin states
    lastPinState[RF_SIGNAL] = digitalRead(Pins::sigRF);
    lastPinState[MU_SIGNAL] = digitalRead(Pins::sigMU);
    lastPinState[BR_SIGNAL] = digitalRead(Pins::sigBR);
    lastPinState[BA_SIGNAL] = digitalRead(Pins::sigBA);
    
    // Set up ISR for MU, BR, BA (always active)
    enableInterrupt(Pins::sigMU);
    enableInterrupt(Pins::sigBR);
    enableInterrupt(Pins::sigBA);
    
    // RF starts in inactive state, will be enabled by updateRFLogging
    disableInterrupt(Pins::sigRF);
    
    // Initialize timing for RF cycle
    rfActive = false;
    rfStateChangeTime = millis() + Timing::RF_INACTIVE_PERIOD;
}

void SignalLogger::updateRFLogging() {
    unsigned long currentTime = millis();
    
    // Check if it's time to change RF logging state
    if (currentTime >= rfStateChangeTime) {
        if (rfActive) {
            // Active -> Inactive
            disableInterrupt(Pins::sigRF);
            rfActive = false;
            rfStateChangeTime = currentTime + Timing::RF_INACTIVE_PERIOD;
        } else {
            // Inactive -> Active
            enableInterrupt(Pins::sigRF);
            rfActive = true;
            rfStateChangeTime = currentTime + Timing::RF_ACTIVE_PERIOD;
        }
    }
}

bool SignalLogger::isRFActive() const {
    return rfActive;
}

unsigned long SignalLogger::timeUntilRFStateChange() const {
    unsigned long currentTime = millis();
    if (currentTime >= rfStateChangeTime) {
        return 0;
    }
    return rfStateChangeTime - currentTime;
}

void SignalLogger::enableInterrupt(uint8_t pin) {
    // Attach interrupt for both rising and falling edges
    attachInterrupt(digitalPinToInterrupt(pin), 
        pin == Pins::sigRF ? handleRF : 
        pin == Pins::sigMU ? handleMU :
        pin == Pins::sigBR ? handleBR : 
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

void IRAM_ATTR SignalLogger::handleBR() {
    if (instance) {
        instance->processInterrupt(BR_SIGNAL, digitalRead(Pins::sigBR) ? EDGE_RISING : EDGE_FALLING);
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