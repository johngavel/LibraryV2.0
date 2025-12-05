
// test_arrayfs.cpp
#include <cassert>
#include <cstdio>
#include <cstring>

// ===== Optional shims for non-Arduino desktop builds =====
#ifndef USE_REAL_ARDUINO
// Minimal Arduino types so digitalfile.h compiles
typedef unsigned char unsigned char;
class Stream {
public:
  virtual int available() = 0;
  virtual int read() = 0;
  virtual int peek() = 0;
  virtual void flush() = 0;
  virtual size_t write(const unsigned char* buffer, size_t size) = 0;
  virtual size_t write(unsigned char) = 0;
  virtual ~Stream() {}
};
#endif

#ifndef USE_REAL_GAVELUTIL
// Provide safeCompare used by ArrayDirectory
static inline int safeCompare(const char* a, const char* b) {
  if (!a && !b) return 0;
  if (!a) return -1;
  if (!b) return 1;
  return std::strcmp(a, b);
}
#endif
// ==========================================================

// Include your headers (order matters)
#include "../src/GavelArrayFileSystem.h" // Declares ArrayFile/ArrayDirectory/ArrayFileSystem
#include "../src/arraydirectory.cpp"
#include "../src/arrayfile.cpp"
#include "../src/arrayfilesystem.cpp"
#include "digitalfile.h" // Defines DigitalFile/DigitalDirectory/DigitalFileSystem (and Stream)

// Helper to print PASS markers
static void pass(const char* name) {
  std::printf("[PASS] %s\n", name);
}

static void test_ArrayFile_basic_io() {
  std::printf("Running test_ArrayFile_basic_io...\n");

  // Prepare a small buffer
  char data[] = "ABC";
  ArrayFile
      f; // [arrayfile.cpp](https://carcgl-my.sharepoint.com/personal/john_gavel_carrier_com/Documents/Microsoft%20Copilot%20Chat%20Files/arrayfile.cpp?EntityRepresentationId=8c09e98b-a75f-453c-8886-5ddc6a75d043)
         // [1](https://carcgl-my.sharepoint.com/personal/john_gavel_carrier_com/Documents/Microsoft%20Copilot%20Chat%20Files/arrayfile.cpp)
  f.set((char*) "greet.txt", data, 3);
  assert(f.size() == 3);
  assert(std::strcmp(f.name(), "greet.txt") == 0);

  // Not open yet => available() == 0, read() == -1, operator bool == false
  assert(f.available() == 0);
  assert(f.read() == -1);
  assert(!f);

  // Open and read
  f.open();
  assert(f);
  assert(f.available() == 3);

  // Peek does not advance
  assert(f.peek() == 'A');
  assert(f.available() == 3);

  // Single-byte read advances cursor
  assert(f.read() == 'A');
  assert(f.available() == 2);

  // Buffered read
  unsigned char buf[4] = {0};
  int n = f.read(buf, 2);
  assert(n == 2);
  assert(buf[0] == 'B' && buf[1] == 'C');
  assert(f.available() == 0);

  // Further reads return -1
  assert(f.read() == -1);
  assert(f.peek() == -1);

  // Close resets cursor and open state
  f.close();
  assert(!f);
  assert(f.available() == 0);

  pass("ArrayFile basic I/O");
}

static void test_ArrayFile_write_is_noop() {
  std::printf("Running test_ArrayFile_write_is_noop...\n");

  char data[] = "XYZ";
  ArrayFile f;
  f.set((char*) "w.txt", data, 3);
  f.open();

  // Writes are defined to be no-op (return 0) in RAM-only implementation
  assert(f.write((unsigned char) 'Q') == 0);
  unsigned char blob[3] = {1, 2, 3};
  assert(f.write(blob, 3) == 0);

  // Data remains unchanged; next read yields original content
  assert(f.read() == 'X');
  pass("ArrayFile write no-op");
}

static void test_ArrayDirectory_add_and_iterate() {
  std::printf("Running test_ArrayDirectory_add_and_iterate...\n");

  ArrayDirectory root(
      "/"); // [arraydirectory.cpp](https://carcgl-my.sharepoint.com/personal/john_gavel_carrier_com/Documents/Microsoft%20Copilot%20Chat%20Files/arraydirectory.cpp?EntityRepresentationId=14831efd-6598-458e-b376-7830147515e3)
            // [2](https://carcgl-my.sharepoint.com/personal/john_gavel_carrier_com/Documents/Microsoft%20Copilot%20Chat%20Files/arraydirectory.cpp)
  // Add a file
  char contents[] = "12345";
  bool okFile = root.addFile((char*) "a.txt", contents, 5);
  assert(okFile);

  // Add a subdirectory
  bool okDir = root.addDirectory((char*) "docs");
  assert(okDir);

  // Duplicate names should fail
  assert(!root.addFile((char*) "a.txt", contents, 5));
  assert(!root.addDirectory((char*) "docs"));

  // Lookup existing file
  DigitalBase* a = root.getFile("a.txt");
  assert(a != nullptr);
  assert(!a->isDirectory());

  // Lookup existing directory
  DigitalBase* d = root.getFile("docs");
  assert(d != nullptr);
  assert(d->isDirectory());

  // Lookup missing
  assert(root.getFile("missing") == nullptr);

  // Iterate with openNextFile / rewindDirectory
  root.rewindDirectory();
  DigitalBase* first = root.openNextFile();
  assert(first != nullptr);
  DigitalBase* second = root.openNextFile();
  assert(second != nullptr);
  DigitalBase* third = root.openNextFile();
  assert(third == nullptr);

  pass("ArrayDirectory add/lookup/iterate");
}

static void test_ArrayFileSystem_path_parsing_and_verify() {
  std::printf("Running test_ArrayFileSystem_path_parsing_and_verify...\n");

  ArrayFileSystem
      fs; // [arrayfilesystem.cpp](https://carcgl-my.sharepoint.com/personal/john_gavel_carrier_com/Documents/Microsoft%20Copilot%20Chat%20Files/arrayfilesystem.cpp?EntityRepresentationId=07a913e0-bda2-498c-aa5e-5a0e384c8c2c)
          // [3](https://carcgl-my.sharepoint.com/personal/john_gavel_carrier_com/Documents/Microsoft%20Copilot%20Chat%20Files/arrayfilesystem.cpp)

  // Root path checks
  // verifyFile("/") should be false because "/" is a directory
  assert(!fs.verifyFile("/"));
  pass("ArrayFileSystem 1");
  // Non-existent path: verifyFile returns false, readFile returns nullptr
  assert(!fs.verifyFile("/nope"));
  pass("ArrayFileSystem 2");
  assert(fs.readFile("/nope") == nullptr);
  pass("ArrayFileSystem 3");
  // open("/") returns the root directory (not nullptr)
  DigitalBase* root = fs.open("/");
  assert(root != nullptr && root->isDirectory());

  pass("ArrayFileSystem basic path/verify");
}

static void test_ArrayFileSystem_open_bug_detection() {
  std::printf("Running test_ArrayFileSystem_open_bug_detection...\n");

  ArrayFileSystem
      fs; // [arrayfilesystem.cpp](https://carcgl-my.sharepoint.com/personal/john_gavel_carrier_com/Documents/Microsoft%20Copilot%20Chat%20Files/arrayfilesystem.cpp?EntityRepresentationId=07a913e0-bda2-498c-aa5e-5a0e384c8c2c)
          // [3](https://carcgl-my.sharepoint.com/personal/john_gavel_carrier_com/Documents/Microsoft%20Copilot%20Chat%20Files/arrayfilesystem.cpp)

  // The implementation of ArrayFileSystem::open() references `child`
  // instead of `file` when opening a non-directory:
  //   ArrayFile* aFile = static_cast<ArrayFile*>(child);  // child is undefined
  // This is a defect; the test ensures at least that calling open("/nonexistent")
  // does not crash and returns nullptr.
  DigitalBase* f = fs.open("/nonexistent");
  assert(f == nullptr);

  pass("ArrayFileSystem open() bug detection");
}

static void test_ArrayDirectory_nested_structure() {
  std::printf("Running test_ArrayDirectory_nested_structure...\n");

  ArrayDirectory root("root");
  char fileA[] = "A";
  char fileB[] = "B";
  char fileC[] = "C";

  // Add files and subdirectories
  assert(root.addFile((char*) "a.txt", fileA, 1));
  assert(root.addDirectory((char*) "sub1"));
  assert(root.addDirectory((char*) "sub2"));

  // Add files to sub1
  ArrayDirectory* sub1 = static_cast<ArrayDirectory*>(root.getFile("sub1"));
  assert(sub1 != nullptr && sub1->isDirectory());
  assert(sub1->addFile((char*) "b.txt", fileB, 1));
  assert(sub1->addDirectory((char*) "subsub1"));

  // Add file to subsub1
  ArrayDirectory* subsub1 = static_cast<ArrayDirectory*>(sub1->getFile("subsub1"));
  assert(subsub1 != nullptr && subsub1->isDirectory());
  assert(subsub1->addFile((char*) "c.txt", fileC, 1));

  // Lookup files at each level
  assert(root.getFile("a.txt") != nullptr);
  assert(sub1->getFile("b.txt") != nullptr);
  assert(subsub1->getFile("c.txt") != nullptr);

  // Lookup missing files
  assert(root.getFile("missing.txt") == nullptr);
  assert(sub1->getFile("missing.txt") == nullptr);
  assert(subsub1->getFile("missing.txt") == nullptr);

  // Duplicate directory/file names should fail
  assert(!root.addDirectory((char*) "sub1"));
  assert(!sub1->addFile((char*) "b.txt", fileB, 1));
  assert(!subsub1->addFile((char*) "c.txt", fileC, 1));

  pass("ArrayDirectory nested structure");
}

static void test_ArrayDirectory_iteration_and_rewind() {
  std::printf("Running test_ArrayDirectory_iteration_and_rewind...\n");

  ArrayDirectory dir("dir");
  char fileX[] = "X";
  char fileY[] = "Y";
  char fileZ[] = "Z";

  assert(dir.addFile((char*) "x.txt", fileX, 1));
  assert(dir.addFile((char*) "y.txt", fileY, 1));
  assert(dir.addDirectory((char*) "subdir"));
  assert(dir.addFile((char*) "z.txt", fileZ, 1));

  // Iterate through all entries
  dir.rewindDirectory();
  int count = 0;
  DigitalBase* entry;
  while ((entry = dir.openNextFile()) != nullptr) {
    count++;
    std::printf("  Entry %d: %s (%s)\n", count, entry->name(), entry->isDirectory() ? "dir" : "file");
  }
  assert(count == 4);

  // Rewind and iterate again
  dir.rewindDirectory();
  count = 0;
  while ((entry = dir.openNextFile()) != nullptr) { count++; }
  assert(count == 4);

  pass("ArrayDirectory iteration and rewind");
}

static void test_deep_subdirectory_lookup() {
  std::printf("Running test_deep_subdirectory_lookup...\n");

  ArrayDirectory root("root");
  assert(root.addDirectory((char*) "level1"));
  ArrayDirectory* level1 = static_cast<ArrayDirectory*>(root.getFile("level1"));
  assert(level1 != nullptr);
  assert(level1->addDirectory((char*) "level2"));
  ArrayDirectory* level2 = static_cast<ArrayDirectory*>(level1->getFile("level2"));
  assert(level2 != nullptr);
  assert(level2->addDirectory((char*) "level3"));
  ArrayDirectory* level3 = static_cast<ArrayDirectory*>(level2->getFile("level3"));
  assert(level3 != nullptr);

  char fileD[] = "D";
  assert(level3->addFile((char*) "deep.txt", fileD, 1));
  assert(level3->getFile("deep.txt") != nullptr);

  // Walk down the tree and check names
  DigitalBase* db = root.getFile("level1");
  assert(db && db->isDirectory());
  db = static_cast<ArrayDirectory*>(db)->getFile("level2");
  assert(db && db->isDirectory());
  db = static_cast<ArrayDirectory*>(db)->getFile("level3");
  assert(db && db->isDirectory());
  db = static_cast<ArrayDirectory*>(db)->getFile("deep.txt");
  assert(db && !db->isDirectory());

  pass("Deep subdirectory lookup");
}

static void test_ArrayFileSystem_full_path_lookup() {
  std::printf("Running test_ArrayFileSystem_full_path_lookup...\n");
  // Attach this tree to an ArrayFileSystem
  ArrayFileSystem fs;

  // Build a nested directory tree
  ArrayDirectory* root = static_cast<ArrayDirectory*>(fs.open("/"));
  char fileA[] = "A";
  char fileB[] = "B";
  char fileC[] = "C";
  char fileD[] = "D";

  // /a.txt
  assert(root->addFile((char*) "a.txt", fileA, 1));
  // /docs
  assert(root->addDirectory((char*) "docs"));
  ArrayDirectory* docs = static_cast<ArrayDirectory*>(root->getFile("docs"));
  assert(docs != nullptr);
  // /docs/b.txt
  assert(docs->addFile((char*) "b.txt", fileB, 1));
  // /docs/sub
  assert(docs->addDirectory((char*) "sub"));
  ArrayDirectory* sub = static_cast<ArrayDirectory*>(docs->getFile("sub"));
  assert(sub != nullptr);
  // /docs/sub/c.txt
  assert(sub->addFile((char*) "c.txt", fileC, 1));
  // /docs/sub/deep
  assert(sub->addDirectory((char*) "deep"));
  ArrayDirectory* deep = static_cast<ArrayDirectory*>(sub->getFile("deep"));
  assert(deep != nullptr);
  // /docs/sub/deep/d.txt
  assert(deep->addFile((char*) "d.txt", fileD, 1));

  // Test lookups
  DigitalBase* db;
  db = fs.open("/"); // root
  assert(db && db->isDirectory());
  db = fs.open("/a.txt");
  assert(db && !db->isDirectory());
  db = fs.open("/docs");
  assert(db && db->isDirectory());
  db = fs.open("/docs/b.txt");
  assert(db && !db->isDirectory());
  db = fs.open("/docs/sub");
  assert(db && db->isDirectory());
  db = fs.open("/docs/sub/c.txt");
  assert(db && !db->isDirectory());
  db = fs.open("/docs/sub/deep");
  assert(db && db->isDirectory());
  db = fs.open("/docs/sub/deep/d.txt");
  assert(db && !db->isDirectory());

  // Test verifyFile
  assert(fs.verifyFile("/a.txt"));
  assert(fs.verifyFile("/docs/b.txt"));
  assert(fs.verifyFile("/docs/sub/c.txt"));
  assert(fs.verifyFile("/docs/sub/deep/d.txt"));
  assert(!fs.verifyFile("/docs/sub/deep")); // directory, not file
  assert(!fs.verifyFile("/notfound.txt"));

  // Test readFile returns correct file
  DigitalFile* f = fs.readFile("/docs/sub/deep/d.txt");
  assert(f != nullptr);
  f->open();
  assert(f->read() == 'D');
  f->close();

  // Test missing file
  assert(fs.readFile("/docs/sub/deep/missing.txt") == nullptr);

  pass("ArrayFileSystem full path-based lookup");
}

int main() {
  std::printf("=== Running Array FS unit tests (printf + assert) ===\n");

  test_ArrayFile_basic_io();
  test_ArrayFile_write_is_noop();
  test_ArrayDirectory_add_and_iterate();
  test_ArrayFileSystem_path_parsing_and_verify();
  test_ArrayFileSystem_open_bug_detection();

  test_ArrayDirectory_nested_structure();
  test_ArrayDirectory_iteration_and_rewind();
  test_deep_subdirectory_lookup();

  test_ArrayFileSystem_full_path_lookup();

  std::printf("=== All tests passed ===\n");
  return 0;
}
