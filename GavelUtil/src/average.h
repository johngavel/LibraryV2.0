#ifndef __GAVEL_AVG_H
#define __GAVEL_AVG_H

class Average {
public:
  explicit Average(unsigned long windowSize = 1000) : avg(0) { setWindowSize(windowSize); }

  // Fast sample update: fixed-point multiply and accumulate
  inline void sample(unsigned long value) {
    // avg = alpha * value + (1 - alpha) * avg
    // Using Q15 fixed-point: alphaQ15 in [0, 32768]
    avg = ((alphaQ15 * (int32_t) value) + ((32768 - alphaQ15) * avg)) >> 15;
  }

  inline unsigned long getAverage() const { return (unsigned long) avg; }

  inline void setWindowSize(unsigned long windowSize) {
    if (windowSize == 0) windowSize = 1;
    // alpha = 2 / (windowSize + 1)
    float alpha = 2.0f / ((float) windowSize + 1.0f);
    alphaQ15 = (int32_t) (alpha * 32768.0f); // Convert to Q15
  }

  inline void reset() { avg = 0; }

private:
  int32_t avg;      // Q15 scaled average
  int32_t alphaQ15; // Q15 scaled alpha
};

#endif // __GAVEL_AVG_H