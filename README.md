# FlashFlood Forecaster  

FlashFlood Forecaster is an IoT-based system designed to predict and detect flash floods using environmental data collected by sensors. This project uses an ESP-32 microcontroller to monitor temperature, humidity, and atmospheric pressure to estimate the likelihood of flash floods and employs a rain drop sensor to detect water presence during active flooding. The system provides real-time alerts and uploads data to ThingSpeak for visualization and analysis.  

---

## Features  

- **Predictive Flash Flood Monitoring**:  
  - Collects temperature and humidity data using a **DHT-11 sensor**.  
  - Monitors atmospheric pressure using a **BMP-180 sensor**.  
  - Calculates the likelihood of a flash flood based on collected data.  

- **Alert System**:  
  - **LED Indicator**: Activates when the flash flood likelihood exceeds 50%.  

- **Active Flood Detection**:  
  - Detects water presence using a **Rain Drop Sensor**.  
  - **Buzzer Alarm**: Activates immediately upon water detection.  

- **Data Storage and Visualization**:  
  - Uploads collected sensor data to **ThingSpeak**.  
  - Visualizes data trends for further analysis and decision-making.  

---

## Technologies Used  

- **Hardware**:  
  - **ESP-32 Microcontroller**  
  - **DHT-11 Sensor** for humidity and temperature  
  - **BMP-180 Sensor** for atmospheric pressure  
  - **Rain Drop Sensor** for water detection  
  - LED and Buzzer for Alarm Systems  

- **Software**:  
  - **Arduino IDE** for programming  
  - **ThingSpeak** for cloud data storage and visualization  

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

This project was developed as part of the Capstone Program at **Qena STEM School** by **Team 23316**. We would like to express our gratitude to:  
- **Our School Capstone Leaders and Teachers** for their invaluable support and guidance throughout the project.
- **Open-source communities and contributors** for providing resources that made this project possible.    

Special thanks to the wider STEM education network for empowering students to tackle real-world challenges with cutting-edge technology.  
