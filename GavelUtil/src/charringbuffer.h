
#ifndef __GAVEL_RING_BUFFER_H
#define __GAVEL_RING_BUFFER_H

#include <Arduino.h> // optional; remove if not on Arduino

template <size_t N> class CharRingBuffer {
public:
  CharRingBuffer() : head_(0), tail_(0), count_(0){};

  // number of characters currently stored
  inline size_t available() const { return count_; };

  // free space remaining
  inline size_t space() const { return N - count_; };

  // capacity
  inline size_t capacity() const { return N; };

  // true if empty
  inline bool empty() const { return count_ == 0; };

  // true if full
  inline bool full() const { return count_ == N; };

  // write one character; returns true on success, false if full
  bool push(uint8_t c) {
    if (full()) return false;
    buf_[head_] = c;
    head_ = next(head_);
    ++count_;
    return true;
  };

  // read one character; returns -1 if empty, else 0..255
  int pop() {
    if (empty()) return -1;
    uint8_t c = buf_[tail_];
    tail_ = next(tail_);
    --count_;
    return c;
  };

  // look at next character without removing; returns -1 if empty
  int peek() const {
    if (empty()) return -1;
    return buf_[tail_];
  };

  // write up to len bytes; returns number actually written
  size_t write(const uint8_t* src, size_t len) {
    if (!src || len == 0) return 0;
    size_t written = 0;
    while (written < len && !full()) {
      push(src[written]);
      ++written;
    }
    return written;
  };

  // read up to len bytes; returns number actually read
  size_t read(uint8_t* dst, size_t len) {
    if (!dst || len == 0) return 0;
    size_t readCount = 0;
    while (readCount < len && !empty()) {
      int v = pop();
      dst[readCount++] = static_cast<uint8_t>(v);
    }
    return readCount;
  };

  // remove all data
  void clear() { head_ = tail_ = count_ = 0; };

private:
  uint8_t buf_[N];
  size_t head_;  // next write position
  size_t tail_;  // next read position
  size_t count_; // number of bytes stored

  inline size_t next(size_t i) const {
    ++i;
    if (i == N) i = 0;
    return i;
  };
};

#endif // __GAVEL_RING_BUFFER_H