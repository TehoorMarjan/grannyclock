#include "RingBuffer.h"

RingBuffer::RingBuffer() : writeIndex(0), readIndex(0), count(0) {
    // Nothing else to initialize
}

bool RingBuffer::write(SignalType signal, EdgeType edge, uint32_t timestamp) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Check if buffer is full
    if (count >= BufferConfig::BUFFER_SIZE) {
        return false;  // Buffer is full, reject write
    }
    
    // Write the event
    buffer[writeIndex].signalType = signal;
    buffer[writeIndex].edgeType = edge;
    buffer[writeIndex].reserved = 0; // Clear reserved field
    buffer[writeIndex].timestamp = timestamp;
    
    // Advance write index
    writeIndex = (writeIndex + 1) % BufferConfig::BUFFER_SIZE;
    count++;
    
    return true;
}

size_t RingBuffer::read(EventEntry* dest, size_t maxEvents) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Limit to actual available events
    size_t eventsToRead = std::min(maxEvents, count);
    
    // Read events
    for (size_t i = 0; i < eventsToRead; i++) {
        dest[i] = buffer[readIndex];
        readIndex = (readIndex + 1) % BufferConfig::BUFFER_SIZE;
    }
    
    count -= eventsToRead;
    return eventsToRead;
}

bool RingBuffer::isEmpty() const {
    std::lock_guard<std::mutex> lock(mutex);
    return count == 0;
}

size_t RingBuffer::getCount() const {
    std::lock_guard<std::mutex> lock(mutex);
    return count;
}

void RingBuffer::reset() {
    std::lock_guard<std::mutex> lock(mutex);
    writeIndex = 0;
    readIndex = 0;
    count = 0;
}