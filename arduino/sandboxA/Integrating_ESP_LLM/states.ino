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
