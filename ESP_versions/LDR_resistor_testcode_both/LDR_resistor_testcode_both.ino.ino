const int ledPin = 12;   //the number of the LED pin
const int   ldrPin = A0;  //the number of the LDR pin


void setup() {

  Serial.begin(9600);
   pinMode(ledPin, OUTPUT);  //initialize the LED pin as an output
  pinMode(ldrPin, INPUT);   //initialize the LDR pin as an input
}

void loop() {
  int   ldrStatus = analogRead(ldrPin);   //read the status of the LDR value

  //check   if the LDR status is <= 500
  //if it is, the LED is HIGH
Serial.println(ldrStatus);
   if (ldrStatus <=80) {

    digitalWrite(ledPin, HIGH);               //turn LED on

   }
  else {
    digitalWrite(ledPin, LOW);          //turn LED off
  }
}
