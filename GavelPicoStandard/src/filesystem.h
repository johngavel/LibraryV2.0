#ifndef __GAVEL_FILE_SYSTEM_H
#define __GAVEL_FILE_SYSTEM_H

#include <GavelArrayFileSystem.h>
#include <GavelTask.h>

class FileSystem : public Task {
public:
  FileSystem();
  void addCmd(TerminalCommand* __termCmd) override;
  bool setupTask(OutputInterface* __terminal) override;
  bool executeTask() override;
  DigitalFileSystem* getFileSystem() { return &afs; };

private:
  ArrayFileSystem afs;
  void printDirectory(OutputInterface* terminal, const char* path);
  void directory(OutputInterface* terminal);
  void catFile(OutputInterface* terminal);
};

#endif //__GAVEL_FILE_SYSTEM_H
