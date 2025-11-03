#include "../src/callback.h" // Include your CallbackManager header

#include <cassert>
#include <cstdio>

// Global counters for testing
static int counter = 0;

void freeFunction() {
  printf("Free function called!\n");
  counter++;
}

int main() {
  printf("=== Callback Unit Tests ===\n");

  Callback manager;

  // Test 1: Initial size should be 0
  assert(manager.size() == 0);
  printf("[PASS] Initial size is 0\n");

  // Test 2: Add a free function
  manager.addCallback(freeFunction);
  assert(manager.size() == 1);
  printf("[PASS] Added free function, size = %zu\n", manager.size());

  // Test 3: Add a lambda
  manager.addCallback([]() {
    printf("Lambda called!\n");
    counter += 2;
  });
  assert(manager.size() == 2);
  printf("[PASS] Added lambda, size = %zu\n", manager.size());

  // Test 4: Trigger callbacks
  counter = 0;
  manager.trigger();
  assert(counter == 3); // freeFunction (1) + lambda (2)
  printf("[PASS] Trigger executed all callbacks, counter = %d\n", counter);

  // Test 5: Clear callbacks
  manager.clearCallbacks();
  assert(manager.size() == 0);
  printf("[PASS] Cleared callbacks, size = %zu\n", manager.size());

  printf("=== All tests passed! ===\n");
  return 0;
}