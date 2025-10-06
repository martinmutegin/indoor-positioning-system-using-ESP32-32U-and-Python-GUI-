#ifndef BATTERY_STATUS_H
#define BATTERY_STATUS_H
#include <led.h>


const int batteryPin = 34;  // ADC pin for battery

float getBatteryLevel() {
  int raw = analogRead(batteryPin);
  float voltage = raw * (3.3 / 4095.0) * 2;  // if voltage divider used
  Serial.print("Battery Voltage: ");
  Serial.println(voltage);
  return voltage;
}

#endif
