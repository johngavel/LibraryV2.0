#include "ethernetmodule.h"

#include <GavelSPIWire.h>

static char taskname[] = "EthernetModule";

EthernetModule::EthernetModule() : Task(taskname) {}

bool EthernetModule::resetW5500() {
  pinMode(15, OUTPUT);
  digitalWrite(15, LOW);
  delay(200);
  digitalWrite(15, HIGH);
  delay(200);
  if (terminal) terminal->println(PASSED, "W5500 Restart Complete");
  return true;
}

bool EthernetModule::setupW5500() {
  bool status = true;
  status &= resetW5500();
  spiWire.wireTake();
  Ethernet.init(17);
  spiWire.wireGive();
  if (memory.memory.data.isDHCP) {
    spiWire.wireTake();
    Ethernet.begin(memory.memory.data.macAddress, 3000, 1500);
    spiWire.wireGive();
  } else {
    if (memory.getInternal()) {
      spiWire.wireTake();
      Ethernet.begin(memory.memory.data.macAddress, memory.memory.data.ipAddress, memory.memory.data.dnsAddress, memory.memory.data.gatewayAddress,
                     memory.memory.data.subnetMask);
      spiWire.wireGive();
    }
  }
  spiWire.wireTake();
  int hardwareStatus = Ethernet.hardwareStatus();
  spiWire.wireGive();
  if (hardwareStatus == EthernetNoHardware) {
    status = false;
    if (terminal) terminal->println(ERROR, "Ethernet Module was not found.");
  } else if (hardwareStatus == EthernetW5100) {
    if (terminal) terminal->println(PASSED, "W5100 Ethernet controller detected.");
  } else if (hardwareStatus == EthernetW5200) {
    if (terminal) terminal->println(PASSED, "W5200 Ethernet controller detected.");
  } else if (hardwareStatus == EthernetW5500) {
    status &= true;
    if (terminal) terminal->println(PASSED, "W5500 Ethernet controller detected.");
  }
  return status;
}

void EthernetModule::configure() {
  memory.initMemory();
  memory.setInternal(false);
}

void EthernetModule::configure(byte* __macAddress, bool __isDHCP) {
  unsigned char blankAddress[4] = {0, 0, 0, 0};
  configure(__macAddress, __isDHCP, blankAddress, blankAddress, blankAddress, blankAddress);
}
void EthernetModule::configure(byte* __macAddress, bool __isDHCP, byte* __ipAddress, byte* __dnsAddress, byte* __subnetMask, byte* __gatewayAddress) {
  configure();
  memcpy(memory.memory.data.macAddress, __macAddress, sizeof(memory.memory.data.macAddress));
  memory.memory.data.isDHCP = __isDHCP;
  memcpy(memory.memory.data.ipAddress, __ipAddress, sizeof(memory.memory.data.ipAddress));
  memcpy(memory.memory.data.dnsAddress, __dnsAddress, sizeof(memory.memory.data.dnsAddress));
  memcpy(memory.memory.data.subnetMask, __subnetMask, sizeof(memory.memory.data.subnetMask));
  memcpy(memory.memory.data.gatewayAddress, __gatewayAddress, sizeof(memory.memory.data.gatewayAddress));
  memory.setInternal(true);
}

void EthernetModule::addCmd(TerminalCommand* __termCmd) {
  if (__termCmd)
    __termCmd->addCmd("ipconfig", "-ip|-sm|-gw|-dns <address> | -dhcp|-nodhcp", "IP Memory Configuration",
                      [this](TerminalLibrary::OutputInterface* terminal) { ipConfig(terminal); });
  if (__termCmd)
    __termCmd->addCmd("ifconfig", "-ip|-sm|-gw|-dns <address> | -dhcp|-nodhcp", "IP Interface Configuration",
                      [this](TerminalLibrary::OutputInterface* terminal) { ifConfig(terminal); });
}

bool EthernetModule::setupTask(OutputInterface* __terminal) {
  bool status = true;

  terminal = __terminal;

  setRefreshMilli(60000);
  if (memory.getInternal()) { status &= setupW5500(); }
  status &= updateMemory();
  runTimer(status);
  return status;
}

bool EthernetModule::executeTask() {
  spiWire.wireTake();
  int maintain = Ethernet.maintain();
  spiWire.wireGive();
  if (maintain == 4) { updateMemory(); }
  return true;
}

IPAddress EthernetModule::getIPAddress() {
  IPAddress address;
  spiWire.wireTake();
  address = Ethernet.localIP();
  spiWire.wireGive();
  return address;
}

IPAddress EthernetModule::getDNS() {
  IPAddress address;
  spiWire.wireTake();
  address = Ethernet.dnsServerIP();
  spiWire.wireGive();
  return address;
}

IPAddress EthernetModule::getSubnetMask() {
  IPAddress address;
  spiWire.wireTake();
  address = Ethernet.subnetMask();
  spiWire.wireGive();
  return address;
}

IPAddress EthernetModule::getGateway() {
  IPAddress address;
  spiWire.wireTake();
  address = Ethernet.gatewayIP();
  spiWire.wireGive();
  return address;
}

VirtualServer* EthernetModule::getServer(int port) {
  return new WiredServer(this, port);
}

bool EthernetModule::linkStatus() {
  bool status;
  spiWire.wireTake();
  status = Ethernet.linkStatus();
  spiWire.wireGive();
  return status;
}

static void ipAddressToBuffer(IPAddress address, unsigned char* buffer) {
  buffer[0] = address[0];
  buffer[1] = address[1];
  buffer[2] = address[2];
  buffer[3] = address[3];
}

static bool parseIPAddress(const char* ipString, unsigned char* buffer) {
  if (!ipString || !buffer) return 0;

  char temp[32];
  strncpy(temp, ipString, sizeof(temp) - 1);
  temp[sizeof(temp) - 1] = '\0';

  char* token = strtok(temp, ".");
  int octetCount = 0;

  while (token != NULL) {
    // Check if token is numeric
    for (int i = 0; token[i] != '\0'; i++) {
      if (!isdigit((unsigned char) token[i])) return 0;
    }

    int value = atoi(token);
    if (value < 0 || value > 255) return 0;

    buffer[octetCount++] = (unsigned char) value;

    token = strtok(NULL, ".");
  }

  return (octetCount == 4); // Must have exactly 4 octets
}

bool configParser(OutputInterface* terminal, EthernetMemory* memory) {
  unsigned char ip_buffer[4];
  bool actionTaken = false;

  struct ConfigOption {
    const char* flag;
    unsigned char* target;
  } options[] = {{"-ip", memory->memory.data.ipAddress},
                 {"-sm", memory->memory.data.subnetMask},
                 {"-gw", memory->memory.data.gatewayAddress},
                 {"-dns", memory->memory.data.dnsAddress}};

  char* action = terminal->readParameter();
  char* parameter = NULL;

  while (action != NULL) {
    bool matched = false;

    // Check IP-related options
    for (size_t i = 0; i < sizeof(options) / sizeof(options[0]); i++) {
      if (safeCompare(action, options[i].flag) == 0) {
        matched = true;
        parameter = terminal->readParameter();
        if (parameter && parseIPAddress(parameter, ip_buffer)) {
          memcpy(options[i].target, ip_buffer, 4);
          memory->setInternal(true);
          actionTaken = true;
        } else {
          terminal->invalidParameter();
        }
        break;
      }
    }

    // Check DHCP flags
    if (!matched) {
      if (safeCompare(action, "-dhcp") == 0) {
        memory->memory.data.isDHCP = true;
        memory->setInternal(true);
        actionTaken = true;
      } else if (safeCompare(action, "-nodhcp") == 0) {
        memory->memory.data.isDHCP = false;
        memory->setInternal(true);
        actionTaken = true;
      } else {
        terminal->invalidParameter();
      }
    }
    action = terminal->readParameter();
  }

  if (actionTaken) { terminal->println(WARNING, "", "Reboot the unit for changes to take effect."); }
  return actionTaken;
}

void EthernetModule::ipConfig(OutputInterface* terminal) {
  char buffer[20];
  bool linked = linkStatus();

  if (!configParser(terminal, &memory)) {
    terminal->println(PROMPT, "Configured network parameters (from memory):");
    terminal->print(INFO, "MAC Address:  ");
    terminal->println(INFO, getMacString(memory.memory.data.macAddress, buffer, sizeof(buffer)));
    terminal->print(INFO, "IP Address is ");
    terminal->println(INFO, ((memory.memory.data.isDHCP) ? "DHCP" : "Static"));
    terminal->println(INFO, ((linked) ? "Ethernet: Connected" : "Ethernet: Disconnected"));
    terminal->print(INFO, "IP Address:  ");
    terminal->println(INFO, getIPString(memory.memory.data.ipAddress, buffer, sizeof(buffer)));
    terminal->print(INFO, "Subnet Mask:  ");
    terminal->println(INFO, getIPString(memory.memory.data.subnetMask, buffer, sizeof(buffer)));
    terminal->print(INFO, "Gateway:  ");
    terminal->println(INFO, getIPString(memory.memory.data.gatewayAddress, buffer, sizeof(buffer)));
    terminal->print(INFO, "DNS Address:  ");
    terminal->println(INFO, getIPString(memory.memory.data.dnsAddress, buffer, sizeof(buffer)));
  }
  terminal->prompt();
}

void EthernetModule::ifConfig(OutputInterface* terminal) {
  char buffer[20];
  unsigned char ip_buffer[4];
  bool linked = linkStatus();

  if (!configParser(terminal, &memory)) {
    terminal->println(PROMPT, "Active network parameters (from interface):");
    terminal->print(INFO, "MAC Address:  ");
    terminal->println(INFO, getMacString(memory.memory.data.macAddress, buffer, sizeof(buffer)));
    terminal->print(INFO, "IP Address is ");
    terminal->println(INFO, ((memory.memory.data.isDHCP) ? "DHCP" : "Static"));
    terminal->println(INFO, ((linked) ? "Ethernet: Connected" : "Ethernet: Disconnected"));
    terminal->print(INFO, "IP Address:  ");
    ipAddressToBuffer(getIPAddress(), ip_buffer);
    terminal->println(INFO, getIPString(ip_buffer, buffer, sizeof(buffer)));
    terminal->print(INFO, "Subnet Mask:  ");
    ipAddressToBuffer(getSubnetMask(), ip_buffer);
    terminal->println(INFO, getIPString(ip_buffer, buffer, sizeof(buffer)));
    terminal->print(INFO, "Gateway:  ");
    ipAddressToBuffer(getGateway(), ip_buffer);
    terminal->println(INFO, getIPString(ip_buffer, buffer, sizeof(buffer)));
    terminal->print(INFO, "DNS Address:  ");
    ipAddressToBuffer(getDNS(), ip_buffer);
    terminal->println(INFO, getIPString(ip_buffer, buffer, sizeof(buffer)));
  }
  terminal->prompt();
}

bool EthernetModule::updateMemory() {
  unsigned char buffer[4];
  ipAddressToBuffer(getIPAddress(), buffer);
  memcpy(memory.memory.data.ipAddress, buffer, sizeof(memory.memory.data.ipAddress));
  ipAddressToBuffer(getDNS(), buffer);
  memcpy(memory.memory.data.dnsAddress, buffer, sizeof(memory.memory.data.dnsAddress));
  ipAddressToBuffer(getSubnetMask(), buffer);
  memcpy(memory.memory.data.subnetMask, buffer, sizeof(memory.memory.data.subnetMask));
  ipAddressToBuffer(getGateway(), buffer);
  memcpy(memory.memory.data.gatewayAddress, buffer, sizeof(memory.memory.data.gatewayAddress));
  memory.setInternal(true);
  return true;
}
