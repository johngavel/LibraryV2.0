#include "../src/datastructure.cpp"
#include "../src/datastructure.h"

// tests/test_classic_sort_list.cpp
// Comprehensive tests for ClassicSortList using printf + assert (no test framework)

#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>

// ---------- Small assertion helpers to keep outputs readable ---------------
#define ASSERT_TRUE_MSG(cond, msg)                                                                                                                             \
  do {                                                                                                                                                         \
    if (!(cond)) {                                                                                                                                             \
      printf("ASSERT_TRUE failed: %s (line %d): %s\n", __FILE__, __LINE__, msg);                                                                               \
      assert(cond);                                                                                                                                            \
    }                                                                                                                                                          \
  } while (0)

#define ASSERT_FALSE_MSG(cond, msg) ASSERT_TRUE_MSG(!(cond), msg)

#define ASSERT_EQ_UL(a, b)                                                                                                                                     \
  do {                                                                                                                                                         \
    unsigned long _va = (unsigned long) (a);                                                                                                                   \
    unsigned long _vb = (unsigned long) (b);                                                                                                                   \
    if (!(_va == _vb)) {                                                                                                                                       \
      printf("ASSERT_EQ_UL failed: %s (line %d): %lu != %lu\n", __FILE__, __LINE__, _va, _vb);                                                                 \
      assert(_va == _vb);                                                                                                                                      \
    }                                                                                                                                                          \
  } while (0)

#define ASSERT_EQ_INT(a, b)                                                                                                                                    \
  do {                                                                                                                                                         \
    int _va = (int) (a);                                                                                                                                       \
    int _vb = (int) (b);                                                                                                                                       \
    if (!(_va == _vb)) {                                                                                                                                       \
      printf("ASSERT_EQ_INT failed: %s (line %d): %d != %d\n", __FILE__, __LINE__, _va, _vb);                                                                  \
      assert(_va == _vb);                                                                                                                                      \
    }                                                                                                                                                          \
  } while (0)

#define ASSERT_EQ_VEC_INT(vec, ...)                                                                                                                            \
  do {                                                                                                                                                         \
    int expected[] = {__VA_ARGS__};                                                                                                                            \
    size_t n = sizeof(expected) / sizeof(expected[0]);                                                                                                         \
    if (vec.size() != n) {                                                                                                                                     \
      printf("ASSERT_EQ_VEC_INT size failed: %s (line %d): got %zu, expected %zu\n", __FILE__, __LINE__, vec.size(), n);                                       \
      assert(vec.size() == n);                                                                                                                                 \
    }                                                                                                                                                          \
    for (size_t i = 0; i < n; ++i) {                                                                                                                           \
      if (vec[i] != expected[i]) {                                                                                                                             \
        printf("ASSERT_EQ_VEC_INT mismatch at %zu: got %d, expected %d\n", i, vec[i], expected[i]);                                                            \
        assert(vec[i] == expected[i]);                                                                                                                         \
      }                                                                                                                                                        \
    }                                                                                                                                                          \
  } while (0)

// ---------- Comparators -----------------------------------------------------
static int cmp_int_asc(const void* a, const void* b) {
  const int ia = *static_cast<const int*>(a);
  const int ib = *static_cast<const int*>(b);
  return (ia > ib) - (ia < ib);
}

static int cmp_int_desc(const void* a, const void* b) {
  const int ia = *static_cast<const int*>(a);
  const int ib = *static_cast<const int*>(b);
  return (ib > ia) - (ib < ia);
}

struct Pair {
  int key;
  int value;
};

static int cmp_pair_key_then_val(const void* a, const void* b) {
  const Pair& pa = *static_cast<const Pair*>(a);
  const Pair& pb = *static_cast<const Pair*>(b);
  if (pa.key != pb.key) return (pa.key > pb.key) - (pa.key < pb.key);
  return (pa.value > pb.value) - (pa.value < pb.value);
}

static int cmp_pair_by_value(const void* a, const void* b) {
  const Pair& pa = *static_cast<const Pair*>(a);
  const Pair& pb = *static_cast<const Pair*>(b);
  return (pa.value > pb.value) - (pa.value < pb.value);
}

// ---------- Small helpers ---------------------------------------------------
template <typename T> static void push_all(ClassicSortList& list, const std::vector<T>& data) {
  for (const auto& v : data) {
    bool ok = list.push((void*) &v);
    ASSERT_TRUE_MSG(ok, "push failed");
  }
  ASSERT_EQ_UL(list.count(), data.size());
}

template <typename T> static std::vector<T> read_all(ClassicSortList& list) {
  std::vector<T> out(list.count());
  for (unsigned long i = 0; i < list.count(); ++i) {
    bool ok = list.get(i, (void*) &out[i]);
    ASSERT_TRUE_MSG(ok, "get failed");
  }
  return out;
}

// ---------- Each test is a function; main calls them in order ---------------
static void Test_Construct_Basics() {
  printf("Test_Construct_Basics\n");
  ClassicSortList list(10UL, sizeof(int));
  ASSERT_FALSE_MSG(list.error(), "unexpected error after construction");
  ASSERT_TRUE_MSG(list.empty(), "list should be empty");
  ASSERT_EQ_UL(list.count(), 0UL);
  ASSERT_EQ_UL(list.highWaterMark(), 0UL);
}

static void Test_CapacityZero_Path() {
  printf("Test_CapacityZero_Path\n");
  ClassicSortList list(0UL, sizeof(int)); // memError path in ClassicStack
  int x = 1;
  ASSERT_FALSE_MSG(list.push(&x), "push should fail when capacity==0");
  ASSERT_TRUE_MSG(list.sort(cmp_int_asc), "sort(cmp) should pass");
  ASSERT_TRUE_MSG(list.error(), "error flag should be set");
}

static void Test_Push_Get_Count_HWM() {
  printf("Test_Push_Get_Count_HWM\n");
  ClassicSortList list(5UL, sizeof(int));
  int a = 7, b = 3, c = 9;
  ASSERT_TRUE_MSG(list.push(&a), "push a");
  ASSERT_TRUE_MSG(list.push(&b), "push b");
  ASSERT_TRUE_MSG(list.push(&c), "push c");
  ASSERT_EQ_UL(list.count(), 3UL);
  ASSERT_EQ_UL(list.highWaterMark(), 3UL);
  ASSERT_EQ_UL(list.highWaterMark(), 0UL); // reset after read

  int tmp = 0;
  ASSERT_TRUE_MSG(list.get(0, &tmp), "get(0)");
  ASSERT_EQ_INT(tmp, 7);
  ASSERT_TRUE_MSG(list.get(1, &tmp), "get(1)");
  ASSERT_EQ_INT(tmp, 3);
  ASSERT_TRUE_MSG(list.get(2, &tmp), "get(2)");
  ASSERT_EQ_INT(tmp, 9);

  ASSERT_FALSE_MSG(list.get(3, &tmp), "get out-of-range should fail");
  ASSERT_TRUE_MSG(list.error(), "listError should be set after OOB get");
}

static void Test_Clear_Allows_Reuse() {
  printf("Test_Clear_Allows_Reuse\n");
  ClassicSortList list(4UL, sizeof(int));
  int v = 42;
  ASSERT_TRUE_MSG(list.push(&v), "push v");
  ASSERT_EQ_UL(list.count(), 1UL);
  list.clear();
  ASSERT_TRUE_MSG(list.empty(), "clear should make empty");
  ASSERT_TRUE_MSG(list.push(&v), "push after clear");
  ASSERT_EQ_UL(list.count(), 1UL);
}

static void Test_SetSortFunction_And_Sort_Overloads() {
  printf("Test_SetSortFunction_And_Sort_Overloads\n");
  {
    ClassicSortList list(3UL, sizeof(int));
    ASSERT_FALSE_MSG(list.setSortFunction(nullptr), "setSortFunction(nullptr) should fail");
    ASSERT_TRUE_MSG(list.error(), "error after null comparator");
  }
  {
    ClassicSortList list(3UL, sizeof(int));
    push_all<int>(list, {2, 1});
    ASSERT_FALSE_MSG(list.sort(), "sort() without stored comparator should fail");
    ASSERT_TRUE_MSG(list.error(), "error flag expected");
  }
  {
    ClassicSortList list(8UL, sizeof(int));
    push_all<int>(list, {5, 1, 4, 2, 8, 7, 6, 3});
    ASSERT_TRUE_MSG(list.setSortFunction(&cmp_int_asc), "set cmp asc");
    ASSERT_TRUE_MSG(list.sort(), "sort() should succeed with stored cmp");
    auto got = read_all<int>(list);
    ASSERT_EQ_VEC_INT(got, 1, 2, 3, 4, 5, 6, 7, 8);
  }
  {
    ClassicSortList list(8UL, sizeof(int));
    push_all<int>(list, {5, 1, 4, 2, 8, 7, 6, 3});
    ASSERT_TRUE_MSG(list.sort(&cmp_int_desc), "sort one-off desc");
    auto got = read_all<int>(list);
    ASSERT_EQ_VEC_INT(got, 8, 7, 6, 5, 4, 3, 2, 1);
  }
  {
    ClassicSortList list(4UL, sizeof(int));
    ASSERT_TRUE_MSG(list.sort(&cmp_int_asc), "sorting empty should succeed");
    ASSERT_EQ_UL(list.count(), 0UL);
  }
  {
    ClassicSortList list(4UL, sizeof(int));
    int x = 9;
    ASSERT_TRUE_MSG(list.push(&x), "push single");
    ASSERT_TRUE_MSG(list.sort(&cmp_int_asc), "sorting single should succeed");
    auto got = read_all<int>(list);
    ASSERT_EQ_UL(got.size(), 1UL);
    ASSERT_EQ_INT(got[0], 9);
  }
}

static void Test_Sort_Structs_And_Comparator_Differences() {
  printf("Test_Sort_Structs_And_Comparator_Differences\n");
  {
    ClassicSortList list(6UL, sizeof(Pair));
    push_all<Pair>(list, {{2, 99}, {1, 10}, {1, 5}, {2, 1}, {1, 10}, {2, 1}});
    ASSERT_TRUE_MSG(list.sort(&cmp_pair_key_then_val), "sort pairs by key then value");
    // Verify non-decreasing in (key, value)
    Pair prev{-2147483647, -2147483647};
    for (unsigned long i = 0; i < list.count(); ++i) {
      Pair cur{};
      ASSERT_TRUE_MSG(list.get(i, &cur), "get pair");
      bool ok = (prev.key < cur.key) || (prev.key == cur.key && prev.value <= cur.value);
      ASSERT_TRUE_MSG(ok, "pairs not in non-decreasing (key,value)");
      prev = cur;
    }
  }
  {
    ClassicSortList listA(5UL, sizeof(Pair));
    ClassicSortList listB(5UL, sizeof(Pair));
    std::vector<Pair> data = {{3, 1}, {2, 5}, {3, 0}, {1, 2}, {2, 2}};
    push_all<Pair>(listA, data);
    push_all<Pair>(listB, data);
    ASSERT_TRUE_MSG(listA.sort(&cmp_pair_by_value), "sort by value");
    ASSERT_TRUE_MSG(listB.sort(&cmp_pair_key_then_val), "sort by key/value");

    bool identical = true;
    for (unsigned long i = 0; i < listA.count(); ++i) {
      Pair a{}, b{};
      ASSERT_TRUE_MSG(listA.get(i, &a), "get A");
      ASSERT_TRUE_MSG(listB.get(i, &b), "get B");
      if (a.key != b.key || a.value != b.value) {
        identical = false;
        break;
      }
    }
    ASSERT_FALSE_MSG(identical, "orders should differ for different comparators");
  }
}

static void Test_Swap_Behavior() {
  printf("Test_Swap_Behavior\n");
  {
    ClassicSortList list(4UL, sizeof(int));
    push_all<int>(list, {10, 20, 30, 40});
    ASSERT_TRUE_MSG(list.swap(1, 2), "swap 1<->2");
    auto got = read_all<int>(list);
    ASSERT_EQ_VEC_INT(got, 10, 30, 20, 40);
  }
  {
    ClassicSortList list(3UL, sizeof(int));
    push_all<int>(list, {1, 2, 3});
    ASSERT_TRUE_MSG(list.swap(2, 2), "swap same index should be no-op");
    auto got = read_all<int>(list);
    ASSERT_EQ_VEC_INT(got, 1, 2, 3);
  }
  {
    ClassicSortList list(3UL, sizeof(int));
    push_all<int>(list, {1, 2, 3});
    ASSERT_FALSE_MSG(list.swap(0, 3), "swap out-of-range should fail");
    ASSERT_TRUE_MSG(list.error(), "error flag set after bad swap");
  }
  {
    ClassicSortList list(3UL, sizeof(int));
    push_all<int>(list, {1, 2, 3});
    ASSERT_TRUE_MSG(list.swap(0, 2), "swap 0,2");
    ASSERT_TRUE_MSG(list.swap(0, 2), "swap back 0,2");
    auto got = read_all<int>(list);
    ASSERT_EQ_VEC_INT(got, 1, 2, 3);
  }
}

static void Test_Pop_And_Sort() {
  printf("Test_Pop_And_Sort\n");
  ClassicSortList list(6UL, sizeof(int));
  push_all<int>(list, {5, 4, 3, 2, 1});
  int popped = -1;
  ASSERT_TRUE_MSG(list.pop(&popped), "pop last pushed");
  ASSERT_EQ_INT(popped, 1); // ClassicStack is LIFO
  ASSERT_EQ_UL(list.count(), 4UL);
  ASSERT_TRUE_MSG(list.sort(&cmp_int_asc), "sort ascending after pop");
  auto got = read_all<int>(list);
  ASSERT_EQ_VEC_INT(got, 2, 3, 4, 5);
}

static void Test_AlreadySorted_And_ReverseSorted() {
  printf("Test_AlreadySorted_And_ReverseSorted\n");
  {
    ClassicSortList list(6UL, sizeof(int));
    push_all<int>(list, {1, 2, 3, 4, 5, 6});
    ASSERT_TRUE_MSG(list.sort(&cmp_int_asc), "sort ascending (already sorted)");
    auto got = read_all<int>(list);
    ASSERT_EQ_VEC_INT(got, 1, 2, 3, 4, 5, 6);
  }
  {
    ClassicSortList list(6UL, sizeof(int));
    push_all<int>(list, {6, 5, 4, 3, 2, 1});
    ASSERT_TRUE_MSG(list.sort(&cmp_int_asc), "sort ascending (reverse input)");
    auto got = read_all<int>(list);
    ASSERT_EQ_VEC_INT(got, 1, 2, 3, 4, 5, 6);
  }
}

static void Test_Full_Push_Fails() {
  printf("Test_Full_Push_Fails\n");
  ClassicSortList list(3UL, sizeof(int));
  int a = 1, b = 2, c = 3, d = 4;
  ASSERT_TRUE_MSG(list.push(&a), "push a");
  ASSERT_TRUE_MSG(list.push(&b), "push b");
  ASSERT_TRUE_MSG(list.push(&c), "push c");
  ASSERT_TRUE_MSG(list.full(), "should be full");
  ASSERT_FALSE_MSG(list.push(&d), "push beyond capacity should fail");
  ASSERT_TRUE_MSG(list.error(), "error flag set on overflow push");
}

int main() {
  printf("=== Running ClassicSortList tests (printf + assert) ===\n");
  Test_Construct_Basics();
  Test_CapacityZero_Path();
  Test_Push_Get_Count_HWM();
  Test_Clear_Allows_Reuse();
  Test_SetSortFunction_And_Sort_Overloads();
  Test_Sort_Structs_And_Comparator_Differences();
  Test_Swap_Behavior();
  Test_Pop_And_Sort();
  Test_AlreadySorted_And_ReverseSorted();
  Test_Full_Push_Fails();
  printf("=== All ClassicSortList tests passed. ===\n");
  return 0;
}
