// ----------------------------------------------------------------------------
// TimeTriggeredScheduler.h
//
//
// Author: Peter Polidoro
// ----------------------------------------------------------------------------

#ifndef TTS_H_
#define TTS_H_
#include <util/atomic.h>

#define TASK_COUNT_MAX 32
#define CLOCK_CYCLES_PER_MILLISECOND  F_CPU/1000L

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

class TTS {
 public:
  TTS();

  void init();
  uint32_t ms(void);
  uint32_t us(void);
  uint32_t setTime(uint32_t time);
  uint8_t addTaskUsingTime(uint32_t time,  void (*userFunc)(int), int arg, long period, long count, bool checkConflict);
  uint8_t addTaskUsingDelay(uint32_t delay,  void (*userFunc)(int), int arg, long period, long count, bool checkConflict);
  uint8_t addTaskUsingOffset(uint8_t taskIdOrigin, uint32_t offset,  void (*userFunc)(int), int arg, long period, long count, bool checkConflict);
  bool removeTask(uint8_t taskId);
  bool removeAllTasks();
  bool enableTask(uint8_t taskId);
  bool disableTask(uint8_t taskId);
  void update();
  bool getTaskDetails(uint8_t taskId, uint32_t& time, int& arg, long& period, long& count, bool& free, bool& enabled);
  bool activeTasks();
  int countActiveTasks();

 private:
  volatile uint32_t timer5_millis;

  uint32_t conflictWindow;

  struct task_t {
    uint32_t time;
    bool free;
    bool enabled;
    long period;
    long count;
    long inc;
    void (*func)(int);
    int arg;
  };

  task_t tasks[TASK_COUNT_MAX];

  bool startTimer();
  uint8_t findAvailableTaskId(uint32_t time, bool checkConflict);
};

extern TTS tts;

inline void TTS::update() {
  timer5_millis++;
  for (uint8_t taskId = 0; taskId < TASK_COUNT_MAX; taskId++) {
    if ((!tasks[taskId].free) && (tasks[taskId].time <= timer5_millis)) {
      if (tasks[taskId].enabled) {
        if ((tasks[taskId].count < 0) || (tasks[taskId].inc < tasks[taskId].count)){
          (*tasks[taskId].func)(tasks[taskId].arg);
          tasks[taskId].inc++;
        } else {
          tasks[taskId].period = -1;
        }
      }
      if (0 < tasks[taskId].period) {
        while (tasks[taskId].time <= timer5_millis) {
          tasks[taskId].time += tasks[taskId].period;
        }
      } else {
        removeTask(taskId);
      }
    }
  }
}


#endif // TTS_H_



