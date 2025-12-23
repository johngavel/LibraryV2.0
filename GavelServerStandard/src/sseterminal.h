#ifndef __GAVEL_SSE_TERM_H
#define __GAVEL_SSE_TERM_H

#include <GavelDebug.h>
#include <GavelFileSystem.h>
#include <GavelTask.h>
#include <GavelUtil.h>

class SSECmd : public DynamicFile {
public:
  SSECmd()
      : DynamicFile("sse_command.json", READ_WRITE, _fileBuffer, sizeof(_fileBuffer)),
        _stream(_buffer, sizeof(_buffer)){};
  bool createReadData() override { return true; };

  virtual bool parseWriteData() override {
    DEBUG((String("Received Command: ") + String(_fileBuffer)).c_str());

    TinyJsonParser::Events ev;
    ev.onKey = [](void*, const char* k) {
      Serial.print("key: ");
      Serial.println(k);
    };

    return true;
  };

  Stream* stream() { return &_stream; };

private:
  char _fileBuffer[200];
  unsigned char _buffer[200];
  CharRingBuffer _stream;
};

class SSEEvent : public StreamFile {
public:
  SSEEvent() : StreamFile("sse_events.stream", READ_ONLY), _stream(_buffer, sizeof(_buffer)){};
  bool createReadData() override {
    DEBUG("Open SSE Event");
    return true;
  };

  virtual bool parseWriteData() override { return true; };

  Stream* stream() { return &_stream; };

private:
  unsigned char _buffer[4096];
  CharRingBuffer _stream;
};

class SSETerminal : public Task {
public:
  SSETerminal() : Task("SSETerminal"), terminal(event.stream(), command.stream()){};
  void addCmd(TerminalCommand* __termCmd) override {};
  void reservePins(BackendPinSetup* pinsetup) override {};
  bool setupTask(OutputInterface* __terminal) override { return true; };
  bool executeTask() override { return true; };

private:
  Terminal terminal;
  SSEEvent event;
  SSECmd command;
};
#endif // __GAVEL_SSE_TERM_H
