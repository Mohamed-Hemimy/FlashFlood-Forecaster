# FlashFlood Forecaster  

FlashFlood Forecaster is an IoT-based system designed to predict and detect flash floods using environmental data collected by sensors. This project uses an ESP-32 microcontroller to monitor temperature, humidity, and atmospheric pressure to estimate the likelihood of flash floods and employs a rain drop sensor to detect water presence during active flooding. The system provides real-time alerts and uploads data to ThingSpeak for visualization and analysis.  

---

## Features  

- **Predictive Flash Flood Monitoring**:  
  Uses temperature, humidity, and pressure sensors to calculate the likelihood of a flash flood.  
  - **Alert System**:  
    - LED indicator activates when the likelihood exceeds 50%.  

- **Active Flood Detection**:  
  Detects water using a rain drop sensor.  
  - **Buzzer Alarm**:  
    - Activates immediately upon water detection.  

- **Data Storage and Visualization**:  
  - Uploads collected sensor data to ThingSpeak.  
  - Data is visualized for further analysis and long-term monitoring.  

---

## Technologies Used  

- **Hardware**:  
  - ESP-32 Microcontroller  
  - Humidity, Temperature, and Pressure Sensors (e.g., BME280)  
  - Rain Drop Sensor  
  - LED and Buzzer for Alarm Systems  

- **Software**:  
  - Arduino IDE for programming  
  - ThingSpeak for cloud data storage and visualization  

---

## Installation  

No setup is required! Simply:  
1. Download this repository.  
2. Open the code file in the Arduino IDE.  
3. Upload it to the ESP-32 microcontroller.  

---

## Contribution  

Contributions are welcome! If you'd like to improve this project or adapt it for other flood detection purposes, feel free to:  
1. Fork this repository.  
2. Make your changes.  
3. Submit a pull request with a clear explanation of your updates.  

---

## License  

This project is licensed for academic and educational purposes only. You may use or modify this code for learning, research, or real-world flood detection projects. **Commercial use is strictly prohibited.**  

If you'd like to use this project commercially, please contact the author for permission.  

---

## Acknowledgments  

Special thanks to all contributors and the open-source community for supporting IoT-based environmental monitoring initiatives.  
