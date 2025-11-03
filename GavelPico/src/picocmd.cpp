#include "picocmd.h"

void PicoCommand::uploadPico(OutputInterface* terminal) {
  if (terminal) terminal->println(WARNING, "Rebooting in USB Mode....");
  rebootCallBacks.trigger();
  delay(100);
  rp2040.rebootToBootloader();
}

void PicoCommand::rebootPico(OutputInterface* terminal) {
  if (terminal) terminal->println(WARNING, "Rebooting....");
  rebootCallBacks.trigger();
  delay(100);
  rp2040.reboot();
}
