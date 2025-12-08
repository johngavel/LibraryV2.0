#include "taskmanager.h"

#include "asciitable/asciitable.h"
#include "idle.h"

#include <GavelProgram.h>
#include <GavelUtil.h>

TaskManager::TaskManager() : Task("TaskManager"), queue(20, sizeof(Task*)) {
  id = TASK_MANAGER_ID;
};

void TaskManager::reservePins(BackendPinSetup* pinsetup) {
  if (pinsetup != nullptr) {
    for (unsigned long i = 0; i < queue.count(); i++) { getTask(i)->reservePins(pinsetup); }
  }
}

bool TaskManager::setupTask(OutputInterface* terminal) {
  bool returnValue = true;
  bool setupValue = false;
  setRefreshMilli(10);
  StringBuilder sb;
  if (terminal) {
    terminal->clearScreen();
    terminal->println(
        PROMPT,
        (sb + ProgramInfo::AppName + " Version: " + ProgramInfo::MajorVersion + "." + ProgramInfo::MinorVersion + "." + ProgramInfo::BuildVersion).c_str());
    sb.clear();
    terminal->println(PROMPT, "************************************************************");
  }
  for (unsigned long i = 0; i < queue.count(); i++) {
    Task* t = getTask(i);
    setupValue = t->setup(terminal);
    returnValue &= setupValue;
    t->addCmd(TERM_CMD);
    if (terminal) {
      sb + t->getName() + " Task Initialization Complete";
      if (setupValue)
        terminal->println(PASSED, sb.c_str());
      else
        terminal->println(ERROR, sb.c_str());
      sb.clear();
    }
  }
  Task* t_ptr = this;
  queue.push(&t_ptr);
  addCmd(TERM_CMD);

  setupIdle();
  if (terminal) {
    sb + this->getName() + " Task Initialization Complete";
    if (returnValue)
      terminal->println(PASSED, sb.c_str());
    else
      terminal->println(ERROR, sb.c_str());
    sb.clear();
    terminal->println(PASSED, "Setup Complete");
    terminal->println(PROMPT, "************************************************************");
    terminal->banner();
    terminal->prompt();
  }
  return returnValue;
}

bool TaskManager::loop() {
  lock.take();
  execution.start();
  execution.stop();
  lock.give();
  bool returnValue = executeTask();
  return returnValue;
};

bool TaskManager::executeTask() {
  unsigned long timeToIdle = 100;
  bool returnValue = true;
  bool loopValue = false;
  int running_core = rp2040.cpuid();
  for (unsigned long i = 0; i < queue.count(); i++) {
    Task* t = getTask(i);
    if (t->runTask() && (t->getID() != TASK_MANAGER_ID) && (t->getID() != IDLE_ID)) {
      if (t->getCore() == running_core) {
        loopValue = t->loop();
        returnValue &= loopValue;
        timeToIdle = (t->timeRemainingMilli() < timeToIdle) ? t->timeRemainingMilli() : timeToIdle;
      }
    }
  }
  StringBuilder sb = "Time to Idle> Core(";
  sb + running_core + ") : " + timeToIdle;
  idleTask[running_core].setDelay(timeToIdle);
  idleTask[running_core].loop();
  return returnValue;
}

void TaskManager::addCmd(TerminalCommand* __termCmd) {
  if (__termCmd)
    __termCmd->addCmd("system", "", "Prints a list of Tasks running in the system", [this](TerminalLibrary::OutputInterface* terminal) { system(terminal); });
}

void TaskManager::add(Task* task) {
  lock.take();
  queue.push(&task);
  lock.give();
}

Task* TaskManager::getTask(unsigned long index) {
  Task* t_ptr = nullptr;
  if (index >= queue.count()) return nullptr;
  queue.get(index, &t_ptr);
  return t_ptr;
}

unsigned long TaskManager::getTaskCount() {
  return queue.count();
}

void TaskManager::system(OutputInterface* terminal) {
  if (!terminal) return;
  Task* task;
  AsciiTable table(terminal);
  double coreUtil[CPU_CORES] = {0.0, 0.0};

  bool verbose = false;
  char* parameter = terminal->readParameter();
  if ((parameter != NULL) && (safeCompare(parameter, "-v") == 0)) verbose = true;

  terminal->banner();
  table.addColumn(Magenta, "ID", 6);
  table.addColumn(Green, "Core", 6);
  table.addColumn(Normal, "Task Name", 19);
  table.addColumn(Yellow, "Time(ms)", 10);
  table.addColumn(Yellow, "Max(ms)", 10);
  table.addColumn(Yellow, "Min(ms)", 10);
  table.addColumn(Cyan, "Rate(ms)", 10);
  table.addColumn(Cyan, "% CPU", 8);
  table.printHeader();
  for (unsigned long i = 0; i < queue.count(); i++) {
    queue.get(i, &task);
    if (verbose || (task->getID() != IDLE_ID)) {
      StringBuilder id;
      StringBuilder name;
      StringBuilder timeString = "-";
      StringBuilder highString = "-";
      StringBuilder lowString = "-";
      StringBuilder rateString = "-";
      StringBuilder percentString = "-";
      StringBuilder coreString = "-";

      id = task->getID();
      name = task->getName();
      if (task->runTask()) {
        double time = task->getExecutionTime()->time();
        timeString = (time / 1000.0);
        double high = task->getExecutionTime()->highWaterMark();
        highString = (high / 1000.0);
        double low = task->getExecutionTime()->lowWaterMark();
        lowString = (low / 1000.0);
        double rate = task->getRefreshRate();
        rateString = (rate / 1000);
        double timePerSec = 1000000.0 / rate;
        double timeTakenPerSec = timePerSec * time;
        percentString = timeTakenPerSec / 1000000.0 * 100;
        percentString + "%";
        coreString = task->getCore();
        coreUtil[task->getCore()] += timeTakenPerSec;
      }
      table.printData(id.c_str(), coreString.c_str(), name.c_str(), timeString.c_str(), highString.c_str(), lowString.c_str(), rateString.c_str(),
                      percentString.c_str());
    }
  }

  for (int i = 0; i < CPU_CORES; i++) {
    StringBuilder percentString = "CPU Core ";
    percentString + i + ": " + (coreUtil[i] / 10000.0) + " %";
    terminal->println(HELP, percentString.c_str());
  }
  table.printDone("System Complete");
  terminal->prompt();
}

void TaskManager::setupIdle() {
  for (int i = 0; i < CPU_CORES; i++) {
    idleTask[i].setCore(i);
    idleTask[i].setup(terminal);
    Task* t_ptr = &idleTask[i];
    queue.push(&t_ptr);
  }
}