#ifndef __GAVEL_ARRAY_FILE_SYSTEM_CLASS_H
#define __GAVEL_ARRAY_FILE_SYSTEM_CLASS_H

#include "arraydirectory.h"

#include <GavelInterfaces.h>
#include <GavelTask.h>

struct DirectoryStat {
  int directories = 0;
  int files = 0;
  int size = 0; // total bytes

  // In-place aggregation
  constexpr DirectoryStat& operator+=(const DirectoryStat& rhs) noexcept {
    directories += rhs.directories;
    files += rhs.files;
    size += rhs.size;
    return *this;
  }

  // Non-member + to allow implicit conversions on both sides
  [[nodiscard]] friend constexpr DirectoryStat operator+(DirectoryStat lhs, const DirectoryStat& rhs) noexcept {
    lhs += rhs; // reuse += logic
    return lhs;
  }
};

class FileSystem : public DigitalFileSystem, public Task {
public:
  FileSystem();
  virtual void addCmd(TerminalCommand* __termCmd) override;
  virtual void reservePins(BackendPinSetup* pinsetup) override {};
  virtual bool setupTask(OutputInterface* __terminal) override;
  virtual bool executeTask() override;

  virtual DigitalBase* open(const char* path, FileMode mode = READ_MODE) override;
  virtual bool format() override;
  virtual bool verifyFile(const char* path) override;
  virtual DigitalFile* readFile(const char* path) override;
  virtual DigitalFile* writeFile(const char* path) override;
  virtual bool deleteFile(const char* path) override;

private:
  DigitalBase* getFile(const char* path);
  ArrayDirectory* root;

  DirectoryStat printDirectory(OutputInterface* terminal, DigitalDirectory* dir, bool recursive);
  void directory(OutputInterface* terminal);
  void catFile(OutputInterface* terminal);
  void writeFile(OutputInterface* terminal);
  void changedir(OutputInterface* terminal);
};

#endif // __GAVEL_ARRAY_FILE_SYSTEM_CLASS_H
