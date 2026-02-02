#ifndef __GAVEL_SERVER_REGISTER_FILES_H
#define __GAVEL_SERVER_REGISTER_FILES_H

#include "GavelServerStandard.h"
#include "webpage/webpage_all.h"

void registerStaticHTMLFiles(ArrayDirectory* dir) {
  dir->addFile(new StaticFile(errorhtml_string, errorhtml, errorhtml_len));
  dir->addFile(new StaticFile(ipconfightml_string, ipconfightml, ipconfightml_len));
  dir->addFile(new StaticFile(licensehtml_string, licensehtml, licensehtml_len));
  dir->addFile(new StaticFile(serverhtml_string, serverhtml, serverhtml_len));
  dir->addFile(new StaticFile(templatehtml_string, templatehtml, templatehtml_len));
  dir->addFile(new StaticFile(terminalhtml_string, terminalhtml, terminalhtml_len));
}

void registerStaticJSFiles(ArrayDirectory* dir) {
  dir->addFile(new StaticFile(buildinfojs_string, buildinfojs, buildinfojs_len));
  dir->addFile(new StaticFile(buttonsjs_string, buttonsjs, buttonsjs_len));
  dir->addFile(new StaticFile(datatablejs_string, datatablejs, datatablejs_len));
  dir->addFile(new StaticFile(filebuttonjs_string, filebuttonjs, filebuttonjs_len));
  dir->addFile(new StaticFile(hwtablejs_string, hwtablejs, hwtablejs_len));
  dir->addFile(new StaticFile(ipconfigjs_string, ipconfigjs, ipconfigjs_len));
  dir->addFile(new StaticFile(ipinfojs_string, ipinfojs, ipinfojs_len));
  dir->addFile(new StaticFile(librarytablejs_string, librarytablejs, librarytablejs_len));
  dir->addFile(new StaticFile(mainjs_string, mainjs, mainjs_len));
  dir->addFile(new StaticFile(redirectjs_string, redirectjs, redirectjs_len));
  dir->addFile(new StaticFile(serverinfojs_string, serverinfojs, serverinfojs_len));
  dir->addFile(new StaticFile(temperaturejs_string, temperaturejs, temperaturejs_len));
  dir->addFile(new StaticFile(terminaljs_string, terminaljs, terminaljs_len));
  dir->addFile(new StaticFile(uijs_string, uijs, uijs_len));
  dir->addFile(new StaticFile(utiljs_string, utiljs, utiljs_len));
}

void registerStaticCSSFiles(ArrayDirectory* dir) {
  dir->addFile(new StaticFile(stylecss_string, stylecss, stylecss_len));
}

#endif // __GAVEL_SERVER_REGISTER_FILES_H
