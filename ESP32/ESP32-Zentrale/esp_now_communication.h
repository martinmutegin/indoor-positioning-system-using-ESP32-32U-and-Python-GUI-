 esp-now.h#ifndef ESP_NOW_COMMUNICATION_H
#define ESP_NOW_COMMUNICATION_H


#include "wifi_manager.h"
#include <led.h>
#include <esp_now.h>
#include <WiFi.h>

#define DEVICE_NAME "ZENTRALE"

bool shouldSend = true;
unsigned long stopUntil = 0;
unsigned long lastSendTime = 0;

// Peer definitions
typedef struct {
  const char* name;
  uint8_t mac[6];
} NamedPeer;

NamedPeer peers[] = {
//{"ZENTRALE", {0x6C, 0xC8, 0x40, 0x34, 0xBD, 0x18}}, # no need to waste space of the buffer.
  {"ANCHOR1",  {0x6C, 0xC8, 0x40, 0x35, 0x80, 0x7C}},
  {"ANCHOR2",  {0x6C, 0xC8, 0x40, 0x33, 0x84, 0xB0}},
  {"TAG",      {0x6C, 0xC8, 0x40, 0x34, 0xCF, 0x0C}}
};

//
const char* getPeerNameByMac(const uint8_t* mac) {
  for (auto& peer : peers) {
    if (memcmp(mac, peer.mac, 6) == 0) {
      return peer.name;
    }
  }
  return "UNKNOWN";
}

typedef struct {
  char sender[16];
  char msg[64];
} struct_message;

struct_message incomingData;
struct_message outgoingData;
//================================================END======================================================================================================


//===========================================RECEIVE CALLBACK==============================================================================================
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
//================================================END======================================================================================================

//===========================================SEND CALLBACK=================================================================================================
void onSendData(const wifi_tx_info_t *tx_info, esp_now_send_status_t status) {
  const char* peerName = getPeerNameByMac(tx_info->des_addr);
  if (status == ESP_NOW_SEND_SUCCESS) {
     turnOnWhiteLED();
    turnOnBlueLED();
    delay(200);
    turnOffBlueLED();
    turnOffWhiteLED();
    Serial.printf("📤 Send to [%s]:  SUCCESS\n", peerName);
   
  } else {
    turnOffWhiteLED();
    turnOnRedLED();
    delay(200);
    turnOffRedLED();
    turnOnWhiteLED();
    Serial.printf("📤 Send to [%s]:  FAIL\n", peerName);
  }
}
//================================================END======================================================================================================

//===========================================INIT ESP-NOW==================================================================================================
void setupESPNow() {
  WiFi.mode(WIFI_STA);
  turnOnBlueLED();
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("❌ ESP-NOW init failed");
    turnOffBlueLED();    // <-- fixed here: call function without 'void'
    ESP.restart();
  }
  esp_now_register_send_cb(onSendData);
  esp_now_register_recv_cb(onReceiveData);

  Serial.println(" ESP-NOW initialized");
}
//================================================END======================================================================================================

//===========================================ADD ALL PEERS=================================================================================================
void addAllPeers() {
  for (auto &peer : peers) {
    esp_now_peer_info_t info = {};
    memcpy(info.peer_addr, peer.mac, 6);
    info.channel = 0;
    info.encrypt = false;

    if (!esp_now_is_peer_exist(peer.mac)) {
      if (esp_now_add_peer(&info) == ESP_OK) {
        Serial.printf(" Added peer: %s\n", peer.name);
      } else {
        Serial.printf("Failed to add peer: %s\n", peer.name);
      }
    }
  }
}
//================================================END======================================================================================================

//===========================================DATA SEND TO PEER=============================================================================================

void sendDataToPeer(const uint8_t *peerMac, const char* message) {
  strncpy(outgoingData.sender, DEVICE_NAME, sizeof(outgoingData.sender) - 1);
  outgoingData.sender[sizeof(outgoingData.sender) - 1] = '\0';
  strncpy(outgoingData.msg, message, sizeof(outgoingData.msg) - 1);
  outgoingData.msg[sizeof(outgoingData.msg) - 1] = '\0';

  esp_err_t result = esp_now_send(peerMac, (uint8_t*)&outgoingData, sizeof(outgoingData));
  if (result != ESP_OK) {
    Serial.println(" Error");
  }
}

// SEND PERIODICALLY
void handleSendEvery(unsigned long intervalMs) {
     if (millis() - lastSendTime > intervalMs) {
        for(int i = 0; i< sizeof(peers)/sizeof(peers[0]); i++){
        sendDataToPeer(peers[i].mac, "Hello from " DEVICE_NAME);
        }
        lastSendTime = millis();
     }
}

//================================================END=========================================================================================================

//============================================STOP ESP-NOW COMMUNICATION======================================================================================
void stopESPNowCommunication() {
  Serial.println(" Stopping");

  esp_now_register_send_cb(nullptr);
  esp_now_register_recv_cb(nullptr);
  Serial.println("Callbacks unregistered");

  for (int i = 0; i < sizeof(peers)/sizeof(peers[0]); i++) {
    esp_err_t res = esp_now_del_peer(peers[i].mac);
    if (res == ESP_OK) {
      Serial.printf(" Peer %d removed\n", i);
    } else {
      Serial.printf("Failed to remove peer %d: %d\n", i, res);
    }
  }
  esp_err_t deinitRes = esp_now_deinit();
  if (deinitRes == ESP_OK) {
    Serial.println(" ESP-NOW deinitialized");
  } else {
    Serial.printf("❌ Failed to deinit ESP-NOW: %d\n", deinitRes);
  }

  Serial.println("🛑 ESP-NOW communication stopped");

  shouldSend = false;
  stopUntil = millis() + 30000UL;  // 30 seconds from now
  connectToAP();
}
//=================================================END==========================================================================================================

//======================================RESUME ESP-NOW COMMUNICATION============================================================================================
void resumeESPNowCommunication() {
  disconnect();
  setupESPNow();
  addAllPeers();
  shouldSend = true;
  Serial.println("▶️ communication resumed");
}

#endif
