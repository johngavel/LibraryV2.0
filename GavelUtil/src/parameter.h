#ifndef __GAVEL_PARAMETER_H
#define __GAVEL_PARAMETER_H

#include <Arduino.h>

#define MAX_PARAMETERS 150
#define MAX_PARAMETER_LENGTH 32
#define MAX_VALUE_LENGTH 32
#define LIST ParameterList::get()

class ParameterList {
public:
  class Parameter {
  public:
    Parameter() : parameter(""), value(""){};
    char parameter[MAX_PARAMETER_LENGTH];
    char value[MAX_VALUE_LENGTH];
  };
  static ParameterList* get();
  void clear();
  void addParameter(char* parameter, char* value);
  int getCount();
  char* getParameter(int index);
  char* getValue(int index);

private:
  ParameterList() : parameterCount(0){};
  static ParameterList* list;
  Parameter parameters[MAX_PARAMETERS];
  int parameterCount;
};

#endif // __GAVEL_PARAMETER_H
