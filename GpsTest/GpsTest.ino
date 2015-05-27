/*
 * Ublox GPS Test
 *
 * Arduino Due, LCD shield and Ublox GPS on Software Serial
 */
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>


// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8,9,4,5,6,7);
const int backlight = 10;
const int buttonsPin = A0;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(3, 2);


void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  ss.begin(9600);
  
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  pinMode(backlight, OUTPUT);
  pinMode(buttonsPin, INPUT);
  analogWrite(backlight, 127);
  
  lcd.setCursor(0,0);
  lcd.write("Hello GPS!");
  lcd.setCursor(0,1);
  lcd.write("Searching...");
}

void loop() {
  // Feed the GPS data from Serial into the TinyGPS library
  while (ss.available() > 0) {
    if (gps.encode(ss.read())) {
      displayInfo();
    }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.write("No GPS detected!");
    while(true);
  }
}

void displayInfo()
{
  // Location
  if (gps.location.isValid())
  {
    lcd.setCursor(0,0);
    lcd.write("Pos:");
    lcd.print(gps.location.lat(), 2);
    lcd.write(",");
    lcd.print(gps.location.lng(), 2);
  }
  
  // Time
  if (gps.time.isValid())
  {
    lcd.setCursor(0,1);
    lcd.write("Time: ");
    if (gps.time.hour() < 10) lcd.write("0");
    lcd.print(gps.time.hour());
    lcd.write(":");
    if (gps.time.minute() < 10) lcd.write("0");
    lcd.print(gps.time.minute());
    lcd.write(":");
    if (gps.time.second() < 10) lcd.write("0");
    lcd.print(gps.time.second());
  }
  
}
