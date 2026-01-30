#include "picocmd.h"

#include <GavelFileSystem.h>
#include <PicoOTA.h>

void PicoCommand::upload(OutputInterface* terminal) {
  if (terminal) terminal->println(WARNING, "Rebooting in USB Mode....");
  rebootCallBacks.trigger();
  delay(100);
  rp2040.rebootToBootloader();
}

void PicoCommand::reboot(OutputInterface* terminal) {
  if (terminal) terminal->println(WARNING, "Rebooting....");
  rebootCallBacks.trigger();
  delay(100);
  rp2040.reboot();
}

void PicoCommand::upgrade(OutputInterface* terminal) {
  if (terminal) terminal->println(WARNING, "Upgrade....");
  picoOTA.begin();
  picoOTA.addFile("pico.bin");
  picoOTA.commit();
  LittleFS.end();
  reboot(terminal);
}
