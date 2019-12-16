#include "LedControl.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>

const int RS = 9;
const int enable = 8;
const int d4 = 4;
const int d5 = 5;
const int d6 = 6;
const int d7 = 7;

LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);

const bool endScreen[8][8] = { 
  // 0  1  2  3  4  5  6  7  
  {1, 1, 0, 0, 0, 0, 1, 1}, // 0
  {1, 1, 0, 0, 0, 0, 1, 1}, // 1
  {0, 0, 1, 0, 0, 1, 0, 0}, // 2
  {0, 0, 0, 1, 1, 0, 0, 0}, // 3
  {0, 0, 0, 1, 1, 0, 0, 0}, // 4
  {0, 0, 1, 0, 0, 1, 0, 0}, // 5
  {1, 1, 0, 0, 0, 0, 1, 1}, // 6
  {1, 1, 0, 0, 0, 0, 1, 1}  // 7
};

const int brightness = 0;
const int dinPin = 12;
const int clkPin = 11;
const int loadPin = 10;
const int noOfDrivers = 1;

// joystick pins
const int joyX = A0;
const int joyY = A1;
const int swPin = A2;

//joystick values
int posX = 3, posY = 3;
int valX, valY, swValue;
boolean movedX = false, movedY = false;
int lastSwState = 1;
unsigned long lastDebounceTime = 0;
int swState = 0;
int ledState = 0;

// current shell row
int shRow;
// initial shell column
int initSh;
// enemy shell row & column
int enShRow, enShRow2; // max 2 enemy shells on screen at a time
int enShCol, enShCol2;


int flag = 0;
int flagSh = 0;
int flagSh2 = 0;

int score = 0;
int highScore = 0;
int lives = 3;


// timer for the traveling tank shell
unsigned long prevMil = 0;
// timer for the traveling enemy tank shell
unsigned long prevMil2 = 0, prevMil3 = 0, startMil = 0;
// timer for restarting game
unsigned long endMil = 0;
// score timer
unsigned long scoreMil = 0;

boolean gameOver = false;
boolean start = false;

class pixel {
  public:
  int posX, posY;
};

pixel tank[9];

LedControl lc = LedControl(dinPin, clkPin, loadPin, noOfDrivers);

bool matrix[8][8] = {
// 0  1  2  3  4  5  6  7  
  {0, 0, 0, 0, 0, 0, 0, 0}, // 0
  {0, 0, 0, 0, 0, 0, 0, 0}, // 1
  {0, 0, 0, 0, 0, 0, 0, 0}, // 2
  {0, 0, 0, 0, 0, 0, 0, 0}, // 3
  {0, 0, 0, 0, 0, 0, 0, 0}, // 4
  {0, 0, 0, 0, 0, 0, 0, 0}, // 5
  {0, 0, 0, 0, 0, 0, 0, 0}, // 6
  {0, 0, 0, 0, 0, 0, 0, 0} // 7
};

void moveLeft() {
  for (int i = 0; i < 7; i++) {
    tank[i].posX++;
  }
}

void moveRight() {
    for (int i = 0; i < 7; i++) {
    tank[i].posX--;
  }
}

void movement() {
  valX = analogRead(joyX);
  valY = analogRead(joyY);

  if (valY < 400)
  {
    if (movedY == false)
    {
      moveRight();
      movedY = true;
    }
  }
  else
  {
    if (valY > 600)
    {
      if (movedY == false)
      {
        moveLeft();
        movedY = true;
      }
    }
    else movedY = false;
  }

  // for wall collision
  for (int i = 0; i < 7; i++) {
    if (tank[i].posY == 8) tank[i].posY = 0;
    if (tank[i].posY == -1) tank[i].posY = 7;
    if (tank[i].posX == 8) tank[i].posX = 0;
    if (tank[i].posX == -1) tank[i].posX = 7;
  }
}

void displayMatrix() {
  for (int i = 0; i < 7; i++) {
    lc.setLed(0, tank[i].posX, tank[i].posY, 1);
  }
  
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      lc.setLed(0, i, j, 0);
    }
  }
}

void tankShell() { // generates your tank's shell
  unsigned long currMil = millis();
/*
  int reading = digitalRead(swPin);
  if(reading != lastSwState) {
    lastDebounceTime = millis();
  }
*/  
  //if((millis() - lastDebounceTime) > 50) {
    lc.setLed(0, initSh, shRow, 1);
    if (currMil - prevMil > 600) {
      prevMil = currMil;
      if (shRow >= 0) {
        shRow--;
      }
      else {
        shRow = tank[0].posY;
        flag = 0;
      }
    }
  //}
}

void enemyShell() { // generates random incoming enemy shell
  unsigned long currMil = millis();
  lc.setLed(0, enShCol, enShRow, 1);
  if (currMil - prevMil2 > 200) {
    prevMil2 = currMil;
    if (enShRow >= 7) {
      enShRow = 0;
      flagSh = 0;
    }
    else {
      enShRow++;
    }
  }
}

void endGame() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      lc.setLed(0, i, j, endScreen[i][j]);
    }
  }
  gameOver = true;
}

void checkCollision() {
  for (int i = 0; i < 7; i++) {
    if (tank[i].posX == enShCol && tank[i].posY == enShRow) {
      lives--;
      endGame();
      endMil = millis();
      if (highScore < score) {
        highScore = score;
        lcd.setCursor(0, 0);
        lcd.print("HIGHSCORE: ");
        lcd.print(highScore);
      }
    }
  }
}

void setup() {
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, brightness); // sets brightness (between 0 and 15)
  lc.clearDisplay(0);
  Serial.begin(9600);
  pinMode(swPin, INPUT_PULLUP);

  lcd.begin(16, 2);
  lcd.clear();

  // tank's initial pixels positions
  tank[0].posY = 5;
  tank[0].posX = 4;
  
  tank[1].posY = 6;
  tank[1].posX = 3;
  
  tank[2].posY = 6;
  tank[2].posX = 4;
  
  tank[3].posY = 6;
  tank[3].posX = 5;
  
  tank[4].posY = 7;
  tank[4].posX = 3;
  
  tank[5].posY = 7;
  tank[5].posX = 4;
  
  tank[6].posY = 7;
  tank[6].posX = 5;

  // current shell row
  shRow = tank[0].posX;
}

void loop() {
  Serial.println(lives);
  int reading = digitalRead(swPin);
  if (reading != lastSwState) {
    lastDebounceTime = millis();
  }
  if((millis() - lastDebounceTime) > 50) {
    if (reading != swState) {
      swState = reading;
      if (swState == LOW) {
        start = true;
      }
    }
  }
  if (start == true) {
    if (lives <= 0) {
      start = false;
    }
    if (gameOver == true) {
      endGame();
      lcd.setCursor(0, 0);
      lcd.print("Restarting in ");
      if (millis() - endMil % 1000 == 0) {
        lcd.setCursor(14, 0);
        lcd.print(millis() - endMil / 1000);
      }
      if (millis() - endMil % 2000 == 0) {
        lcd.setCursor(14, 0);
        lcd.print(millis() - endMil / 1000);
      }
      if (millis() - endMil % 3000 == 0) {
        lcd.setCursor(14, 0);
        lcd.print(millis() - endMil / 1000);
      }
      if (millis() - endMil % 4000 == 0) {
        lcd.setCursor(14, 0);
        lcd.print(millis() - endMil / 1000);
      }
      if (millis() - endMil % 5000 == 0) {
        lcd.setCursor(14, 0);
        lcd.print(millis() - endMil / 1000);
      }
      if (millis() - endMil > 2000) {
        gameOver = false;
      }
    }
    else {
      unsigned long scMil = millis();
      if (scMil - scoreMil > 1000) {
        score++;
        scoreMil = scMil;
      }
      lcd.setCursor(0, 0);
      lcd.print("Your score: ");
      lcd.print(score);
      movement();
      displayMatrix();
      if (flag == 0) {
        initSh = tank[0].posX;
        flag = 1;
      }
      tankShell();
      if (flagSh == 0) {
        enShCol = random(8);
        flagSh = 1;
      }
      enemyShell();
      checkCollision();    
    }
  }
  else {
    start = false;
    int eeAddress = 0;
    EEPROM.put(eeAddress, highScore);
    score = 0;
    lives = 3;
  }
  lastSwState = reading;
}
