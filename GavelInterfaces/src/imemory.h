#ifndef GAVEL_MEMORY_H_
#define GAVEL_MEMORY_H_

#include <GavelUtil.h>
#include <Terminal.h>
#include <cstddef> // std::size_t
#include <string.h>

class IMemory {
public:
  IMemory() : updated(false) {}
  virtual ~IMemory() = default;

  // Read-only access (const overload)
  virtual const unsigned char& operator[](std::size_t index) const = 0; // Total number of bytes

  // Read/write access
  virtual unsigned char& operator[](std::size_t index) = 0;

  // Total number of bytes
  virtual std::size_t size() const noexcept = 0;

  virtual void initMemory() = 0;
  virtual void printData(OutputInterface* terminal) = 0;
  bool getUpdated() { return updated; };
  void setUpdated(bool __updated) { updated = __updated; };
 
private:
  bool updated;
};

// class TemplateIMemory : public IMemory {
// public:
//   typedef struct {
//     unsigned char data[6];
//     unsigned char spare[10];
//   } Data;
//   // 6 + 10 = 16 bytes
//   static_assert(sizeof(Data) == 16, "TemplateIMemory size unexpected - check packing/padding.");

//   typedef union {
//     Data data;
//     unsigned char buffer[sizeof(Data)];
//   } Union;

//   TemplateIMemory() { memset(memory.buffer, 0, sizeof(Union.buffer)); };

//   TemplateIMemory(const unsigned char* src, std::size_t n) {
//     const std::size_t cap = sizeof(Memory::buffer);
//     const std::size_t m = (n < cap) ? n : cap;
//     for (std::size_t i = 0; i < m; ++i) memory.buffer[i] = src[i];
//     for (std::size_t i = m; i < cap; ++i) memory.buffer[i] = 0;
//   }

//   // IMemory overrides
//   const unsigned char& operator[](std::size_t index) const override {
//     return memory.buffer[index];
//   }
//   unsigned char& operator[](std::size_t index) override {
//     // Unchecked, like std::vector::operator[]
//     return memory.buffer[index];
//   }

//   std::size_t size() const noexcept override { return sizeof(Memory::buffer); }

//   void initMemory() override {
//     memory.data.data[0] = 0xDE;
//     memory.data.data[1] = 0xAD;
//     memory.data.data[2] = 0xCC;
//     memory.data.data[3] = 0x10;
//     memory.data.data[4] = 0x11;
//     memory.data.data[5] = 0x12;
//     memset(memory.data.spare, 0, sizeof(memory.data.spare));
//   }

//   void printData(OutputInterface* terminal) override {
//     char buffer[20];
//     terminal->print(INFO, "Data: ");
//     terminal->println(INFO, getMacString(memory.data.data, buffer, sizeof(buffer)));
//   }
//   Memory memory;

// private:
// };

#endif // GAVEL_MEMORY_H_