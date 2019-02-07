// ----------------------------------------------------------------------------
// TimeTriggeredScheduler.h
//
//
// Author: Peter Polidoro
// ----------------------------------------------------------------------------
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "TimeTriggeredScheduler.h"
#include <Streaming.h>

//---------- constructor ----------------------------------------------------

TTS::TTS() {
  for (uint8_t taskId = 0; taskId < TASK_COUNT_MAX; taskId++) {
    removeTask(taskId);
  }
  conflictWindow = 10;
}

//---------- public ----------------------------------------------------

void TTS::init() {
  timer5_millis = 0;
  startTimer();
}

uint32_t TTS::ms() {
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  uint32_t m;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    m = timer5_millis;
  }
  return m;
#else
  return (millis());
#endif
}

uint32_t TTS::setTime(uint32_t time=0) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    timer5_millis = time;
  }
}

uint8_t TTS::addTaskUsingTime(uint32_t time,  void (*userFunc)(int), int arg, long period=-1, long count=-1, bool checkConflict=false) {
  uint8_t taskId = findAvailableTaskId(time,checkConflict);
  if (taskId < TASK_COUNT_MAX) {
    tasks[taskId].time = time;
    tasks[taskId].func = userFunc;
    // if (combineArgsIfConflict) {
    //   tasks[taskId].arg = arg & tasks[taskId].arg;
    // } else {
    //   tasks[taskId].arg = arg;
    // }
    tasks[taskId].arg = arg;
    tasks[taskId].period = period;
    tasks[taskId].count = count;
    tasks[taskId].inc = 0;
    tasks[taskId].enabled = true;
    tasks[taskId].free = false;
  }
  return taskId;
}

uint8_t TTS::addTaskUsingDelay(uint32_t delay,  void (*userFunc)(int), int arg, long period=-1, long count=-1, bool checkConflict=false) {
  uint32_t time_now = ms();
  uint32_t time = time_now + delay;

  // Figure out how to use function pointers to make this work!
  // uint8_t taskId = addTaskUsingTime(time, userFunc, arg, period, checkConflict);
  // return taskId;

  uint8_t taskId = findAvailableTaskId(time,checkConflict);
  if (taskId < TASK_COUNT_MAX) {
    tasks[taskId].time = time;
    tasks[taskId].func = userFunc;
    tasks[taskId].arg = arg;
    tasks[taskId].period = period;
    tasks[taskId].count = count;
    tasks[taskId].inc = 0;
    tasks[taskId].enabled = true;
    tasks[taskId].free = false;
  }
  return taskId;
}

uint8_t TTS::addTaskUsingOffset(uint8_t taskIdOrigin, uint32_t offset,  void (*userFunc)(int), int arg, long period, long count, bool checkConflict) {
  uint32_t time_base = tasks[taskIdOrigin].time;
  uint32_t time = time_base + offset;

  // Figure out how to use function pointers to make this work!
  // uint8_t taskId = addTaskUsingTime(time, userFunc, arg, period, checkConflict);
  // return taskId;

  uint8_t taskId = findAvailableTaskId(time,checkConflict);
  if (taskId < TASK_COUNT_MAX) {
    tasks[taskId].time = time;
    tasks[taskId].func = userFunc;
    tasks[taskId].arg = arg;
    tasks[taskId].period = period;
    tasks[taskId].count = count;
    tasks[taskId].inc = 0;
    tasks[taskId].enabled = true;
    tasks[taskId].free = false;
  }
  return taskId;
}

bool TTS::removeTask(uint8_t taskId) {
  if (taskId < TASK_COUNT_MAX) {
    tasks[taskId].time = 0;
    tasks[taskId].free = true;
    tasks[taskId].enabled = false;
    tasks[taskId].period = -1;
    tasks[taskId].count = -1;
    tasks[taskId].inc = 0;
    tasks[taskId].arg = 0;
  }
}

bool TTS::removeAllTasks() {
  for (uint8_t taskId = 0; taskId < TASK_COUNT_MAX; taskId++) {
    removeTask(taskId);
  }
}

bool TTS::enableTask(uint8_t taskId) {
  if (taskId < TASK_COUNT_MAX) {
    tasks[taskId].enabled = true;
  }
}

bool TTS::disableTask(uint8_t taskId) {
  if (taskId < TASK_COUNT_MAX) {
    tasks[taskId].enabled = false;
  }
}

bool TTS::getTaskDetails(uint8_t taskId, uint32_t& time, int& arg, long& period, long& count, bool& free, bool& enabled) {
  time = tasks[taskId].time;
  arg = tasks[taskId].arg;
  period = tasks[taskId].period;
  count = tasks[taskId].count;
  free = tasks[taskId].free;
  enabled = tasks[taskId].enabled;
}

bool TTS::activeTasks() {
  bool active_tasks = false;
  uint8_t taskId = 0;
  while ((!active_tasks) && (taskId < TASK_COUNT_MAX)) {
    if ((!tasks[taskId].free) && tasks[taskId].enabled) {
      active_tasks = true;
    }
    taskId++;
  }
  return active_tasks;
}

int TTS::countActiveTasks() {
  int active_tasks = 0;
  for (uint8_t taskId = 0; taskId < TASK_COUNT_MAX; taskId++) {
    if ((!tasks[taskId].free) && tasks[taskId].enabled) {
      active_tasks++;
    }
  }
  return active_tasks;
}


//------------------ private -----------------------------------------------

bool TTS::startTimer() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    TCCR5A = 0;                 // clear control register A
    TCCR5B = _BV(WGM53);        // set mode 8: phase and frequency correct pwm, stop the timer
    long cycles = (F_CPU / 2000); // the counter runs backwards after TOP, interrupt is at BOTTOM so divide microseconds by 2
    unsigned char clockSelectBits = _BV(CS50);  // no prescale, full xtal
    ICR5 = cycles;                // ICR5 is TOP in p & f correct pwm mode

    TCCR5B &= ~(_BV(CS50) | _BV(CS51) | _BV(CS52));
    TCCR5B |= clockSelectBits;   // reset clock select register, and starts the clock
    TIMSK5 = _BV(TOIE5);         // sets the timer overflow interrupt enable bit
  }
}

uint8_t TTS::findAvailableTaskId(uint32_t time, bool checkConflict) {
  uint8_t taskId = 0;
  // if (combineArgsIfConflict) {
  //   uint32_t time_task;
  //   for (taskId = 0; taskId < TASK_COUNT_MAX; taskId++) {
  //     if (!tasks[taskId].free) {
  //       time_task = tasks[taskId].time;
  //       if (abs(time_task - time) < conflictWindow/2) {
  //         return taskId;
  //       }
  //     }
  //   }
  // }
  while (!tasks[taskId].free) {
    taskId++;
  }
  if (TASK_COUNT_MAX <= taskId) {
    return 255;
  }
  return taskId;
}

TTS tts;

ISR(TIMER5_OVF_vect) {
  tts.update();
}
