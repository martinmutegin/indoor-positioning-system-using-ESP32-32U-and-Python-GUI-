#include <WiFi.h>
#include <esp_now.h>
#include "esp_now_communication.h"
#include "power_manager.h"
#include "led.h"
#include "RSSI_AP.h"


void setup() {
  Serial.begin(115200);
  delay(100);
  setupLEDs();          // Set LED pin modes

  setupESPNow();        // Initialize ESP-NOW
  addAllPeers();        // Add all peers from peer list


}

void loop() {
  
  if (shouldSend) {
    handleSendEvery(2000);  // Sending every 2 seconds
    if (millis() > stopUntil) {
      stopESPNowCommunication();
    }
  } else {
    if (millis() > ESP_NOWResume) {
      resumeESPNowCommunication();
    }
  }
}
