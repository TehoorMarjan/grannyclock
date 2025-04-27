#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include "Config.h"
#include <mutex>

class RingBuffer {
public:
    /**
     * Default constructor initializes an empty buffer
     */
    RingBuffer();

    /**
     * Try to write an event to the buffer
     * @param signal The signal type that triggered the event
     * @param edge The edge type (rising or falling)
     * @param timestamp The timestamp when the event occurred
     * @return true if write was successful, false if buffer was full
     */
    bool write(SignalType signal, EdgeType edge, uint32_t timestamp);

    /**
     * Read events from the buffer and transfer them to a destination array
     * @param dest Destination array where to copy events
     * @param maxEvents Maximum number of events to read
     * @return Number of events actually read
     */
    size_t read(EventEntry* dest, size_t maxEvents);

    /**
     * Check if the buffer is empty
     * @return true if the buffer is empty, false otherwise
     */
    bool isEmpty() const;

    /**
     * Get the number of events currently in the buffer
     * @return Number of events in buffer
     */
    size_t getCount() const;

    /**
     * Reset the buffer to empty state
     */
    void reset();

private:
    EventEntry buffer[BufferConfig::BUFFER_SIZE];
    size_t writeIndex;  // Where to write next
    size_t readIndex;   // Where to read next
    size_t count;       // Number of items in buffer
    mutable std::mutex mutex; // Mutex for thread safety
};

#endif // RINGBUFFER_H