#ifndef __GAVEL_SCREEN
#define __GAVEL_SCREEN

// #define SSD1306_NO_SPLASH

#include <Adafruit_SSD1306.h>
#include <GavelTask.h>

#define SCREEN_LINES 8

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

typedef enum { JAXSON, DRAGON, LIGHT, VUMETER, GAVEL, BITMAP_LENGTH } BITMAP;

class RefreshScreen {
public:
  virtual void screen() = 0;
  unsigned long refresh;
};

class Screen : public Task {
public:
  Screen();
  virtual void addCmd(TerminalCommand* __termCmd);
  virtual bool setupTask(OutputInterface* __terminal);
  virtual bool executeTask();

  void setRefreshScreen(RefreshScreen* __refreshScreen, unsigned long __refresh) {
    refreshScreen = __refreshScreen;
    refreshScreen->refresh = __refresh;
  };
  void setScreen(String line1 = "", String line2 = "", String line3 = "", String line4 = "", String line5 = "", String line6 = "", String line7 = "",
                 String line8 = "");
  void setScreen(BITMAP bitmap, String caption);
  void setScreen(unsigned char* bitmap, String caption, unsigned long width, unsigned long height);
  Adafruit_SSD1306* getDisplay() { return &display; };
  unsigned char* getBitmap(BITMAP bitmap);

private:
  Adafruit_SSD1306 display;
  void beginScreen();
  void printLnScreen(String line);
  void printLnScreen() { printLnScreen(""); };
  void endScreen();
  void setScreen(BITMAP bitmap);
  void setScreen(unsigned char* bitmap, unsigned long width, unsigned long height);
  void setCaption(String caption);

  Mutex screenAccess;

  RefreshScreen* refreshScreen;
  void bitmap(OutputInterface* terminal);
};

#endif
