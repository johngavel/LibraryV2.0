#ifndef __GAVEL_TEST_TERMINAL_H
#define __GAVEL_TEST_TERMINAL_H

enum TYPE { INFO, ERROR };
class OutputInterface {
public:
  void print(TYPE t, char* s) {};
  void println(TYPE t, char* s) {};
};
#endif //