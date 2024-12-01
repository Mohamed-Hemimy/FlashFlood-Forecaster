#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

// Create an instance of the sensor
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

void setup() {
  Serial.begin(115200); // Start serial communication
  Serial.println("Initializing BMP-180 Sensor...");

  // Initialize the BMP-180
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP-180 sensor, check wiring!");
    while (1);
  }
  Serial.println("BMP-180 Initialized!");
}

void loop() {
  sensors_event_t event;
  bmp.getEvent(&event);

  if (event.pressure) {
    // Display the pressure
    Serial.print("Pressure: ");
    Serial.print(event.pressure);
    Serial.println(" hPa");

    // Display the temperature
    float temperature;
    bmp.getTemperature(&temperature);
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    // Calculate and display altitude
    float seaLevelPressure = 1013.25; // Standard sea level pressure in hPa
    Serial.print("Altitude: ");
    Serial.print(bmp.pressureToAltitude(seaLevelPressure, event.pressure));
    Serial.println(" meters");
  } else {
    Serial.println("Sensor error.");
  }

  delay(2000); // Wait for 2 seconds before the next reading
}
