#define RAIN_ANALOG 34
#define Buzzer 26

void setup() {
  // put your setup code here, to run once:
  pinMode(RAIN_ANALOG, INPUT);
  analogReadResolution(10);
  Serial.begin(9600);
  pinMode(Buzzer, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Rain amount: ");
  int rain_amount = analogRead(RAIN_ANALOG);
  Serial.println(analogRead(RAIN_ANALOG));
  delay(1000);
  if (rain_amount < 750){
    digitalWrite(Buzzer, HIGH);
    delay(500);
    digitalWrite(Buzzer, LOW);
    delay(500);
  }
}