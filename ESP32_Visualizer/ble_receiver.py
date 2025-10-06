import asyncio
from bleak import BleakClient, BleakScanner
import csv
from datetime import datetime


class BLEReceiver:
    def __init__(self, callback=None):
        self.callback = callback
        self.client = None
        self.TARGET_MAC = "6C:C8:40:34:BD:1A"
        self.CHARACTERISTIC_UUID = "abcd1234-abcd-1234-abcd-1234567890ab"
        self.READY_CHARACTERISTIC_UUID = "abcd1234-abcd-1234-abcd-1234567890ac"

        self.chunk_buffer = ""
        self.message_started = False


    def process_message(self,full_message):
       mac_name_map = {
        "6C:C8:40:34:BD:18": "ZENTRALE",
        "6C:C8:40:35:80:7C": "ANCHOR1",
        "6C:C8:40:33:84:B0": "ANCHOR2",
        "6C:C8:40:34:CF:0C": "TAG"
       }


       device_name = "UNKNOWN"
       if "MAC:" in full_message:
           try:
              mac_part = full_message.split("MAC:")[1].split("|")[0].strip().upper()
              device_name = mac_name_map.get(mac_part, mac_part)
           except Exception as e:
              print(f"⚠️ Could not extract MAC: {e}")

       if "RSSI:" in full_message:
            try:
                raw_rssi = full_message.split("RSSI:")[1].split("|")[0].strip()
                rssi_values = [int(v.strip()) for v in raw_rssi.split(",") if v.strip().lstrip("-").isdigit()]
                if rssi_values:
                    self.save_rssi_to_csv(rssi_values, device_name)
                    print(f"💾 Saved {len(rssi_values)} RSSI values for {device_name}")
                else:
                    print(f"⚠️ No valid RSSI values found in: {raw_rssi}")
            except Exception as e:
                print(f"⚠️ Failed to parse RSSI: {e}")


    def save_rssi_to_csv(self, rssi_list, device_name):
            
        RSSI_LOG = (r"C:\Users\njerum\Documents\DHBW_Projekte\T1000\GUI\ESP32_Visualizer\rssi_log.csv")
        print(f"💾 Writing to CSV: device={device_name}, RSSI count={len(rssi_list)}")

        try:
            with open(RSSI_LOG, mode="a", newline="") as file:
                writer = csv.writer(file)
                timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                writer.writerow([timestamp, device_name] + rssi_list)
        except Exception as e:
            print(f"error writting in csv:{e}")

    def handle_notify(self, sender, data: bytearray):
            # Decode bytes safely to string, ignoring errors
            chunk = data.decode("utf-8", errors="ignore").strip()
            print(f"📦 Chunk received: {chunk}")

            while chunk:
                if not self.message_started:
                    start_idx = chunk.find("START:")
                    if start_idx == -1:
                        # Start marker not found, discard chunk
                        return
                    self.message_started = True
                    self.chunk_buffer = ""
                    chunk = chunk[start_idx + len("START:"):]
                end_idx = chunk.find("END.")
                if end_idx == -1:
                    # No end marker found yet, accumulate and wait for more
                    self.chunk_buffer += chunk
                    chunk = ""
                else:
                    # End marker found: accumulate and process full message
                    self.chunk_buffer += chunk[:end_idx]
                    full_message = self.chunk_buffer.strip()
                    print(f"📥 Full message reassembled: {full_message}")

                    # Call user callback with the full message
                    if self.callback:   
                        try:
                                self.callback(full_message)
                        except Exception as e:
                                print(f"⚠️ callback error: {e}")

                        # Reset buffer and state for next message
                        self.chunk_buffer = ""
                        self.message_started = False

                        # Continue with any leftover data after END.
                        chunk = chunk[end_idx + len("END.") :]

    async def connect(self):
            print("🔍 Scanning for ESP32 devices...")
            devices = await BleakScanner.discover(timeout=20.0)
            esp = next((d for d in devices if d.address.upper() == self.TARGET_MAC), None)

            if not esp:
                print("❌ ESP32 device not found.")
                return False

            print(f"✅ Found ESP32: {esp.name} ({esp.address})")

            self.client = BleakClient(esp.address)
            try:
                await self.client.connect()
            except Exception as e:
                print(f"❌ Failed to connect: {e}")
                return False

            await self.client.start_notify(self.CHARACTERISTIC_UUID, self.handle_notify)
            print("✅ Notifications enabled.")

            # Give device time to settle
            await asyncio.sleep(1)

            await self.client.write_gatt_char(self.READY_CHARACTERISTIC_UUID, b"READY", response=True)
            print("📨 Sent READY signal.")
            return True

    async def disconnect(self):
        if self.client and self.client.is_connected:
            await self.client.disconnect()
            print("🔌 Disconnected from ESP32")

