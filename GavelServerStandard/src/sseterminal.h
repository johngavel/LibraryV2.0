#ifndef __GAVEL_SSE_TERM_H
#define __GAVEL_SSE_TERM_H

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
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, _fileBuffer);

    if (error) return false;

    const char* cmd = doc["command"];
    _stream.println(cmd);

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
    char charBuffer[256];
    unsigned int lengthBuffer = 0;
    memset(charBuffer, 0, sizeof(charBuffer));
    if (_stream.available()) {
      while (_stream.available()) {
        unsigned char c = (unsigned char) _stream.pop();
        if (c == '\n') {
          charBuffer[lengthBuffer++] = '\\';
          charBuffer[lengthBuffer++] = 'n';
          sseBroadcastData(charBuffer);
          memset(charBuffer, 0, sizeof(charBuffer));
          lengthBuffer = 0;
        } else {
          charBuffer[lengthBuffer++] = c;
        }
        if (lengthBuffer >= sizeof(charBuffer)) {
          sseBroadcastData(charBuffer);
          memset(charBuffer, 0, sizeof(charBuffer));
          lengthBuffer = 0;
        }
      }
      sseBroadcastData(charBuffer);
    }
    return true;
  };

  virtual bool parseWriteData() override { return true; };

  Stream* stream() { return &_stream; };

  void sseBroadcastData(const String& payload) {
    // Emits a named event with one data line
    String line = String("data: ") + String(payload) + String("\n\n");
    print(line.c_str());
  }
  void sseBroadcastEvent(const char* eventName, const String& payload) {
    // Emits a named event with one data line
    String line = String("event: ") + String(eventName) + String("\ndata: ") + String(payload) + String("\n\n");
    print(line.c_str());
  }

private:
  unsigned char _buffer[4096];
  CharRingBuffer _stream;
};

class SSETerminal : public Task {
public:
  SSETerminal() : Task("SSETerminal"), terminal(command.stream(), event.stream()){};
  void configure(const char* __promptString, void (*function)(OutputInterface*)) {
    strncpy(promptString, __promptString, 20);
    bannerFunction = function;
  };
  void addCmd(TerminalCommand* __termCmd) override {
    if (__termCmd) {
      __termCmd->addCmd("connect", "", "Prints welcome message for connecting clients",
                        [this](TerminalLibrary::OutputInterface* terminal) { connectedCmd(terminal); });
    }
  };
  void reservePins(BackendPinSetup* pinsetup) override {};
  bool setupTask(OutputInterface* __terminal) override {
    setRefreshMilli(10);
    terminal.setup();
    terminal.setEcho(false);
    terminal.setColor(true);
    if (bannerFunction) terminal.setBannerFunction(bannerFunction);
    terminal.setPromptString(promptString);
    terminal.setTerminalName("SSE Terminal");
    heartbeat.setRefreshSeconds(5);
    return true;
  };
  bool executeTask() override {
    if (heartbeat.expired()) event.sseBroadcastEvent("heartbeat", "ping");

    terminal.loop();
    event.createReadData();
    return true;
  };
  SSEEvent event;
  SSECmd command;
  Terminal terminal;

private:
  Timer heartbeat;
  void (*bannerFunction)(OutputInterface*) = nullptr;
  char promptString[20];
  void connectedCmd(OutputInterface* terminal) {
    terminal->banner();
    terminal->prompt();
  }
};
#endif // __GAVEL_SSE_TERM_H
