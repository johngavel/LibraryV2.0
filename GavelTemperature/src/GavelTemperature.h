#ifndef __GAVEL_TEMPERATURE_H
#define __GAVEL_TEMPERATURE_H

#include <DHT.h>
#include <GavelInterfaces.h>
#include <GavelTask.h>

class Temperature : public Task, public IMemory, public Hardware {
public:
  typedef struct {
    long drift;
  } TemperatureData;

  static_assert(sizeof(TemperatureData) == 4, "ProgramMemory size unexpected - check packing/padding.");

  typedef union {
    TemperatureData data;
    unsigned char buffer[sizeof(TemperatureData)];
  } TemperatureUnion;
  TemperatureUnion memory;

  Temperature()
      : Task("Temperature"),
        IMemory("Temperature"),
        Hardware("Temperature"),
        temperature(0),
        validTemp(false),
        dht(nullptr),
        configured(false),
        pin(0) {
    memory.data.drift = 0;
  };
  // Virtual Task Methods
  virtual void addCmd(TerminalCommand* __termCmd) override;
  virtual void reservePins(BackendPinSetup* pinsetup) override;
  virtual bool setupTask(OutputInterface* __terminal) override;
  virtual bool executeTask() override;

  // Virtual IMemory Methods
  virtual const unsigned char& operator[](std::size_t index) const override { return memory.buffer[index]; };
  virtual unsigned char& operator[](std::size_t index) override { return memory.buffer[index]; };
  virtual std::size_t size() const noexcept override { return sizeof(TemperatureUnion); };
  virtual void initMemory() override { memory.data.drift = 0; };
  virtual void printData(OutputInterface* terminal) override {
    StringBuilder sb;
    sb = "Drift: ";
    sb + memory.data.drift;
    terminal->println(INFO, sb.c_str());
  }
  virtual void updateExternal() override { setInternal(true); };
  virtual JsonDocument createJson() override {
    JsonDocument doc;
    doc["tempdrift"] = memory.data.drift;
    doc["tempvalid"] = validTemperature();
    doc["temperature"] = getTemperature();
    return doc;
  };
  virtual bool parseJson(JsonDocument& doc) override {
    if (!doc["tempdrift"].isNull()) {
      setDrift(doc["tempdrift"]);
      return true;
    }
    return false;
  };

  void configure(int __pin);
  bool validTemperature();
  bool isConfigured() { return configured; };
  int getTemperature();
  virtual bool isWorking() const override { return validTemp; };
  void setDrift(long _drift) {
    memory.data.drift = _drift;
    setInternal(true);
  }

private:
  int readTemperature();
  int temperature;
  bool validTemp;
  DHT* dht;
  const unsigned long refreshRateValid = 60000;
  const unsigned long refreshRateInValid = 4000;
  bool configured;
  unsigned long pin;

  void temperatureStatus(OutputInterface* terminal);
  void driftCommand(OutputInterface* terminal);
};

#endif // __GAVEL_TEMPERATURE_H