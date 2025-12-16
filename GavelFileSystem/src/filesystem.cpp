#include "filesystem.h"

#include <GavelUtil.h>

FileSystem::FileSystem() : Task("FileSystem") {
  root = new ArrayDirectory("/");
};

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
    __termCmd->addCmd("cat", "<path><filename>", "Displays the file",
                      [this](TerminalLibrary::OutputInterface* terminal) { catFile(terminal); });
  }
}

DigitalBase* FileSystem::open(const char* path, FileMode mode) {
  DigitalBase* file = getFile(path);
  if (file && !file->isDirectory()) { file->open(mode); }
  return file;
}

// Returns next component in `out`. Collapses redundant slashes, no '.' or '..'.
// Advances `p` past the component and any trailing slashes.
// Returns false when no further components remain.
static bool nextComponent(const char*& p, char* out, size_t outsz) {
  // Skip any number of leading slashes
  while (*p == '/') ++p;
  if (*p == '\0') return false;

  size_t i = 0;
  while (*p && *p != '/') {
    if (i + 1 < outsz) out[i++] = *p; // leave room for '\0'
    ++p;
  }
  out[i] = '\0';

  // Skip any number of trailing slashes
  while (*p == '/') ++p;
  return true;
}

DigitalBase* FileSystem::getFile(const char* path) {
  if (!path || !*path) return nullptr;

  const char* p = path;
  while (*p == '/') p++;
  if (*p == '\0') return root;

  ArrayDirectory* curDir = root;
  DigitalBase* current = curDir;

  char component[100];

  while (true) {
    bool hasComponent = nextComponent(p, component, sizeof(component));
    if (!hasComponent) break;
    bool lastComponent = (*p == '\0');
    DigitalBase* child = curDir->getFile(component);
    if (!child) return nullptr;
    if (!lastComponent) {
      if (!child->isDirectory()) return nullptr;
      curDir = static_cast<ArrayDirectory*>(child);
      current = curDir;
    } else {
      current = child;
      break;
    }
  }

  return current;
};

bool FileSystem::format() {
  return false;
};

bool FileSystem::verifyFile(const char* path) {
  DigitalBase* file = getFile(path);
  if (file == nullptr) return false;
  if (file->isDirectory()) return false;
  return true;
};

DigitalFile* FileSystem::readFile(const char* path) {
  DigitalBase* file = open(path, READ_MODE);
  if (file == nullptr) return nullptr;
  if (file->isDirectory()) return nullptr;
  if (!file) return nullptr;
  if (file->getPermission() != READ_ONLY) return nullptr;
  return static_cast<DigitalFile*>(file);
};

DigitalFile* FileSystem::writeFile(const char* path) {
  DigitalBase* file = open(path, WRITE_MODE);
  if (file == nullptr) return nullptr;
  if (file->isDirectory()) return nullptr;
  if (!file) return nullptr;
  if (file->getPermission() != WRITE_ONLY) return nullptr;
  return static_cast<DigitalFile*>(file);
};

bool FileSystem::deleteFile(const char* path) {
  return false;
};

DirectoryStat FileSystem::printDirectory(OutputInterface* terminal, const char* path) {
  DirectoryStat total;
  StringBuilder sb;
  ClassicQueue queue(10, 32);
  sb + "Directory: " + path;
  terminal->println(INFO, sb.c_str());
  sb.clear();
  DigitalDirectory* dir = static_cast<DigitalDirectory*>(open(path));
  DigitalBase* base = dir->openNextFile();
  while (base) {
    if (!base->isDirectory()) {
      DigitalFile* file = static_cast<DigitalFile*>(base);
      total.files++;
      total.size += file->size();
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
      total.directories++;
    }
    base = dir->openNextFile();
  }
  dir->rewindDirectory();
  sb + "   " + total.files + " File(s)";
  char tmpBuf[80];
  strncpy(tmpBuf, sb.c_str(), sizeof(tmpBuf));
  tab(22, tmpBuf, sizeof(tmpBuf));
  terminal->print(INFO, tmpBuf);
  sb.clear();
  sb + total.size;
  terminal->println(INFO, sb.c_str());
  sb.clear();
  terminal->println();

  for (unsigned long i = 0; i < queue.count(); i++) total += printDirectory(terminal, (char*) queue.get(i));
  return total;
}

void FileSystem::directory(OutputInterface* terminal) {
  StringBuilder sb;
  char tmpBuf[80];
  DirectoryStat total = printDirectory(terminal, "/");
  terminal->println();
  terminal->println(PASSED, "Total: ");
  sb + "   " + total.directories + " Dir(s)";
  sb + "   " + total.files + " File(s)";
  strncpy(tmpBuf, sb.c_str(), sizeof(tmpBuf));
  tab(22, tmpBuf, sizeof(tmpBuf));
  terminal->print(INFO, tmpBuf);
  sb.clear();
  sb + total.size;
  terminal->println(INFO, sb.c_str());
  sb.clear();
  terminal->println();
  terminal->prompt();
}

void FileSystem::catFile(OutputInterface* terminal) {
  char* value;
  char buffer[2] = {0, 0};
  value = terminal->readParameter();
  if (value != NULL) {
    if (verifyFile(value)) {
      DigitalFile* file = readFile(value);
      if (file && !file->isDirectory()) {
        for (int i = 0; i < file->size(); i++) {
          buffer[0] = file->read();
          terminal->print(INFO, buffer);
          if (buffer[0] == '\n') terminal->print(INFO, "\r");
        }
        file->close();
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
