#ifndef DATA_PACKET_H
#define DATA_PACKET_H

#include <vector>
#include <esp_now.h>
#include <WiFi.h>


// Constants
#define BATTERY_ADC_PIN 34
#define R1 100000
#define R2 100000
#define RSSI_COUNT 10
#define MAX_DEVICES 3

extern bool shouldReadSelfPacket;
extern int8_t toBuffer[];
extern bool sentOverBLE;
extern bool deviceConnected;
extern bool readyToSendBLE;

void sendSendersOverBLE();

// Structure of the incoming data packet
struct DataPacket {
  int8_t rssi[RSSI_COUNT];
  uint8_t battery;
};


// Structure to hold information per device
struct DeviceData {
  uint8_t mac[6];
  DataPacket packet;

  bool equals(const uint8_t* otherMac) const {
    for (int i = 0; i < 6; i++) {
      if (mac[i] != otherMac[i]) return false;
    }
    return true;
  }
};

// Global data container
extern std::vector<DeviceData> devices;

DeviceData* findDevice(const uint8_t* mac);
DeviceData* addOrUpdateDevice(const uint8_t* mac, const DataPacket& packet);
void printAllDevices();
void saveSelfData();
void espnowconfig();
void onDataRecv(const esp_now_recv_info_t* recv_info, const uint8_t* data, int len);
#endif
