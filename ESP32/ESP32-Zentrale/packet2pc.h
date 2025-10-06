#ifndef PACKET2PC_H
#define PACKET2PC_H


#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <String>
#include "incomingData.h"
#include <esp_now.h>

#define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID "abcd1234-abcd-1234-abcd-1234567890ab"
#define READY_CHARACTERISTIC_UUID "abcd1234-abcd-1234-abcd-1234567890ac"  // New UUID

void sendSendersOverBLE();

BLECharacteristic* pCharacteristic;
BLECharacteristic* pReadyCharacteristic;

bool deviceConnected = false;
bool readyToSendBLE = false;

class MyServerCallbacks : public BLEServerCallbacks {

void onConnect(BLEServer* pServer) override{
    deviceConnected = true;
    Serial.println("✅ BLE device connected");  
  }


  void onDisconnect(BLEServer* pServer) override {
    deviceConnected = false;
    Serial.println("❌ BLE device disconnected");
  }
};

class ReadyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pChar) override {
    String value = pChar->getValue();
    Serial.printf("📨 Received write: %s\n", value.c_str());
    if (value == "READY" ) {
       Serial.println("✅ Client is ready. Sending data...");
       readyToSendBLE = true;
       } else {
          Serial.println("🚫 Already sent data, ignoring READY.");
       }
    }
  
};

void sendChunkedNotification(const String& payload) {
  const size_t maxChunkSize = 20;  // Default BLE payload size
  String wrappedPayload = "START:" + payload + "END.";
  size_t len = wrappedPayload.length();
  size_t index = 0;

  while (index < len && deviceConnected) {
    // Calculate the end index of this chunk
    size_t chunkEnd = index + maxChunkSize;
    if (chunkEnd > len) chunkEnd = len;

    // Take tentative chunk
    String chunk = wrappedPayload.substring(index, chunkEnd);

    // If chunk is full size and next char exists, try to avoid splitting tokens
    if (chunkEnd < len) {
      // Find last comma or space in chunk to split on
      int splitPos = chunk.lastIndexOf(',');
      if (splitPos == -1) splitPos = chunk.lastIndexOf(' ');
      // Only split if found and not too close to start
      if (splitPos > 0) {
        chunk = chunk.substring(0, splitPos + 1);  // include the comma or space
        chunkEnd = index + chunk.length();
      }
    }

    pCharacteristic->setValue(chunk.c_str());
    pCharacteristic->notify();
    Serial.printf("📤 Chunk sent: %s\n", chunk.c_str());

    index = chunkEnd;
    delay(1500);  // Allow client to process each chunk
  }
}


void sendSendersOverBLE() {
  if (!deviceConnected) {
    Serial.println("🚫 No BLE device connected.");
    return;
  }

  Serial.println("📤 Sending devices over BLE...");

  for (const auto& dev : devices) {
    DeviceData sender = dev;

    String payload;
    char macStr[18];
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X",
            sender.mac[0], sender.mac[1], sender.mac[2],
            sender.mac[3], sender.mac[4], sender.mac[5]);

    payload += "MAC: ";
    payload += macStr;
    payload += " | Battery: " + String(sender.packet.battery);
    payload += " | RSSI: ";

    for (int i = 0; i < RSSI_COUNT; ++i) {
      payload += String(sender.packet.rssi[i]) + ",";
    }

    Serial.printf("📦 Sending payload (%d bytes): %s\n", payload.length(), payload.c_str());

    sendChunkedNotification(payload);  // 🔹 Send in chunks
    delay(1000);  // Wait before sending next device
  }

  Serial.println("✅ All devices sent.");
  devices.clear();
  sentOverBLE = true;
  Serial.println("🧹 Device buffer cleared.");
}


void bleInit() {
  BLEDevice::init("ESP32_Sender");
  BLEServer* pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService* pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
 pCharacteristic->addDescriptor(new BLE2902());

  pReadyCharacteristic = pService->createCharacteristic(
                        READY_CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
                      );
pReadyCharacteristic->setCallbacks(new ReadyCallbacks());
Serial.println("🛬 Ready characteristic initialized");

  pService->start();
  
  pServer->getAdvertising()->start();
  Serial.println("BLE started");
  delay(2000);
  Serial.println(BLEDevice::getAddress().toString().c_str());

}
  
 void calltosend() {

   if (sentOverBLE) return;  // Prevent double entry
   Serial.println("🚫 Already sent over BLE, skipping init.");
    //sentOverBLE = true;

   Serial.printf("📦 Devices in buffer: %d\n", devices.size());

    esp_err_t deinitRes = esp_now_deinit();
  if (deinitRes == ESP_OK) {
    Serial.println(" ESP-NOW deinitialized");
  } else {
    Serial.printf("❌ Failed to deinit ESP-NOW: %d\n", deinitRes);
  }

  Serial.println("🛑 ESP-NOW communication stopped");
  bleInit();
}
#endif