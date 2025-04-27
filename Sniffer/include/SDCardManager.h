#ifndef SDCARD_MANAGER_H
#define SDCARD_MANAGER_H

#include "Config.h"
#include "RingBuffer.h"
#include <SD.h>

class SDCardManager {
public:
    /**
     * Constructor
     * @param buffer Reference to the ring buffer to read from
     */
    SDCardManager(RingBuffer& buffer);
    
    /**
     * Initialize the SD card
     * @return true if initialization was successful
     */
    bool begin();
    
    /**
     * Check if SD card is present and accessible
     * @return true if SD card is ready
     */
    bool isCardPresent();
    
    /**
     * Save buffered events to the SD card
     * @return true if operation was successful, false if card not ready or error occurred
     */
    bool saveEvents();
    
private:
    RingBuffer& eventBuffer;
    bool cardInitialized = false;
    
    /**
     * Check if the data file exists and create it with header if needed
     * @return true if file is ready for writing
     */
    bool prepareDataFile();
    
    /**
     * Convert an event entry to a CSV line
     * @param entry Event entry to convert
     * @param buffer Output buffer
     * @param bufferSize Size of output buffer
     */
    void eventToCSV(const EventEntry& entry, char* buffer, size_t bufferSize);
};

#endif // SDCARD_MANAGER_H