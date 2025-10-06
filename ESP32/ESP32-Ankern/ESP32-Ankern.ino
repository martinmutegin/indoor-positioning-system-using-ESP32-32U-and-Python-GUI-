#include "esp_now_communication.h"
#include "led.h"
#include "sleep_manager.h"
//#include "WiFi.h"
#include "credentials.h"
#include "wifi_manager.h"
#include "sendtozentrale.h"

//============================================== NETWORK_CREDENTIALS =====================================================================================================================================
//AP
const char* AP_SSID = "TAG-Access-Point";
const char* AP_PASSWORD = "12345678";

//======================================================END ==============================================================================================================================================


void setup(){

  Serial.begin(115200);
  /*WiFi.mode(WIFI_STA);  // Set WiFi to station mode
  Serial.print("ESP32 MAC Address: ");
  Serial.println(WiFi.macAddress());*/
  setupLEDs();

  setupESPNow(); 
  addAllPeers();        // Add all peers from peer list


  analogReadResolution(12);
  analogSetAttenuation(ADC_11db); // Max input ~3.3V

 
}
void loop() {
   unsigned long now = millis();

    if (shouldSend && millis() >15000) {
      stopESPNowCommunication();
    }
    if (shouldSend) {
      handleSendEvery(3000); 
    
    }
    if (shouldsendpacket) {
        for(int i=0; i<10; i++){
        int8_t rssi = (int8_t)WiFi.RSSI();        // ✅ Read actual RSSI
         toBuffer[i] = rssi;
        }
    }
  }