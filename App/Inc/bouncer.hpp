#ifndef BOUNCER_HPP
#define BOUNCER_HPP

#include <cstdint>

class Bouncer {
public:
  Bouncer(int32_t min, int32_t max, int32_t step)
      : min_(min), max_(max), step_(step), value_((min + max) / 2) {}
  void reset(int32_t start) {
    if (start <= min_) {
      value_ = min_;
    } else if (start >= max_) {
      value_ = max_;
    } else {
      value_ = start;
    }
  }
  int32_t next() {
    value_ += step_;
    if (value_ >= max_) {
      value_ = max_;
      step_ = -step_;
    } else if (value_ <= min_) {
      value_ = min_;
      step_ = -step_;
    }
    return value_;
  }
  int32_t value() const { return value_; }

private:
  int32_t min_;
  int32_t max_;
  int32_t step_;
  int32_t value_;
};

#endif /* BOUNCER_HPP */