import csv
import os
import pandas as pd
from datetime import datetime



csv_file_path = (r"C:\Users\njerum\Documents\DHBW_Projekte\T1000\GUI\ESP32_Visualizer\rssi_log.csv")

class KalmanFilter:
    def __init__(self, process_noise=1e-3, measurement_noise=1.0):
        self.q = process_noise               # How much we believe the value can change (e.g., sudden signal loss)
        self.r = measurement_noise           # How noisy we think the RSSI measurements are
        self.x = None                        # The best guess of the actual signal
        self.p = 1.0                         # Confidence in the estimate (lower is better)

    def update(self, measurement):
        if self.x is None:
            self.x = measurement

        self.p += self.q
        k = self.p / (self.p + self.r)
        self.x += k * (measurement - self.x)
        self.p *= (1 - k)
        return self.x

# This dictionary will hold the filtered mean RSSI for each device
filtered_rssi_data = {}

# RSSI CSV file format:
# timestamp,device_name,rssi1,rssi2,...,rssi10

def process_rssi_file(csv_file_path):
    
    filtered_rssi_data = {}

    df = pd.read_csv(csv_file_path, header=None)
    df.columns = ["timestamp", "device"] + [f"rssi_{i}" for i in range(len(df.columns) - 2)]

    anchor_data = df[df["device"].isin(["ANCHOR1", "ANCHOR2", "ZENTRALE"])]
    
    with open(csv_file_path, newline="") as file:
        reader = csv.reader(file)
        
        for anchor in ["ANCHOR1", "ANCHOR2", "ZENTRALE"]:
            kf = KalmanFilter()
            anchor_df = anchor_data[anchor_data["device"] == anchor]
            rssi_series = anchor_df[[col for col in anchor_df.columns if "rssi_" in col]].mean(axis=1)
            filtered_series = [kf.update(measurement) for measurement in rssi_series]

            # Store the **last filtered value** for this anchor
            filtered_rssi_data[anchor] = filtered_series[-1] if filtered_series else -100

        
        return filtered_rssi_data