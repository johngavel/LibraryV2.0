#include "GavelTemperature.h"

bool Temperature::validTemperature() {
  return validTemp;
}
void Temperature::configure(int __pin) {
  pin = __pin;
  configured = true;
}

void Temperature::addCmd(TerminalCommand* __termCmd) {
  __termCmd->addCmd("temperature", "", "Temperature Status",
                    [this](TerminalLibrary::OutputInterface* terminal) { Temperature::temperatureStatus(terminal); });
}

void Temperature::reservePins(BackendPinSetup* pinsetup) {
  if (pinsetup != nullptr) { pinsetup->addReservePin(GPIO_DEVICE_CPU_BOARD, pin, "DHT Pin"); }
}

bool Temperature::setupTask(OutputInterface* __terminal) {
  if (configured) {
    dht = new DHT(pin, DHT11);
    setRefreshMilli(refreshRateInValid);
    dht->begin();
    readTemperature();
    runTimer(true);
  } else {
    __terminal->println(ERROR, "Temperature Sensor Unconfigured");
    runTimer(false);
    return false;
  }
  return true;
}

bool Temperature::executeTask() {
  readTemperature();
  if (validTemp) {
    setRefreshMilli(refreshRateValid);
  } else {
    setRefreshMilli(refreshRateInValid);
  }
  return true;
}

int Temperature::readTemperature() {
  if (dht != nullptr) {
    int value;
    value = ((int) dht->readTemperature(true) + memory.data.drift);
    if ((value > 0) && (value < 150)) {
      validTemp = true;
      temperature = value;
    }
  }
  return temperature;
}

int Temperature::getTemperature() {
  return temperature;
}

void Temperature::temperatureStatus(OutputInterface* terminal) {
  terminal->print(INFO, "Temperature Sensor is ");
  (isConfigured()) ? terminal->println(INFO, "Configured ") : terminal->println(WARNING, "Unconfigured ");
  terminal->print(INFO, "Temperature Data is ");
  (validTemperature()) ? terminal->println(INFO, "Valid ") : terminal->println(WARNING, "Invalid ");
  terminal->print(INFO, "Temperature: ");
  terminal->print(INFO, String(getTemperature()));
  terminal->println(INFO, "°F.");
  terminal->prompt();
}
