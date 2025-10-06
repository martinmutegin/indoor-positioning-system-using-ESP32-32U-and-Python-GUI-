#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include "led.h"
#include "credentials.h"
#include "sendtozentrale.h"

unsigned long AP_timelimit = 0;

//=================================GENERAL CONNECTION ============================================================================================================================================


//=================================================RSSI CHECK=====================================================================================================================================
long signalStrength() {     //check connection for 10 seconds
  long rssi = WiFi.RSSI();
  Serial.print("RSSI: ");
  Serial.print(rssi);
  Serial.println(" dBm");
  delay(1000);  // Check every second
  return rssi;
}
//===================================================END==========================================================================================================================================

//=================================CLIENT CONNECTION TO TAG ACCESS POINT FOR RSSI=================================================================================================================

void connectToAP() {
  WiFi.begin(AP_SSID, AP_PASSWORD);
  Serial.print("Connecting to AP");
  
  int retries = 0;
  const int maxRetries = 20;  // Wait ~10 seconds total

    while (WiFi.status() != WL_CONNECTED && retries < maxRetries) {
      turnOnYellowLED();
      turnOnBlueLED();
      turnOnWhiteLED();
      delay(500);
      turnOffYellowLED();
      turnOnBlueLED();
      turnOnWhiteLED();
      Serial.print(".");   
      retries++;
    }
      if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\n✅ Connected to AP!");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      turnOnYellowLED();
      turnOnBlueLED();
      for(int i=0; i<10; i++){
          signalStrength();
        }
      sendingInit();

      AP_timelimit = millis() + 45000;
    
      
      } else {
        Serial.println("\n❌ Failed to connect to AP. Stopping program.");
        ESP.restart();
    
  }
}
//===================================================END==========================================================================================================================================

//==========================================DISCONNECT Both WIFI/AP===============================================================================================================================
void disconnect() {
    WiFi.disconnect(true); // Disconnect and erase credentials
    Serial.println("Network disconnected.");
    turnOffYellowLED();
    turnOffBlueLED();
}
//===================================================END==========================================================================================================================================

#endif