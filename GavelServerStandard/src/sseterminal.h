#ifndef __GAVEL_SSE_TERM_H
#define __GAVEL_SSE_TERM_H

#include <GavelDebug.h>
#include <GavelFileSystem.h>
#include <GavelTask.h>
#include <GavelUtil.h>

class SSECmd : public DynamicFile {
public:
  SSECmd() : DynamicFile("sse_command.json", READ_WRITE, _fileBuffer, sizeof(_fileBuffer)){};
  bool createReadData() override { return true; };

  virtual bool parseWriteData() override {
    DEBUG((String("Received Command: ") + String(_fileBuffer)).c_str());
    return true;
  };

private:
  char _fileBuffer[200];
};

class SSEEvent : public StreamFile {
public:
  SSEEvent() : StreamFile("sse_events.stream", READ_ONLY){};
  bool createReadData() override {
    DEBUG("Open SSE Event");
    return true;
  };

  virtual bool parseWriteData() override { return true; };

private:
};

class SSETerminal : public Task {
public:
  SSETerminal() : Task("SSETerminal"){};
  void addCmd(TerminalCommand* __termCmd) override {};
  void reservePins(BackendPinSetup* pinsetup) override {};
  bool setupTask(OutputInterface* __terminal) override { return true; };
  bool executeTask() override { return true; };

private:
};
#endif // __GAVEL_SSE_TERM_H
