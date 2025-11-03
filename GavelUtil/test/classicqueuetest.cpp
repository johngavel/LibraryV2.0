#include "../src/datastructure.cpp"
#include "../src/datastructure.h"

#include <cassert>
#include <cstdio>
#include <cstring>

class Task {
public:
  Task(int i) : id(i){};
  int id = -1;
  char name[20] = "task";
};

void testClassicQueue() {
  printf("Testing ClassicQueue...\n");
  const unsigned long CAPACITY = 3;
  ClassicQueue queue(CAPACITY, sizeof(int));

  // Test empty queue
  assert(queue.empty());
  assert(!queue.full());
  assert(queue.count() == 0);
  printf("Empty queue checks passed.\n");

  // Push elements
  int val1 = 10, val2 = 20, val3 = 30;
  assert(queue.push(&val1));
  assert(queue.push(&val2));
  assert(queue.push(&val3));
  assert(queue.full());
  assert(queue.count() == CAPACITY);
  printf("Push and full checks passed.\n");

  // Try pushing to full queue
  int val4 = 40;
  assert(!queue.push(&val4));
  printf("Push to full queue check passed.\n");

  // Get elements by index
  int out;
  assert(queue.get(0, &out) && out == val1);
  assert(queue.get(1, &out) && out == val2);
  assert(queue.get(2, &out) && out == val3);
  printf("Get by index checks passed.\n");

  // Pop elements
  assert(queue.pop(&out) && out == val1);
  assert(queue.count() == 2);
  assert(!queue.full());
  assert(queue.pop(&out) && out == val2);
  assert(queue.pop(&out) && out == val3);
  assert(queue.empty());
  printf("Pop checks passed.\n");

  // Pop from empty queue
  assert(!queue.pop(&out));
  printf("Pop from empty queue check passed.\n");

  // Clear and reuse
  queue.push(&val1);
  queue.push(&val2);
  queue.clear();
  assert(queue.empty());
  printf("Clear and reuse checks passed.\n");

  // My Test
  unsigned char memory[5 * sizeof(Task*)];
  ClassicQueue q(5, sizeof(Task*), memory);
  Task t1 = Task(1);
  Task t2 = Task(2);
  Task* t1_ptr = &t1;
  Task* t2_ptr = &t2;
  Task* t_ptr = nullptr;
  q.push(&t1_ptr);
  q.push(&t2_ptr);
  q.get(0, &t_ptr);
  assert(t1_ptr == t_ptr);
  q.get(1, &t_ptr);
  assert(t2_ptr == t_ptr);
  printf("ClassicQueue pointer tests passed. \n");

  printf("All ClassicQueue tests passed!\n");
}

void testClassicStack() {
  printf("Testing ClassicStack...\n");
  const unsigned long CAPACITY = 3;
  ClassicStack stack(CAPACITY, sizeof(int));

  // Test empty stack
  assert(stack.empty());
  assert(!stack.full());
  assert(stack.count() == 0);
  printf("Empty stack checks passed.\n");

  // Push elements
  int val1 = 100, val2 = 200, val3 = 300;
  assert(stack.push(&val1));
  assert(stack.push(&val2));
  assert(stack.push(&val3));
  assert(stack.full());
  assert(stack.count() == CAPACITY);
  printf("Push and full checks passed.\n");

  // Try pushing to full stack
  int val4 = 400;
  assert(!stack.push(&val4));
  printf("Push to full stack check passed.\n");

  // Get elements by index
  int out;
  assert(stack.get(0, &out) && out == val1);
  assert(stack.get(1, &out) && out == val2);
  assert(stack.get(2, &out) && out == val3);
  printf("Get by index checks passed.\n");

  // Pop elements (LIFO)
  assert(stack.pop(&out) && out == val3);
  assert(stack.count() == 2);
  assert(!stack.full());
  assert(stack.pop(&out) && out == val2);
  assert(stack.pop(&out) && out == val1);
  assert(stack.empty());
  printf("Pop checks passed.\n");

  // Pop from empty stack
  assert(!stack.pop(&out));
  printf("Pop from empty stack check passed.\n");

  // Clear and reuse
  stack.push(&val1);
  stack.push(&val2);
  stack.clear();
  assert(stack.empty());
  printf("Clear and reuse checks passed.\n");

  printf("All ClassicStack tests passed!\n");
}

int main() {
  testClassicQueue();
  testClassicStack();
}