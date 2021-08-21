#include "timer.h"

Timer::Timer() {
  this->reset();
}

float Timer::reset() {
  TimePoint now = Clock::now();
  Duration duration = now - this->last;

  this->last = now;

  return duration.count();
}

