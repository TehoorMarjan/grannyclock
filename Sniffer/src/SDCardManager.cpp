#include "SDCardManager.h"
#include <SPI.h>

SDCardManager::SDCardManager(RingBuffer& buffer) : eventBuffer(buffer) {
    // Nothing else to initialize
}

bool SDCardManager::begin() {
    // Configure SPI pins for SD card
    SPI.begin(Pins::SD_CLK, Pins::SD_MISO, Pins::SD_MOSI, Pins::SD_CS);
    
    // Initialize SD card
    cardInitialized = SD.begin(Pins::SD_CS);
    
    return cardInitialized;
}

bool SDCardManager::isCardPresent() {
    // Try to reinitialize if not already initialized
    if (!cardInitialized) {
        cardInitialized = SD.begin(Pins::SD_CS);
    }
    
    return cardInitialized;
}

bool SDCardManager::saveEvents() {
    // Check if SD card is available
    if (!isCardPresent()) {
        return false;
    }
    
    // Prepare the data file
    if (!prepareDataFile()) {
        return false;
    }
    
    // If buffer is empty, nothing to do
    if (eventBuffer.isEmpty()) {
        return true;
    }
    
    // Open the file for appending
    File dataFile = SD.open(BufferConfig::DATA_FILE_PATH, FILE_APPEND);
    if (!dataFile) {
        return false;
    }
    
    // Process events in batches to avoid large memory allocations
    constexpr size_t BATCH_SIZE = 64;
    EventEntry eventBatch[BATCH_SIZE];
    char csvLine[64]; // Buffer for CSV line (should be plenty for "XX,Y,4294967295\n")
    
    size_t eventsRead;
    bool success = true;
    
    do {
        // Read a batch of events from the buffer
        eventsRead = eventBuffer.read(eventBatch, BATCH_SIZE);
        
        // Process each event in the batch
        for (size_t i = 0; i < eventsRead; i++) {
            // Convert event to CSV
            eventToCSV(eventBatch[i], csvLine, sizeof(csvLine));
            
            // Write to file
            if (dataFile.println(csvLine) == 0) {
                success = false;
                break;
            }
        }
    } while (eventsRead > 0 && success);
    
    // Close the file
    dataFile.close();
    
    return success;
}

bool SDCardManager::prepareDataFile() {
    // Check if file exists
    if (SD.exists(BufferConfig::DATA_FILE_PATH)) {
        return true; // File exists, no header needed
    }
    
    // Create new file and write header
    File dataFile = SD.open(BufferConfig::DATA_FILE_PATH, FILE_WRITE);
    if (!dataFile) {
        return false;
    }
    
    // Write header
    bool success = (dataFile.println(BufferConfig::CSV_HEADER) > 0);
    
    // Close file
    dataFile.close();
    
    return success;
}

void SDCardManager::eventToCSV(const EventEntry& entry, char* buffer, size_t bufferSize) {
    // Format: "XX,Y,timestamp" (Signal, Edge, Timestamp)
    snprintf(buffer, bufferSize, "%s,%s,%lu", 
        signalTypeToString(static_cast<SignalType>(entry.signalType)),
        edgeTypeToString(static_cast<EdgeType>(entry.edgeType)),
        entry.timestamp);
}