#include <LiquidCrystal.h>

// initialize the  library by associating any needed LCD interface pin
// with the arduino pin number  it is connected to
const int rs = 12, en = 11, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int analogInput = A0;
float vout = 0.0;
float vin = 0.0;
float R1 = 100000.0;  // resistance of R1 (100K) - see schematics
float R2 = 10000.0;   // resistance of R2 (10K) - see schematics
int value = 0;

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // Print a message to the LCD.
  lcd.print("Samuel Iuele");
  pinMode(analogInput, INPUT);
  Serial.begin(9600);
  delay(10000);
}

void loop() {

  // read the value at analog input
  value = analogRead(analogInput);
  vout = (value * 5.0) / 1024.0;  // see text
  vin = vout / (R2 / (R1 + R2));
  delay(500);
  //  if (vin<0.09) {
  //   vin=0.0;//statement to quash undesired reading !
  //  }
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row,  since counting begins with 0):
  lcd.setCursor(0, 0);
  lcd.print("Volt Meter");
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  unsigned int len = sizeof(vin);
  len = len + 2;

  lcd.print(vin);
  lcd.print("  ");
  //lcd.setCursor(len, 1);
  lcd.print("Volts");
  lcd.print("  ");
}
