/*OLED*/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

/*Transmitter*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "printf.h"

const uint64_t pipeOut = 0xE8E8F0F0E1LL;

RF24 radio(9, 10);

struct MyData {
  byte roll;
  byte pitch;
  byte throttle;
  byte yaw;
  byte mode;
};

MyData data;

void resetData() 
{
  data.throttle = 0;
  data.yaw = 127;
  data.pitch = 127;
  data.roll = 127;
  data.mode = 0;
}

void setup()   {                
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();   // clears the screen and buffer

  printf_begin();
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.printDetails();
  radio.openWritingPipe(pipeOut);
  resetData();
}

int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse)
{
  val = constrain(val, lower, upper);
  if ( val < middle )
    val = map(val, lower, middle, 0, 128);
  else
    val = map(val, middle, upper, 128, 255);
  return ( reverse ? 255 - val : val );
}

void loop() {

  int sig = analogRead(A6);

  if(sig <= 200)
  {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Aeonic Control");
    display.setTextSize(2);
    display.setCursor(0,30);
    display.println("ANG Mode");
    display.display();
    display.clearDisplay();
  }

  if(sig >= 201 && sig <= 700)
  {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Aeonic Control");
    display.setTextSize(2);
    display.setCursor(0,30);
    display.println("ACRO Mode");
    display.display();
    display.clearDisplay();
  }  

  if(sig >= 701)
  {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Aeonic Control");
    display.setTextSize(2);
    display.setCursor(0,30);
    display.println("HZN Mode");
    display.display();
    display.clearDisplay();
  } 

  Serial.println(sig);

  data.roll = mapJoystickValues( analogRead(A0), 120, 509, 1009, false );
  data.pitch      = mapJoystickValues( analogRead(A1), 4, 500, 914, false );
  data.throttle    = mapJoystickValues( analogRead(A2), 2, 494, 1005, false );
  data.yaw     = mapJoystickValues( analogRead(A3), 56, 510, 1009, false );
  data.mode = mapJoystickValues( sig, 0, 512, 1023, false);

  radio.write(&data, sizeof(MyData));
}
