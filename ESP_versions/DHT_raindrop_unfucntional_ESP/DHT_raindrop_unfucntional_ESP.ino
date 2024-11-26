#include <DHT22.h>  // Include the DHT22 library

// Pin Definitions
#define RAIN_ANALOG 34
#define Buzzer 26
#define DHTPIN 25
#define Rain_LED 27
#define DHT_LED 24

DHT22 dht22(DHTPIN);  // Create a DHT22 object using the SDA pin

unsigned long lastFlashTime = 0;    // Timing for LED flashing
unsigned long lastBuzzerTime = 0;   // Timing for buzzer alerts
unsigned long lastMeasurementTime = 0; // Timing for measurements
bool buzzerState = false;           // State of the buzzer

void setup() {
  pinMode(RAIN_ANALOG, INPUT);
  analogReadResolution(10);
  pinMode(Buzzer, OUTPUT);
  pinMode(Rain_LED, OUTPUT);
  pinMode(DHT_LED, OUTPUT);
  Serial.begin(115200); // Baud rate for DHT22 library
  Serial.println(F("Weather Monitoring System Initialized"));
}

void loop() {
  unsigned long currentTime = millis(); // Get the current time

  // Take measurements every 3 seconds
  if (currentTime - lastMeasurementTime >= 3000) {
    lastMeasurementTime = currentTime;

    // Read sensor data
    int rain_amount = analogRead(RAIN_ANALOG);
    float temperature = dht22.getTemperature();
    float humidity = dht22.getHumidity();

    // Print rain amount
    Serial.print("Rain amount: ");
    Serial.println(rain_amount);

    // Rain alert
    if (rain_amount < 750) {
      rainAlert();
    }

    // Check sensor validity
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println(F("Error: Invalid temperature or humidity reading!"));
      return; // Skip further processing if readings are invalid
    }

    // Print temperature and humidity
    Serial.print(F("Humidity: "));
    Serial.print(humidity);
    Serial.print(F("%  Temperature: "));
    Serial.print(temperature);
    Serial.println(F("°C"));

    // Calculate flash flood likelihood
    int flashFloodLikelihood = calculateFloodLikelihood(temperature, humidity);

    // Flash LED and trigger buzzer based on likelihood
    flashLED(flashFloodLikelihood);
    floodBuzzerAlert(flashFloodLikelihood);
  }

  // Keep checking LED flashing and buzzer alerts outside of the 3-second interval
  flashLED(0); // Default to no flashing if not called in calculation
  floodBuzzerAlert(0); // Default to no buzzing if not called in calculation
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

  Serial.print(F("Flash Flood Likelihood: "));
  Serial.print(likelihood);
  Serial.println(F("%"));

  return likelihood;
}

// Function to alert for rain
void rainAlert() {
  digitalWrite(Rain_LED, HIGH);
  digitalWrite(Buzzer, HIGH);
  delay(500);
  digitalWrite(Rain_LED, LOW);
  digitalWrite(Buzzer, LOW);
  delay(500);
}

// Function to flash LED based on likelihood
void flashLED(int likelihood) {
  unsigned long currentTime = millis();
  int blinkDelay = 1000;

  if (likelihood >= 50 && likelihood < 60) blinkDelay = 1000;
  else if (likelihood >= 60 && likelihood < 80) blinkDelay = 500;
  else if (likelihood >= 80) blinkDelay = 250;

  if (currentTime - lastFlashTime >= blinkDelay) {
    digitalWrite(DHT_LED, !digitalRead(DHT_LED)); // Toggle LED
    lastFlashTime = currentTime;
  }
}

// Function to alert with buzzer based on flood likelihood
void floodBuzzerAlert(int likelihood) {
  unsigned long currentTime = millis();

  if (likelihood >= 70 && likelihood < 80) {
    if (currentTime - lastBuzzerTime >= 1000) {
      buzzerState = !buzzerState;
      digitalWrite(Buzzer, buzzerState ? HIGH : LOW);
      if (!buzzerState) lastBuzzerTime = currentTime;
    }
  } else if (likelihood >= 80) {
    if (currentTime - lastBuzzerTime >= 500) {
      buzzerState = !buzzerState;
      digitalWrite(Buzzer, buzzerState ? HIGH : LOW);
      if (!buzzerState) lastBuzzerTime = currentTime;
    }
  } else {
    digitalWrite(Buzzer, LOW);
  }
}