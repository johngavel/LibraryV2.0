#ifndef __GAVEL_REBOOT_FILE_H
#define __GAVEL_REBOOT_FILE_H

#include <GavelInterfaces.h>

class RebootFile : public DynamicFile {
public:
  RebootFile(DeviceCmd* device)
      : DynamicFile("reboot.json", READ_WRITE, _fileBuffer, sizeof(_fileBuffer)), _device(device){};
  virtual bool createReadData() override { return true; };

  virtual bool parseWriteData() override {
    if (!_device) return false;
    _device->reboot();
    return true;
  };

private:
  char _fileBuffer[200];
  DeviceCmd* _device;
};

class UpgradeFile : public DynamicFile {
public:
  UpgradeFile(DeviceCmd* device)
      : DynamicFile("upgrade.json", READ_WRITE, _fileBuffer, sizeof(_fileBuffer)), _device(device){};
  virtual bool createReadData() override { return true; };

  virtual bool parseWriteData() override {
    if (!_device) return false;
    _device->upgrade();
    return true;
  };

private:
  char _fileBuffer[200];
  DeviceCmd* _device;
};

class UploadFile : public DynamicFile {
public:
  UploadFile(DeviceCmd* device)
      : DynamicFile("upload.json", READ_WRITE, _fileBuffer, sizeof(_fileBuffer)), _device(device){};
  virtual bool createReadData() override { return true; };

  virtual bool parseWriteData() override {
    if (!_device) return false;
    _device->upload();
    return true;
  };

private:
  char _fileBuffer[200];
  DeviceCmd* _device;
};

#endif // __GAVEL_REBOOT_FILE_H
