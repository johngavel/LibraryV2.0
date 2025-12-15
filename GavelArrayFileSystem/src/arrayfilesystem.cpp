#include "arrayfilesystem.h"

#include <GavelUtil.h>

ArrayFileSystem::ArrayFileSystem() {
  root = new ArrayDirectory("/");
};

DigitalBase* ArrayFileSystem::open(const char* path, FileMode mode) {
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

DigitalBase* ArrayFileSystem::getFile(const char* path) {
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

bool ArrayFileSystem::format() {
  return false;
};

bool ArrayFileSystem::verifyFile(const char* path) {
  DigitalBase* file = getFile(path);
  if (file == nullptr) return false;
  if (file->isDirectory()) return false;
  return true;
};

DigitalFile* ArrayFileSystem::readFile(const char* path) {
  DigitalBase* file = open(path);
  if (file == nullptr) return nullptr;
  if (file->isDirectory()) return nullptr;
  return static_cast<DigitalFile*>(file);
};

DigitalFile* ArrayFileSystem::writeFile(const char* path) {
  return nullptr;
};

bool ArrayFileSystem::deleteFile(const char* path) {
  return false;
};
