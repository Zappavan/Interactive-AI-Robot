#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
extern Adafruit_SSD1306 display;  // declare, not define

// --- Blink timing global ---
extern unsigned long nextBlink;


// ---------- Setup ----------
void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  randomSeed(analogRead(0));
  nextBlink = millis() + randomBlinkInterval();
  setupWiFi();   // boot up WiFi listener
}

void loop() {
  wifiLoop();    // keep checking for new messages
  runMood();     // run last mood + neutral fallback
}
