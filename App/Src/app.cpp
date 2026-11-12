#include "app.h"
#include "main.h"
#include "periodic_timer.hpp"
#include "usbd_cdc_if.h"
#include <cstdio>

static uint8_t txBuf[64];
static volatile uint8_t txStatus;

static PeriodicTimer blinkTimer(500);
static PeriodicTimer printTimer(1000);

void app_setup(void) {
  // Initialization code here
  blinkTimer.reset(HAL_GetTick());
  printTimer.reset(HAL_GetTick());
}

void app_loop(void) {
  // Main loop code here

  uint32_t now = HAL_GetTick();
  if (blinkTimer.expired(now)) {
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
  }

  if (printTimer.expired(now)) {
    int len = std::snprintf((char *)txBuf, sizeof(txBuf), "tick: %lu\r\n", now);
    txStatus = CDC_Transmit_FS(txBuf, len);
  }
}