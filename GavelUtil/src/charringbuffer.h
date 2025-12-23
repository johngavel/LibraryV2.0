#ifndef __GAVEL_RING_BUFFER_H
#define __GAVEL_RING_BUFFER_H

#include <Arduino.h> // optional; remove if not on Arduino

class CharRingBuffer : public Stream {
public:
  CharRingBuffer(unsigned char* buf, unsigned int size) : buf_(buf), size_(size), head_(0), tail_(0), count_(0){};

  // number of characters currently stored
  inline virtual int available() override { return count_; };

  // free space remaining
  inline unsigned int space() const { return size_ - count_; };

  // capacity
  inline unsigned int capacity() const { return size_; };

  // true if empty
  inline bool empty() const { return count_ == 0; };

  // true if full
  inline bool full() const { return count_ == size_; };

  // write one character; returns true on success, false if full
  bool push(unsigned char c) {
    if (full()) return false;
    buf_[head_] = c;
    head_ = next(head_);
    ++count_;
    return true;
  };

  // read one character; returns -1 if empty, else 0..255
  int pop() {
    if (empty()) return -1;
    unsigned char c = buf_[tail_];
    tail_ = next(tail_);
    --count_;
    return c;
  };

  // look at next character without removing; returns -1 if empty
  virtual int peek() override {
    if (empty()) return -1;
    return buf_[tail_];
  };

  // write up to len bytes; returns number actually written
  int write(const unsigned char* src, int len) {
    if (!src || len == 0) return 0;
    int written = 0;
    while (written < len && !full()) {
      push(src[written]);
      ++written;
    }
    return written;
  };

  virtual size_t write(uint8_t ch) override {
    if (push(ch)) return 1;
    return 0;
  }

  // read up to len bytes; returns number actually read
  int read(unsigned char* dst, int len) {
    if (!dst || len == 0) return 0;
    int readCount = 0;
    while (readCount < len && !empty()) {
      int v = pop();
      dst[readCount++] = static_cast<unsigned char>(v);
    }
    return readCount;
  };

  virtual int read() override { return pop(); }

  // remove all data
  void clear() { head_ = tail_ = count_ = 0; };

private:
  unsigned char* buf_;
  unsigned int size_;
  unsigned int head_;  // next write position
  unsigned int tail_;  // next read position
  unsigned int count_; // number of bytes stored

  inline unsigned int next(unsigned int i) const {
    ++i;
    if (i == size_) i = 0;
    return i;
  };
};

#endif // __GAVEL_RING_BUFFER_H