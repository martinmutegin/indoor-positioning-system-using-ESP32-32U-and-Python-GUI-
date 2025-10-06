#include <WiFi.h>
#include "led.h"
#include "credentials.h"
#include "wifi_manager.h"
#include "incomingData.h"


unsigned long AP_timelimit = 0;

void connectToWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  
  int retries = 0;
  const int maxRetries = 20;  // Wait ~10 seconds total

  while (WiFi.status() != WL_CONNECTED && retries < maxRetries) {
    delay(100);
    turnOnGreenLED();
    delay(500);
    turnOffGreenLED();
    Serial.print(".");   
    retries++;
  }
      if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\n✅ Connected to WiFi!");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      turnOnGreenLED();
    } else {
      Serial.println("\n❌ Failed to connect to WiFi. Stopping program.");
      ESP.restart();
    }
  
}
//===================================================END==========================================================================================================================================

//=================================================RSSI CHECK=====================================================================================================================================
long signalStrength() {     //check connection for 10 seconds
  long rssi = WiFi.RSSI();
  Serial.print("RSSI: ");
  Serial.print(rssi);
  Serial.println(" dBm");
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
      turnOnGreenLED();
      delay(500);
      turnOffGreenLED();
      Serial.print(".");   
      retries++;
    }
      if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\n✅ Connected to AP!");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      turnOnGreenLED();
      turnOnBlueLED();
      turnOnWhiteLED();
      for(int i=0; i<10; i++){
          signalStrength();
        }
      espnowconfig();
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
    turnOffGreenLED();
    turnOffBlueLED();
}
//===================================================END==========================================================================================================================================
