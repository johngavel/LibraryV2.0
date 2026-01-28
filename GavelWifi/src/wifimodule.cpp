#include "wifimodule.h"

#include <GavelUtil.h>

static bool configParser(OutputInterface* terminal, WifiMemory* memory);

void WifiVirtualServer::begin() {
  server = new WiFiServer(port);
  server->begin();
}

Client* WifiVirtualServer::accept() {
  return clientManager.setClient(server->accept());
}

bool WifiModule::linkStatus() {
  bool status;
  status = WiFi.isConnected();
  return status;
}

void WifiModule::addCmd(TerminalCommand* __termCmd) {
  if (__termCmd)
    __termCmd->addCmd("ipconfig", "-ssid <network>| -pass <password>", "IP Memory Configuration",
                      [this](TerminalLibrary::OutputInterface* terminal) { wifiStat(terminal); });
  if (__termCmd)
    __termCmd->addCmd("wifiscan", "", "Scans the Wifi for Networks",
                      [this](TerminalLibrary::OutputInterface* terminal) { wifiScan(terminal); });
}

void WifiModule::reservePins(BackendPinSetup* pinsetup) {}

bool WifiModule::setupTask(OutputInterface* __terminal) {
  bool initializing = true;
  unsigned int wifiStatus;

  runTask(false);
  WiFi.mode(WIFI_STA);
  WiFi.setHostname("GavelWifiModule");
  __terminal->print(WARNING, "Connecting Wifi to " + String(memory.memory.data.ssid));
  WiFi.begin(memory.memory.data.ssid, memory.memory.data.password);
  while (initializing) {
    wifiStatus = WiFi.status();
    switch (wifiStatus) {
    case WL_NO_MODULE:
      __terminal->println();
      __terminal->println(ERROR, "Wifi Module Failed to Connect - No Module");
      initializing = false;
      status = false;
      break;
    case WL_NO_SSID_AVAIL:
      __terminal->println();
      __terminal->println(ERROR, "Wifi Module Failed to Connect - No SSID Available");
      initializing = false;
      status = false;
      break;
    case WL_CONNECT_FAILED:
      __terminal->println();
      __terminal->println(ERROR, "Wifi Module Failed to Connect - Connection Failed");
      initializing = false;
      status = false;
      break;
    case WL_CONNECTED:
      __terminal->println();
      terminal->println(PASSED, "IP Address: ", getIPAddress().toString().c_str());
      initializing = false;
      status = true;
      break;
    default:
      delay(500);
      __terminal->print(INFO, ".");
      break;
    }
  }
  return status;
}

bool WifiModule::executeTask() {
  return true;
}

String WifiModule::getSSID() {
  String network;
  network = WiFi.SSID();
  return network;
}

IPAddress WifiModule::getIPAddress() {
  IPAddress address;
  address = WiFi.localIP();
  return address;
}

IPAddress WifiModule::getDNS() {
  IPAddress address;
  address = WiFi.dnsIP();
  return address;
}

IPAddress WifiModule::getSubnetMask() {
  IPAddress address;
  address = WiFi.subnetMask();
  return address;
}

IPAddress WifiModule::getGateway() {
  IPAddress address;
  address = WiFi.gatewayIP();
  return address;
}

VirtualServer* WifiModule::getServer(int port) {
  return new WifiVirtualServer(this, port);
}

void WifiModule::wifiStat(OutputInterface* terminal) {
  if (!configParser(terminal, &memory)) {
    IPAddress ipAddress = getIPAddress();
    bool linked = linkStatus();
    terminal->println(INFO, "Network: " + getSSID() + ((linked) ? " Connected" : " Unconnected"));
    terminal->println(INFO, "  IP Address:  " + String(ipAddress[0]) + String(".") + String(ipAddress[1]) +
                                String(".") + String(ipAddress[2]) + String(".") + String(ipAddress[3]));
    ipAddress = getSubnetMask();
    terminal->println(INFO, "  Subnet Mask: " + String(ipAddress[0]) + String(".") + String(ipAddress[1]) +
                                String(".") + String(ipAddress[2]) + String(".") + String(ipAddress[3]));
    ipAddress = getGateway();
    terminal->println(INFO, "  Gateway:     " + String(ipAddress[0]) + String(".") + String(ipAddress[1]) +
                                String(".") + String(ipAddress[2]) + String(".") + String(ipAddress[3]));
    ipAddress = getDNS();
    terminal->println(INFO, "  DNS Server:  " + String(ipAddress[0]) + String(".") + String(ipAddress[1]) +
                                String(".") + String(ipAddress[2]) + String(".") + String(ipAddress[3]));
  }
  terminal->prompt();
}

static const char* encToString(uint8_t enc) {
  switch (enc) {
  case ENC_TYPE_NONE: return "NONE";
  case ENC_TYPE_TKIP: return "WPA";
  case ENC_TYPE_CCMP: return "WPA2";
  case ENC_TYPE_AUTO: return "AUTO";
  }
  return "UNKN";
}

void WifiModule::wifiScan(OutputInterface* terminal) {
  terminal->println(INFO, "Begining Scan.........");
  auto cnt = WiFi.scanNetworks();
  if (!cnt) {
    terminal->println(INFO, "No Networks Found.");
  } else {
    char line[80];
    sprintf(line, "Found %d networks\n", cnt);
    terminal->println(INFO, line);
    sprintf(line, "%32s %5s %17s %2s %4s", "SSID", "ENC", "BSSID        ", "CH", "RSSI");
    terminal->println(INFO, line);
    for (auto i = 0; i < cnt; i++) {
      uint8_t bssid[6];
      char macString[20];
      WiFi.BSSID(i, bssid);
      sprintf(line, "%32s %5s %17s %2d %4ld", WiFi.SSID(i), encToString(WiFi.encryptionType(i)),
              getMacString(bssid, macString, sizeof(macString)), WiFi.channel(i), WiFi.RSSI(i));
      terminal->println(INFO, line);
    }
  }
  terminal->println(PASSED, "Scan Complete");
  terminal->prompt();
}

JsonDocument WifiModule::createJson() {
  char temp[128];
  JsonDocument doc;
  doc["macAddress"] = getMacString(getMACAddress(), temp, sizeof(temp));
  doc["ipAddress"] = getIPAddress().toString().c_str();
  doc["subnetMask"] = getSubnetMask().toString().c_str();
  doc["gatewayAddress"] = getGateway().toString().c_str();
  doc["dnsAddress"] = getDNS().toString().c_str();
  doc["isDHCP"] = true;
  doc["allowDHCP"] = true;
  doc["isWifi"] = true;

  return doc;
};

bool WifiModule::parseJson(JsonDocument& doc) {
  return true;
};

bool configParser(OutputInterface* terminal, WifiMemory* memory) {
  bool actionTaken = false;

  struct ConfigOption {
    const char* flag;
    char* target;
  } options[] = {{"-ssid", memory->memory.data.ssid}, {"-pass", memory->memory.data.password}};

  char* action = terminal->readParameter();
  char* parameter = NULL;

  while (action != NULL) {
    bool matched = false;

    // Check IP-related options
    for (size_t i = 0; i < sizeof(options) / sizeof(options[0]); i++) {
      if (safeCompare(action, options[i].flag) == 0) {
        matched = true;
        parameter = terminal->readParameter();
        if (parameter) {
          strncpy(options[i].target, parameter, 15);
          memory->setInternal(true);
          actionTaken = true;
        } else {
          terminal->invalidParameter();
        }
        break;
      }
    }
    if (!matched) { terminal->invalidParameter(); }
    action = terminal->readParameter();
  }

  if (actionTaken) { terminal->println(WARNING, "", "Reboot the unit for changes to take effect."); }
  return actionTaken;
}