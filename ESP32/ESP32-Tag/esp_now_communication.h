#ifndef ESP_NOW_COMMUNICATION_H
#define ESP_NOW_COMMUNICATION_H


#include <esp_now.h>
#include <WiFi.h>
#include <led.h>
#include <RSSI_AP.h>



#define DEVICE_NAME "TAG"

bool isConnected = false;
bool shouldSend = true;
unsigned long stopUntil = 0;
unsigned long ESP_NOWResume = 0;

/// === Add all peer MAC addresses ===
typedef struct {
  const char* name;
  uint8_t mac[6];
} NamedPeer;

NamedPeer peers[] = {
  {"ZENTRALE", {0x6C, 0xC8, 0x40, 0x34, 0xBD, 0x18}},
  {"ANCHOR1",  {0x6C, 0xC8, 0x40, 0x35, 0x80, 0x7C}},
  {"ANCHOR2",  {0x6C, 0xC8, 0x40, 0x33, 0x84, 0xB0}},
//{"TAG",      {0x6C, 0xC8, 0x40, 0x34, 0xCF, 0x0C}}
};

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

struct_message outgoingData;
struct_message incomingData;

unsigned long lastSendTime = 0;

// === RECEIVE CALLBACK ===
void onReceiveData(const esp_now_recv_info_t *recv_info, const uint8_t *incomingDataRaw, int len) {
  memcpy(&incomingData, incomingDataRaw, sizeof(incomingData));

  const char* senderName = getPeerNameByMac(recv_info->src_addr);

  Serial.printf("📩 From [%s] → %s\n",
    senderName,
    incomingData.msg
  );
}

// === SEND CALLBACK ===
void onSendData(const wifi_tx_info_t *tx_info, esp_now_send_status_t status) {
  const char* peerName = getPeerNameByMac(tx_info->des_addr);

  if ( status == ESP_NOW_SEND_SUCCESS){
      Serial.printf("📤 Send to [%s]: %s\n", 
        peerName,
         "✅ SUCCESS"
        );
  }else{
        Serial.printf("📤 Send to [%s]: %s\n", 
        peerName,
        "❌ FAIL"
        );
                turnOffWhiteLED(); 
                turnOnRedLED();
                delay(200);
                turnOffRedLED();
                turnOnWhiteLED();
  }
}

// === ESP-NOW Setup ===
void setupESPNow() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  turnOnBlueLED();

  if (esp_now_init() != ESP_OK) {
    Serial.println("❌ ESP-NOW init failed");
    turnOffBlueLED();
    ESP.restart();
  }

  esp_now_register_send_cb(onSendData);
  esp_now_register_recv_cb(onReceiveData);

  Serial.println("✅ ESP-NOW initialized");
  
  
}

// === Peer Add ===
bool addPeer(const uint8_t *mac) {
  if (!mac) return false;

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, mac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (!esp_now_is_peer_exist(mac)) {
    esp_err_t res = esp_now_add_peer(&peerInfo);
    if (res == ESP_OK) {
      Serial.println("✅ Peer added");
      return true;
    } else {
      Serial.println("❌ Failed to add peer");
      return false;
    }
  }
  return true; // already exists
}

// === Add all peers ===
void addAllPeers() {
  for (const auto &peer : peers) {
    addPeer(peer.mac);
  }
}

// === SEND FUNCTION ===
void sendDataToPeer(const uint8_t *peerMac, const char* message) {
  if (!peerMac || !message) {
    Serial.println("❌ sendDataToPeer called with null argument");
    return;
  }

  // zero before use
  memset(&outgoingData, 0, sizeof(outgoingData));

  strncpy(outgoingData.sender, DEVICE_NAME, sizeof(outgoingData.sender));
  strncpy(outgoingData.msg, message, sizeof(outgoingData.msg));

  esp_err_t result = esp_now_send(peerMac, (uint8_t *)&outgoingData, sizeof(outgoingData));
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
  stopUntil = millis() + 30000UL;  // the espnow communication  runs for 30seconds before being switched off.
  setupRSSI();
}

// RESUME ESP-NOW COMMUNICATION
void resumeESPNowCommunication() {
  disconnectRSSI();
  setupESPNow();
  addAllPeers();
  shouldSend = true;
  ESP_NOWResume = (stopUntil + 90000UL); //resume sending 25seconds after stopping
  Serial.println("▶️ ESP-NOW communication resumed");

}
#endif