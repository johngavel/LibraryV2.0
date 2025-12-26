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
    __termCmd->addCmd("cd", "<path>", "Only goes up and down, via .. or <name>",
                      [this](TerminalLibrary::OutputInterface* terminal) { changedir(terminal); });
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
  if (file->getPermission() == WRITE_ONLY) return nullptr;
  return static_cast<DigitalFile*>(file);
};

DigitalFile* FileSystem::writeFile(const char* path) {
  DigitalBase* file = open(path, WRITE_MODE);
  if (file == nullptr) return nullptr;
  if (file->isDirectory()) return nullptr;
  if (!file) return nullptr;
  if (file->getPermission() == READ_ONLY) return nullptr;
  return static_cast<DigitalFile*>(file);
};

bool FileSystem::deleteFile(const char* path) {
  return false;
};

DirectoryStat FileSystem::printDirectory(OutputInterface* terminal, DigitalDirectory* dir, bool recursive) {
  DirectoryStat total;
  StringBuilder sb;
  ClassicQueue queue(10, 32);
  if (dir == nullptr) return total;
  dir->rewindDirectory();
  sb + "Directory: " + dir->name();
  terminal->println(INFO, sb.c_str());
  sb.clear();
  sb + "<DIR>  " + ".";
  terminal->println(INFO, sb.c_str());
  sb.clear();
  sb + "<DIR>  " + "..";
  terminal->println(INFO, sb.c_str());
  sb.clear();
  DigitalBase* base = dir->openNextFile();
  while (base) {
    if (!base->isDirectory()) {
      DigitalFile* file = static_cast<DigitalFile*>(base);
      total.files++;
      total.size += file->size();
      char filedesc[100];
      strncpy(filedesc, file->name(), sizeof(filedesc));
      terminal->print(INFO, tab(22, filedesc, sizeof(filedesc)));
      numToA(file->size(), filedesc, sizeof(filedesc));
      terminal->print(INFO, tab(8, filedesc, sizeof(filedesc)));
      switch (file->getPermission()) {
      case READ_ONLY: terminal->println(INFO, "R"); break;
      case WRITE_ONLY: terminal->println(INFO, "W"); break;
      case READ_WRITE: terminal->println(INFO, "RW"); break;
      default: break;
      }
      file->close();
    }
    if (base->isDirectory()) {
      DigitalDirectory* directory = static_cast<DigitalDirectory*>(base);
      sb + "<DIR>  " + directory->name();
      terminal->println(INFO, sb.c_str());
      sb.clear();
      queue.push((void*) directory->name());
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

  if (recursive) {
    for (unsigned long i = 0; i < queue.count(); i++) {
      total += printDirectory(terminal, dir->getDirectory((char*) queue.get(i)), recursive);
    }
  }
  return total;
}

void FileSystem::directory(OutputInterface* terminal) {
  StringBuilder sb;
  char tmpBuf[80];
  DigitalDirectory* context = static_cast<DigitalDirectory*>(terminal->getContext(0));
  if (context == nullptr) {
    context = static_cast<DigitalDirectory*>(open("/"));
    terminal->setContext(0, (void*) context);
  }

  bool recursive = false;
  char* parameter = terminal->readParameter();
  if ((parameter != NULL) && (safeCompare(parameter, "-r") == 0)) recursive = true;

  DirectoryStat total = printDirectory(terminal, context, recursive);
  terminal->println();
  if (recursive) {
    terminal->println(PASSED, "Total: ");
    sb + "  " + total.directories + " Dir(s)";
    sb + "  " + total.files + " File(s)  ";
    strncpy(tmpBuf, sb.c_str(), sizeof(tmpBuf));
    terminal->print(INFO, tmpBuf);
    sb.clear();
    sb + total.size;
    terminal->println(INFO, sb.c_str());
    sb.clear();
    terminal->println();
  }
  terminal->prompt();
}

void FileSystem::catFile(OutputInterface* terminal) {
  char* value;
  DigitalDirectory* context = static_cast<DigitalDirectory*>(terminal->getContext(0));
  if (context == nullptr) {
    context = static_cast<DigitalDirectory*>(open("/"));
    terminal->setContext(0, (void*) context);
  }
  value = terminal->readParameter();
  if (!value) {
    terminal->invalidParameter();
    terminal->println();
    terminal->prompt();
    return;
  }

  DigitalFile* file = context->open(value);

  if (file && !file->isDirectory()) {
    // Prefer a larger buffer to minimize overhead
    char buf[512];

    while (file->available() > 0) {
      int n = file->read((unsigned char*) buf, sizeof(buf) - 1);
      if (n <= 0) break;
      buf[n] = 0;
      terminal->print(INFO, buf);
    }
    file->close();
  } else {
    terminal->println(ERROR, "\"" + String(value) + "\" File does not exist!!!");
  }

  terminal->println();
  terminal->prompt();
}

void FileSystem::changedir(OutputInterface* terminal) {
  // Get current context or initialize to root
  DigitalDirectory* current = static_cast<DigitalDirectory*>(terminal->getContext(0));
  if (current == nullptr) {
    current = static_cast<DigitalDirectory*>(open("/"));
    terminal->setContext(0, (void*) current);
  }

  // Read the requested path
  char* value = terminal->readParameter();
  if (!value || !*value) {
    terminal->println(ERROR, "No path provided.");
    terminal->prompt();
    return;
  }

  // Decide starting point: absolute -> root, relative -> current
  DigitalDirectory* start = (value[0] == '/') ? static_cast<DigitalDirectory*>(open("/")) : current;

  if (!start) {
    terminal->println(ERROR, "Root directory is unavailable.");
    terminal->prompt();
    return;
  }

  // Work copy we can tokenize safely (no heap fragmentation with String)
  // NOTE: If your environment lacks std::strncpy, ensure value is not huge.
  char pathBuf[256];
  strncpy(pathBuf, value, sizeof(pathBuf) - 1);
  pathBuf[sizeof(pathBuf) - 1] = '\0';

  // Tokenize on '/' (skip empty tokens caused by leading/trailing/multiple '/')
  DigitalDirectory* resolved = start;
  char* token = strtok(pathBuf, "/");

  while (token != nullptr) {
    // Handle current-directory and empty segments
    if (token[0] == '\0' || (token[0] == '.' && token[1] == '\0')) {
      // no-op
    }
    // Handle parent-directory ".."
    else if (token[0] == '.' && token[1] == '.' && token[2] == '\0') {
      DigitalDirectory* parent = resolved->getParent();
      if (parent != nullptr) {
        resolved = parent;
      } else {
        // already at root, stay there (common shell behavior)
        // Alternatively, treat as error:
        // terminal->println(ERROR, "Cannot go above root.");
        // return terminal->prompt();
      }
    }
    // Normal directory name
    else {
      DigitalDirectory* next = resolved->getDirectory(token);
      if (next == nullptr) {
        terminal->println(ERROR, "Directory does not exist: " + String(token));
        terminal->prompt();
        return; // abort without changing context
      }
      resolved = next;
    }

    token = strtok(nullptr, "/");
  }

  // Success: update context atomically
  terminal->setContext(0, (void*) resolved);
  terminal->prompt();
}
