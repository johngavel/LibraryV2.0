#include "../src/idgenerator.h"

#include <cassert>
#include <cstdio>
#include <type_traits>
#include <utility>
#include <vector>

// A simple derived type for tests
struct Thing : public Identifiable {
  explicit Thing(const char* name_ = "") : Identifiable(), name(name_) {}
  const char* name;
};

// Helper for printing pass/fail banners
void print_banner(const char* title) {
  printf("\n==== %s ====\n", title);
}

// ---- Test 1: Sequential construction yields sequential IDs starting from 1000
void test_sequential_construction() {
  print_banner("Test 1: Sequential construction");

  Thing a("a");
  Thing b("b");
  Thing c("c");

  printf("IDs: a=%d, b=%d, c=%d\n", a.getId(), b.getId(), c.getId());

  // Expect starting at 1000, 1001, 1002
  assert(a.getId() == 1000);
  assert(b.getId() == 1001);
  assert(c.getId() == 1002);
}

// ---- Test 2: getId() is stable and does not change post-construction
void test_getId_stability() {
  print_banner("Test 2: getId stability");

  Thing t("t");
  int id1 = t.getId();
  int id2 = t.getId();
  int id3 = t.getId();

  printf("Repeated getId(): %d, %d, %d\n", id1, id2, id3);

  assert(id1 == id2);
  assert(id2 == id3);
}

// ---- Test 3: Large batch creation is unique and strictly increasing
void test_batch_uniqueness_and_monotonicity() {
  print_banner("Test 3: Batch uniqueness & monotonicity");

  const int N = 10000;
  std::vector<Thing> objs;
  objs.reserve(N);

  for (int i = 0; i < N; ++i) {
    objs.emplace_back("batch");
    if (i > 0) {
      // strictly increasing
      assert(objs[i].getId() == objs[i - 1].getId() + 1);
    }
  }

  int first = objs.front().getId();
  int last = objs.back().getId();

  printf("Batch first ID=%d, last ID=%d, count=%d\n", first, last, N);

  // Useful sanity bounds: last should be first + (N-1)
  assert(last == first + (N - 1));
}

// ---- Test 4: Direct IdGenerator calls interleave correctly with Identifiable
void test_direct_generator_calls() {
  print_banner("Test 4: Direct IdGenerator interleaving");

  // Grab a few IDs directly
  int g1 = IdGenerator::generateId(); // Next after previous tests
  int g2 = IdGenerator::generateId();
  Thing x("x");
  int g3 = IdGenerator::generateId();
  Thing y("y");

  printf("Direct IDs g1=%d, g2=%d, then x=%d, then g3=%d, then y=%d\n", g1, g2, x.getId(), g3, y.getId());

  // Check strict monotonicity across mixed sources
  assert(g2 == g1 + 1);
  assert(x.getId() == g2 + 1);
  assert(g3 == x.getId() + 1);
  assert(y.getId() == g3 + 1);
}

// ---- Test 5: Copy is deleted; Move preserves ID (runtime) + compile-time checks
void test_copy_move_semantics() {
  print_banner("Test 5: Copy deleted; move preserves ID");

  // Compile-time checks (do not require runtime, but useful and safe)
  static_assert(!std::is_copy_constructible<Identifiable>::value, "Identifiable should not be copy-constructible");
  static_assert(!std::is_copy_assignable<Identifiable>::value, "Identifiable should not be copy-assignable");

  // Move should be allowed (implicitly declared) and should preserve the ID value in the moved-to object.
  static_assert(std::is_move_constructible<Thing>::value, "Thing should be move-constructible");
  static_assert(std::is_move_assignable<Thing>::value, "Thing should be move-assignable");

  Thing a("a");
  int original = a.getId();

  Thing b = std::move(a); // Move construct
  printf("After move-construct: original ID=%d, moved-to ID=%d\n", original, b.getId());
  assert(b.getId() == original);

  Thing c("c");
  int c_before = c.getId();
  c = std::move(b); // Move assign
  printf("After move-assign: source ID=%d (moved-from), dest ID=%d (was %d)\n", b.getId(), c.getId(), c_before);

  // After move-assign, the destination should now carry the moved ID.
  assert(c.getId() == original);

  // NOTE: The moved-from object's internals are valid but unspecified; we don't assert on b.getId().
}

// ---- Test 6: Polymorphic/inheritance sanity (IDs remain unique across derived types)
struct Widget : public Identifiable {
  Widget() : Identifiable() {}
};

struct Gizmo : public Identifiable {
  Gizmo() : Identifiable() {}
};

void test_inheritance_uniqueness() {
  print_banner("Test 6: Inheritance uniqueness");

  Widget w1, w2;
  Gizmo g1, g2, g3;

  printf("Widget IDs: %d, %d; Gizmo IDs: %d, %d, %d\n", w1.getId(), w2.getId(), g1.getId(), g2.getId(), g3.getId());

  // Strictly increasing across all constructions
  assert(w2.getId() == w1.getId() + 1);
  assert(g1.getId() == w2.getId() + 1);
  assert(g2.getId() == g1.getId() + 1);
  assert(g3.getId() == g2.getId() + 1);
}

// ---- Test 7: Basic boundary sanity (no wraparound in practical ranges)
// We won't reach INT_MAX here, but we ensure increments behave sanely over a chunk.
void test_boundary_sanity() {
  print_banner("Test 7: Boundary sanity sample");

  // Take a few steps forward and ensure arithmetic behaves nominally.
  int b1 = IdGenerator::generateId();
  int b2 = IdGenerator::generateId();
  int b3 = IdGenerator::generateId();

  printf("Boundary sample IDs: %d, %d, %d\n", b1, b2, b3);

  assert(b2 == b1 + 1);
  assert(b3 == b2 + 1);
}

int main() {
  printf("Running IdGenerator / Identifiable tests...\n");

  test_sequential_construction();
  test_getId_stability();
  test_batch_uniqueness_and_monotonicity();
  test_direct_generator_calls();
  test_copy_move_semantics();
  test_inheritance_uniqueness();
  test_boundary_sanity();

  printf("\nAll tests completed successfully.\n");
  return 0;
}