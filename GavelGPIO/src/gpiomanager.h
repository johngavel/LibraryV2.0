#ifndef __GAVEL_GPIO_MANAGER_CLASS_H
#define __GAVEL_GPIO_MANAGER_CLASS_H

#include <GavelTask.h>
#include <GavelUtil.h>

#define MAX_PINS 64

typedef enum {
  GPIO_AVAILABLE,
  GPIO_INPUT,
  GPIO_OUTPUT,
  GPIO_LED,
  GPIO_BUTTON,
  GPIO_PULSE,
  GPIO_PWM,
  GPIO_TONE,
  GPIO_ADC,
  GPIO_RESERVED,
  GPIO_UNACCESSIBLE,
  GPIO_UNDEFINED,
  GPIO_MAX_TYPES
} GPIO_TYPE;

typedef enum { GPIO_INTERNAL, GPIO_EXTERNAL_EXPANDER_1, GPIO_EXTERNAL_EXPANDER_2, GPIO_UNKNOWN_LOCATION } GPIO_LOCATION;

class Pin {
public:
  int pinNumber = 0;
  GPIO_TYPE type = GPIO_AVAILABLE;
  GPIO_LOCATION location = GPIO_INTERNAL;
  char pinDescription[20];
  void configure(int __pinNumber, GPIO_TYPE __type, GPIO_LOCATION __location, const char* __pinDescription) {
    pinNumber = __pinNumber;
    type = __type;
    location = __location;
    strncpy(pinDescription, __pinDescription, sizeof(pinDescription));
  };
};

int pinCmp(const void*, const void*);

class PinList {
public:
  PinList() : list(MAX_PINS, sizeof(Pin)) { list.setSortFunction(pinCmp); };
  bool push(Pin* element) { return list.push((void*) element); };
  Pin* get(unsigned long index) { return (Pin*) list.get(index); };
  unsigned long count() { return list.count(); };
  bool sort() { return list.sort(); };
  bool error() { return list.error(); };

private:
  ClassicSortList list;
};

class HardwareDescription {
public:
  HardwareDescription(){};
  virtual PinList& getListofPins() = 0;
  const char* getName();

private:
  const char* name;
};

class GPIOManager : public Task {
public:
  GPIOManager();
  void addCmd(TerminalCommand* __termCmd) override;
  bool setupTask(OutputInterface* __terminal) override;
  bool executeTask() override;

  void addPins(PinList* __pinList);

private:
  PinList pinList;
};

#endif // __GAVEL_GPIO_MANAGER_CLASS_H