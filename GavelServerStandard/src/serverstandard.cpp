#include "GavelServerStandard.h"
#include "debugAPI.h"
#include "rebootfile.h"
#include "register.h"
#include "serverconfig.h"
#include "sseterminal.h"

static void setupUpgrade(ArrayDirectory* dir) {
  LittleFS.format();
  LittleFS.begin();
  AliasDirectory* var = new AliasDirectory("var");
  var->setParent(dir);
  var->addAlias("pico.bin", "/pico.bin", READ_WRITE);
  dir->addDirectory(var);
}

static ServerConfig serverConfig;
static JsonFile serverFile(&serverConfig, "server-info.json", READ_WRITE);

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
  if (programMem) {
    dir->addFile(new JsonFile(programMem, "build-info.json", READ_ONLY));
    serverConfig.programInfo = true;
  }
  if (ethernet) {
    dir->addFile(new JsonFile(ethernet, "ip-info.json", READ_WRITE));
    serverConfig.ethernetInfo = true;
  }
  if (memory) {
    dir->addFile(new JsonFile(memory, "export.json", READ_WRITE, JsonFile::LARGE_BUFFER_SIZE));
    serverConfig.memoryInfo = true;
  }
  if (license) {
    dir->addFile(new JsonFile(license, "license-info.json", READ_ONLY, JsonFile::LARGE_BUFFER_SIZE));
    serverConfig.licenseInfo = true;
  }
  if (hwlist) {
    dir->addFile(new JsonFile(hwlist, "hw-info.json", READ_ONLY));
    serverConfig.hwInfo = true;
  }
  if (device) {
    dir->addFile(new RebootFile(device));
    serverConfig.rebootInfo = true;
    dir->addFile(new UpgradeFile(device));
    serverConfig.upgradeInfo = true;
    dir->addFile(new UploadFile(device));
    serverConfig.uploadInfo = true;
  }
  dir->addFile(new APIFile(new DebugAPI(), "debug", READ_WRITE));
  if (taskManager) {
    setupTerminalAPI(dir, taskManager);
    serverConfig.terminalInfo = true;
  }

  dir->addFile(&serverFile);
  server->setErrorPage(errorhtml_string);
}
