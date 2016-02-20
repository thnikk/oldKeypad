/*************************************************
  RGBW Keypad
    An update wasn't exactly needed here, but it 
    gave me a great opportunity to clean up my 
    hideous mess of code I was using before. 
    Hopefully now it's easier to make changes and
    see everything that's going on.

  For more info, please visit:
  ----> http://thnikk.moe/
  ----> https://www.etsy.com/shop/thnikk
  ----> twitter.com/thnikk
  ----> https://github.com/thnikk

  Written by thnikk.
*************************************************/

/*********** NeoPixel library setup *************/
#include <Adafruit_NeoPixel.h>
#define NUMPIXELS 2     // Number of neopixels
#define PIN       3     // Output pin
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRBW + NEO_KHZ800);

/************** Other libraries *****************/
#include <Bounce2.h>    // For debounced keyboard
#include <Keyboard.h>   // For Esc keypresses (since pressed and released)
#include <EEPROM.h>     // For saving values between reboots

/**************** Key mapping ********************
***** ASCII Table: http://tinyurl.com/ai4d *******
****** Arduino: http://tinyurl.com/oyzg5be ******/
#define b1p 122         // Default (Z)
#define b2p 120         // Default (X)
#define b1pA 99         // Alternate (C)
#define b2pA 118        // Alternate (V)
#define b3p KEY_ESC

/**************** Button pins *******************/
const int b1 = 2;
const int b2 = 4;
const int b3 = 5;

/***************** Variables ********************/
byte ledMax = 200;      // Maximum LED brightness
byte mapping = 0;       // Button mapping
int mod = 2;            // += and -= for reactive
byte buttonMode = 0;    // Initialization (LED mode 0-4 {overwritten by EEPROM loading})
int j = 0;              // Initialization (short color for cycle)
byte b1Press = 0;       // Initialization (counter for BPS)
byte b2Press = 0;       // Initialization
byte pressCount = 0;    // Initialization (number of presses per second in bps)
byte bV = 0;            // Initialization (counter for release side button actions)
int sc1 = 0;            // Initialization (short per-key color for wheel)
int sc2 = 0;            // Initialization
int oldColor = 400;     // Initialization (for transitioning in BPS)
int newColor = 400;     // Initialization
int bVal = 0;           // Initialization (brightness 0-200)
float brightness = 0;   // Initialization (brightness 0.XX for multiplication)
  
struct RGBW {           // Struct for both LEDs. 
  byte r;
  byte g;
  byte b;
  byte w;
};
RGBW L1 = {0, 0, 0, 0};
RGBW L2 = {0, 0, 0, 0};

/************** Universal colors ****************/
byte r = 0;
byte g = 0;
byte b = 0;
byte w = 0;

/*************** Millis timer *******************/
  // universal mode millis
  unsigned long prevMillis = 0;
  // brightness millis
  unsigned long prevMillis1 = 0;
  // side button millis
  unsigned long prevMillisB = 0;
  // secondary counter for bpm
  unsigned long prevMillis2 = 0;
  // cycle speed
  unsigned long interval = 10;
  // reactive mode speed
  unsigned long react = 1;
  // polling rate for bps
  unsigned long bpmTime = 1000;
  // side button (escape)
  unsigned long DB = 10;
  // side button (change button modes)
  unsigned long SH = 500;
  // side button (brightness/ccustom colors)
  unsigned long LH = 1500;
  // side button (mapping)
  unsigned long SLH = 8000;

/***************** Debounce ********************/
Bounce b1d = Bounce();
Bounce b2d = Bounce();

void setup() {
  pixels.begin(); // Needed to start neopixels

  /* button initialization */
  pinMode(b1, INPUT_PULLUP);
  pinMode(b2, INPUT_PULLUP);
  pinMode(b3, INPUT_PULLUP);

  /* More debounce */
  b1d.attach(b1);
  b1d.interval(8); // Debounce time
  b2d.attach(b2);
  b2d.interval(8); // Debounce time

  /* EEPROM initialization (runs when code is first uploaded) */
  if (EEPROM.read(9) != 0) { // add to 0 for updates to EEPROM management/preset
    EEPROM.write(0, 4);
    EEPROM.write(1, 200);
    EEPROM.write(2, 200);
    EEPROM.write(3, 150);
    EEPROM.write(4, 200);
    EEPROM.write(5, 200);
    EEPROM.write(6, 50);
    EEPROM.write(7, 125);
    EEPROM.write(9, 0);
  }

  /* Load from EEPROM */
  buttonMode = EEPROM.read(0);
  sc1        = EEPROM.read(1) + EEPROM.read(2) + EEPROM.read(3);
  sc2        = EEPROM.read(4) + EEPROM.read(5) + EEPROM.read(6);
  bVal       = EEPROM.read(7);
  mapping    = EEPROM.read(8);
}

void loop() {
  /* Mode selection */
  switch(buttonMode) {
    case 0:
      cycle();
      break;
    case 1:
      reactive(0);
      break;
    case 2:
      reactive(1);
      break;
    case 3:
      bps();
      break;
    case 4:
      custom();
      break;
  }

  serialDebug(0); // Set to 1 for debugging info in serial monitor

  sideButton();
  if (bV < 3) keyboard();

  /* Applies colors to LEDs */
  bright();
  setLED();
}

void serialDebug(int offOn) {
  if (offOn == 1) {
    Serial.print("custom: (");
    Serial.print(sc1);
    Serial.print(", ");
    Serial.print(sc2);
    Serial.print(") ");

    Serial.print("EEPROM: (");
    Serial.print(EEPROM.read(0));
    Serial.print(", ");
    Serial.print(EEPROM.read(1) + EEPROM.read(2) + EEPROM.read(3));
    Serial.print(", ");
    Serial.print(EEPROM.read(4) + EEPROM.read(5) + EEPROM.read(6));
    Serial.print(") ");

    Serial.print("mapping: (");
    Serial.print(EEPROM.read(8));
    Serial.print(") ");

    unsigned long curMillis = millis();
    Serial.print("millis: (");
    Serial.print(curMillis - prevMillisB);
    Serial.print(") ");
    // Always write new line at end
    Serial.println();
  }
}

/* Color wheel function */
void wheel(int sColor, int button) {
  /* convert sColor to RGB */
  if (sColor == 0) {
    r = 200;
    g = 0;
    b = 0;
  }
  if (sColor >= 1 && sColor <= 199) {
    r = (sColor * -1) + 200;
    g = sColor;
    b = 0;
  }
  if (sColor == 200) {
    r = 0;
    g = 200;
    b = 0;
  }
  if (sColor >= 201 && sColor <= 399) {
    r = 0;
    g = ((sColor - 200) * -1) + 200;
    b = sColor - 200;
  }
  if (sColor == 400) {
    r = 0;
    g = 0;
    b = 200;
  }
  if (sColor >= 401 && sColor <= 599) {
    r = sColor - 400;
    b = ((sColor - 400) * -1) + 200;
    g = 0;
  }

  /****** Apply color to LED ******/
  if (button == 0 || button == 2) {
      L1.r = r;
      L1.g = g;
      L1.b = b;
      L1.w = w;
  }
  if (button == 1 || button == 2) {
      L2.r = r;
      L2.g = g;
      L2.b = b;
      L2.w = w;
  }
}

/* Cycle mode */
void cycle() {
  w = 0;
  wheel(j, 2);
  unsigned long curMillis = millis();
  if ((curMillis - prevMillis) > interval) {
    j++;
    prevMillis = curMillis;
  }
  if (j >= 600){
    j  = 0;
  }
}

/* Reactive and pushOff modes (depending on flip value) */
void reactive(byte flip) {
  if ((!digitalRead(b1) && flip == 0) || (digitalRead(b1) && flip == 1)) {
    blank(0);
    L1.w = ledMax;
  }
  if ((!digitalRead(b2) && flip == 0) || (digitalRead(b2) && flip == 1)) {
    blank(1);
    L2.w = ledMax;
  }
  unsigned long curMillis = millis();
  if ((curMillis - prevMillis) > react) {
    if ((digitalRead(b1)&& flip == 0) || (!digitalRead(b1) && flip == 1)) {
      if (L1.w > 0) {
        L1.w-=mod;
        L1.r+=mod;
      }
      if (L1.w == 0 && L1.r > 0) {
        L1.r-=mod;
        L1.g+=mod;   
      }
      if (L1.r == 0 && L1.g > 0) {
        L1.g-=mod;
        L1.b+=mod;   
      }
      if (L1.g == 0 && L1.b > 0) {
        L1.b-=mod;  
      }
    }
    if ((digitalRead(b2) && flip == 0) || (!digitalRead(b2) && flip == 1)) {
      if (L2.w > 0) {
        L2.w-=mod;
        L2.r+=mod;
      }
      if (L2.w == 0 && L2.r > 0) {
        L2.r-=mod;
        L2.g+=mod;   
      }
      if (L2.r == 0 && L2.g > 0) {
        L2.g-=mod;
        L2.b+=mod;   
      }
      if (L2.g == 0 && L2.b > 0) {
        L2.b-=mod;  
      }
    }
    prevMillis = curMillis;
  }
}

/* Beats per second mode */
void bps() {
  if (!digitalRead(b1)) {
    b1Press = 1;
    L1.w = ledMax;
  }
  if (!digitalRead(b2)) {
    b2Press = 1;
    L2.w = ledMax;
  }
  if (digitalRead(b1)) {
    if (b1Press == 1) {
      pressCount++;
    }
    b1Press = 0;
    L1.w = 0;
  }
  if (digitalRead(b2)) {
    if (b2Press == 1) {
      pressCount++;
    }
    b2Press = 0;
    L2.w = 0;
  }
  unsigned long curMillis = millis();
  if ((curMillis-prevMillis) > bpmTime) {
    newColor = ((pressCount*-30)+400);
    pressCount = 0;
    prevMillis = curMillis;
  }
  if ((curMillis-prevMillis2) > 5) {
    if (oldColor != newColor) {
      if (newColor > oldColor) {
        oldColor++;
      }
      if (newColor < oldColor) {
        oldColor--;
      }
    }
    prevMillis2 = curMillis;
  }
  wheel(oldColor, 2);
}

/* Custom mode */
void custom() {
  w = 0;
  wheel(sc1, 0);
  wheel(sc2, 1);
  unsigned long curMillis = millis();
  if ((curMillis - prevMillis) > 10) {
    if(!digitalRead(b1) && bV == 3) {
      sc1++;
      if (sc1 > 600) {
        sc1 = 0;
      }
    }
    if(!digitalRead(b2) && bV == 3) {
      sc2++;
      if (sc2 > 600) {
        sc2 = 0;
      }
    }
    prevMillis = curMillis;
  }
}

/* Brightness changer */
void bright() {
  unsigned long curMillis = millis();
  if ((curMillis - prevMillis1) > 10) {
    if(!digitalRead(b2) && bV == 4) {
      bVal++;
      if (bVal > ledMax) {
        bVal = ledMax;
      }
    }
    if(!digitalRead(b1) && bV == 4) {
      bVal--;
      if (bVal < 50) {
        bVal = 50;
      }
    }
    brightness = bVal * 0.005;
    prevMillis1 = curMillis;
  }
}

/* Side button magic */
void sideButton() {
  unsigned long curMillis = millis();

  /* When pressed */
  if (!digitalRead(b3)){
    if ((curMillis - prevMillisB) > DB && (curMillis - prevMillisB) < SH) { // side button
      if (bV == 0) {
        bV = 1;
      }
    }
    if ((curMillis - prevMillisB) > SH && (curMillis - prevMillisB) < LH) { // button mode switcher
      if (bV == 1) {
        bV = 2;
        blink(1);
      }
    }
    if ((curMillis - prevMillisB) > LH && (curMillis - prevMillisB) < SLH) {
      if (buttonMode == 4) { // custom colors
        if (bV == 2) {
          bV = 3;
          blink(2);
        }
      }
      if (buttonMode != 4) {
        if (bV == 2) { // brightness
          bV = 4;
          blink(2);
        }
      }
      if (!digitalRead(b1) || !digitalRead(b2)) { // Resets timer when button is pressed
        prevMillisB = prevMillisB + LH;
      }
    }
    if ((curMillis - prevMillisB) > SLH) { // mapping switcher
      if (bV == 3 || bV == 4) {
        bV = 5;
        blink(2);
      }
    }
  }

  /* When released */
  if (digitalRead(b3)){
    if (bV == 1) { // Press and release ESC
      Keyboard.press(b3p);
      delay(12);
      Keyboard.release(b3p);
    }
    if (bV == 2) { // change LED mode
      buttonMode++;
      if (buttonMode > 4) {
        buttonMode = 0;
      }
      blank(2);
      EEPROM.write(0, buttonMode);
    }
    if (bV == 3) { // Breaks custom sColor values into 3 bytes and saves them to EEPROM
      if (sc1 <= 200) {
        EEPROM.write(1, sc1);
        EEPROM.write(2, 0);
        EEPROM.write(3, 0);
      }
      if (sc1 > 200 && sc1 <= 400) { 
        EEPROM.write(1, 200);
        EEPROM.write(2, sc1-200);
        EEPROM.write(3, 0);
      }
      if (sc1 > 400) { 
        EEPROM.write(1, 200);
        EEPROM.write(2, 200);
        EEPROM.write(3, sc1-400);
      }

      if (sc2 <= 200) {
        EEPROM.write(4, sc2);
        EEPROM.write(5, 0);
        EEPROM.write(6, 0);
      }
      if (sc2 > 200 && sc2 <= 400) { 
        EEPROM.write(4, 200);
        EEPROM.write(5, sc2-200);
        EEPROM.write(6, 0);
      }
      if (sc2 > 400) { 
        EEPROM.write(4, 200);
        EEPROM.write(5, 200);
        EEPROM.write(6, sc2-400);
      }
    }
    if (bV == 4) { // Write brigtness to EEPROM
      EEPROM.write(7, bVal);
    }
    if (bV == 5) { // Set button mapping and write to EEPROM
      mapping++;
      if (mapping > 1) mapping = 0;
      EEPROM.write(8, mapping);
    }
    prevMillisB = curMillis;
    bV = 0;
  }
}

/* Blink function */ 
  void blink(int bA) {
    if (bA >= 1) {
      for (int i=0; i <= 300; i++) {
        binLED(0);
      }
      for (int i = 0; i<= 500; i++) {
        binLED(1);
      }
      for (int i = 0; i<= 300; i++) {
        binLED(0);
      }
      if (bA == 2) {
        for (int i = 0; i<= 500; i++) {
          binLED(1);
        }
        for (int i = 0; i<= 300; i++) {
          binLED(0);
        }
      }
    }
  }

  void binLED(int onOff) {
    if (onOff == 0) {
      pixels.setPixelColor(0, pixels.Color(0, 0, 0, 0));
      pixels.setPixelColor(1, pixels.Color(0, 0, 0, 0));
    }
    if (onOff == 1) {
      pixels.setPixelColor(0, pixels.Color(0, 0, 0, 200));
      pixels.setPixelColor(1, pixels.Color(0, 0, 0, 200));
    }
    pixels.show();
  }

/* Blanking function when chaniging modes */
void blank(int button) {
    if (button == 0 || button == 2) {
      L1.r = 0;
      L1.g = 0;
      L1.b = 0;
  }
  if (button == 1 || button == 2) {
      L2.r = 0;
      L2.g = 0;
      L2.b = 0;
  }
}

/* Does actual keyboard stuff */
void keyboard() {
  b1d.update();
  b2d.update();
  switch(mapping){
    case 0:
      if (b1d.read() == LOW) {
        Keyboard.press(b1p);
      }
      if (b1d.read() == HIGH) {
        Keyboard.release(b1p);
      }
      if (b2d.read() == LOW) {
        Keyboard.press(b2p);
      }
      if (b2d.read() == HIGH) {
        Keyboard.release(b2p);
      }
      break;
    case 1:
      if (b1d.read() == LOW) {
        Keyboard.press(b1pA);
      }
      if (b1d.read() == HIGH) {
        Keyboard.release(b1pA);
      }
      if (b2d.read() == LOW) {
        Keyboard.press(b2pA);
      }
      if (b2d.read() == HIGH) {
        Keyboard.release(b2pA);
      }
      break;
  }
}

/* Sets LED colors */
void setLED() {
  pixels.setPixelColor(0, pixels.Color(L1.r*brightness, L1.g*brightness, L1.b*brightness, L1.w*brightness));
  pixels.setPixelColor(1, pixels.Color(L2.r*brightness, L2.g*brightness, L2.b*brightness, L2.w*brightness));
  pixels.show();
}