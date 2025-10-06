#ifndef ESP_NOW_COMMUNICATION_H
#define ESP_NOW_COMMUNICATION_H


#include <led.h>
#include <esp_now.h>
#include <WiFi.h>
#include "wifi_manager.h"

#define DEVICE_NAME "ANKER 2"

bool shouldSend = true;
unsigned long stopUntil = 0;
unsigned long lastSendTime = 0;

// === Add all peer MAC addresses ===
typedef struct {
  const char* name;
  uint8_t mac[6];
} NamedPeer;

NamedPeer peers[] = {
  {"ZENTRALE", {0x6C, 0xC8, 0x40, 0x34, 0xBD, 0x18}},
//{"ANCHOR1",  {0x6C, 0xC8, 0x40, 0x35, 0x80, 0x7C}},
//{"ANCHOR2",  {0x6C, 0xC8, 0x40, 0x33, 0x84, 0xB0}},
  {"TAG",      {0x6C, 0xC8, 0x40, 0x34, 0xCF, 0x0C}}  
};

const char* getPeerNameByMac(const uint8_t* mac) {
  for (auto& peer : peers) {
    if (memcmp(mac, peer.mac, 6) == 0) {
      return peer.name;
    }
  }
  return "UNKNOWN";
}

typedef struct{
  char sender[16];
  char msg[64];
} struct_message;

struct_message incomingData;
struct_message outgoingData;


// === RECEIVE CALLBACK ===
void onReceiveData(const esp_now_recv_info_t *recv_info, const uint8_t *incomingDataRaw, int len) {
      
      memcpy(&incomingData, incomingDataRaw, sizeof(incomingData));
      const char* senderName = getPeerNameByMac(recv_info->src_addr);

      turnOnWhiteLED();
      Serial.printf("📩 From [%s] → %s\n",
      senderName,
      incomingData.msg    
      );
      delay(500);
      turnOffWhiteLED();
}

// === SEND CALLBACK ===
void onSendData(const wifi_tx_info_t *tx_info, esp_now_send_status_t status) {
  const char* peerName = getPeerNameByMac(tx_info->des_addr);
       if (peerName == "ZENTRALE" && status == ESP_NOW_SEND_SUCCESS){
            turnOnYellowLED();
            delay(200);
            turnOffYellowLED();
            Serial.printf("📤 Send to [%s]: ✅ SUCCESS\n", peerName);
          }else{
            turnOnRedLED();
            turnOnYellowLED();
            delay(200);
            turnOffYellowLED();
            turnOffRedLED();
            
            ("📤 Send to [%s]: ❌ FAIL\n", peerName);
          }
       
          Serial.printf("📤 Send to [%s]: %s\n", 
          peerName,
          status == ESP_NOW_SEND_SUCCESS ? "✅ SUCCESS" : "❌ FAIL"
        );
      
      delay(500);
      turnOffWhiteLED();
}

// === INITIALIZE ESPNOW ===
void setupESPNow() {

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
  esp_now_register_send_cb(onSendData);
  esp_now_register_recv_cb(onReceiveData);

  Serial.println("✅ ESP-NOW initialized in bidirectional mode.");
}

// === ADD A PEER ===
void addAllPeers() {
  for (auto &peer : peers) {
    esp_now_peer_info_t info = {};
    memcpy(info.peer_addr, peer.mac, 6);
    info.channel = 0;
    info.encrypt = false;

    if (!esp_now_is_peer_exist(peer.mac)) {
      if (esp_now_add_peer(&info) == ESP_OK) {
        Serial.printf("✅ Added peer: %s\n", peer.name);
      } else {
        Serial.printf("❌ Failed to add peer: %s\n", peer.name);
      }
    }
  }
}

// === SEND FUNCTION ===
void sendDataToPeer(const uint8_t *peerMac, const char* message) {
 
  strncpy(outgoingData.sender, DEVICE_NAME, sizeof(outgoingData.sender));
  strncpy(outgoingData.msg, message, sizeof(outgoingData.msg));

  esp_err_t result = esp_now_send(peerMac, (uint8_t *) &outgoingData, sizeof(outgoingData));
  if (result != ESP_OK) {
    Serial.println("❌ Error sending data");
  }
}

// === SEND PERIODICALLY ===
void handleSendEvery(unsigned long intervalMs) {
  
    if (millis() - lastSendTime > intervalMs) {
      for(int i = 0; i< sizeof(peers)/sizeof(peers[0]); i++){
        sendDataToPeer(peers[i].mac, "Hello from " DEVICE_NAME);
      }
        lastSendTime = millis();
        
    }
}

// STOP ESP-NOW COMMUNICATION
void stopESPNowCommunication() {
  Serial.println("⏸ Stopping ESP-NOW...");

  esp_now_register_send_cb(nullptr);
  esp_now_register_recv_cb(nullptr);
  Serial.println("Callbacks unregistered");

  for (int i = 0; i < sizeof(peers)/sizeof(peers[0]); i++) {
    esp_err_t res = esp_now_del_peer(peers[i].mac);
    if (res == ESP_OK) {
      Serial.printf("✅ Peer %d removed\n", i);
    } else {
      Serial.printf("❌ Failed to remove peer %d: %d\n", i, res);
    }
  }
  esp_err_t deinitRes = esp_now_deinit();
  if (deinitRes == ESP_OK) {
    Serial.println("✅ ESP-NOW deinitialized");
  } else {
    Serial.printf("❌ Failed to deinit ESP-NOW: %d\n", deinitRes);
  }

  Serial.println("🛑 ESP-NOW communication stopped");

  shouldSend = false;
  stopUntil = millis() + 30000UL;  // 30 seconds from now
  connectToAP();
}
// RESUME ESP-NOW COMMUNICATION
void resumeESPNowCommunication() {
  disconnect();
  setupESPNow();
  addAllPeers();
  shouldSend = true;
  Serial.println("▶️ ESP-NOW communication resumed");
}
#endif
