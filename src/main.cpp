#include <Arduino.h>
#include <game.h>
#include <vector>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <Audio.h>

Audio audio;
int volume = 15;

int snakeX = 63;
int snakeY = 31;

bool unPause = false;

uint32_t lastTrackSecond = 999;
unsigned long lastButtonPress = 0;
const unsigned long buttonCooldown = 200;

int screenSaverDirection = -1;
int screenSaverX = 64;
int screenSaverY = 32;

int gamePick = 0;
String gameList[2] = {"Snake", "Flappy Bird"};


std::vector<String> mp3List;
int mp3Pick = 0;

static const unsigned char PROGMEM menu_icons[] = {0x00,0x00,0x07,0xe0,0x00,0x00,0x00,0x00,0x00,0x03,0xf8,0x00,0x00,0x00,0x00,0x3f,0xff,0xe0,0x00,0x00,0x00,0x00,0x0f,0xfe,0x0f,0xfe,0x00,0x00,0x0f,0xff,0xf8,0x20,0x00,0x00,0x00,0x00,0x38,0x00,0x00,0x03,0xf0,0x00,0x0f,0xc0,0x07,0xe0,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x18,0x00,0x08,0x3f,0xf8,0x20,0x00,0x00,0x00,0x00,0x61,0x80,0x00,0x00,0x0c,0x00,0x0f,0xc0,0x00,0x20,0x00,0x00,0x00,0x00,0x41,0x80,0x00,0x06,0x04,0x00,0x08,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x41,0x80,0x00,0x06,0x04,0x00,0x08,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0xcf,0xf0,0x00,0x30,0x06,0x00,0x08,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x8f,0xf0,0x00,0x30,0xc2,0x00,0x08,0x00,0x00,0x40,0x00,0x00,0x00,0x01,0x81,0x80,0x00,0x00,0xc2,0x00,0x08,0x00,0x00,0x40,0x00,0x00,0x00,0x01,0x01,0x80,0x00,0x06,0x03,0x00,0x10,0x00,0x00,0x40,0x00,0x00,0x00,0x01,0x01,0x80,0x00,0x06,0x01,0x00,0x10,0x00,0x00,0x40,0x00,0x00,0x00,0x02,0x00,0x03,0xff,0x80,0x01,0x80,0x10,0x00,0x00,0x40,0x00,0x00,0x00,0x06,0x00,0x06,0x00,0x80,0x00,0xc0,0x10,0x00,0x00,0x40,0x00,0x00,0x00,0x04,0x00,0x0c,0x00,0xc0,0x00,0x40,0x10,0x00,0x00,0x40,0x00,0x00,0x00,0x08,0x00,0x08,0x00,0x40,0x00,0x60,0x10,0x00,0x03,0xc0,0x00,0x00,0x00,0x18,0x00,0x18,0x00,0x20,0x00,0x20,0x10,0x00,0x03,0xc0,0x00,0x00,0x00,0x10,0x00,0x10,0x00,0x30,0x00,0x20,0xf0,0x00,0x03,0xc0,0x00,0x00,0x00,0x10,0x00,0x30,0x00,0x18,0x00,0x60,0xf0,0x00,0x03,0xc0,0x00,0x00,0x00,0x18,0x00,0x20,0x00,0x0c,0x00,0x40,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0xc0,0x00,0x06,0x00,0x40,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x0e,0x01,0x80,0x00,0x02,0x00,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xfe,0x00,0x00,0x03,0xc1,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7f,0x00};

int menuOption = 0;
int menuX = 0;

enum sysState {screenSaver, menu, mp3Select, gameSelect, mp3Playing, runningGame};
sysState state = screenSaver;

//gaem functions
void snake(){
  display.clearDisplay();
  display.drawRect(snakeX, snakeY, 5, 5, 1);
  if(BTNL_pressed) snakeX--;
  if(BTNR_pressed) snakeX++;
  if(BTND_pressed) snakeY++;
  if(BTNU_pressed) snakeY--;

  if(snakeX < 0) snakeX = 0;
  if(snakeX > 126) snakeX = 126;
  if(snakeY < 0) snakeY = 0;
  if(snakeY > 62) snakeY = 62;

  display.display();
}

void flappyBird(){

}

//other funtions
void updateGamePick(){
  if(BTND_pressed == true && gamePick > 0){
    gamePick--;
  }
  if(BTNU_pressed == true && gamePick < 1){
    gamePick++;
  }
}

void updateMp3Pick(){
  if(mp3List.empty()){
    return;
  } 

  if(BTND_pressed == true){
    mp3Pick++;
    if(mp3Pick >= mp3List.size()) mp3Pick = mp3List.size() - 1; 
  }
  if(BTNU_pressed == true){
    mp3Pick--;
    if(mp3Pick < 0) mp3Pick = 0; 
  }
}


void mp3DirSetup(){
  File root = SD.open("/music");
  File file = root.openNextFile();

  while(file){
    if(!file.isDirectory()) {
          String fileName = file.name();  
          if(fileName.endsWith(".mp3") || fileName.endsWith(".MP3")) {
              mp3List.push_back(fileName);
              Serial.printf("Found MP3: %s\n", fileName.c_str());
          }
    }
    file = root.openNextFile();
  }

}

void playSelectSong(){
  String trackPath = "/music/" + mp3List[mp3Pick];
  Serial.print("Now playing: ");
  Serial.println(trackPath);
  audio.connecttoFS(SD, trackPath.c_str());
}

// draw screen states

void drawScreenSaver(){
  display.clearDisplay();
  display.fillCircle(screenSaverX, screenSaverY, 5, 1);

  display.display();
}

void drawMenu(void) {
    display.clearDisplay();
    display.setTextColor(1);
    display.setTextWrap(false);
    display.setCursor(24, 10);
    display.print("MP3");
    display.drawRect(menuX, 0, 64, 64, 1);
    display.setCursor(81, 9);
    display.print("Games");
    display.drawBitmap(17, 24, menu_icons, 107, 24, 1);
    display.display();
}

void drawMp3Select(){
  display.clearDisplay();
  display.fillRect(18, 29, 92, 7, 1);
  display.setTextSize(1);
  display.setTextColor(0);
  display.setCursor(19, 29);
  if(!mp3List.empty()) {
    display.print(mp3List[mp3Pick]);
  } else {
    display.print("No MP3s Found");
  }
  display.drawTriangle(64, 59, 80, 43, 48, 43, 1);
  display.drawTriangle(64, 4, 80, 20, 48, 20, 1);

  display.setCursor(1,1);
  display.setTextColor(1);
  display.println("Songs: " + String(mp3List.size()));

  display.display();
}
void drawGameSelect(){
  display.clearDisplay();
  
  display.fillRect(18, 29, 92, 7, 1);
  display.setTextColor(0);
  display.setCursor(19, 29);
  if(!mp3List.empty()) {
    display.print(gameList[gamePick]);
  } else {
    display.print("No Games Found");
  }
  display.drawTriangle(64, 59, 80, 43, 48, 43, 1);
  display.drawTriangle(64, 4, 80, 20, 48, 20, 1);
  display.display();
}
void draw_polygon_6(void) {
    display.drawLine(40, 38, 40, 50, 1);
    display.drawLine(40, 50, 30, 44, 1);
    display.drawLine(30, 44, 40, 38, 1);
}


void draw_polygon_7(void) {
    display.drawLine(83, 38, 83, 38, 1);
    display.drawLine(83, 38, 83, 50, 1);
    display.drawLine(83, 50, 93, 44, 1);
    display.drawLine(93, 44, 83, 38, 1);
}


void draw_polygon_8(void) {
    display.drawLine(56, 36, 56, 52, 1);
    display.drawLine(56, 52, 71, 44, 1);
    display.drawLine(71, 44, 56, 36, 1);
}

void drawplay(void) {
    uint32_t currentSeconds = audio.getAudioCurrentTime();
    display.clearDisplay();
    display.setTextColor(1);
    display.setTextSize(2);
    display.setTextWrap(false);
    display.setCursor(0, 4);
    display.print(mp3List[mp3Pick]);
    display.drawLine(96, 38, 96, 50, 1);
    display.drawLine(28, 38, 28, 50, 1);
    draw_polygon_6();
    draw_polygon_7();
    draw_polygon_8();
    display.setTextSize(1);
    display.setCursor(1, 21);
    display.println(String(currentSeconds / 60) + ":" + String(currentSeconds % 60));
    display.display();
}
void drawpause(void) {
    uint32_t currentSeconds = audio.getAudioCurrentTime();
    display.clearDisplay();
    display.setTextColor(1);
    display.setTextSize(2);
    display.setTextWrap(false);
    display.setCursor(0, 2);
    display.print(mp3List[mp3Pick]);
    display.drawLine(96, 38, 96, 50, 1);
    display.drawLine(28, 38, 28, 50, 1);
    draw_polygon_6();
    draw_polygon_7();
    display.drawRect(68, 37, 2, 17, 1);
    display.drawRect(58, 37, 2, 17, 1);
    display.setTextSize(1);
    display.setCursor(1, 21);
    display.print(currentSeconds);
    display.display();
}




void determineScreenDirection(){
  if(screenSaverDirection == -1 and screenSaverX <= 32){
    screenSaverDirection = 1;
  }
  if(screenSaverDirection == 1 and screenSaverX >= 96){
    screenSaverDirection = -1;
  }
  screenSaverX = screenSaverX + screenSaverDirection;
}

// actual stuff

void setup(){
  Serial.begin(115200);
  screenSetup();
  
  SPI.begin(18, 19, 23, 5); 

  if (!SD.begin(5, SPI)) {
      Serial.println("Card Mount Failed!");
      return;
  }
  Serial.println("SD Card Mounted");

  if(!SD.exists("/music")) SD.mkdir("/music");

  mp3DirSetup();

  // ts is important for assembly
  audio.setPinout(26, 25, 27); 
}

void loop(){
  updateButtons();
  determineScreenDirection();

  if(BTNH_pressed == true){
    state = menu;
    audio.stopSong();
    lastTrackSecond = 999;
    snakeX = 63;
    snakeY = 31;
  }

  if (state == screenSaver) {
      drawScreenSaver();
  } 
  else if (state == menu) {
      drawMenu();
  }
  else if (state == mp3Select) {
      drawMp3Select();
  }
  else if (state == gameSelect) {
      drawGameSelect();
  }
  else if (state == mp3Playing) {
      uint32_t currentSeconds = audio.getAudioCurrentTime();
      if (currentSeconds != lastTrackSecond) {
          if (unPause == false){ 
            drawplay(); 
          } else if (unPause == true){
            drawpause();
          }
          lastTrackSecond = currentSeconds;
      }
  }

  switch(state) {
    case screenSaver:
      if (BTNOK_pressed) {
        state = menu;
        lastButtonPress = millis();
      }
      break;

    case menu:
      if (millis() - lastButtonPress >= buttonCooldown) {
        if (menuOption == 0) {
          if (BTNL_pressed || BTNR_pressed) {
            menuX = 64;
            menuOption = 1;
            lastButtonPress = millis();
          } else if (BTNOK_pressed) {
            state = mp3Select;
            lastButtonPress = millis();
          }
        } else if (menuOption == 1) {
          if (BTNL_pressed || BTNR_pressed) {
            menuX = 0;
            menuOption = 0;
            lastButtonPress = millis();
          } else if (BTNOK_pressed) {
            state = gameSelect;
            lastButtonPress = millis();
          }
        }
      }
      break;

    case mp3Select:
      if (millis() - lastButtonPress >= buttonCooldown) {
        if (BTNOK_pressed && !mp3List.empty()) {
          playSelectSong();
          state = mp3Playing;
          lastButtonPress = millis();
        } 
        else if (BTND_pressed || BTNU_pressed) {
          updateMp3Pick();
          lastButtonPress = millis();
        }
      }
      break;

    case mp3Playing:
      if (millis() - lastButtonPress >= buttonCooldown) {
        if (BTNOK_pressed) {
          unPause = !unPause;
          audio.pauseResume();
          lastButtonPress = millis();
        } 
        else if (BTNR_pressed) {
          if (!mp3List.empty()) {
            mp3Pick++;
            if (mp3Pick >= mp3List.size()) mp3Pick = 0;
            lastTrackSecond = 999;
            playSelectSong();
          }
          lastButtonPress = millis();
        } 
        else if (BTNL_pressed) {
          if (!mp3List.empty()) {
            mp3Pick--;
            if (mp3Pick < 0) mp3Pick = mp3List.size() - 1;
            lastTrackSecond = 999;
            playSelectSong();
          }
          lastButtonPress = millis();
        } 
        else if (BTND_pressed) {
          if (volume > 0) volume--;
          lastButtonPress = millis();
        } 
        else if (BTNU_pressed) {
          if (volume < 21) volume++;
          lastButtonPress = millis();
        }
      }
      break;

    case gameSelect:
      if (millis() - lastButtonPress >= buttonCooldown) {
        if (BTNOK_pressed) {
          state = runningGame;
          lastButtonPress = millis();
        }
        else if (BTND_pressed || BTNU_pressed) {
          updateGamePick();
          lastButtonPress = millis();
        }
      }
      break;

    case runningGame:
      if (gamePick == 0) {
        snake(); 
      } else if (gamePick == 1) {
        flappyBird(); 
      }
      break;
    }

  audio.setVolume(volume);
  audio.loop();
}