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
  if (isDHCP) {
    spiWire.wireTake();
    Ethernet.begin(macAddress, 3000, 1500);
    spiWire.wireGive();
    ipChanged = true;
  } else {
    if ((macAddress != nullptr) && (ipAddress != nullptr) && (dnsAddress != nullptr) && (gatewayAddress != nullptr) && (subnetMask != nullptr)) {
      spiWire.wireTake();
      Ethernet.begin(macAddress, ipAddress, dnsAddress, gatewayAddress, subnetMask);
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

void EthernetModule::configure(byte* __macAddress, bool __isDHCP, byte* __ipAddress, byte* __dnsAddress, byte* __subnetMask, byte* __gatewayAddress) {
  isConfigured = true;
  macAddress = __macAddress;
  isDHCP = __isDHCP;
  ipAddress = __ipAddress;
  dnsAddress = __dnsAddress;
  subnetMask = __subnetMask;
  gatewayAddress = __gatewayAddress;
}

void EthernetModule::addCmd(TerminalCommand* __termCmd) {
  if (__termCmd) __termCmd->addCmd("ip", "", "IP Stats", [this](TerminalLibrary::OutputInterface* terminal) { ipStat(terminal); });
}

bool EthernetModule::setupTask(OutputInterface* __terminal) {
  bool status = true;

  terminal = __terminal;

  setRefreshMilli(60000);
  if (isConfigured) { status &= setupW5500(); }
  runTimer(status);
  return status;
}

bool EthernetModule::executeTask() {
  spiWire.wireTake();
  int maintain = Ethernet.maintain();
  spiWire.wireGive();
  if (maintain == 4) { ipChanged = true; }
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
  IPAddress ipAddress = getIPAddress();
  bool linked = linkStatus();
  terminal->print(INFO, "MAC Address:  ");
  terminal->print(INFO, String(macAddress[0], HEX) + ":");
  terminal->print(INFO, String(macAddress[1], HEX) + ":");
  terminal->print(INFO, String(macAddress[2], HEX) + ":");
  terminal->print(INFO, String(macAddress[3], HEX) + ":");
  terminal->print(INFO, String(macAddress[4], HEX) + ":");
  terminal->println(INFO, String(macAddress[5], HEX));
  terminal->println(INFO, "IP Address is " + String((isDHCP) ? "DHCP" : "Static"));
  terminal->println(INFO, String((linked) ? "Connected" : "Unconnected"));
  terminal->println(INFO, "  IP Address:  " + String(ipAddress[0]) + String(".") + String(ipAddress[1]) + String(".") + String(ipAddress[2]) + String(".") +
                              String(ipAddress[3]));
  ipAddress = getSubnetMask();
  terminal->println(INFO, "  Subnet Mask: " + String(ipAddress[0]) + String(".") + String(ipAddress[1]) + String(".") + String(ipAddress[2]) + String(".") +
                              String(ipAddress[3]));
  ipAddress = getGateway();
  terminal->println(INFO, "  Gateway:     " + String(ipAddress[0]) + String(".") + String(ipAddress[1]) + String(".") + String(ipAddress[2]) + String(".") +
                              String(ipAddress[3]));
  ipAddress = getDNS();
  terminal->println(INFO, "  DNS Server:  " + String(ipAddress[0]) + String(".") + String(ipAddress[1]) + String(".") + String(ipAddress[2]) + String(".") +
                              String(ipAddress[3]));
  terminal->prompt();
}
