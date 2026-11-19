#include "app.h"
#include "bouncer.hpp"
#include "main.h"
#include "periodic_timer.hpp"
#include "servo.hpp"
#include "usbd_cdc_if.h"
#include <cstdint>
#include <cstdio>

constexpr int32_t kPwmMax = 1000;
constexpr int32_t kPwmMin = 0;
constexpr int32_t kPwmStep = 10;
constexpr int32_t kServoStepUs = 10;
constexpr int32_t kServoMinUs = 1000;
constexpr int32_t kServoMaxUs = 2000; // TIM3 ARR (999) + 1, set in CubeMX

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim2;

static uint8_t txBuf[64];
static volatile uint8_t txStatus;

static Bouncer brightnessBouncer(kPwmMin, kPwmMax, kPwmStep);
static Bouncer sweepBouncer(kServoMinUs, kServoMaxUs, kServoStepUs);

static PeriodicTimer blinkTimer(500);
static PeriodicTimer printTimer(1000);
static PeriodicTimer fadeTimer(10);

static PeriodicTimer servoTimer(20); // 50Hz update rate for servo

static Servo servo1(&htim2, TIM_CHANNEL_2, kServoMinUs, kServoMaxUs);

void app_setup(void) {
  // Initialization code here
  blinkTimer.reset(HAL_GetTick());
  printTimer.reset(HAL_GetTick());
  fadeTimer.reset(HAL_GetTick());
  servoTimer.reset(HAL_GetTick());

  brightnessBouncer.reset(kPwmMin);
  sweepBouncer.reset(servo1.pulseUs());

  if (HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1) != HAL_OK) {
    Error_Handler();
  }

  if (!servo1.start()) {
    Error_Handler();
  }
}

void app_loop(void) {
  // Main loop code here
  uint32_t now = HAL_GetTick();

  if (fadeTimer.expired(now)) {
    const int32_t brightness = brightnessBouncer.next();
    const int32_t duty = brightness * brightness / kPwmMax;
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uint32_t)duty);
  }

  if (blinkTimer.expired(now)) {
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
  }

  if (printTimer.expired(now)) {
    int len = std::snprintf((char *)txBuf, sizeof(txBuf), "tick: %lu\r\n", now);
    txStatus = CDC_Transmit_FS(txBuf, len);
  }

  if (servoTimer.expired(now)) {
    const int32_t sweepUs = sweepBouncer.next();
    servo1.setPulseUs(sweepUs);
  }
}