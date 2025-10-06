import asyncio
from ble_receiver import BLEReceiver
from kalman_filter import process_rssi_file
from trilateration import rssi_to_distance, trilaterate
from visualizer import TagVisualizer
from config import ANCHORS
import math
import csv
from datetime import datetime



RSSI_LOG = (r"C:\Users\njerum\Documents\DHBW_Projekte\T1000\GUI\ESP32_Visualizer\rssi_log.csv")
MAX_MESSAGES = 3  # Stop after receiving this many messages


import os

async def collect_ble_data():
    print("📡 Starting BLE receiver...")

    ble = BLEReceiver()

    def callback(msg):
        ble.process_message(msg)  # Save to CSV inside callback

    ble.callback = callback
    await ble.connect()

    print("⏳ Waiting for messages...")

    
    initial_lines = 0
    if os.path.exists(RSSI_LOG):
        with open(RSSI_LOG, "r") as f:
            initial_lines = sum(1 for _ in f)

    while True:
        await asyncio.sleep(0.1)
        with open(RSSI_LOG, "r") as f:
            current_lines = sum(1 for _ in f)
        if current_lines - initial_lines >= MAX_MESSAGES:
            break

    print(f"✅ Collected {MAX_MESSAGES} new messages.")
    await ble.disconnect()


def process_and_trilaterate():
    """Process the CSV and compute trilateration."""
    print("📈 Running Kalman filter on collected RSSI values...")

    filtered_data = process_rssi_file(RSSI_LOG)

    print("📐 Converting to distances and trilaterating...")
    anchor_order = ["ZENTRALE", "ANCHOR1", "ANCHOR2"]
    distances = []


    for anchor in anchor_order:
        rssi = filtered_data.get(anchor, None)
        if rssi is None:
            print(f"⚠️ Missing RSSI for anchor {anchor}, using -100 dBm.")
            rssi = -100
        distances.append(rssi_to_distance(rssi))

    print(f"📏 Distances: {dict(zip(anchor_order, distances))}")

    try:
        x, y = trilaterate(distances)
    except Exception as e:
        print(f"❌ Error during trilateration: {e}")
        return None

    if math.isnan(x) or math.isnan(y):
        print("❌ Position could not be calculated.")
        return None

    print(f"📍 Estimated position: x={x:.2f}, y={y:.2f}")
    return (x, y), dict(zip(anchor_order, distances))


def save_position(x, y, path="position_log.csv"):
    with open(path, "a", newline="") as f:
        writer = csv.writer(f)
        writer.writerow([datetime.now().isoformat(), x, y])


def show_gui(position,distances):
    """Launch the GUI with the given position."""
    print("🖼️ Launching GUI...")
    visualizer = TagVisualizer()
    visualizer.update_tag_position(*position, distances=distances)
    visualizer.run()


async def main():
    
    await collect_ble_data()
    result = process_and_trilaterate()
    if result:
        position, distances = result
        save_position(*position)
        show_gui(position, distances)


if __name__ == "__main__":
    asyncio.run(main())
