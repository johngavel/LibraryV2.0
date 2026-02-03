#ifndef __GAVEL_LIBRARIES_H
#define __GAVEL_LIBRARIES_H
/* Auto-generated: library names, versions, and license names */

typedef struct {
  const char* name;
  const char* version;
  const char* license_name;
  const char* link;
} LibraryInfo;

static const LibraryInfo libraries[] = {
    {"Adafruit BusIO", "1.17.4", "LICENSE", "https://github.com/adafruit/Adafruit_BusIO"},
    {"Adafruit GFX Library", "1.12.4", "license.txt", "https://github.com/adafruit/Adafruit-GFX-Library"},
    {"Adafruit MCP4725", "2.0.2", "license.txt", "https://github.com/adafruit/Adafruit_MCP4725"},
    {"Adafruit NeoPixel", "1.15.3", "COPYING", "https://github.com/adafruit/Adafruit_NeoPixel"},
    {"Adafruit SSD1306", "2.5.16", "license.txt", "https://github.com/adafruit/Adafruit_SSD1306"},
    {"Adafruit Unified Sensor", "1.1.15", "LICENSE.txt", "https://github.com/adafruit/Adafruit_Sensor"},
    {"arduino-cli", "1.4.1", "LICENSE.txt", "https://github.com/arduino/arduino-cli"},
    {"Arduino IDE", "2.3.7", "LICENSE.txt", "https://github.com/arduino/arduino-ide"},
    {"ArduinoJson", "7.4.2", "LICENSE.txt", "https://arduinojson.org/?utm_source=meta&utm_medium=library.properties"},
    {"Arduino Pico", "5.5.0", "LICENSE", "https://github.com/earlephilhower/arduino-pico"},
    {"Credential", "1.0.0", "N/A", "https://github.com/"},
    {"DHT sensor library", "1.4.6", "license.txt", "https://github.com/adafruit/DHT-sensor-library"},
    {"Ethernet", "2.0.2", "README.adoc", "https://www.arduino.cc/en/Reference/Ethernet"},
    {"Gavel Libraries", "2.0.0", "", "https://github.com/johngavel/LibraryV2.0"},
    {"I2C_EEPROM", "1.9.4", "LICENSE", "https://github.com/RobTillaart/I2C_EEPROM.git"},
    {"TCA9555", "0.4.4", "LICENSE", "https://github.com/RobTillaart/TCA9555"},
    {"Terminal", "1.0.12", "license.txt", "https://github.com/johngavel/Terminal"},
};

#define LIBRARY_COUNT (sizeof(libraries) / sizeof(libraries[0]))

#define ADAFRUIT_BUSIO_INDEX 0
#define ADAFRUIT_GFX_LIBRARY_INDEX 1
#define ADAFRUIT_MCP4725_INDEX 2
#define ADAFRUIT_NEOPIXEL_INDEX 3
#define ADAFRUIT_SSD1306_INDEX 4
#define ADAFRUIT_UNIFIED_SENSOR_INDEX 5
#define ARDUINO_CLI_INDEX 6
#define ARDUINO_IDE_INDEX 7
#define ARDUINOJSON_INDEX 8
#define ARDUINO_PICO_INDEX 9
#define CREDENTIAL_INDEX 10
#define DHT_SENSOR_LIBRARY_INDEX 11
#define ETHERNET_INDEX 12
#define GAVEL_LIBRARIES_INDEX 13
#define I2C_EEPROM_INDEX 14
#define TCA9555_INDEX 15
#define TERMINAL_INDEX 16

#define MAX_LIBRARIES 22

#endif /* __GAVEL_LIBRARIES_H */
