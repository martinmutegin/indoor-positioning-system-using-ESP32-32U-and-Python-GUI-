#ifndef SENDTOZENTRALE_H
#define SENDTOZENTRALE_H

#include <esp_now.h>
#include <WiFi.h>
#include "esp_now_communication.h"
#include "wifi_manager.h"

#define BATTERY_ADC_PIN 34
#define R1 100000
#define R2 100000
#define RSSI_COUNT 10

// Zentral MAC address (receiver)
static uint8_t zentraleMAC[] = {0x6C, 0xC8, 0x40, 0x34, 0xBD, 0x18};

int8_t toBuffer[10];

// ==================== DATA STRUCTURE ==================== //
typedef struct {
  int8_t rssi[RSSI_COUNT];
  uint8_t battery;
} DataPacket;
DataPacket packet;


// ==================== FUNCTION DECLARATIONS ==================== //
void sendingInit();
void espnowinit();
void packtetToGo();
void addZentrale();
void OnDataSent(const wifi_tx_info_t* mac_addr, esp_now_send_status_t status);


// ==================== GLOBAL DEFINITIONS ==================== //

bool shouldsendpacket = false;
bool zentraleAdded = false;

// ==================== FUNCTION IMPLEMENTATIONS ==================== //

void sendingInit() {

  for (int i = 0; i < RSSI_COUNT; i++) {
    toBuffer[i] = (int8_t)WiFi.RSSI();
    delay(100);  // Optional: give time between samples
  }

  
  packtetToGo();                // ✅ Pass it to packet
  espnowinit();

  //int8_t rssi = (int8_t)WiFi.RSSI();        // ✅ Read actual RSSI
  addZentrale();
  // ✅ Send the packet
  if(zentraleAdded){
    shouldsendpacket = true;
    delay(5000);
    esp_err_t result = esp_now_send(zentraleMAC, (uint8_t*)&packet, sizeof(packet));
    if (result == ESP_OK) {
      Serial.println("📤 Packet sent successfully");
    } else {
      Serial.println(result);
      Serial.print("❌ Failed to send packet. Error code: ");
      
    }
  }
}


// Function to fill the packet with RSSI and battery data
void packtetToGo() {
   Serial.println("Preparing packet with RSSI buffer:");
  
    for (int i = 0; i < RSSI_COUNT; i++) {
      packet.rssi[i] = toBuffer[i];
      Serial.print("RSSI[");
      Serial.print(i);
      Serial.print("] = ");
      Serial.println(packet.rssi[i]);
    }

  // Battery reading as before
  int raw = analogRead(BATTERY_ADC_PIN);
  float voltage = (raw / 4095.0) * 3.3;
  float actualVoltage = voltage * ((R1 + R2) / (float)R2);
  float percent = ((actualVoltage - 3.0) / (4.2 - 3.0)) * 100.0;
  packet.battery = constrain((uint8_t)percent, 0, 100);

  Serial.print("Sending RSSI array: ");
   Serial.printf("🔋 Battery level: %d%%\n", packet.battery);
for (int i = 0; i < RSSI_COUNT; i++) {
  Serial.print(packet.rssi[i]);
  Serial.print(" ");
}
Serial.println();
 shouldsendpacket = true;
}


// ESP-NOW send callback
void OnDataSent(const wifi_tx_info_t* mac_addr, esp_now_send_status_t status) {
  Serial.print("Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "✅ Success" : "❌ Fail");
}
void espnowinit() {

  turnOnBlueLED();
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("❌ ESP-NOW init failed");
    delay(500);
    turnOffBlueLED();
    ESP.restart();
  }
 //bidirectional data send, activation of esp 
  esp_now_register_send_cb(OnDataSent);

  Serial.println("✅ ESP-NOW initialized in send mode.");
}

// Add zentrale as peer
void addZentrale() {


  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, zentraleMAC, 6);
  peerInfo.channel = 0;       // Use same Wi-Fi channel
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) == ESP_OK) {
    Serial.println("✅ Zentrale added as peer");
    zentraleAdded = true;    
  } else {
    Serial.println("❌ Failed to add zentrale");
  }
}

#endif // SENDTOZENTRALE_H
