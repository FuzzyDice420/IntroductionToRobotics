#include "LedControl.h"

const int brightness = 0;
const int dinPin = 12;
const int clkPin = 11;
const int loadPin = 10;
const int noOfDrivers = 1;

// joystick pins
const int pinX = A0;
const int pinY = A1;

//joystick values
int xValue = 0;
int yValue = 0;
int minXThreshold = 400;
int maxXThreshold = 600;
int minYThreshold = 400;
int maxYThreshold = 600;
bool joyMovedX = false;
bool joyMovedY = false;

int prevRow = 0;
int prevCol = 0;

bool matrix[8][8] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};

class Player {
 public:
  int row = 6;
  int col = 5;
};

Player player;

LedControl lc = LedControl(dinPin, clkPin, loadPin, noOfDrivers);

void updateMatrix() {
  lc.setLed(0, player.row, player.col, 1);
  lc.setLed(0, prevRow, prevCol, 0);
}

void setup() {
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, brightness); // sets brightness (between 0 and 15)
  lc.clearDisplay(0);
  Serial.begin(9600);
}

void loop() {
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  Serial.println(xValue);
  Serial.println(yValue);
  Serial.print("player row: ");
  Serial.println(player.row);
  Serial.print("player col: ");
  Serial.print(player.col);

  updateMatrix();
  
  if (xValue < minXThreshold && joyMovedX == false) {
    prevRow = player.row;
    prevCol = player.col;
    matrix[prevRow][prevCol] = 0;
    if (player.col < 0) {
      player.col = 9;
    }
    player.col--;
    joyMovedX = true;
  }

  if (xValue > maxXThreshold && joyMovedX == false) {
    prevRow = player.row;
    prevCol = player.col;
    matrix[prevRow][prevCol] = 0;
    if (player.col > 9) {
      player.col = 0;
    }
    player.col++;
    joyMovedX = true;
  }

  if (yValue < minYThreshold && joyMovedY == false) {
    prevRow = player.row;
    prevCol = player.col;
    matrix[prevRow][prevCol] = 0;
    if (player.row < 0) {
      player.row = 9;
    }
    player.row--;
    joyMovedY = true;
  }

  if (yValue < maxYThreshold && joyMovedY == false) {
    prevRow = player.row;
    prevCol = player.col;
    matrix[prevRow][prevCol] = 0;
    if (player.col > 9) {
      player.col = 0;
    }
    player.row++;
    joyMovedY = true;
  }

  if (xValue >= minXThreshold && xValue <= maxXThreshold) {
    joyMovedX = false;
  }

  if (yValue >= minYThreshold && yValue <= maxYThreshold) {
    joyMovedY = false;
  }
}
