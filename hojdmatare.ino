/*
Höjdmätare
Saves a value when you first press the button. 
Saves a second value when you press again and compares those values.
Calculates the difference and displays how far up or down you moved between the times you pressed the button. 
Resets when you press the button a third time.

Uses example code from i2c_BMP280.h
By Nils Bergendorff
*/


//libraries
#include <Wire.h>
#include "i2c.h"
#include "U8glib.h"
#include "i2c_BMP280.h"

//construct object
BMP280 bmp280;
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);

//constants
const int buttonPin = 4;
const int len = 15;

//global variables
float savedValue = 0;
int buttonState = 0;
float savedValue2 = 0;
int buffer[15];
int idx = 0;

void setup() {

  //start communications
  Serial.begin(115200);

  //checks if sensor is connected and initiates it
   Serial.print("Probe BMP280: ");
    if (bmp280.initialize()) Serial.println("Sensor found");
    else
    {
        Serial.println("Sensor missing");
        while (1) {}
    }

  pinMode(buttonPin, INPUT);

  //settings
  u8g.setFont(u8g_font_unifont);

  bmp280.setEnabled(0);  // onetime-measure:
  bmp280.triggerMeasurement();
}

void loop() {

  //print to serial monitor
  Serial.println(measPres());
  Serial.println(savedValue);
  Serial.println(savedValue2);
  

  buttonState = digitalRead(buttonPin);
  //if button is pressed
  if (buttonState == LOW) {
    //if button has not been pressed before save value to savedValue
    if (savedValue == 0) {

      savedValue = measPres(); 
    //if button has been pressed once save value to savedValue2
    } else if (savedValue2 == 0) {

      savedValue2 = measPres(); 
    //if button has been pressed twice reset both values
    } else {
      savedValue = 0;
      savedValue2 = 0; 
    }
  }
  //if button has been pressed twice compare both values and show the difference on oled
  if (savedValue != 0) {
    if (savedValue2 != 0) {
      float result = (savedValue - savedValue2);
      result = result * 8;
      oledWrite("Skillnad m " + String(result)); 
    } else {
      oledWrite("1 varde kravs"); //if button has been pressed once
    }
  } else {
    oledWrite("2 varden kravs"); //if button has not been pressed
  }


  delay(200);
}

float measPres() {
  
  //measurement
  float pascal;
  bmp280.awaitMeasurement();
  bmp280.getPressure(pascal);
  bmp280.triggerMeasurement();

  //takes the mean of the 15 latest values
  buffer[idx] = pascal / 100;
  idx = (idx + 1) % len;

  float mean = 0;

  for (int i = 0; i < len; i++) {

    mean = mean + buffer[i];
  }

  return mean / len;
}

//writes input on the oled screen
void oledWrite(String text) {
  u8g.firstPage();
  do {
    u8g.drawStr(0, 36, text.c_str());
  } while (u8g.nextPage());
}
