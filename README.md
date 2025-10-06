# Indoor Positioning System Using ESP32-32U and Python GUI

An indoor positioning system was developed utilizing Wi-Fi Received Signal Strength Indicator (RSSI) measurements to estimate user location.  
The objective of this study was to evaluate the accuracy and reliability of the system and to provide recommendations regarding its potential for practical implementation.

---

##  Overview
This project demonstrates an **indoor positioning system (IPS)** based on **Wi-Fi RSSI values** collected from multiple ESP32-32U modules.  
The data is processed using a **Python-based GUI** to estimate and visualize the position of a moving tag device relative to fixed anchors.

The system aims to:
- Measure the performance of RSSI-based localization in indoor environments.
- Evaluate positioning accuracy and consistency.
- Provide insights into its feasibility for practical indoor navigation applications.

---

##  System Requirements

### 1. Software
- **Programming IDEs:**
  - Visual Studio Code, Code::Blocks, PyCharm, or Spyder (for GUI and data processing).
- **Arduino IDE** (for ESP32 firmware upload)
  - Required additional libraries:
    1. `WiFi.h` (built-in)
    2. `WiFiClient.h` (built-in)
    3. `Arduino_JSON` or `ArduinoJson` (for data formatting)
    4. `BLEDevice.h`, `BLEServer.h`, `BLEUtils.h`, `BLE2902.h` install library package-(used BLE for ESP to PC communication.)
       
### 2. Hardware
- **4 × ESP32-32U modules**
  - 3 devices act as **anchors** (fixed reference points).
  - 1 device acts as the **tag** (mobile node).
- **Minimum 3 external power sources** (e.g., USB power banks or adapters for each ESP32 anchor).
- **Wi-Fi access point** (router or hotspot) for network connectivity.

---

## How It Works
1. Each ESP32 anchor measures RSSI values from the tag’s Wi-Fi signal.
2. The anchors send these RSSI readings to the central ESP32 who then sends them to the pc where they are to be processed and visualized.
3. The Python GUI processes the RSSI data (filtering using Kalman filter)and applies an estimation algorithm ( trilateration).
4. The estimated position is displayed in real time on the GUI.
---

## Python GUI
The GUI visualizes the tag’s estimated location within a predefined map or coordinate system.  
---

## Installation & Usage
1. Upload the ESP32 firmware to each board using Arduino IDE.
2. Run the Python GUI script on your PC.
---

## Results & Evaluation
- The system accuracy depends on:
  - Indoor interference and multipath effects.
  - Distance between anchors.
  - RSSI signal stability.
- Preliminary tests show that the system achieves **2,5 m** in **40 m²** accuracy under controlled conditions.

---

## Future Improvements
- Implement machine learning models to improve accuracy.
- Expand the number of anchors for better coverage.
- Integrate MQTT or WebSocket for faster data transmission.

---

## License
This project is open-source and available under the [MIT License](https://github.com/martinmutegin/indoor-positioning-system-using-ESP32-32U-and-Python-GUI-/blob/main/LICENSE.md).

---

## Contact
Developed by **[Martin Mutegi Njeru]**  
📧 Email: martinmutegin@gmail.com 
🔗 GitHub: [martinmutegin](https://github.com/martinmutegin)

