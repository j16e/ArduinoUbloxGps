/*
 * Arduino Ublox GPS
 *
 * Arduino Due, LCD shield and Ublox GPS over Software Serial
 *
 * Author: John Blackmore <john@blackmore.com>
 * Website: http://j16e.com
 */
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>


// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8,9,4,5,6,7);
const int backlightPin = 10;
const int buttonsPin = A0;
int backlightLevel = 127;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(3, 2);

// Keep track of which screen we are on
int currentScreen = 0;

// Signal Icon
byte signalIcon[8] = {
	0b00000,
	0b11111,
	0b00000,
	0b01110,
	0b00000,
	0b00100,
	0b00100,
	0b00000
};

// Direction Icon
byte courseIcon[8] = {
	0b00000,
	0b00000,
	0b00100,
	0b01110,
	0b01110,
	0b11011,
	0b10001,
	0b00000
};

// Setup Check
bool initiating = true;

void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  ss.begin(9600);
  
  // Custom characters for LCD
  lcd.createChar(0, signalIcon);
  lcd.createChar(1, courseIcon);
  
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  pinMode(backlightPin, OUTPUT);
  pinMode(buttonsPin, INPUT);
  analogWrite(backlightPin, backlightLevel);
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.write("Arduino UbloxGPS");
  delay(2500);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.write("Hello GPS!");
  lcd.setCursor(0,1);
  lcd.write("Searching...");
}

void loop() {
  // Feed the GPS data from Serial into the TinyGPS library
  while (ss.available() > 0) {
    if (gps.encode(ss.read())) {
      updateDisplay();
    }
  }
  
  // Check for user input and process
  readButtons();

  if (initiating && millis() > 5000) {
    if(gps.charsProcessed() < 10) {
      lcd.setCursor(0,1);
      lcd.write("No GPS detected");
      while(true); // effectively halts execution until reset
    } else {
      initiating = false;
    }
  }
}

void updateDisplay()
{
  /*
   * Update the display based on the currently selected screen
   */
  switch(currentScreen) {
    case 0:
      // Home Screen
      drawHomeScreen();
      break;
      
    case 1:
      // Location
      drawLocationScreen();
      break;
    
    case 2:
      // Speed & Direction
      drawSpeedDirectionScreen();
      break;
    
    case 3:
      // Accuracy
      drawAccuracyScreen();
      break;
    
    case 4:
      // Date & Time
      drawDateTimeScreen();
      break;
      
    case 5:
      // Head Up Display
      drawHudScreen();
      break;
      
    case 6:
      // Settings
      drawSettingsScreen();
      break;
    
    default:
      // nothing...
      break;
  }
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do {
    while (ss.available()) {
      gps.encode(ss.read());
    }
  } while (millis() - start < ms);
}

void drawHomeScreen() {
  lcd.setCursor(0,1);
  lcd.print("GPS detected...");
  smartDelay(1000);
  currentScreen = 1;
  changeScreen();
}

void drawLocationScreen() {
  lcd.setCursor(7,0);
  lcd.print(gps.location.lat(), 6);
  lcd.setCursor(7,1);
  lcd.print(gps.location.lng(), 6);
}

void drawSpeedDirectionScreen() {
  char spd[8] = " ";
  if (gps.speed.isValid()) {
    sprintf(spd, "%4d mph", (int)gps.speed.mph());
  }
  lcd.setCursor(8,0);
  lcd.print(spd);
  
  char dir[8] = "-";
  if (gps.course.isValid()) {
    sprintf(dir, "%4d deg", (int)gps.course.deg());
  }
  lcd.setCursor(8,1);
  lcd.print(dir);
}

void drawAccuracyScreen() {
  char sats[4] = "-";
  sprintf(sats, "%4d", (int)gps.satellites.value());
  lcd.setCursor(12,0);
  lcd.print(sats);

  char hdop[6] = "-";
  sprintf(hdop, "%6d", (int)gps.hdop.value());
  lcd.setCursor(10,1);
  lcd.print(hdop);
}

void drawDateTimeScreen() {
  char d[10] = "---------";
  if(gps.date.isValid()) {
    sprintf(d, "%04d-%02d-%02d", gps.date.year(), gps.date.month(), gps.date.day());
  }
  lcd.setCursor(6,0);
  lcd.print(d);
  
  char t[10] = "---------";
  if(gps.time.isValid()) {
    sprintf(t, "%02d:%02d:%02d", gps.time.hour(), gps.time.minute(), gps.time.second());
  }
  lcd.setCursor(8,1);
  lcd.print(t);
}

void drawHudScreen() {
  // Speed
  char spd[8] = " ";
  if (gps.speed.isValid()) {
    sprintf(spd, "%4d mph", (int)gps.speed.mph());
  }
  lcd.setCursor(8,0);
  lcd.print(spd);
  
  // Direction
  lcd.setCursor(0,1);
  lcd.write((uint8_t)1);
  char dir[3] = "-";
  if (gps.course.isValid()) {
    sprintf(dir, "%s", TinyGPSPlus::cardinal((int)gps.course.deg()));
  }
  lcd.print(dir);
  
  // Satellites
  char s[2] = "0";
  if (gps.satellites.isValid()) {
    sprintf(s, "%d", gps.satellites.value());
  }
  lcd.setCursor(0,0);
  lcd.write((uint8_t)0);
  lcd.print(s);
  
  // Time HH:MM
  char t[5] = "-";
  if(gps.time.isValid()) {
    sprintf(t, "%02d:%02d", gps.time.hour(), gps.time.minute());
  }
  lcd.setCursor(11,1);
  lcd.print(t);
}

void drawSettingsScreen() {
  char b[4];
  sprintf(b, "%4d", backlightLevel);
  lcd.setCursor(12,0);
  lcd.print(b);
}

void readButtons() {
  int x;
  x = analogRead (0);
  bool screenChanged = false;
  if (x < 60) {
    // Right
    backlightLevel += 20;
    smartDelay(200);
  }
  else if (x < 200) {
    // Up
    currentScreen -= 1;
    screenChanged = true;
  }
  else if (x < 400){
    // Down
    currentScreen += 1;
    screenChanged = true;
  }
  else if (x < 600){
    // Left
    backlightLevel -= 20;
    smartDelay(200);
  }
  else if (x < 800){
    // Select
    currentScreen = 0;
    screenChanged = true;
  }
  
  // bounds checking
  if (currentScreen > 6) {
    currentScreen = 1;
  } else if (currentScreen < 0) {
    currentScreen = 6;
  }
  
  if (backlightLevel > 255) {
    backlightLevel = 255;
  } else if (backlightLevel < 0) {
    backlightLevel = 0;
  }
  // TODO: only need to do this if the level changed ;)
  analogWrite(backlightPin, backlightLevel);
  
  if (screenChanged) {
    changeScreen();
  }
}

void changeScreen() {
  char* splash;
  char* line1;
  char* line2;
  
  switch(currentScreen) {
    case 0:
      splash = "Arduino UbloxGPS";
      line1 = "Hello GPS!";
      line2 = "Searching...";
      break;
      
    case 1:
      splash = "1. Location";
      line1 = "Lat:";
      line2 = "Lng:";
      break;
    
    case 2:
      splash = "2. Speed/Course";
      line1 = "Speed:";
      line2 = "Course:";
      break;
    
    case 3:
      splash = "3. Accuracy";
      line1 = "Satellites:";
      line2 = "HDOP:";
      break;
    
    case 4:
      splash = "4. Date/Time";
      line1 = "Date:";
      line2 = "Time:";
      break;
    
    case 5:
      splash = "5. HUD Mode";
      line1 = "";
      line2 = "";
      break;
    
    case 6:
      splash = "6. Settings";
      line1 = "Backlight:";
      line2 = " ";
      break;
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(splash);
  smartDelay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
}
