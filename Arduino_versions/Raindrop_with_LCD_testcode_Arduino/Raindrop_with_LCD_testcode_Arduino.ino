#include <OneWire.h>
#include <DallasTemperature.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int raindropPin = A0;       // Analog pin for the simulated raindrop sensor
const int servoPin = 3;           // Digital pin for the servo motor
const int lcdColumns = 20;        // Number of columns in your LCD
const int lcdRows = 4;            // Number of rows in your LCD

Servo umbrellaServo;              // Create a servo object for the umbrella
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  // Change the address if needed

const int rainThreshold = 400;    // Adjust the raindrop value
 
void setup() {
    Serial.begin(9600);
    lcd.begin(lcdColumns, lcdRows);  // Initialize the LCD screen
    lcd.print("Display Rain Fall Details");
    delay(2000);
    lcd.clear();
}

void loop() 
{
    int raindropValue = analogRead(raindropPin);
    lcd.setCursor(0, 0);
    
if (raindropValue >0)
{
      		lcd.print("Yes Raining");
}
    	else 
{	lcd.print("No Rain");
}

Serial.print("Raindrop: ");
Serial.println(raindropValue);
    
if (raindropValue == 0)
      { 
        Serial.println("No Rain!");
        lcd.print("Umbrella: Not Required ");
      }
      else if (raindropValue < rainThreshold)
      {
Serial.println("Moderate Rain!");
            lcd.print("Umbrella: Open ");
      }
      else
      {
          Serial.println("Heavy Rain!");
        lcd.print("Umbrella: Open ");

      }

    delay(1000); // Adjust delay based on your needs
}
