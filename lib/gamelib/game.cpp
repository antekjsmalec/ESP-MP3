#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <iostream>
#include <string>
#include "game.h"

#define BTNOK 14
#define BTNR 32
#define BTNL 5
#define BTNU 13
#define BTND 12
#define BTNH 17

#define SCRH 64
#define SCRW 128

bool BTNOK_pressed = false;
bool BTNR_pressed = false;
bool BTNL_pressed = false;
bool BTNU_pressed = false;
bool BTND_pressed = false;
bool BTNH_pressed = false;

void updateButtons() {
    BTNOK_pressed = (digitalRead(BTNOK) == LOW);
    BTNR_pressed = (digitalRead(BTNR) == LOW);
    BTNL_pressed = (digitalRead(BTNL) == LOW);
    BTNU_pressed = (digitalRead(BTNU) == LOW);
    BTND_pressed = (digitalRead(BTND) == LOW);
    BTNH_pressed = (digitalRead(BTNH) == LOW);
}

void object(int type, int sizeX, int sizeY, int posX, int posY, bool fill) {
  display.clearDisplay();
  switch (type)
  {
  case 0:
    if(fill == true){
        display.fillRect(posX, posY, sizeX, sizeY, 1);
    }else{
        display.drawRect(posX, posY, sizeX, sizeY, 1);
    }
    break;
  
  case 1:
    if(fill == true){
        display.fillCircle(posX, posY, sizeX, 1);
    }else{
        display.drawCircle(posX, posY, sizeX, 1);
    }
    break;
  }
  display.display();
}

void screenSetup(){
  pinMode(BTNR, INPUT_PULLUP);
  pinMode(BTNL, INPUT_PULLUP);
  pinMode(BTNU, INPUT_PULLUP);
  pinMode(BTND, INPUT_PULLUP);
  pinMode(BTNOK, INPUT_PULLUP);
  pinMode(BTNH, INPUT_PULLUP);


  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  Wire.setClock(4000000); 
  
  Serial.begin(115200);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(1);
  display.setTextWrap(false);
}

Adafruit_SSD1306 display(SCRW, SCRH, &Wire, -1);
