#include "GavelServerStandard.h"
#include "debugAPI.h"
#include "hardwarefile.h"
#include "import.h"
#include "rebootfile.h"
#include "register.h"
#include "sseterminal.h"

#include <GavelEEProm.h>

static SSETerminal terminal;

static void setupUpload(ArrayDirectory* dir) {
  LittleFS.format();
  LittleFS.begin();
  AliasDirectory* var = new AliasDirectory("var");
  var->setParent(dir);
  var->addAlias("pico.bin", "/pico.bin", READ_WRITE);
  dir->addDirectory(var);
}

static void setupTerminalAPI(ArrayDirectory* dir, TaskManager* taskManager) {
  StringBuilder sb = ProgramInfo::ShortName;
  sb + ":\\> ";
  terminal.configure(sb.c_str(), banner);

  dir->addFile(&terminal.command);
  dir->addFile(&terminal.event);
  taskManager->add(&terminal);
  DBG_REGISTER(&terminal.terminal);
}

void loadServerStandard(EthernetMemory* ethernet, ServerModule* server, FileSystem* fs, TaskManager* taskManager,
                        EEpromMemory* memory) {
  ArrayDirectory* dir = static_cast<ArrayDirectory*>(fs->open("/"));
  dir->addDirectory("www");

  dir = static_cast<ArrayDirectory*>(fs->open("/www"));
  dir->addDirectory("api");
  dir->addDirectory("js");
  dir->addDirectory("style");
  setupUpload(dir);

  registerStaticHTMLFiles(static_cast<ArrayDirectory*>(fs->open("/www")));
  registerStaticJSFiles(static_cast<ArrayDirectory*>(fs->open("/www/js")));
  registerStaticCSSFiles(static_cast<ArrayDirectory*>(fs->open("/www/style")));

  dir = static_cast<ArrayDirectory*>(fs->open("/www/api"));
  dir->addFile(new JsonFile(&programMem, "build-info.json", READ_ONLY));
  dir->addFile(new JsonFile(ethernet, "ip-info.json", READ_WRITE));
  dir->addFile(new JsonFile(new ImportFile(memory), "export.json", READ_WRITE, JsonFile::LARGE_BUFFER_SIZE));
  dir->addFile(new JsonFile(&license, "license-info.json", READ_ONLY, JsonFile::LARGE_BUFFER_SIZE));
  dir->addFile(new RebootFile());
  dir->addFile(new UpgradeFile());
  dir->addFile(new JsonFile(new HardwareFile(&hardwareList), "hw-info.json", READ_ONLY));
  dir->addFile(new APIFile(new DebugAPI(), "debug", READ_WRITE));
  setupTerminalAPI(dir, taskManager);

  server->setErrorPage(errorhtml_string);
}
