import numpy as np
from config import ANCHORS, TX_POWER, ENV_FACTOR
from kalman_filter import process_rssi_file
from scipy.optimize import least_squares



def rssi_to_distance(rssi):
    return 10 ** ((TX_POWER - rssi) / (10 * ENV_FACTOR))

def trilaterate(distances):
    """
    Estimate tag position from distances to 3 anchors using nonlinear least squares.
    distances: [d0, d1, d2] corresponding to anchors in order ZENTRALE, ANCHOR1, ANCHOR2
    """
    anchors_raw = list(ANCHORS.values())
    anchors = np.array([(a[0], a[1]) for a in anchors_raw])

    def residuals(pos):
        return [np.linalg.norm(np.array(pos) - anchor) - d for anchor, d in zip(anchors, distances)]

    initial_guess = np.mean(anchors, axis=0)
    result = least_squares(residuals, initial_guess)
    return result.x[0], result.x[1]

   


def pick_rssi_values():

    CSV_PATH = r"C:\Users\njerum\Documents\DHBW_Projekte\T1000\GUI\ESP32_Visualizer\rssi_log.csv"

    # Step 1: Get filtered RSSI values
    filtered_rssi = process_rssi_file(CSV_PATH)

    # Step 2: Map device names to distances
    anchor_order = ["ZENTRALE", "ANCHOR1", "ANCHOR2"]

    distances = []

    for anchor in anchor_order:
        rssi = filtered_rssi.get(anchor)
        if rssi is not None:
            distances.append(rssi_to_distance(rssi))
        else:
            raise ValueError(f"No RSSI value found for anchor: {anchor}")


    print(f"📏 Distances: {dict(zip(anchor_order, distances))}")

    # Step 3: Trilaterate
   

    x, y = trilaterate(distances)
    print(f"📍 Tag position estimated at: X = {x:.2f} m, Y = {y:.2f} m")

if __name__ == "__main__":
    pick_rssi_values()