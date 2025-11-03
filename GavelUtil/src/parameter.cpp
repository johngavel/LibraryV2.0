#include "parameter.h"

ParameterList* ParameterList::list = nullptr;

ParameterList* ParameterList::get() {
  if (list == nullptr) list = new ParameterList();
  return list;
}

int ParameterList::getCount() {
  return parameterCount;
}

void ParameterList::clear() {
  parameterCount = 0;
}

void ParameterList::addParameter(char* parameter, char* value) {
  if (parameterCount < MAX_PARAMETERS) {
    strncpy(parameters[parameterCount].parameter, parameter, MAX_PARAMETER_LENGTH);
    strncpy(parameters[parameterCount].value, value, MAX_VALUE_LENGTH);
    parameterCount++;
  }
}

char* ParameterList::getParameter(int index) {
  return parameters[index].parameter;
}

char* ParameterList::getValue(int index) {
  return parameters[index].value;
}
