#include "wifi_manager.h"
#include "esp_now_communication.h"
#include "led.h"
#include "credentials.h"
#include "incomingData.h"




//============================================== NETWORK_CREDENTIALS =====================================================================================================================================
//AP
const char* AP_SSID = "TAG-Access-Point";
const char* AP_PASSWORD = "12345678";

//WIFI
/*const char* WIFI_SSID = "martinrouterking";
const char* WIFI_PASSWORD = "65476963";*/

const char* WIFI_SSID = "GRP-Wifi-Pub";
const char* WIFI_PASSWORD = "wifi.bt.1974.32768";
//======================================================END ==============================================================================================================================================


void setup() {

  Serial.begin(115200);
  setupLEDs();

  connectToWiFi();

  setupESPNow();
  addAllPeers();

}

void loop() {
  
    if (shouldSend && millis() >15000) {
        stopESPNowCommunication();
        //calltosend
    }
     
    if (shouldSend) {
      handleSendEvery(3000); 
    }
    if (shouldReadSelfPacket) {
        for(int i=0; i<10; i++){
        int8_t rssi = (int8_t)WiFi.RSSI();        // Read actual RSSI
         toBuffer[i] = rssi;
        }
    }

     if (deviceConnected && readyToSendBLE && !sentOverBLE) {
    Serial.println("🚀 Conditions met. Sending over BLE...");
    sendSendersOverBLE();
  }
}
