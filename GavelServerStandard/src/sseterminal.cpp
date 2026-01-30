#include "sseterminal.h"

#include <GavelProgram.h>

void setupTerminalAPI(ArrayDirectory* dir, TaskManager* taskManager) {
  static SSETerminal terminal;
  StringBuilder sb = ProgramInfo::ShortName;
  sb + ":\\> ";
  terminal.configure(sb.c_str(), banner);

  dir->addFile(&terminal.command);
  dir->addFile(&terminal.event);
  taskManager->add(&terminal);
  DBG_REGISTER(&terminal.terminal);
}