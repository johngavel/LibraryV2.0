#include "analysis.h"

unsigned long determineFactor(unsigned long risingEdge) {
  //  return 100;
  if (risingEdge > 110000) return 10;
  if (risingEdge > 11000) return 500;
  if (risingEdge > 1100) return 1000;
  if (risingEdge > 110) return 50000;
  if (risingEdge > 11) return 100000;
  return 100000;
}

bool validChannel(CHANNEL chan) {
  return ((chan >= CHANNEL_1) && (chan < MAX_CHANNEL));
}

Analysis::Analysis() {
  setMaxVoltage(MAX_ANALOG);
  lastSample.timestamp = 0;
  for (unsigned long chan = CHANNEL_1; chan < MAX_CHANNEL; chan++) {
    lastSample.signal[chan] = UNKNOWN;
    timestampLastRisingSignal[chan] = 0;
    currentSignalState[chan] = LOW_SIGNAL;
  }
}

void Analysis::setMaxVoltage(unsigned long volt) {
  lock();
  maxAnalog = volt;
  highLimitAnalog = (maxAnalog * 75) / 100;
  lowLimitAnalog = (maxAnalog * 25) / 100;
  unlock();
}

void Analysis::sampleReceived(RawSample* sample) {
  unsigned long diffTime;
  lock();
  diffTime = sample->timestamp - lastSample.timestamp;
  if (diffTime > 1000) diffTime = 1000;
  averageSampleTime.sample(diffTime);
  lastSample.timestamp = sample->timestamp;
  for (int channel = CHANNEL_1; channel < MAX_CHANNEL; channel++) {
    SIGNAL signal = UNKNOWN;
    lastSample.raw[channel] = sample->raw[channel];
    if (sample->raw[channel] > highLimitAnalog) signal = HIGH_SIGNAL;
    if (sample->raw[channel] < lowLimitAnalog) signal = LOW_SIGNAL;
    lastSample.signal[channel] = signal;

    averageAnalog[channel].sample(sample->raw[channel]);

    if (signal == HIGH_SIGNAL) averagePeakAnalog[channel].sample(sample->raw[channel]);

    if ((currentSignalState[channel] == LOW_SIGNAL) && (signal == HIGH_SIGNAL)) { // Rising Edge
      lastSample.signal[channel] = RISING_EDGE;
      currentSignalState[channel] = HIGH_SIGNAL;
      diffTime = sample->timestamp - timestampLastRisingSignal[channel];
      averageRisingEdgeTime[channel].setWindowSize(determineFactor(diffTime));
      averageRisingEdgeTime[channel].sample(diffTime);
      timestampLastRisingSignal[channel] = sample->timestamp;
    }
    if ((currentSignalState[channel] == HIGH_SIGNAL) && (signal == LOW_SIGNAL)) { // Falling Edge
      lastSample.signal[channel] = FALLING_EDGE;
      currentSignalState[channel] = LOW_SIGNAL;
      diffTime = sample->timestamp - timestampLastRisingSignal[channel];
      averageHighSignalTime[channel].setWindowSize(determineFactor(averageRisingEdgeTime[channel].getAverage()));
      averageHighSignalTime[channel].sample(diffTime);
    }
  }
  unlock();
}

bool Analysis::getCurrentSample(ProcessedSample* sample) {
  bool returnBool = false;
  lock();
  if (sample != nullptr) {
    memcpy(sample, &lastSample, sizeof(ProcessedSample));
    returnBool = true;
  }
  unlock();
  return returnBool;
}

bool Analysis::getSamplesPerSecond(double* samplesPerSecond) {
  bool returnBool = false;
  lock();
  if ((samplesPerSecond != nullptr) && (averageSampleTime.getAverage() != 0.0)) {
    *samplesPerSecond = (1000000.0 / averageSampleTime.getAverage());
    returnBool = true;
  }
  unlock();
  return returnBool;
}

bool Analysis::getPeakAnalog(CHANNEL channel, double* volt) {
  bool returnBool = false;
  lock();
  if (validChannel(channel) && (volt != nullptr)) {
    *volt = averagePeakAnalog[channel].getAverage();
    returnBool = true;
  }
  unlock();
  return returnBool;
}

bool Analysis::getAvgAnalog(CHANNEL channel, double* volt) {
  bool returnBool = false;
  lock();
  if (validChannel(channel) && (volt != nullptr)) {
    *volt = averageAnalog[channel].getAverage();
    returnBool = true;
  }
  unlock();
  return returnBool;
}

bool Analysis::getHertz(CHANNEL channel, double* hertz) {
  bool returnBool = false;
  lock();
  if (validChannel(channel) && (hertz != nullptr) && (averageRisingEdgeTime[channel].getAverage() != 0.0)) {
    *hertz = (1000000.0 / averageRisingEdgeTime[channel].getAverage());
    returnBool = true;
  }
  unlock();
  return returnBool;
}

bool Analysis::getDutyCycle(CHANNEL channel, double* dutyCycle) {
  bool returnBool = false;
  lock();
  if (validChannel(channel) && (dutyCycle != nullptr) && (averageRisingEdgeTime[channel].getAverage() != 0.0)) {
    *dutyCycle = ((averageHighSignalTime[channel].getAverage() * 100) / averageRisingEdgeTime[channel].getAverage());
    returnBool = true;
  }
  unlock();
  return returnBool;
}
