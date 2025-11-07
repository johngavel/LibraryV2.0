#include "../src/imemory.h" // adjust if your filename differs

#include "GavelUtil.h"
#include "Terminal.h"

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstring>

class TemplateIMemory : public IMemory {
public:
  struct Data {
    unsigned char data[6];
    unsigned char spare[10];
  };
  // 6 + 10 = 16 bytes
  static_assert(sizeof(Data) == 16, "TemplateIMemory size unexpected - check packing/padding.");

  typedef union {
    Data data;
    unsigned char buffer[sizeof(Data)];
  } Memory;

  TemplateIMemory() { memset(memory.buffer, 0, sizeof(Data)); };

  TemplateIMemory(const unsigned char* src, std::size_t n) {
    const std::size_t cap = sizeof(Memory::buffer);
    const std::size_t m = (n < cap) ? n : cap;
    for (std::size_t i = 0; i < m; ++i) memory.buffer[i] = src[i];
    for (std::size_t i = m; i < cap; ++i) memory.buffer[i] = 0;
  }

  // IMemory overrides
  const unsigned char& operator[](std::size_t index) const override { return memory.buffer[index]; }
  unsigned char& operator[](std::size_t index) override {
    // Unchecked, like std::vector::operator[]
    return memory.buffer[index];
  }

  std::size_t size() const noexcept override { return sizeof(Memory::buffer); }

  void initMemory() override {
    memory.data.data[0] = 0xDE;
    memory.data.data[1] = 0xAD;
    memory.data.data[2] = 0xCC;
    memory.data.data[3] = 0x10;
    memory.data.data[4] = 0x11;
    memory.data.data[5] = 0x12;
    memset(memory.data.spare, 0, sizeof(memory.data.spare));
  }

  void printData(OutputInterface* terminal) override {
    char buffer[20];
    terminal->print(INFO, "Data: ");
    terminal->println(INFO, getMacString(memory.data.data, buffer, sizeof(buffer)));
  }

  void updateExternal() override {}
  Memory memory;

private:
};

static void dump_bytes(const unsigned char* p, std::size_t n, const char* label) {
  std::printf("%s (%zu):", label, n);
  for (std::size_t i = 0; i < n; ++i) std::printf(" %02X", (unsigned) p[i]);
  std::printf("\n");
}

int main() {
  std::printf("== TemplateIMemory basic tests ==\n");

  // 1) size() must be 16 (struct layout)
  {
    TemplateIMemory mem;
    std::printf("[T1] size() = %zu\n", mem.size());
    assert(mem.size() == 16);
  }

  // 2) Default construction zero-initializes
  {
    TemplateIMemory mem;
    for (std::size_t i = 0; i < mem.size(); ++i) { assert(mem[i] == 0); }
    std::printf("[T2] default zero-init OK\n");
  }

  // 3) Write via operator[] and read back (const and non-const)
  {
    TemplateIMemory mem;
    for (std::size_t i = 0; i < mem.size(); ++i) mem[i] = (unsigned char) i;

    TemplateIMemory& cmem = mem;
    for (std::size_t i = 0; i < cmem.size(); ++i) assert(cmem[i] == (unsigned char) i);
    std::printf("[T3] operator[] R/W OK\n");
  }

  // 4) Constructor from source buffer (copy + zero remainder)
  {
    unsigned char src[64];
    for (std::size_t i = 0; i < sizeof(src); ++i) src[i] = (unsigned char) (0x10 + (i & 0xFF));

    TemplateIMemory mem(src, 15);
    for (std::size_t i = 0; i < 15; ++i) assert(mem[i] == src[i]);
    for (std::size_t i = 15; i < mem.size(); ++i) assert(mem[i] == 0);

    dump_bytes(&mem[0], mem.size(), "[T4]");
    std::printf("[T4] ctor(src, n) OK\n");
  }

  // 5) Union overlay sanity: write typed fields, verify raw
  {
    TemplateIMemory mem;

    // write MAC (first 6 bytes)
    unsigned char mac[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x01};
    std::memcpy(mem.memory.data.data, mac, 6);

    // Verify raw layout spots
    for (int i = 0; i < 6; ++i) assert(mem[(std::size_t) i] == mac[i]);
    assert(mem[1] == 0xAD && mem[2] == 0xBE && mem[5] == 0x01);

    dump_bytes(&mem[0], 24, "[T5]");
    std::printf("[T5] struct <-> raw overlay OK\n");
  }

  // 6) Pointer Tests
  {
    TemplateIMemory mem;
    IMemory* imem = &mem;

    // write MAC (first 6 bytes)
    unsigned char mac[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x01};
    (*imem)[0] = 0xDE;
    (*imem)[1] = 0xAD;
    (*imem)[2] = 0xBE;
    (*imem)[3] = 0xEF;
    (*imem)[4] = 0x00;
    (*imem)[5] = 0x01;

    // Verify raw layout spots
    for (int i = 0; i < 6; ++i) assert(mem[(std::size_t) i] == mac[i]);
    assert((*imem)[1] == 0xAD && (*imem)[2] == 0xBE && (*imem)[5] == 0x01);

    dump_bytes(&mem[0], 24, "[T6]");
    std::printf("[T6] Pointer to IMemory OK\n");
  }

  std::printf("== All tests PASSED ==\n");
  return 0;
}
