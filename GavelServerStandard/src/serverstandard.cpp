#include "GavelServerStandard.h"
#include "buildinfo.h"
#include "errorhtml.h"
#include "footer.h"
#include "header.h"
#include "style.h"

void loadServerStandard(ServerModule* server, FileSystem* fs) {
  ArrayDirectory* dir = static_cast<ArrayDirectory*>(static_cast<ArrayFileSystem*>(fs->getFileSystem())->open("/"));
  dir->addDirectory("www");
  dir = static_cast<ArrayDirectory*>(static_cast<ArrayFileSystem*>(fs->getFileSystem())->open("/www"));
  dir->addDirectory("api");
  dir->addDirectory("js");
  dir->addDirectory("style");
  dir->addFile(errorhtml_string, errorhtml, errorhtml_len);
  dir = static_cast<ArrayDirectory*>(static_cast<ArrayFileSystem*>(fs->getFileSystem())->open("/www/js"));
  dir->addFile(buildinfojs_string, buildinfojs, buildinfojs_len);
  dir->addFile(footerjs_string, footerjs, footerjs_len);
  dir->addFile(headerjs_string, headerjs, headerjs_len);
  dir = static_cast<ArrayDirectory*>(static_cast<ArrayFileSystem*>(fs->getFileSystem())->open("/www/api"));
  dir->addFile(&programInfo);
  dir = static_cast<ArrayDirectory*>(static_cast<ArrayFileSystem*>(fs->getFileSystem())->open("/www/style"));
  dir->addFile(stylecss_string, stylecss, stylecss_len);

  server->setErrorPage(errorhtml_string);
}
