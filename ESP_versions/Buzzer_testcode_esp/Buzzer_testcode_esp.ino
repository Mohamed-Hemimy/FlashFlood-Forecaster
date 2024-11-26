#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address and dimensions (0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // Initialize the LCD with column and row count
  lcd.begin(16, 2); 
  // Turn on the backlight
  lcd.backlight();

  // Display "Hello, World!" on the LCD
  lcd.setCursor(0, 0); // Set cursor to column 0, row 0
  lcd.print("Hello, World!");
}

void loop() {
  // Nothing needed here
}
