#include <Bounce.h>
#include <EEPROM.h>

// button mapping
#define key1p KEY_Z
#define key2p KEY_X
#define key3p KEY_ESC

// alternate button mapping
#define key1pa KEY_C
#define key2pa KEY_V 

// button states
#define cycle 0
#define fadeout 1
#define pushoff 2
#define custom 3
#define BPM 4

// button pins
const int key1 = 0;
const int key2 = 1;
const int key3 = 21;

// led pins
const int ledb1 = 3;
const int ledr1 = 4;
const int ledg1 = 6;
const int ledb2 = 16;
const int ledr2 = 17;
const int ledg2 = 20;

// millis timer
  // previousMillis values for millis timers
  unsigned long previousMillis = 0;
  unsigned long previousMillisB1 = 0;
  unsigned long previousMillisB2 = 0;
  unsigned long previousMillisB3 = 0;
  unsigned long previousMillisBlink = 0;
  unsigned long previousMillisKB = 0;
  unsigned long previousMillisBPM = 0;
  unsigned long previousMillisSpeed = 0;
  unsigned long previousMillisCD = 0;
  // intervals for millis timers
  unsigned long interval = 10;
  unsigned long intfastcycle = 6;
  unsigned long intBlink = 75;
  unsigned long fast = 1;
  unsigned long hold = 500; 
  unsigned long longhold = 1000;
  unsigned long kbchold = 6000;
  unsigned long rst = 0;
  unsigned long intBPM = 1000;
  unsigned long intSpeed = 5;
  unsigned long cooldown = 200;

// EEPROM load
int r1 = EEPROM.read(1); // button 1 red
int g1 = EEPROM.read(2); // button 1 green
int b1 = EEPROM.read(3); // button 1 blue
int r2 = EEPROM.read(4); // button 2 red
int g2 = EEPROM.read(5); // button 2 green
int b2 = EEPROM.read(6); // button 2 blue
int rgbsB1 = EEPROM.read(7); // color cycle counter for button 1
int rgbsB2 = EEPROM.read(8); // color cycle counter for button 2
int kbc = EEPROM.read(9);

// "applied" LED values for transitioning of BPM mode
int r1a = 0;
int g1a = 0;
int b1a = 0;
int r2a = 0;
int g2a = 0;
int b2a = 0;

// LED max and add values
int ledmax = 200;
int ledmin = 0;
int addv = ledmax/100;

// bpm counters
int key1c = 0; // 0 or 1 for release ++
int key2c = 0; // same
int bp5s = 0; // reasonable max of 70
int bp5sPending = 0; // last value of bp5s prior to reset

// other counters
int crossFade = 10; // crossfade value for leds
int buttonState = 0; // button state for led modes
int bssw = 0; // esc + bS counter
int bssw2 = 0; // reset for blink
int blinkv = 0; // blink counter
int blinkc = 0; // for blink only running once
int bSmax = 4; // amount of button states for reset
int epc = 0; // eeprom check counter
int faderst = 0;
int test = 0;
int b3 = 0;
int b3cooldown = 0;

Bounce button0 = Bounce(0, 8);
Bounce button1 = Bounce(1, 8);
Bounce button2 = Bounce(2, 8);

void setup() {
  // button pin modes
  pinMode(key1, INPUT_PULLUP);
  pinMode(key2, INPUT_PULLUP);
  pinMode(key3, INPUT_PULLUP);
  
  // led pin modes
  pinMode(ledr1, OUTPUT);
  pinMode(ledg1, OUTPUT);
  pinMode(ledb1, OUTPUT);
  pinMode(ledr2, OUTPUT);
  pinMode(ledg2, OUTPUT);
  pinMode(ledb2, OUTPUT);
}

void loop() {
  if (analogRead(21) > 1000) {
    b3 = 1;
  }
  if (analogRead(21) < 1000) {
    b3 = 0;
  }
  
  unsigned long currentMillis = millis();
  
  // fix for default values being too high
  if (EEPROM.read(1) > 200 || EEPROM.read(2) > 200 || EEPROM.read(3) > 200 || EEPROM.read(4) > 200 || EEPROM.read(5) > 200 || EEPROM.read(6) > 200 ) {
    EEPROM.write(1, 0);
    EEPROM.write(2, 0);
    EEPROM.write(3, 0);
    EEPROM.write(4, 0);
    EEPROM.write(5, 0);
    EEPROM.write(6, 0);
    EEPROM.write(7, 0);
    EEPROM.write(8, 0);
  }

  // writes buttonState value to eeprom
  if (epc == 0) {
    buttonState = EEPROM.read(0);
    epc++;
  }
  
  // code constantly applying LED values
  if (blinkv == 0 && buttonState != BPM){
    analogWrite(ledr1, r1);
    analogWrite(ledg1, g1);
    analogWrite(ledb1, b1);
    analogWrite(ledr2, r2);
    analogWrite(ledg2, g2);
    analogWrite(ledb2, b2);
  }
  
  // button 3 cooldown code
  if (b3cooldown == 0) {
    previousMillisCD = currentMillis;
  }
  
  if (b3cooldown == 1) {
    if ((currentMillis - previousMillisCD) > cooldown) {
      b3cooldown = 0;
    }
  }
  //
  
  // buttonState switch
  if (b3 == 0) {
    previousMillisB3 = currentMillis;
  }
  
  if (b3 == 1) {
    if ((currentMillis - previousMillisB3) == 1) {
      bssw2 = 1;
    }
    if ((currentMillis - previousMillisB3) == hold) {
    } 
    if (buttonState == custom) {
      if ((currentMillis - previousMillisB3) == longhold) {

      }
    }
    if ((currentMillis - previousMillisB3) == kbchold) {

    }   
    // escape key
    if ((currentMillis - previousMillisB3) > 1 && (currentMillis - previousMillisB3) < hold) {
      bssw = 1;
    }
    // bs changer
    if ((currentMillis - previousMillisB3) > hold && (currentMillis - previousMillisB3) < longhold) {
      bssw = 2;
      if (blinkc == 0) {
        bssw2 = 2;
        blinkv = 1;
        blinkc = 1;
      }
    }
    if (buttonState != custom) {
      if ((currentMillis - previousMillisB3) > hold) {
        if (!digitalRead(key1) || !digitalRead(key2)) {
          previousMillisB3 = currentMillis - hold;
        }
      }
    }
    // set led values 
    if (buttonState == custom) {
      if ((currentMillis - previousMillisB3) > longhold) {
        if (blinkc == 1) {
          bssw2 = 3;
          blinkv = 1;
          blinkc = 2;
        }
        bssw = 3;
        if (!digitalRead(key1) || !digitalRead(key2)) {
          previousMillisB3 = currentMillis - longhold;
        }
      }
    }
    if ((currentMillis - previousMillisB3) > kbchold) {
      if (blinkc == 2) {
        bssw2 = 3;
        blinkv = 1;
        blinkc = 0;
        test = 1;
      }
      bssw = 4;
    }
  }
  
  // release actions
  if (b3 == 0) {
    test = 0;
    blinkc = 0;
    if (bssw == 1) {
      if (b3cooldown == 0) {
        Keyboard.press(key3p);
        delay(12);
        Keyboard.release(key3p);
        b3cooldown = 1;
      }
    }
    if(bssw == 2) {
      buttonState++;
      EEPROM.write(0, buttonState);
      if (buttonState != custom) {
	    r1 = 0;
	    g1 = 0;
	    b1 = 0;
	    r2 = 0;
	    g2 = 0;
	    b2 = 0;
            rgbsB1 = 0;
    	    rgbsB2 = 0;
  	  }
  	  if (buttonState == custom) {
    		r1 = EEPROM.read(1);
    		g1 = EEPROM.read(2);
    		b1 = EEPROM.read(3);
    		r2 = EEPROM.read(4);
    		g2 = EEPROM.read(5);
    		b2 = EEPROM.read(6);
    		rgbsB1 = EEPROM.read(7);
    		rgbsB2 = EEPROM.read(8);
  	  }
    }
    if (bssw == 3) {
        EEPROM.write(1, r1);
        EEPROM.write(2, g1);
        EEPROM.write(3, b1);
        EEPROM.write(4, r2);
        EEPROM.write(5, g2);
        EEPROM.write(6, b2);
        EEPROM.write(7, rgbsB1);
        EEPROM.write(8, rgbsB2);
    }
    if (bssw == 4) {
      kbc++;
      if (kbc > 1) {
        kbc = 0;
      }
      EEPROM.write(9, kbc);
      bssw = 3;
      EEPROM.write(1, r1);
      EEPROM.write(2, g1);
      EEPROM.write(3, b1);
      EEPROM.write(4, r2);
      EEPROM.write(5, g2);
      EEPROM.write(6, b2);
      EEPROM.write(7, rgbsB1);
      EEPROM.write(8, rgbsB2);
    }
    if (bssw != 4) {
      bssw = 0;
    }
  }
  
  // blink code
  if (blinkv == 0) {
    previousMillisBlink = currentMillis;
  }
  if (blinkv == 1) {
    analogWrite(ledr1, 0);
    analogWrite(ledg1, 0);
    analogWrite(ledb1, 0);
    analogWrite(ledr2, 0);
    analogWrite(ledg2, 0);
    analogWrite(ledb2, 0);
    if ((currentMillis - previousMillisBlink) >= intBlink) {
      blinkv++;
      previousMillisBlink = currentMillis;
    }
  }
  if (blinkv == 2) {
    analogWrite(ledr1, ledmax);
    analogWrite(ledg1, ledmax);
    analogWrite(ledb1, ledmax);
    analogWrite(ledr2, ledmax);
    analogWrite(ledg2, ledmax);
    analogWrite(ledb2, ledmax);
    if ((currentMillis - previousMillisBlink) >= intBlink) {
      blinkv++;
      previousMillisBlink = currentMillis;
    }
  }
  if (blinkv == 3) {
    analogWrite(ledr1, 0);
    analogWrite(ledg1, 0);
    analogWrite(ledb1, 0);
    analogWrite(ledr2, 0);
    analogWrite(ledg2, 0);
    analogWrite(ledb2, 0);
    if (bssw2 == 2) {
      blinkv = 0;
    }
    if (bssw2 == 3) { 
      if ((currentMillis - previousMillisBlink) >= intBlink) {
        blinkv++;
        previousMillisBlink = currentMillis;
        if (test != 1) {
        r1 = EEPROM.read(1);
        g1 = EEPROM.read(2);
        b1 = EEPROM.read(3);
        r2 = EEPROM.read(4);
        g2 = EEPROM.read(5);
        b2 = EEPROM.read(6);
        rgbsB1 = EEPROM.read(7);
        rgbsB2 = EEPROM.read(8);
        }
        if (test == 1) {
          test = 2;
        }
      }
    }
  }
  if (blinkv == 4) {
    analogWrite(ledr1, ledmax);
    analogWrite(ledg1, ledmax);
    analogWrite(ledb1, ledmax);
    analogWrite(ledr2, ledmax);
    analogWrite(ledg2, ledmax);
    analogWrite(ledb2, ledmax);
    if ((currentMillis - previousMillisBlink) >= intBlink) {
      blinkv++;
      previousMillisBlink = currentMillis;
    }
  }
  if (blinkv == 5) {
    analogWrite(ledr1, 0);
    analogWrite(ledg1, 0);
    analogWrite(ledb1, 0);
    analogWrite(ledr2, 0);
    analogWrite(ledg2, 0);
    analogWrite(ledb2, 0);
    if ((currentMillis - previousMillisBlink) >= intBlink) {
      blinkv = 0;
      previousMillisBlink = currentMillis;
    }
  }

  // color cycle
  if (buttonState == cycle) {
    r2 = r1;
    g2 = g1;
    b2 = b1;
    if ((currentMillis - previousMillis) >= interval) {

      if (rgbsB1 == 0) {
        r1++;
        if (r1 >= ledmax){
          rgbsB1++;
        } 
      }
      
      if (rgbsB1 == 1) {
        r1--;
        g1++;
        if (g1 >= ledmax){
          rgbsB1++;
        }
      }
      
      if (rgbsB1 == 2) {
        g1--;
        b1++;
        if (b1 >= ledmax){
          rgbsB1++;
        }
      }
      
      if (rgbsB1 == 3) {
        b1--;
        r1++;
        if (r1 >= ledmax){
          rgbsB1=1;
        }
      }
      previousMillis = currentMillis; 
    }
  }
  
  // bs looper
  if (buttonState > bSmax) {
    buttonState = 0;
  }
  
  // fade out
  if (buttonState == fadeout) {
    if (faderst == 0) {
      r1 = 0;
      g1 = 0;
      b1 = 0;
      r2 = 0;
      g2 = 0;
      b2 = 0;
      faderst++;
    }
    if (!digitalRead(key1)){
      r1 = ledmax;
      g1 = ledmax;
      b1 = ledmax;
    }
    if (!digitalRead(key2)){
      r2 = ledmax;
      g2 = ledmax;
      b2 = ledmax;
    }
    if (!digitalRead(key3)){
      r1 = ledmax;
      g1 = ledmax;
      b1 = ledmax;
      r2 = ledmax;
      g2 = ledmax;
      b2 = ledmax;
    }
    if ((currentMillis - previousMillis) >= fast) {
      if (digitalRead(key1)){
        if (g1 > 0 && b1 > 0 && r1 > 0) {
          g1-=addv;
          b1-=addv;
        }
        if (b1 == 0 && r1 > 0) {
          r1-=addv;
          g1+=addv;
        }
        if (g1 > 0 && r1 == 0) {
          g1-=addv;
          b1+=addv;
        }
        if (g1 == 0 && b1 > 0) {
          b1-=addv;
        }
      }
      if (digitalRead(key2)){
        if (g2 > 0 && b2 > 0 && r2 > 0) {
          g2-=addv;
          b2-=addv;
        }
        if (b2 == 0 && r2 > 0) {
          r2-=addv;
          g2+=addv;
        }
        if (g2 > 0 && r2 == 0) {
          g2-=addv;
          b2+=addv;
        }
        if (g2 == 0 && b2 > 0) {
          b2-=addv;
        }
      }
      previousMillis = currentMillis;
    }
  }
  
    // push off
  if (buttonState == pushoff) {
    if (digitalRead(key1)){
      r1 = ledmax;
      g1 = ledmax;
      b1 = ledmax;
    }
    if (digitalRead(key2)){
      r2 = ledmax;
      g2 = ledmax;
      b2 = ledmax;
    }
    if ((currentMillis - previousMillis) >= fast) {
      if (!digitalRead(key1)){
        if (g1 > 0 && b1 > 0 && r1 > 0) {
          g1-=addv;
          b1-=addv;
        }
        if (b1 == 0 && r1 > 0) {
          r1-=addv;
          g1+=addv;
        }
        if (g1 > 0 && r1 == 0) {
          g1-=addv;
          b1+=addv;
        }
        if (g1 == 0 && b1 > 0) {
          b1-=addv;
        }
      }
      if (!digitalRead(key2)){
        if (g2 > 0 && b2 > 0 && r2 > 0) {
          g2-=addv;
          b2-=addv;
        }
        if (b2 == 0 && r2 > 0) {
          r2-=addv;
          g2+=addv;
        }
        if (g2 > 0 && r2 == 0) {
          g2-=addv;
          b2+=addv;
        }
        if (g2 == 0 && b2 > 0) {
          b2-=addv;
        }
      }
      previousMillis = currentMillis;
    }
  }
  
  // individual adjust
  if (buttonState == custom) {
    if (!digitalRead(key1) && b3 == 1) {
      if ((currentMillis - previousMillisB1) >= intfastcycle) {

        if (rgbsB1 == 0) {
          r1++;
          if (r1 >= ledmax){
            rgbsB1++;
          }
        }
        
        if (rgbsB1 == 1) {
          r1--;
          g1++;
          if (g1 >= ledmax){
            rgbsB1++;
          }
        }
        
        if (rgbsB1 == 2) {
          g1--;
          b1++;
          if (b1 >= ledmax){
            rgbsB1++;
          }
        }
        
        if (rgbsB1 == 3) {
          b1--;
          r1++;
          if (r1 >= ledmax){
            rgbsB1=1;
          }
        }
        previousMillisB1 = currentMillis; 
      }
    }

    if (!digitalRead(key2) && b3 == 1) {
      if ((currentMillis - previousMillisB2) >= intfastcycle) {

        if (rgbsB2 == 0) {
          r2++;
          if (r2 >= ledmax){
            rgbsB2++;
          }
        }
        
        if (rgbsB2 == 1) {
          r2--;
          g2++;
          if (g2 >= ledmax){
            rgbsB2++;
          }
        }
        
        if (rgbsB2 == 2) {
          g2--;
          b2++;
          if (b2 >= ledmax){
            rgbsB2++;
          }
        }
        
        if (rgbsB2 == 3) {
          b2--;
          r2++;
          if (r2 >= ledmax){
            rgbsB2=1;
          }
        }
        previousMillisB2 = currentMillis; 
      }
    }
  }

  // BPM mood
  if (buttonState == BPM) {

    // Turns LEDs white when keys are pressed and applies color values when not pressed
    if (!digitalRead(key1)) {
      analogWrite(ledr1, ledmax);
      analogWrite(ledg1, ledmax);
      analogWrite(ledb1, ledmax);
    }
    if (!digitalRead(key2)) {
      analogWrite(ledr2, ledmax);
      analogWrite(ledg2, ledmax);
      analogWrite(ledb2, ledmax);
    }
    if (digitalRead(key1)) {
      analogWrite(ledr1, r1a);
      analogWrite(ledg1, g1a);
      analogWrite(ledb1, b1a);
    }
    if (digitalRead(key2)) {
      analogWrite(ledr2, r2a);
      analogWrite(ledg2, g2a);
      analogWrite(ledb2, b2a);
    }
    
    // transitioner
    if ((currentMillis - previousMillisSpeed) > intSpeed) {
      if (r1 < r1a) {
        r1a--;
      }
      if (r1 > r1a) {
        r1a++;
      }
      if (g1 < g1a) {
        g1a--;
      }
      if (g1 > g1a) {
        g1a++;
      }
      if (b1 < b1a) {
        b1a--;
      }
      if (b1 > b1a) {
        b1a++;
      }
      if (r2 < r2a) {
        r2a--;
      }
      if (r2 > r2a) {
        r2a++;
      }
      if (g2 < g2a) {
        g2a--;
      }
      if (g2 > g2a) {
        g2a++;
      }
      if (b2 < b2a) {
        b2a--;
      }
      if (b2 > b2a) {
        b2a++;
      }
      previousMillisSpeed = currentMillis;
    }
    
    // b3 setup
    if (analogRead(key3) > 1000) {
      b3 = 1;
    }
    if (analogRead(key3) < 1000) {
      b3 = 0;
    }
    
    // bp5s reset
    if ((currentMillis - previousMillisBPM) > intBPM) {
      bp5sPending = bp5s;
      bp5s = 0;
      previousMillisBPM = currentMillis;
    }
    if (bp5sPending == 0) {
      r1 = ledmin;
      g1 = ledmin;
      b1 = ledmin;
      r2 = ledmin;
      g2 = ledmin;
      b2 = ledmin;
    }
    
    if (bp5sPending < 2 && bp5sPending != 0) {
      r1 = ledmin;
      r2 = ledmin;
      g1 = ledmin;
      g2 = ledmin;
      b1 = ledmax/2;
      b2 = ledmax/2;    
    }
    
    if (bp5sPending < 4 && bp5sPending > 2) {
      r1 = ledmin;
      r2 = ledmin;
      g1 = ledmin;
      g2 = ledmin;
      b1 = ledmax;
      b2 = ledmax;    
    }
    
    if (bp5sPending < 6 && bp5sPending > 4) {
      r1 = ledmin;
      r2 = ledmin;
      g1 = ledmax/2;   
      g2 = ledmax/2;   
      b1 = ledmax/2;
      b2 = ledmax/2;    
    }
    
    if (bp5sPending < 8 && bp5sPending > 6) {
      r1 = ledmin;
      r2 = ledmin;
      g1 = ledmax;
      g2 = ledmax;
      b1 = ledmin;
      b2 = ledmin;
    }
    
    if (bp5sPending < 10 && bp5sPending > 8) {
      r1 = ledmax/2;
      r2 = ledmax/2;
      g1 = ledmax/2;
      g2 = ledmax/2;
      b1 = ledmin;
      b2 = ledmin;
    }
    
    if (bp5sPending > 10) {
      r1 = ledmax;
      r2 = ledmax;
      g1 = ledmin;
      g2 = ledmin;
      b1 = ledmin;
      b2 = ledmin;
    }
  }


  
  // keyboard code
  button0.update();
  button1.update();
  
  if (kbc == 0) {
    if (b3 == 0) { 
      if (button0.fallingEdge()) {
        Keyboard.press(key1p);
        key1c = 1;
      }
      if (button0.risingEdge()) {
        Keyboard.release(key1p);
        if (key1c == 1) {
          bp5s++;
          key1c = 0;
        }
      }
      if (button1.fallingEdge()) {
        Keyboard.press(key2p);
        key2c = 1;
      }
      if (button1.risingEdge()) {
        Keyboard.release(key2p);
        if (key2c == 1) {
          bp5s++;
          key2c = 0;
        }
      }
    }
  }

  if (kbc == 1) {
    if (b3 == 0) { 
      if (button0.fallingEdge()) {
        Keyboard.press(key1pa);
        key1c = 1;
      }
      if (button0.risingEdge()) {
        Keyboard.release(key1pa);
        if (key1c == 1) {
          bp5s++;
          key1c = 0;
        }
      }
      if (button1.fallingEdge()) {
        Keyboard.press(key2pa);
        key2c = 1;
      }
      if (button1.risingEdge()) {
        Keyboard.release(key2pa);
        if (key2c == 1) {
          bp5s++;
          key2c = 0;
        }
      }
    }
  }
}

