#include "screen.h"

#include "bitmap.h"

#include <Adafruit_SSD1306.h>
#include <GavelI2CWire.h>
#include <GavelProgram.h>

static char taskname[] = "Screen";
Screen::Screen() : Task(taskname), display(Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET)), refreshScreen(nullptr) {}

void Screen::addCmd(TerminalCommand* __termCmd) {
  if (__termCmd)
    __termCmd->addCmd("bitmap", "[n]", "Displays an image on the screen", [this](TerminalLibrary::OutputInterface* terminal) { bitmap(terminal); });
}

bool Screen::setupTask(OutputInterface* __terminal) {
  i2cWire.wireTake();

  i2cWire.getWire()->beginTransmission(SCREEN_ADDRESS);
  unsigned char error = i2cWire.getWire()->endTransmission();
  bool foundDevice = (error == 0);

  if (!foundDevice) {
    runTimer(false);
    __terminal->println(ERROR, "SSD1306 Display Not Connected");
    i2cWire.wireGive();
    return false;
  }
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    runTimer(false);
    __terminal->println(ERROR, "SSD1306 Display Not Failed to Start");
    i2cWire.wireGive();
    return false;
  }
  runTimer(true);
  // Clear the buffer
  display.clearDisplay();
  i2cWire.wireGive();
  setScreen(JAXSON, String(ProgramInfo::AppName) + " v" + String(ProgramInfo::MajorVersion) + String(".") + String(ProgramInfo::MinorVersion));
  return true;
}

void Screen::beginScreen() {
  i2cWire.wireTake();
  reset();
  display.clearDisplay();
  display.setCursor(0, 0); // Start at top-left corner
  display.setTextSize(1);  // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  i2cWire.wireGive();
}

void Screen::printLnScreen(String line) {
  i2cWire.wireTake();
  display.println(line);
  i2cWire.wireGive();
}

void Screen::endScreen() {
  i2cWire.wireTake();
  display.display();
  i2cWire.wireGive();
}

void Screen::setScreen(unsigned char* bitmap, unsigned long width, unsigned long height) {
  beginScreen();
  i2cWire.wireTake();
  display.drawBitmap(0, 0, bitmap, width, height, WHITE);
  i2cWire.wireGive();
}

void Screen::setCaption(String caption) {
  String captionString = caption.substring(0, 21);
  i2cWire.wireTake();
  display.setCursor(0, 56); // Start at top-left corner
  int center = (21 - captionString.length()) / 2;
  for (int i = 0; i < center; i++) display.print(" ");
  display.print(captionString);
  for (int i = (center + captionString.length()); i <= 21; i++) display.print(" ");
  i2cWire.wireGive();
}

unsigned char* Screen::getBitmap(BITMAP bitmap) {
  unsigned char* pointer = nullptr;
  if (bitmap < BITMAP_LENGTH) pointer = (unsigned char*) bitmap_allArray[bitmap];
  return pointer;
}

void Screen::setScreen(BITMAP bitmap) {
  if (bitmap < BITMAP_LENGTH) { setScreen(getBitmap(bitmap), 128, 64); }
}

bool Screen::executeTask() {
  if (refreshScreen != nullptr) {
    setRefreshMilli(refreshScreen->refresh);
    refreshScreen->screen();
  }
  return true;
}

void Screen::setScreen(unsigned char* bitmap, String caption, unsigned long width, unsigned long height) {
  screenAccess.take();
  setScreen(bitmap, width, height);
  setCaption(caption);
  endScreen();
  screenAccess.give();
}

void Screen::setScreen(BITMAP bitmap, String caption) {
  screenAccess.take();
  setScreen(bitmap);
  setCaption(caption);
  endScreen();
  setRefreshMilli(5000);
  screenAccess.give();
}

void Screen::setScreen(String line1, String line2, String line3, String line4, String line5, String line6, String line7, String line8) {
  screenAccess.take();
  beginScreen();
  printLnScreen(line1);
  printLnScreen(line2);
  printLnScreen(line3);
  printLnScreen(line4);
  printLnScreen(line5);
  printLnScreen(line6);
  printLnScreen(line7);
  printLnScreen(line8);
  endScreen();
  screenAccess.give();
}

void Screen::bitmap(OutputInterface* terminal) {
  char* value;
  StopWatch time;
  value = terminal->readParameter();
  if (value != NULL) {
    BITMAP bitmap = (BITMAP) atoi(value);

    if (bitmap < BITMAP_LENGTH) {
      time.start();
      setScreen(bitmap, String("Test Bitmap " + String(bitmap)));
      time.stop();
      terminal->println(PASSED, "Bitmap Displayed - " + String(time.time() / 1000.0, 3) + " us");
    } else
      terminal->println(FAILED, "Unknown Bitmap, only valid 0 - " + String(BITMAP_LENGTH - 1));
  } else
    terminal->invalidParameter();
  terminal->prompt();
}
