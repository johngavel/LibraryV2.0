#include "GavelServerStandard.h"
#include "ipinfofile.h"
#include "rebootfile.h"
#include "sseterminal.h"
#include "webpage/webpage_all.h"

void loadServerStandard(EthernetMemory* ethernet, ServerModule* server, FileSystem* fs) {
  ArrayDirectory* dir = static_cast<ArrayDirectory*>(fs->open("/"));
  dir->addDirectory("www");
  dir = static_cast<ArrayDirectory*>(fs->open("/www"));
  dir->addDirectory("api");
  dir->addDirectory("js");
  dir->addDirectory("style");
  dir->addFile(new StaticFile(errorhtml_string, errorhtml, errorhtml_len));
  dir->addFile(new StaticFile(serverhtml_string, serverhtml, serverhtml_len));
  dir->addFile(new StaticFile(terminalhtml_string, terminalhtml, terminalhtml_len));
  dir = static_cast<ArrayDirectory*>(fs->open("/www/js"));
  dir->addFile(new StaticFile(buildinfojs_string, buildinfojs, buildinfojs_len));
  dir->addFile(new StaticFile(consolejs_string, consolejs, consolejs_len));
  dir->addFile(new StaticFile(footerjs_string, footerjs, footerjs_len));
  dir->addFile(new StaticFile(headerjs_string, headerjs, headerjs_len));
  dir->addFile(new StaticFile(ipinfojs_string, ipinfojs, ipinfojs_len));
  dir->addFile(new StaticFile(rebootbuttonjs_string, rebootbuttonjs, rebootbuttonjs_len));
  dir->addFile(new StaticFile(redirectjs_string, redirectjs, redirectjs_len));
  dir = static_cast<ArrayDirectory*>(fs->open("/www/api"));
  dir->addFile(&programInfo);
  dir->addFile(new IpInfoFile(ethernet));
  dir->addFile(new RebootFile());
  dir->addFile(new SSECmd());
  dir->addFile(new SSEEvent());
  dir = static_cast<ArrayDirectory*>(fs->open("/www/style"));
  dir->addFile(new StaticFile(stylecss_string, stylecss, stylecss_len));

  server->setErrorPage(errorhtml_string);
}
