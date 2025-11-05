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
    if (memory.getUpdated()) {
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
  status &= updateMemory();
  return status;
}

void EthernetModule::configure() {
  memory.initMemory();
  memory.setUpdated(false);
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
  memory.setUpdated(true);
}

void EthernetModule::addCmd(TerminalCommand* __termCmd) {
  if (__termCmd) __termCmd->addCmd("ip", "", "IP Stats", [this](TerminalLibrary::OutputInterface* terminal) { ipStat(terminal); });
}

bool EthernetModule::setupTask(OutputInterface* __terminal) {
  bool status = true;

  terminal = __terminal;

  setRefreshMilli(60000);
  if (memory.getUpdated()) { status &= setupW5500(); }
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

void EthernetModule::ipStat(OutputInterface* terminal) {
  char buffer[20];
  IPAddress ipAddress = getIPAddress();
  bool linked = linkStatus();
  terminal->print(INFO, "MAC Address:  ");
  terminal->println(INFO, getMacString(memory.memory.data.macAddress, buffer, sizeof(buffer)));
  terminal->print(INFO, "IP Address is ");
  terminal->println(INFO, ((memory.memory.data.isDHCP) ? "DHCP" : "Static"));
  terminal->println(INFO, ((linked) ? "Connected" : "Unconnected"));
  terminal->print(INFO, "IP Address:  ");
  terminal->println(INFO, getIPString(memory.memory.data.ipAddress, buffer, sizeof(buffer)));
  terminal->print(INFO, "Subnet Mask:  ");
  terminal->println(INFO, getIPString(memory.memory.data.subnetMask, buffer, sizeof(buffer)));
  terminal->print(INFO, "Gateway:  ");
  terminal->println(INFO, getIPString(memory.memory.data.gatewayAddress, buffer, sizeof(buffer)));
  terminal->print(INFO, "DNS Address:  ");
  terminal->println(INFO, getIPString(memory.memory.data.dnsAddress, buffer, sizeof(buffer)));
  terminal->prompt();
}

static void ipAddressToBuffer(IPAddress address, unsigned char* buffer) {
  buffer[0] = address[0];
  buffer[1] = address[1];
  buffer[2] = address[2];
  buffer[3] = address[3];
}

bool EthernetModule::updateMemory() {
  unsigned char buffer[4];
  ipAddressToBuffer(getIPAddress(), buffer);
  memcpy(memory.memory.data.ipAddress, buffer, sizeof(memory.memory.data.ipAddress));
  ipAddressToBuffer(getDNS(), buffer);
  memcpy(memory.memory.data.ipAddress, buffer, sizeof(memory.memory.data.ipAddress));
  ipAddressToBuffer(getSubnetMask(), buffer);
  memcpy(memory.memory.data.ipAddress, buffer, sizeof(memory.memory.data.ipAddress));
  ipAddressToBuffer(getGateway(), buffer);
  memcpy(memory.memory.data.ipAddress, buffer, sizeof(memory.memory.data.ipAddress));
  memory.setUpdated(true);
  return true;
}
