#include "filesystem.h"

#include <GavelUtil.h>

static char taskname[] = "FileSystem";

FileSystem::FileSystem() : Task(taskname){};

bool FileSystem::setupTask(OutputInterface* __terminal) {
  return true;
}

bool FileSystem::executeTask() {
  return true;
}

void FileSystem::addCmd(TerminalCommand* __termCmd) {
  if (__termCmd) {
    __termCmd->addCmd("dir", "<path><filename>", "Directory of list of the entire device",
                      [this](TerminalLibrary::OutputInterface* terminal) { directory(terminal); });
    __termCmd->addCmd("cat", "<path><filename>", "Displays the file", [this](TerminalLibrary::OutputInterface* terminal) { catFile(terminal); });
  }
}

void FileSystem::printDirectory(OutputInterface* terminal, const char* path) {
  int size = 0;
  int count = 0;
  StringBuilder sb;
  ClassicQueue queue(10, 32);
  sb + "Directory: " + path;
  terminal->println(INFO, sb.c_str());
  sb.clear();
  DigitalDirectory* dir = static_cast<DigitalDirectory*>(afs.open(path));
  DigitalBase* base = dir->openNextFile();
  while (base) {
    if (!base->isDirectory()) {
      DigitalFile* file = static_cast<DigitalFile*>(base);
      count++;
      size += file->size();
      char filename[100];
      strncpy(filename, file->name(), sizeof(filename));
      terminal->print(INFO, tab(22, filename, sizeof(filename)));
      terminal->println(INFO, String(file->size()));
      file->close();
    }
    if (base->isDirectory()) {
      DigitalDirectory* directory = static_cast<DigitalDirectory*>(base);
      sb + "<DIR>  " + directory->name();
      terminal->println(INFO, sb.c_str());
      sb.clear();

      sb + path + directory->name() + "/";
      queue.push((void*) sb.c_str());
      sb.clear();
    }
    base = dir->openNextFile();
  }
  dir->rewindDirectory();
  sb + "   " + count + " File(s)";
  char tmpBuf[80];
  strncpy(tmpBuf, sb.c_str(), sizeof(tmpBuf));
  tab(22, tmpBuf, sizeof(tmpBuf));
  terminal->print(INFO, tmpBuf);
  sb.clear();
  sb + size;
  terminal->println(INFO, sb.c_str());
  sb.clear();
  terminal->println();
  for (unsigned long i = 0; i < queue.count(); i++) printDirectory(terminal, (char*) queue.get(i));
}

void FileSystem::directory(OutputInterface* terminal) {
  printDirectory(terminal, "/");
  terminal->prompt();
}

void FileSystem::catFile(OutputInterface* terminal) {
  char* value;
  char buffer[2] = {0, 0};
  value = terminal->readParameter();
  if (value != NULL) {
    if (afs.verifyFile(value)) {
      DigitalFile* file = afs.readFile(value);
      if (file && !file->isDirectory()) {
        for (int i = 0; i < file->size(); i++) {
          buffer[0] = file->read();
          terminal->print(INFO, buffer);
          if (buffer[0] == '\n') terminal->print(INFO, "\r");
        }
      }
    } else {
      terminal->println(ERROR, "\"" + String(value) + "\" File does not exist!!!");
    }
  } else {
    terminal->invalidParameter();
  }
  terminal->println();
  terminal->prompt();
}
