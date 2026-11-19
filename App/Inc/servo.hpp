#ifndef SERVO_HPP
#define SERVO_HPP

#include "main.h"
#include <cstdint>

class Servo {
public:
  Servo(TIM_HandleTypeDef *htim, uint32_t channel, uint16_t minUs,
        uint16_t maxUs)
      : htim_(htim), channel_(channel), minUs_(minUs), maxUs_(maxUs),
        pulseUs_((minUs_ + maxUs_) / 2) {}
  bool start() {
    setPulseUs(pulseUs_);
    if (HAL_TIM_PWM_Start(htim_, channel_) != HAL_OK) {
      return false;
    }
    return true;
  }
  void setPulseUs(uint16_t us) {
    if (us < minUs_) {
      pulseUs_ = minUs_;
    } else if (us > maxUs_) {
      pulseUs_ = maxUs_;
    } else {
      pulseUs_ = us;
    }
    __HAL_TIM_SET_COMPARE(htim_, channel_, pulseUs_);
  }
  uint16_t pulseUs() const { return pulseUs_; }

private:
  TIM_HandleTypeDef *htim_;
  uint32_t channel_;
  uint16_t minUs_;
  uint16_t maxUs_;
  uint16_t pulseUs_;
};

#endif /* SERVO_HPP */