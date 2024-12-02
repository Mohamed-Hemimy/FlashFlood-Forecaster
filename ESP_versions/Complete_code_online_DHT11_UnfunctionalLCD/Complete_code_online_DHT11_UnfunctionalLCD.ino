#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Wi-Fi Credentials
const char* ssid = "Galaxy A21S";
const char* password = "14141414";

// ThingSpeak API Credentials
const char* server = "http://api.thingspeak.com/update";
const char* apiKey = "OP69QZXZU16XN4R9";

// Pin Definitions
#define RAIN_ANALOG 34         // Analog pin for rain sensor
#define LDR_ANALOG 25          // Analog pin for LDR sensor
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

// Timing variables
unsigned long lastDisplayTime = 0;
unsigned long lastMeasurementTime = 0;
unsigned long lastBuzzerTime = 0;
bool displayToggle = false;

// Rain threshold for detection
const int rainThreshold = 800;

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

  // Initialize rain sensor, LDR, buzzer, and flood LED
  pinMode(RAIN_ANALOG, INPUT);
  pinMode(LDR_ANALOG, INPUT);
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
  unsigned long currentTime = millis();

  // Take measurements every 3 seconds
  if (currentTime - lastMeasurementTime >= 3000) {
    lastMeasurementTime = currentTime;

    // Read rain sensor data
    int rain_amount = analogRead(RAIN_ANALOG);
    bool isRaining = (rain_amount < rainThreshold);

    // Trigger buzzer if rain is detected
    if (isRaining && currentTime - lastBuzzerTime >= 1000) {
      digitalWrite(Buzzer, HIGH);
      delay(200);
      digitalWrite(Buzzer, LOW);
      lastBuzzerTime = currentTime;
    }

    // Read LDR sensor data
    int ldr_value = analogRead(LDR_ANALOG);

    // Read temperature, humidity, and pressure
    float temperature = dht_sensor.readTemperature();
    float humidity = dht_sensor.readHumidity();
    sensors_event_t bmp_event;
    bmp.getEvent(&bmp_event);
    float pressure = bmp_event.pressure;

    // Calculate heat index
    float heat_index = dht_sensor.computeHeatIndex(temperature, humidity);

    // Calculate flash flood likelihood
    int flashFloodLikelihood = calculateFloodLikelihood(temperature, humidity, pressure, ldr_value);

    // Respond to flash flood likelihood with LED
    if (flashFloodLikelihood >= 70) {
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
    Serial.print("LDR Value: ");
    Serial.println(ldr_value);
    Serial.print("Flood Likelihood (%): ");
    Serial.println(flashFloodLikelihood);
    Serial.println("===========================");

    // Send data to ThingSpeak (pass rain_amount as argument)
    sendToThingSpeak(temperature, humidity, pressure, ldr_value, isRaining, flashFloodLikelihood, heat_index, rain_amount);
  }
}

// Function to send data to ThingSpeak
void sendToThingSpeak(float temperature, float humidity, float pressure, int ldr_value, bool isRaining, int floodLikelihood, float heatIndex, int rain_amount) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(server) + "?api_key=" + apiKey +
                 "&field1=" + String(temperature) +
                 "&field2=" + String(humidity) +
                 "&field3=" + String(pressure) +
                 "&field4=" + String(ldr_value) +
                 "&field5=" + String(rain_amount) +  // use the rain_amount here
                 "&field6=" + String(floodLikelihood) +
                 "&field7=" + String(heatIndex) + 
                 "&field8=" + String(isRaining ? 1 : 0);

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
int calculateFloodLikelihood(float temperature, float humidity, float pressure, int ldr_value) {
  int likelihood = 0;
  if (humidity >= 50) {
    if (humidity < 60) likelihood = 20;
    else if (humidity < 80) likelihood = 50;
    else likelihood = 80;
    if (temperature < 20) likelihood += 10;
    if (temperature > 30) likelihood -= 10;
    if (temperature > 35) likelihood -= 10;
    if (pressure < 1000) likelihood += 10;
    if (pressure < 980) likelihood += 20;
    if (ldr_value < 500) likelihood += 10;
  }
  likelihood = constrain(likelihood, 0, 100);
  return likelihood;
}