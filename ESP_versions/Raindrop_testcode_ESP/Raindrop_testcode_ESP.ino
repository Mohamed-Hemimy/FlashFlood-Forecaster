#define RAIN_ANALOG 34
#define RAIN_DIGITAL 16

void setup() {
  // put your setup code here, to run once:
  pinMode(RAIN_ANALOG, INPUT);
  pinMode(RAIN_DIGITAL, INPUT);
  analogReadResolution(10);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Rain amount: ");
  Serial.println(analogRead(RAIN_ANALOG));
  if (digitalRead(RAIN_DIGITAL))
    Serial.println("It's raining!");
  delay(1000);

}