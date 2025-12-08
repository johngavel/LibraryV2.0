#ifndef __GAVEL_ANALYSIS_H
#define __GAVEL_ANALYSIS_H

#include "analogdata.h"

#include <GavelUtil.h>

class Analysis {
public:
  Analysis();
  void setMaxVoltage(unsigned long volt);
  void sampleReceived(RawSample* sample);
  bool getCurrentSample(ProcessedSample* sample);
  bool getSamplesPerSecond(double* samplesPerSecond);
  bool getPeakAnalog(CHANNEL channel, double* volt);
  bool getAvgAnalog(CHANNEL channel, double* volt);
  bool getHertz(CHANNEL channel, double* hertz);
  bool getDutyCycle(CHANNEL channel, double* dutyCycle);
  void lock() { semLock.take(); };
  void unlock() { semLock.give(); };

private:
  unsigned long maxAnalog;
  unsigned long highLimitAnalog;
  unsigned long lowLimitAnalog;
  Average averageSampleTime;
  Average averageRisingEdgeTime[MAX_CHANNEL];
  Average averageHighSignalTime[MAX_CHANNEL];
  ProcessedSample lastSample;
  unsigned long timestampLastRisingSignal[MAX_CHANNEL];
  SIGNAL currentSignalState[MAX_CHANNEL];
  Average averageAnalog[MAX_CHANNEL];
  Average averagePeakAnalog[MAX_CHANNEL];
  SemLock semLock;
};

#endif // __GAVEL_ANALYSIS_H
