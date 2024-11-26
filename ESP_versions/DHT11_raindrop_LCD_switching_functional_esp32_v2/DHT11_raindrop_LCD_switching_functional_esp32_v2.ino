#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pin Definitions
#define RAIN_ANALOG 34         // Analog pin for rain sensor
#define Buzzer 26              // Digital pin for buzzer
#define DHT_SENSOR_PIN 13      // ESP32 pin GPIO13 connected to DHT11 sensor
#define DHT_SENSOR_TYPE DHT11  // Define DHT sensor type
#define Rain_LED 27            // LED for rain alert
#define DHT_LED 14             // LED for flash flood probability
#define I2C_SDA 21             // Define your SDA pin
#define I2C_SCL 22             // Define your SCL pin

// Initialize DHT and LCD
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16x2 LCD

// Timing variables
unsigned long lastMeasurementTime = 0;  // Timing for measurements
unsigned long lastDisplayTime = 0;      // Timing for alternating display
unsigned long lastFlashTime = 0;        // Timing for LED flashing
bool displayTempHumidity = true;        // Flag to alternate display between temp/humidity and rain/likelihood

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize rain sensor and buzzer
  pinMode(RAIN_ANALOG, INPUT);
  analogReadResolution(10); // Set ADC resolution to 10 bits
  pinMode(Buzzer, OUTPUT);
  pinMode(Rain_LED, OUTPUT);
  pinMode(DHT_LED, OUTPUT);

  // Initialize the DHT sensor
  dht_sensor.begin();

  // Initialize the LCD
  Wire.begin(I2C_SDA, I2C_SCL);
  lcd.init();
  lcd.backlight();

  Serial.println("Flash Flood Monitoring System Initialized");
  lcd.setCursor(0, 0);
  lcd.print("System Init...");
  delay(2000);
}

void loop() {
  unsigned long currentTime = millis(); // Get the current time

  // Take measurements every 2 seconds
  if (currentTime - lastMeasurementTime >= 2000) {
    lastMeasurementTime = currentTime;

    // Read sensor data
    int rain_amount = analogRead(RAIN_ANALOG);
    float temperature = dht_sensor.readTemperature();
    float humidity = dht_sensor.readHumidity();

    // Ensure readings are valid
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Error: Invalid temperature or humidity reading!");
      lcd.setCursor(0, 1);
      lcd.print("Sensor Error!");
      return;
    }

    // Calculate flash flood likelihood
    int flashFloodLikelihood = calculateFloodLikelihood(temperature, humidity);

    // Switch display every 1 second
    if (currentTime - lastDisplayTime >= 1000) {
      lastDisplayTime = currentTime;
      displayTempHumidity = !displayTempHumidity;  // Toggle display mode
    }

    // Display data based on the selected mode
    if (displayTempHumidity) {
      // Display temperature and humidity on LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("T:");
      lcd.print(temperature);
      lcd.print("C H:");
      lcd.print(humidity);
      lcd.print("%");
    } else {
      // Display rain amount and flood likelihood on LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Rain: ");
      lcd.print(rain_amount);
      lcd.setCursor(0, 1);
      lcd.print("Likelihood: ");
      lcd.print(flashFloodLikelihood);
      lcd.print("%");
    }

    // Trigger rain alert if necessary
    if (rain_amount > 300) { // Adjust the threshold based on your sensor's output range
      rainAlert();
    } else {
      stopRainAlert(); // Stop alert if no rain
    }

    // Print to Serial Monitor
    Serial.print("Rain amount: ");
    Serial.println(rain_amount);
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print("%  |  Temperature: ");
    Serial.print(temperature);
    Serial.println("°C");
    Serial.print("Flash Flood Likelihood: ");
    Serial.print(flashFloodLikelihood);
    Serial.println("%");
  }

  // Keep checking LED flashing outside of the 2-second interval
  flashLED(0); // Default to no flashing if not called in calculation
}

// Function to calculate flash flood likelihood
int calculateFloodLikelihood(float temperature, float humidity) {
  int likelihood = 0;

  if (humidity >= 50) {
    if (humidity < 60) likelihood = 20;
    else if (humidity < 80) likelihood = 50;
    else likelihood = 80;

    // Adjust likelihood based on temperature
    if (temperature < 20) likelihood += 10;
    if (temperature > 30) likelihood -= 10;
    if (temperature > 35) likelihood -= 10;

    // Ensure bounds
    if (likelihood > 100) likelihood = 100;
    if (likelihood < 0) likelihood = 0;
  }

  return likelihood;
}

// Function to alert for rain
void rainAlert() {
  digitalWrite(Rain_LED, HIGH); // Turn on the rain LED
  digitalWrite(Buzzer, HIGH);  // Activate the buzzer
}

// Function to stop rain alert
void stopRainAlert() {
  digitalWrite(Rain_LED, LOW); // Turn off the rain LED
  digitalWrite(Buzzer, LOW);   // Deactivate the buzzer
}

// Function to flash LED based on likelihood
void flashLED(int likelihood) {
  if (likelihood < 30) {
    digitalWrite(DHT_LED, LOW); // Ensure the LED is off
    return; // Exit the function if likelihood is below 30%
  }

  unsigned long currentTime = millis();
  int blinkDelay = 1000; // Default delay

  // Blink faster for higher likelihood
  if (likelihood >= 30 && likelihood < 50) blinkDelay = 1000;
  else if (likelihood >= 50 && likelihood < 80) blinkDelay = 500;
  else if (likelihood >= 80) blinkDelay = 250;

  if (currentTime - lastFlashTime >= blinkDelay) {
    digitalWrite(DHT_LED, !digitalRead(DHT_LED)); // Toggle LED
    lastFlashTime = currentTime;
  }
}