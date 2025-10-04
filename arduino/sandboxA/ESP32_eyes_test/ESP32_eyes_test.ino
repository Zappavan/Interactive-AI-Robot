#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Eye parameters
float curEyeW = 32, curEyeH = 32;
float curGap = 10;
float curYOffset = 16;
float curSlant = 0;
float curRound = 10;

// Breathing
const float driftAmplitude = 5.5;   // vertical movement
const unsigned long breathUpDown = 800; // up/down duration in ms
const unsigned long bottomPause = 2500;  // pause at bottom

// Blink timing
unsigned long nextBlink = 0;

// Eye structs
struct EyeParams {
  float w, h, gap, yOffset, slant, roundness, narrowness;
};

EyeParams neutral   = {35, 35, 10, 16, 0, 10, 1.0};
EyeParams excited   = {35, 35, 10, 16, 0, 10, 1.0};
EyeParams happy     = {35, 35, 10, 16, 0, 10, 1.0};
EyeParams sad       = {35, 35, 10, 16, -1, 12, 1.0};
EyeParams angry     = {35, 35, 10, 16, +1, 12, 1.0};

// ---------- Helper ----------
float getBreathingOffset() {
  unsigned long cycle = 2*breathUpDown + bottomPause;
  unsigned long t = millis() % cycle;

  if(t < breathUpDown){                // moving up from bottom
    return driftAmplitude - (2*driftAmplitude) * ((float)t / breathUpDown);
  } 
  else if(t < 2*breathUpDown){         // moving down from top
    unsigned long td = t - breathUpDown;
    return -driftAmplitude + (2*driftAmplitude) * ((float)td / breathUpDown);
  } 
  else {                               // pause at bottom
    return driftAmplitude;
  }
}



unsigned long randomBlinkInterval() {
  return 1000 + random(0, 2001); // 1-3 seconds
}

// ---------- Draw eyes ----------
void drawEyes(float xOffset=0, float leftScale=1.0, float rightScale=1.0, bool closed=false, bool happyBottomCurve=false){
  display.clearDisplay();

  float verticalOffset = getBreathingOffset();

  int leftX  = (SCREEN_WIDTH/2) - curGap - curEyeW*leftScale;
  int rightX = (SCREEN_WIDTH/2) + curGap;
  int y = curYOffset + verticalOffset;

  float leftH  = curEyeH * leftScale;
  float rightH = curEyeH * rightScale;

  if(closed){
    display.drawLine(leftX + xOffset, y+leftH/2, leftX + curEyeW*leftScale + xOffset, y+leftH/2, SSD1306_WHITE);
    display.drawLine(rightX + xOffset, y+rightH/2, rightX + curEyeW*rightScale + xOffset, y+rightH/2, SSD1306_WHITE);
  } else {
    display.fillRoundRect(leftX + xOffset, y, curEyeW*leftScale, leftH, curRound, SSD1306_WHITE);
    display.fillRoundRect(rightX + xOffset, y, curEyeW*rightScale, rightH, curRound, SSD1306_WHITE);

    if(curSlant != 0){
      if(curSlant > 0){
        display.fillTriangle(leftX + xOffset, y, leftX+curEyeW*leftScale + xOffset, y, leftX+curEyeW*leftScale + xOffset, y+6, SSD1306_BLACK);
        display.fillTriangle(rightX + xOffset, y, rightX+curEyeW*rightScale + xOffset, y, rightX + xOffset, y+6, SSD1306_BLACK);
      } else {
        display.fillTriangle(leftX + xOffset, y, leftX+curEyeW*leftScale + xOffset, y, leftX + xOffset, y+6, SSD1306_BLACK);
        display.fillTriangle(rightX + xOffset, y, rightX+curEyeW*rightScale + xOffset, y, rightX+curEyeW*rightScale + xOffset, y+6, SSD1306_BLACK);
      }
    }

    if(happyBottomCurve){
      int curveRadiusL = leftH;
      int curveRadiusR = rightH;
      int centerYLeft  = y + leftH + curveRadiusL/2;
      int centerYRight = y + rightH + curveRadiusR/2;
      display.fillCircle(leftX + curEyeW*leftScale/2 + xOffset, centerYLeft, curveRadiusL, SSD1306_BLACK);
      display.fillCircle(rightX + curEyeW*rightScale/2 + xOffset, centerYRight, curveRadiusR, SSD1306_BLACK);
    }
  }

  display.display();
}

// ---------- Morph ----------
void morphTo(EyeParams target, int steps=8){
  float startW = curEyeW, startH = curEyeH, startGap = curGap;
  float startYOffset = curYOffset, startSlant = curSlant, startRound = curRound;

  for (int i=1; i<=steps; i++){
    float factor = (float)i/steps;
    curEyeW = startW + (target.w - startW)*factor;
    curEyeH = startH + (target.h - startH)*factor;
    curGap    = startGap + (target.gap - startGap)*factor;
    curYOffset= startYOffset + (target.yOffset - startYOffset)*factor;
    curSlant  = startSlant + (target.slant - startSlant)*factor;
    curRound  = startRound + (target.roundness - startRound)*factor;

    drawEyes();
    delay(30);
  }
}

// ---------- States ----------
void neutralState(int duration){
  morphTo(neutral);
  unsigned long start = millis();
  float sizeDiff = 0.05;

  while(millis()-start<duration){
    drawEyes(8, 1.0+sizeDiff, 1.0-sizeDiff); // right
    delay(150);
    delay(1000);
    drawEyes(0,1.0,1.0); // center
    drawEyes(-8, 1.0-sizeDiff,1.0+sizeDiff); // left
    delay(150);
    delay(1000);
    drawEyes(0,1.0,1.0); // back to center

    if(millis() > nextBlink){
      drawEyes(0,1.0,1.0,true);
      delay(50);
      nextBlink = millis() + randomBlinkInterval();
    }
  }
}

void excitedState(int duration){
  morphTo(excited);
  nextBlink = millis() + randomBlinkInterval();
  unsigned long start = millis();

  while(millis()-start<duration){
    float xJ = random(-2,3); // horizontal jitter
    if(millis() > nextBlink){
      drawEyes(0,1.0,1.0,true,true);
      delay(50);
      nextBlink = millis() + randomBlinkInterval();
    }
    drawEyes(xJ,1.0,1.0,false,true);
    delay(50);
  }
}

void happyState(int duration){
  morphTo(happy);
  nextBlink = millis() + randomBlinkInterval();
  unsigned long start = millis();

  while(millis()-start<duration){
    if(millis() > nextBlink){
      drawEyes(0,1.0,1.0,true,true);
      delay(50);
      nextBlink = millis() + randomBlinkInterval();
    }
    drawEyes(0,1.0,1.0,false,true);
    delay(50);
  }
}

void sadState(int duration){
  morphTo(sad);
  nextBlink = millis() + randomBlinkInterval();
  unsigned long start = millis();

  while(millis()-start<duration){
    if(millis() > nextBlink){
      drawEyes(0,1.0,1.0,true);
      delay(50);
      nextBlink = millis() + randomBlinkInterval();
    }
    drawEyes();
    delay(50);
  }
}

void angryState(int duration){
  morphTo(angry);
  nextBlink = millis() + randomBlinkInterval();
  unsigned long start = millis();

  while(millis()-start<duration){
    float jitter = random(-1,2); // subtle horizontal jitter
    if(millis() > nextBlink){
      drawEyes(0,1.0,1.0,true);
      delay(50);
      nextBlink = millis() + randomBlinkInterval();
    }
    drawEyes(jitter,1.0,1.0);
    delay(50);
  }
}

// ---------- Setup ----------
void setup(){
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  randomSeed(analogRead(0));
  nextBlink = millis() + randomBlinkInterval();
}

// ---------- Loop ----------
void loop(){
  neutralState(5000);
  happyState(5000);
  excitedState(5000);
  sadState(5000);
  angryState(5000);
}
