#include <Arduino.h>
#include "Config.h"
#include "RingBuffer.h"
#include "SignalLogger.h"
#include "SDCardManager.h"
#include "StatusIndicator.h"

// Create the global objects
RingBuffer eventBuffer;
SignalLogger signalLogger(eventBuffer);
SDCardManager sdManager(eventBuffer);
StatusIndicator statusIndicator;

// Timing variables for periodic tasks
unsigned long lastSDWriteTime = 0;

void setup() {
  // Initialize serial for debugging (optional, can be removed for production)
  Serial.begin(115200);
  Serial.println("Clock Signal Sniffer Starting...");
  
  // Initialize status indicator first to show boot progress
  statusIndicator.begin();
  
  // Initialize SD card
  bool sdInitialized = sdManager.begin();
  if (!sdInitialized) {
    Serial.println("Warning: SD card initialization failed");
    statusIndicator.setStatus(STATUS_SD_MISSING);
  } else {
    Serial.println("SD card initialized");
  }
  
  // Initialize signal logging
  signalLogger.begin();
  Serial.println("Signal logger initialized");
  
  // Record startup time for SD write timing
  lastSDWriteTime = millis();
  
  Serial.println("Setup complete, monitoring signals...");
}

void loop() {
  // Update status indicator
  statusIndicator.update();
  
  // Update RF logging state based on timing
  signalLogger.updateRFLogging();
  
  // Only try to write to SD card if we're not in RF active mode and enough time has passed
  unsigned long currentTime = millis();
  if (!signalLogger.isRFActive() && 
      (currentTime - lastSDWriteTime >= Timing::SD_COMMIT_INTERVAL || 
       currentTime < lastSDWriteTime)) { // Handle millis overflow
    
    // Try to save events to SD card
    if (!eventBuffer.isEmpty()) {
      bool success = sdManager.saveEvents();
      
      if (success) {
        // All data saved
        statusIndicator.setStatus(STATUS_OK);
        Serial.println("Data saved to SD card");
      } else if (!sdManager.isCardPresent()) {
        // SD card not available
        statusIndicator.setStatus(STATUS_SD_MISSING);
        Serial.println("Failed to save: SD card not available");
      } else {
        // Other error
        Serial.println("Error saving to SD card");
      }
    }
    
    // Update last write time
    lastSDWriteTime = currentTime;
  }
  
  // Check if buffer is getting full (at 90% capacity)
  if (eventBuffer.getCount() >= (BufferConfig::BUFFER_SIZE * 0.9)) {
    // If SD card is present, try emergency write even if RF logging is active
    if (sdManager.isCardPresent()) {
      Serial.println("Emergency buffer flush");
      sdManager.saveEvents();
    } else {
      // If buffer is full and no SD card, we'll have to stop logging
      if (eventBuffer.getCount() >= BufferConfig::BUFFER_SIZE) {
        statusIndicator.setStatus(STATUS_BUFFER_OVERFLOW);
        Serial.println("ERROR: Buffer overflow, logging stopped");
      }
    }
  }
  
  // Short delay to prevent CPU from running at 100%
  delay(10);
}