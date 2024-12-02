#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Wi-Fi Credentials
const char* ssid = "Galaxy A21S";
const char* password = "14141414";

// ThingSpeak API Credentials
const char* server = "http://api.thingspeak.com/update";
const char* apiKey = "OP69QZXZU16XN4R9";

// Pin Definitions
#define RAIN_ANALOG 34         // Analog pin for rain sensor
#define Buzzer 26              // Digital pin for rain buzzer
#define DHT_SENSOR_PIN 13      // ESP32 pin GPIO13 connected to DHT11 sensor
#define DHT_SENSOR_TYPE DHT11  // Define DHT sensor type
#define DHT_LED 14             // LED for flash flood probability
#define I2C_SDA 21             // I2C SDA pin for both devices
#define I2C_SCL 22             // I2C SCL pin for both devices

// Initialize DHT, LCD, and BMP-180
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16x2 LCD
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

// NTP Client setup
WiFiUDP udp;
NTPClient timeClient(udp, "pool.ntp.org", 2 * 3600, 60000); // GMT+2 (Cairo Time)

// Timing variables
unsigned long lastDisplayTime = 0;    // Timing for LCD updates
unsigned long lastMeasurementTime = 0; // Timing for measurements
unsigned long lastBuzzerTime = 0;     // Timing for rain detection buzzer
bool displayToggle = false;           // Toggles between LCD screens

// Static Rain Threshold (this does not change)
const int rainThreshold = 800;

// Dynamic Thresholds (these will change during the day)
int minTempThreshold = 15;
int maxTempThreshold = 35;
int humidityThreshold = 60;
int pressureThreshold = 1000;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected!");

  // Initialize NTP Client
  timeClient.begin();
  timeClient.setTimeOffset(7200); // GMT+2:00 for Cairo

  // Initialize rain sensor, buzzer, and flood LED
  pinMode(RAIN_ANALOG, INPUT);
  analogReadResolution(10); // Set ADC resolution to 10 bits
  pinMode(Buzzer, OUTPUT);
  pinMode(DHT_LED, OUTPUT);

  // Initialize I2C bus
  Wire.begin(I2C_SDA, I2C_SCL);

  // Initialize the DHT sensor
  dht_sensor.begin();

  // Initialize the LCD
  lcd.init();
  lcd.backlight();

  // Initialize the BMP-180 sensor
  if (!bmp.begin()) {
    Serial.println("Error: BMP-180 sensor not found. Check wiring!");
    lcd.setCursor(0, 0);
    lcd.print("BMP-180 Error!");
    while (1); // Halt execution if BMP-180 fails
  }

  Serial.println("Flash Flood Monitoring System Initialized");
  lcd.setCursor(0, 0);
  lcd.print("System Init...");
  delay(2000);
}

void loop() {
  unsigned long currentTime = millis(); // Get the current time

  // Update NTP time
  timeClient.update();
  int currentHour = timeClient.getHours(); // Get current hour in GMT+2

  // Dynamically adjust the thresholds based on the time of day
  updateDynamicThresholds(currentHour);

  // Take measurements every 30 seconds
  if (currentTime - lastMeasurementTime >= 30000) {
    lastMeasurementTime = currentTime;

    // Read rain sensor data
    int rain_amount = analogRead(RAIN_ANALOG);
    bool isRaining = (rain_amount < rainThreshold);

    // Trigger buzzer if rain is detected
    if (isRaining && currentTime - lastBuzzerTime >= 1000) { // Buzzer rings every second
      digitalWrite(Buzzer, HIGH);
      delay(200); // Buzzer on for 200ms
      digitalWrite(Buzzer, LOW);
      lastBuzzerTime = currentTime;
    }

    // Read temperature, humidity, and pressure
    float temperature = dht_sensor.readTemperature();
    float humidity = dht_sensor.readHumidity();
    sensors_event_t bmp_event;
    bmp.getEvent(&bmp_event);
    float pressure = bmp_event.pressure;

    // Calculate heat index
    float heat_index = dht_sensor.computeHeatIndex(temperature, humidity);

    // Calculate flash flood likelihood
    int flashFloodLikelihood = calculateFloodLikelihood(temperature, humidity, pressure);

    // Respond to flash flood likelihood with LED
    if (flashFloodLikelihood >= 50) {
      digitalWrite(DHT_LED, HIGH);
    } else {
      digitalWrite(DHT_LED, LOW);
    }

    // Log readings to serial monitor
    Serial.println("===== Sensor Readings =====");
    Serial.print("Rain: ");
    Serial.println(isRaining ? "YES" : "NO");
    Serial.print("Rain Amount: ");
    Serial.println(rain_amount);
    Serial.print("Temperature (°C): ");
    Serial.println(temperature);
    Serial.print("Humidity (%): ");
    Serial.println(humidity);
    Serial.print("Heat Index (°C): ");
    Serial.println(heat_index);
    Serial.print("Pressure (hPa): ");
    Serial.println(pressure);
    Serial.print("Flood Likelihood (%): ");
    Serial.println(flashFloodLikelihood);
    Serial.println("===========================");

    // Send data to ThingSpeak
    sendToThingSpeak(temperature, humidity, pressure, isRaining, flashFloodLikelihood, heat_index, rain_amount);
  }

  // Alternate LCD display every 3 seconds
  if (currentTime - lastDisplayTime >= 3000) {
    lastDisplayTime = currentTime;
    displayToggle = !displayToggle;

    // Read temperature, humidity, and pressure again for display
    float temperature = dht_sensor.readTemperature();
    float humidity = dht_sensor.readHumidity();
    sensors_event_t bmp_event;
    bmp.getEvent(&bmp_event);
    float pressure = bmp_event.pressure;
    float heat_index = dht_sensor.computeHeatIndex(temperature, humidity);
    int flashFloodLikelihood = calculateFloodLikelihood(temperature, humidity, pressure);
    bool isRaining = (analogRead(RAIN_ANALOG) < rainThreshold);

    if (displayToggle) {
      // First screen: Rain, Temperature, and Humidity
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Rain:");
      lcd.print(isRaining ? "YES" : "NO");
      lcd.print(" T:");
      lcd.print(temperature);

      lcd.setCursor(0, 1);
      lcd.print("H:");
      lcd.print(humidity);
    } else {
      // Second screen: Pressure, Flood Likelihood, and Heat Index
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("P:");
      lcd.print(pressure);
      lcd.setCursor(0, 1);
      lcd.print("FD:");
      lcd.print(flashFloodLikelihood);
      lcd.print("% HI:");
      lcd.print(heat_index);
    }
  }
}

// Function to send data to ThingSpeak
void sendToThingSpeak(float temperature, float humidity, float pressure, bool isRaining, int floodLikelihood, float heatIndex, int rain_amount) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(server) + "?api_key=" + apiKey +
                 "&field1=" + String(temperature) +
                 "&field2=" + String(humidity) +
                 "&field3=" + String(pressure) +
                 "&field4=" + String(rain_amount) +
                 "&field5=" + String(floodLikelihood) +
                 "&field6=" + String(heatIndex) + 
                 "&field7=" + String(isRaining ? 1 : 0);

    http.begin(url);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      Serial.println("Data sent to ThingSpeak successfully.");
    } else {
      Serial.print("Error sending data: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Wi-Fi not connected.");
  }
}

// Function to calculate flash flood likelihood
int calculateFloodLikelihood(float temperature, float humidity, float pressure) {
  int likelihood = 0;

  // Start with humidity-based calculation
  if (humidity >= 50) {
    if (humidity < 60) likelihood = 20;
    else if (humidity < 80) likelihood = 50;
    else likelihood = 80;

    // Adjust based on temperature
    if (temperature < minTempThreshold) likelihood += 10;
    if (temperature > maxTempThreshold) likelihood += 20;

    // Adjust based on pressure
    if (pressure < pressureThreshold) likelihood += 30;
  }

  return likelihood;
}

// Function to dynamically update thresholds based on time of day
void updateDynamicThresholds(int hour) {
  if (hour >= 6 && hour < 18) {
    minTempThreshold = 15; // Lower threshold during daytime
    maxTempThreshold = 35; // Higher threshold during daytime
    humidityThreshold = 60; // Slightly higher humidity threshold during day
    pressureThreshold = 1010; // Slightly higher pressure during day
  } else {
    minTempThreshold = 10; // Lower threshold during night
    maxTempThreshold = 30; // Higher threshold during night
    humidityThreshold = 70; // Slightly higher humidity threshold at night
    pressureThreshold = 1005; // Slightly lower pressure at night
  }
}