#ifndef RSSI_AP_H
#define RSSI_AP_H

#include <WiFi.h>
#include "led.h"
#include "esp_now_communication.h"

unsigned long AP_timelimit = 0;
const char* ssid = "TAG-Access-Point";
const char* password = "12345678";  

void disconnectRSSI() {
    WiFi.disconnect(true); // Disconnect and erase credentials
    Serial.println("🚫📶  Access Point disconnected.✗");
    turnOffWhiteLED();
    turnOffWhiteLED();
   
}

void setupRSSI() {
  WiFi.softAP(ssid, password);
  Serial.println("📶Access Point Started");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  turnOnWhiteLED();
  turnOnBlueLED();
    
}


#endif