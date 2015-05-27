# Arduino Ublox GPS

This article is a simple get up and going introduction to using the Ublox GPS module with Arduino.

## Hardware

* Arduino Duemilanove (clone) - An Uno should also work fine
* Arduino LCD Keypad Shield [link](http://www.hobbytronics.co.uk/arduino-lcd-keypad-shield)
* Ublox NEO-6M GPS Module 

## Software

* SoftSerial
* LiquidCrystal
* [TinyGPSPlus](https://github.com/mikalhart/TinyGPSPlus)

## Getting Started

First of all we need to get the hardware connected together and get some test data from the GPS module. Connect up the LCD shield to the Arduino, then wire up the GPS module as follows:

* GPS VCC to Arduino 3V3
* GPS RX to Arduino Digital 2 (Not actually needed as we only receive data)
* GPS TX to Arduino Digital 3
* GPS GND to Arduino GND

Then open up `GpsTest.ino` and upload to the Arduino. If all went well after several seconds you should start seeing the time being updated on the LCD display. After a few minutes you should start to see the Latitude and Longitude being updated as the GPS gets a position fix.

## Interface Design

The LCD Shield used is 16 characters wide and 2 rows high, so space is limited when it comes to displaying all the information that will be coming in from the GPS. For this reason I have decided to break down the data into a number of "screens":

### Screen 1: Location

* Latitude (degrees) - `gps.location.lat()`
* Longitude (degrees) - `gps.location.long()`

### Screen 2: Speed and Direction

* Current Speed (miles per hour) - `gps.speed.mph()`
* Heading (degrees) - `gps.course.deg()`

### Screen 3: Accuracy

* Number of Satellites Tracked - `gps.satellites.value()`
* Horizontal Dimension of Precision - `gps.hdop.value()`

### Screen 4: Date/Time

* Date - `gps.date.value()`
* Time - `gps.time.value()`

### Screen 5: Head Up Display

* Top Left - Number of Satellites
* Top Right - Speed
* Bottom Left - Course
* Bottom Right - Time

### Screen 6: Settings

* LCD Brightness

## Screen Transitions

In order to maximise how quickly we can update the display, we need to minimise the amount of writing to the screen. To do this we need to remember a few simple rules:

* Clear the display when transitioning between screens.
* Write the labels in after transitioning.
* Only update the values during the loop (no need to rewrite the labels every time).
* Pad values using `sprintf` to make sure if values drop in size the display is updated correctly.

## Smart Delays

It is worth noting that because we have to continually feed the serial data from the GPS chip to the TinyGPSPlus library we cannot use the default `delay` implementation, as this is a blocking operation and would stop data from being passed in time. So instead we used a custom function `smartDelay` which simply keeps feeding the serial data and nothing else until the set time elapses. We use this to add delays in screen transitions, to de-bounce button presses and some other things.

## Hardware Buttons

The LCD Shield I am using has 6 hardware buttons, of which 5 are usable from a single analog input. These buttons represent up, down, left, right, and select. The remaining button is attached to the Arduino reset line and can be used to reset the program and return the Arduino to it's original powered-on state.

## Menu System

A simple menu system has been implemented to allow us to cycle between the available screens. We keep track of the current screen index, then for each press on the "down" button we advance one screen and every press on the "up" button goes back one screen. Pressing the "select" button currently resets to the initial screen. Pressing "left" will decrease the backlight brightness of the LCD shield and pressing "right" will increase the brightness.

## Future Ideas

* Serial passthrough from GPS to USB via settings option
* Backlight On/Off in settings
* Debouncing the hardware keys, sometimes they register incorrectly
* GPS Fix indicator, perhaps a bi-colour LED?
* Backlight control on 1-16 scale, bar graph to show value?

-----

## Reference

The following list of websites provided inspiration and insight into making this project happen:

* [Arduino Ublox GPS](http://www.instructables.com/id/Arduino-Ublox-GPS/)
* [Arduino LCD Keypad Shield – Basic Menu System](http://www.hackshed.co.uk/arduino-lcd-keypad-shield-basic-menu-system/)
* [Arduino LCD Keypad Shield](http://www.hobbytronics.co.uk/arduino-lcd-keypad-shield) - Sample keypad button code

