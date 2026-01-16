#ifndef __GAVEL_SSE_TERM_H
#define __GAVEL_SSE_TERM_H

#include <GavelFileSystem.h>
#include <GavelTask.h>
#include <GavelUtil.h>

class SSECmd : public DynamicFile {
public:
  SSECmd()
      : DynamicFile("terminal_command.json", READ_WRITE, _fileBuffer, sizeof(_fileBuffer)),
        _stream(_buffer, sizeof(_buffer)){};
  virtual bool createReadData() override { return true; };

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
  SSEEvent() : StreamFile("terminal_events.stream", READ_ONLY), _stream(_buffer, sizeof(_buffer)){};
  virtual bool createReadData() override {
    char charBuffer[256];
    unsigned int lengthBuffer = 0;
    memset(charBuffer, 0, sizeof(charBuffer));
    while (_stream.available()) {
      int byteVal = _stream.pop();
      if (byteVal < 0) break;
      unsigned char c = (unsigned char) byteVal;
      if (c == '\r') continue;
      if (c == '\n') {
        sseBroadcastDataLines(charBuffer, lengthBuffer);
        endSseEvent(); // close the event here
        lengthBuffer = 0;
      } else {
        if (lengthBuffer == sizeof(charBuffer)) {
          // Buffer full mid-line: emit more data lines but DO NOT end event
          sseBroadcastDataLines(charBuffer, lengthBuffer);
          lengthBuffer = 0;
        }
        charBuffer[lengthBuffer++] = static_cast<char>(c);
      }
    }
    // Final flush only if there is pending data
    if (lengthBuffer > 0) {
      sseBroadcastDataLines(charBuffer, lengthBuffer);
      endSseEvent();
    }
    return true;
  };

  virtual bool parseWriteData() override { return true; };

  Stream* stream() { return &_stream; };

  void sseBroadcastDataLines(const char* payload, unsigned int length) {
    size_t w = 0;
    // Write "data: " + payload chunk + "\n" WITHOUT closing the event
    // Choose a chunk size that fits comfortably (e.g., 200 bytes)
    w += write((const uint8_t*) "data: ", 6);     // "data: "
    w += write((const uint8_t*) payload, length); // payload bytes
    w += write((const uint8_t*) "\n", 1);         // end of data line
  };

  // Call this once when the logical message (line) is complete
  inline void endSseEvent() {
    write((const uint8_t*) "\n", 1); // blank line to end event
  }

  void sseBroadcastEvent(const char* eventName, const char* payload) {
    size_t w = 0;
    // Emits a named event with one data line
    w += write((const uint8_t*) "event: ", 7); // "event: "
    w += write((const uint8_t*) eventName, strlen(eventName));
    w += write((const uint8_t*) "\n", 1);                  // end of data line
    w += write((const uint8_t*) "data: ", 6);              // "data: "
    w += write((const uint8_t*) payload, strlen(payload)); // "data: "
    w += write((const uint8_t*) "\n", 1);                  // end of data line
    endSseEvent();
  }

private:
  unsigned char _buffer[8192];
  CharRingBuffer _stream;
};

class SSETerminal : public Task {
public:
  SSETerminal() : Task("SSETerminal"), terminal(command.stream(), event.stream()){};
  void configure(const char* __promptString, void (*function)(OutputInterface*)) {
    strncpy(promptString, __promptString, 20);
    bannerFunction = function;
  };
  virtual void addCmd(TerminalCommand* __termCmd) override {
    if (__termCmd) {
      __termCmd->addCmd("connect", "", "Prints welcome message for connecting clients",
                        [this](TerminalLibrary::OutputInterface* terminal) { connectedCmd(terminal); });
    }
  };
  virtual void reservePins(BackendPinSetup* pinsetup) override {};
  virtual bool setupTask(OutputInterface* __terminal) override {
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
  virtual bool executeTask() override {
    if (heartbeat.expired() && event.isOpen()) event.sseBroadcastEvent("heartbeat", "ping");

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
    terminal->setContext(0, nullptr);
    terminal->banner();
    terminal->prompt();
  }
};
#endif // __GAVEL_SSE_TERM_H
