#ifndef __GAVEL_SSE_TERM_H
#define __GAVEL_SSE_TERM_H

#include <GavelDebug.h>
#include <GavelFileSystem.h>
#include <GavelUtil.h>

class SSECmd : public DynamicFile {
public:
  SSECmd() : DynamicFile("sse_command", READ_WRITE, _fileBuffer, sizeof(_fileBuffer)){};
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
  SSEEvent() : StreamFile("sse_events", READ_ONLY){};
  bool createReadData() override {
    DEBUG("Open SSE Event");
    return true;
  };

  virtual bool parseWriteData() override { return true; };

private:
};
#endif // __GAVEL_SSE_TERM_H
