#include "incomingData.h"
#include "wifi_manager.h"
#include "packet2pc.h"



uint8_t centralMac[6];

bool shouldReadSelfPacket = false;
int8_t toBuffer[10];
uint8_t blacklistedMac[6] = {0x6C, 0xC8, 0x40, 0x34, 0xCF, 0x0C};  // prevents rogue send  from other esp or bluetooth devices 
bool sentOverBLE = false;


std::vector<DeviceData> devices;

// Find existing device by MAC
DeviceData* findDevice(const uint8_t* mac) {
  for (auto& dev : devices) {
    if (dev.equals(mac)) return &dev;
  }
  return nullptr;
}

// Add or update device info
DeviceData* addOrUpdateDevice(const uint8_t* mac, const DataPacket& packet) {
  DeviceData* existing = findDevice(mac);
  if (existing) {
    existing->packet = packet;
    return existing;
  }

  if (devices.size() < MAX_DEVICES) {
    DeviceData newDev;
    memcpy(newDev.mac, mac, 6);
    newDev.packet = packet;
    devices.push_back(newDev);
    return &devices.back();
  }

  // Optional: reject if full
  Serial.println("⚠️ Max device limit reached.");
  return nullptr;
}

// Print all stored device data
void printAllDevices() {
  Serial.println("📊 Device Data Summary:");
  int uniqueCounter = 0;

  for (const auto& dev : devices) {
    Serial.printf("  MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                  dev.mac[0], dev.mac[1], dev.mac[2],
                  dev.mac[3], dev.mac[4], dev.mac[5]);

    Serial.printf("    🔋 Battery: %d%%\n", dev.packet.battery);
    Serial.print("    📶 RSSI: ");
    for (int i = 0; i < RSSI_COUNT; ++i) {
      Serial.printf("%d ", dev.packet.rssi[i]);
    }
    Serial.println("\n");

    uniqueCounter++;
  }
    if (uniqueCounter == MAX_DEVICES && !sentOverBLE){
    calltosend(); 
}

}

void espnowconfig(){
   DataPacket selfPacket;
  for (int i=0;i<RSSI_COUNT; i++){
    toBuffer[i] = (int8_t)WiFi.RSSI();
  }

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  WiFi.macAddress(centralMac);

  Serial.printf("🧠 Central MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                centralMac[0], centralMac[1], centralMac[2],
                centralMac[3], centralMac[4], centralMac[5]);

  if (esp_now_init() != ESP_OK) {
    Serial.println("❌ ESP-NOW init failed");
    ESP.restart();
  }

  esp_now_register_recv_cb(onDataRecv);

  Serial.println("✅ zentrale started on receiver mode");
  
  // Read own RSSI/battery and store as "self"
  saveSelfData();
}



void saveSelfData() {
  DataPacket selfPacket;
  shouldReadSelfPacket = true;
  
  for (int i=0;i<RSSI_COUNT; i++){
    selfPacket.rssi[i] = toBuffer[i];
    Serial.print("RSSI[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(selfPacket.rssi[i]);
  }

  int raw = analogRead(BATTERY_ADC_PIN);
  float voltage = (raw / 4095.0) * 3.3;
  float actualVoltage = voltage * ((R1 + R2) / (float)R2);
  float percent = ((actualVoltage - 3.0) / (4.2 - 3.0)) * 100.0;
  selfPacket.battery = constrain((uint8_t)percent, 0, 100);

  shouldReadSelfPacket = false;
  
  addOrUpdateDevice(centralMac, selfPacket);
  Serial.println("📤 Self data added");
  printAllDevices();
}

void onDataRecv(const esp_now_recv_info_t* recv_info, const uint8_t* data, int len) {
  if (sentOverBLE) return;
  
  if (len != sizeof(DataPacket)) {
    Serial.println("⚠️ Invalid packet size");
    delay(2000);
    return;
  }
   

  // 🔒 Filter out rogue MAC
  if (memcmp(recv_info->src_addr, blacklistedMac, 6) == 0) {
    Serial.println("🚫 Ignored packet from blacklisted MAC");
    return;
  }

  DataPacket packet;
  memcpy(&packet, data, sizeof(DataPacket));

  addOrUpdateDevice(recv_info->src_addr, packet);

  Serial.println("📥 Packet received from peer");
  printAllDevices();
}
