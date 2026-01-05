#include "GavelServerStandard.h"
#include "import.h"
#include "rebootfile.h"
#include "sseterminal.h"
#include "webpage/webpage_all.h"

#include <GavelEEProm.h>

static SSETerminal terminal;

void loadServerStandard(EthernetMemory* ethernet, ServerModule* server, FileSystem* fs, TaskManager* taskManager,
                        EEpromMemory* memory) {
  ArrayDirectory* dir = static_cast<ArrayDirectory*>(fs->open("/"));
  dir->addDirectory("www");
  dir = static_cast<ArrayDirectory*>(fs->open("/www"));
  dir->addDirectory("api");
  dir->addDirectory("js");
  dir->addDirectory("style");
  dir->addFile(new StaticFile(errorhtml_string, errorhtml, errorhtml_len));
  dir->addFile(new StaticFile(ipconfightml_string, ipconfightml, ipconfightml_len));
  dir->addFile(new StaticFile(serverhtml_string, serverhtml, serverhtml_len));
  dir->addFile(new StaticFile(terminalhtml_string, terminalhtml, terminalhtml_len));
  dir->addFile(new StaticFile(licensehtml_string, licensehtml, licensehtml_len));
  dir = static_cast<ArrayDirectory*>(fs->open("/www/js"));
  dir->addFile(new StaticFile(buildinfojs_string, buildinfojs, buildinfojs_len));
  dir->addFile(new StaticFile(terminaljs_string, terminaljs, terminaljs_len));
  dir->addFile(new StaticFile(footerjs_string, footerjs, footerjs_len));
  dir->addFile(new StaticFile(headerjs_string, headerjs, headerjs_len));
  dir->addFile(new StaticFile(ipconfigjs_string, ipconfigjs, ipconfigjs_len));
  dir->addFile(new StaticFile(ipinfojs_string, ipinfojs, ipinfojs_len));
  dir->addFile(new StaticFile(rebootbuttonjs_string, rebootbuttonjs, rebootbuttonjs_len));
  dir->addFile(new StaticFile(redirectjs_string, redirectjs, redirectjs_len));
  dir->addFile(new StaticFile(transferjs_string, transferjs, transferjs_len));
  dir->addFile(new StaticFile(librarytablejs_string, librarytablejs, librarytablejs_len));
  dir = static_cast<ArrayDirectory*>(fs->open("/www/api"));
  dir->addFile(new JsonFile(&programMem, "build-info.json", READ_ONLY));
  dir->addFile(new JsonFile(ethernet, "ip-info.json", READ_WRITE));
  dir->addFile(new JsonFile(new ImportFile(memory), "export.json", READ_WRITE));
  dir->addFile(new JsonFile(&license, "license-info.json", READ_ONLY));
  dir->addFile(new RebootFile());

  dir = static_cast<ArrayDirectory*>(fs->open("/www/style"));
  dir->addFile(new StaticFile(stylecss_string, stylecss, stylecss_len));

  server->setErrorPage(errorhtml_string);

  StringBuilder sb = ProgramInfo::ShortName;
  sb + ":\\> ";
  terminal.configure(sb.c_str(), banner);
  dir = static_cast<ArrayDirectory*>(fs->open("/www/api"));
  dir->addFile(&terminal.command);
  dir->addFile(&terminal.event);
  taskManager->add(&terminal);
}
