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
