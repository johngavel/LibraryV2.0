#include "watchdog.h"

#include <Arduino.h>
#include <GavelUtil.h>

static char taskName[] = "Watchdog";

Watchdog::Watchdog(unsigned long timeout, unsigned long petCycle) : Task(taskName), watchdogTimeout(timeout), watchdogPetCycle(petCycle) {
  monitorCore[0] = true;
  monitorCore[1] = true;
};

static char resetReasonText[][24] = {"Unknown", "Power On / Brownout", "Run pin", "Software", "Watchdog Timer", "Debug reset"};
static void printResetReason(OutputInterface* __terminal) {
  unsigned char reason = rp2040.getResetReason();
  __terminal->println(PASSED, "Reset Reason: ", resetReasonText[reason]);
}

bool Watchdog::setupTask(OutputInterface* __terminal) {
  terminal = __terminal;
  printResetReason(terminal);
  if (monitorCore[0] || monitorCore[1]) {
    rp2040.wdt_begin(watchdogTimeout);
    setRefreshMilli(watchdogPetCycle);
    rp2040.wdt_reset();
  } else {
    runTask(false);
    terminal->println(WARNING, "Watchdog Pet in Disabled.");
  }
  if (monitorCore[0]) setCore(0);
  if (monitorCore[1]) setCore(1);
  return true;
}

bool Watchdog::executeTask() {
  int currentCore = rp2040.cpuid();
  int nextCore = currentCore;

  if (currentCore != getCore() || !monitorCore[currentCore]) {
    setRefreshMilli(0);
    return true;
  }
  setRefreshMilli(watchdogPetCycle);
  if (monitorCore[currentCore]) rp2040.wdt_reset();

  if ((currentCore == 0) && (monitorCore[1] == true))
    nextCore = 1;
  else if ((currentCore == 1) && (monitorCore[0] == true))
    nextCore = 0;

  setCore(nextCore);
  return true;
}

void Watchdog::monitor(unsigned long core, bool monitor) {
  monitorCore[core] = monitor;
}
