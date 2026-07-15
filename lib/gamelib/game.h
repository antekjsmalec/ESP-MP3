#pragma once
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define BTNOK 14
#define BTNR 32
#define BTNL 5
#define BTNU 13
#define BTND 12
#define BTNH 17



#define SCRH 64
#define SCRW 128


extern bool BTNOK_pressed;
extern bool BTNR_pressed;
extern bool BTNL_pressed;
extern bool BTNU_pressed;
extern bool BTND_pressed;
extern bool BTNH_pressed;

extern Adafruit_SSD1306 display;


void object(int type, int sizeX, int sizeY, int posX, int posY, bool fill);
void updateButtons();
void screenSetup();