#include "GavelServerStandard.h"
#include "debugAPI.h"
#include "rebootfile.h"
#include "register.h"
#include "sseterminal.h"

static void setupUpgrade(ArrayDirectory* dir) {
  LittleFS.format();
  LittleFS.begin();
  AliasDirectory* var = new AliasDirectory("var");
  var->setParent(dir);
  var->addAlias("pico.bin", "/pico.bin", READ_WRITE);
  dir->addDirectory(var);
}

void loadServerStandard(JsonInterface* programMem, JsonInterface* hwlist, JsonInterface* license,
                        JsonInterface* ethernet, ServerModule* server, FileSystem* fs, TaskManager* taskManager,
                        JsonInterface* memory, DeviceCmd* device) {
  ArrayDirectory* dir = static_cast<ArrayDirectory*>(fs->open("/"));
  dir->addDirectory("www");

  dir = static_cast<ArrayDirectory*>(fs->open("/www"));
  dir->addDirectory("api");
  dir->addDirectory("js");
  dir->addDirectory("style");
  setupUpgrade(dir);

  registerStaticHTMLFiles(static_cast<ArrayDirectory*>(fs->open("/www")));
  registerStaticJSFiles(static_cast<ArrayDirectory*>(fs->open("/www/js")));
  registerStaticCSSFiles(static_cast<ArrayDirectory*>(fs->open("/www/style")));

  dir = static_cast<ArrayDirectory*>(fs->open("/www/api"));
  if (programMem) dir->addFile(new JsonFile(programMem, "build-info.json", READ_ONLY));
  if (ethernet) dir->addFile(new JsonFile(ethernet, "ip-info.json", READ_WRITE));
  if (memory) dir->addFile(new JsonFile(memory, "export.json", READ_WRITE, JsonFile::LARGE_BUFFER_SIZE));
  if (license) dir->addFile(new JsonFile(license, "license-info.json", READ_ONLY, JsonFile::LARGE_BUFFER_SIZE));
  if (hwlist) dir->addFile(new JsonFile(hwlist, "hw-info.json", READ_ONLY));
  if (device) {
    dir->addFile(new RebootFile(device));
    dir->addFile(new UpgradeFile(device));
    dir->addFile(new UploadFile(device));
  }
  dir->addFile(new APIFile(new DebugAPI(), "debug", READ_WRITE));
  if (taskManager) setupTerminalAPI(dir, taskManager);

  server->setErrorPage(errorhtml_string);
}
