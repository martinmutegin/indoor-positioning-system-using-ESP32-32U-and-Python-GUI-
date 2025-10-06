#ifndef SLEEP_MANAGER_H
#define SLEEP_MANAGER_H

#include <esp_sleep.h>
#include <led.h>


void enterDeepSleep(int seconds) {
  Serial.println("Going to sleep...");
  esp_sleep_enable_timer_wakeup((uint64_t)seconds * 1000000ULL);
  esp_deep_sleep_start();
}

#endif
