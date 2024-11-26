int ldrPin = 19;     // LDR pin
int ldrVal = 0;      // Value of LDR
int ledPin = 2;     // Build in LED pin
const float GAMMA = 0.7;
const float RL10 = 50;
void setup() {
  Serial.begin(9600);  // Initialise the serial monitor
   pinMode(2, OUTPUT);
}
void loop() {
  ldrVal = analogRead(ldrPin); // Read the analog value of the LDR
   float voltage = ldrVal / 1024. * 5;
  float resistance = 2000 * voltage / (1 - voltage / 5);
  float lux = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));

 if (lux < 200) {             // If the LDR value is lower than 200
    digitalWrite(ledPin, HIGH);   // Turn buildin LED on
  } else {
    digitalWrite(ledPin, LOW);    // Turn buildin LED off
  }
  Serial.println(lux);      // Show the value in the serial monitor
  delay(1000);                 // Pause 1000ms
}
