#define Buzzer 4
void setup(){
    Serial.begin(115200);
    Serial.println("Hello Buzz!");
    pinMode(Buzzer, OUTPUT);
}

void loop(){
    delay(1000);
    digitalWrite(Buzzer, HIGH);
    delay(1000);
    digitalWrite(Buzzer, LOW);
}