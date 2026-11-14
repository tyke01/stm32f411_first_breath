#include "app.h"
#include "main.h"
#include "periodic_timer.hpp"
#include "usbd_cdc_if.h"
#include <cstdio>

constexpr int32_t kPwmMax = 1000; // TIM3 ARR (999) + 1, set in CubeMX

extern TIM_HandleTypeDef htim3;

static uint8_t txBuf[64];
static volatile uint8_t txStatus;

static int32_t brightness = 0;
static int32_t dutyStep = 10;

static PeriodicTimer blinkTimer(500);
static PeriodicTimer printTimer(1000);
static PeriodicTimer fadeTimer(10);

void app_setup(void) {
  // Initialization code here
  blinkTimer.reset(HAL_GetTick());
  printTimer.reset(HAL_GetTick());
  fadeTimer.reset(HAL_GetTick());

  if (HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1) != HAL_OK) {
    Error_Handler();
  }
}

void app_loop(void) {
  // Main loop code here

  uint32_t now = HAL_GetTick();

  if (fadeTimer.expired(now)) {
    brightness += dutyStep;
    if (brightness >= kPwmMax) {
      brightness = kPwmMax;
      dutyStep = -10;
    } else if (brightness <= 0) {
      brightness = 0;
      dutyStep = 10;
    }

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
}