#pragma once
#include <time.h>

// Reference: https://create.stephan-brumme.com/windows-and-linux-code-timing/

class Timer {
 public:
  Timer() {}

  void Start() { clock_gettime(CLOCK_MONOTONIC, &t1); }

  float Stop() {
    clock_gettime(CLOCK_MONOTONIC, &t2);
    return t2.tv_sec - t1.tv_sec + (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;
  }

  int64_t GetTicks() {
    clock_gettime(CLOCK_MONOTONIC, &t2);
    return t2.tv_sec * 1000000000 + t2.tv_nsec;
  }

  int64_t SecondsToTicks(float s) { return int64_t(1000000000.0 * s); }

  float StopStart() {
    const float result = Stop();
    t1 = t2;
    return result;
  }

 private:
  timespec frequency;  // ticks per second
  timespec t1, t2;     // ticks
};
