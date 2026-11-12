
#ifndef PERIODIC_TIMER_HPP
#define PERIODIC_TIMER_HPP

#include <cstdint>

class PeriodicTimer {
public:
  explicit PeriodicTimer(uint32_t periodms) : period_(periodms), last_(0) {}
  void reset(uint32_t now) { last_ = now; }
  bool expired(uint32_t now) {
    if (now - last_ >= period_) {
      last_ = now;
      return true;
    }
    return false;
  }

private:
  uint32_t period_;
  uint32_t last_;
};

#endif /* PERIODIC_TIMER_HPP */