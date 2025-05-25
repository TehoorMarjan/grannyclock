#ifndef STATUS_INDICATOR_H
#define STATUS_INDICATOR_H

#include "Config.h"

class StatusIndicator {
public:
    /**
     * Initialize the status indicator
     */
    void begin();
    
    /**
     * Update the status indicator state
     * Should be called regularly in the main loop
     */
    void update();
    
    /**
     * Set the current status
     * @param status The status code to display
     */
    void setStatus(StatusCode status);
    
    /**
     * Get the current status
     * @return Current status code
     */
    StatusCode getStatus() const;

private:
    StatusCode currentStatus = STATUS_OK;
    unsigned long lastBlinkTime = 0;
    bool blinkState = false;
    
    /**
     * Set the RGB LED color
     * @param red Red intensity (0-255)
     * @param green Green intensity (0-255)
     * @param blue Blue intensity (0-255)
     */
    void setColor(uint8_t red, uint8_t green, uint8_t blue);
};

#endif // STATUS_INDICATOR_H