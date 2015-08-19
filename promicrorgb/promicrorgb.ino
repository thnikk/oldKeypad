#include <WS2812.h>    // Library for RGB LEDs
#include <Bounce2.h>   // bounce library for keboard functionality
#include <EEPROM.h>    // EEPROM library for saving

// button mapping
#define key1p 122      // default mapping (z)
#define key2p 120      // default mapping (x)
#define key1pA 99      // alternate mapping (c)
#define key2pA 118     // alternate mapping (v)
#define key3p KEY_ESC

// LEDs
#define outputPin 3
#define LEDCount 2
WS2812 LED(LEDCount);
cRGB value;

int ledMode = EEPROM.read(0);
const int cycle = 0;
const int reactive = 1;
const int pushOff = 2;
const int BPM = 3;
const int custom = 4;

int buttonMapping = EEPROM.read(9);
const int ZX = 0;
const int CV = 1;

int ledMin = 0;
int ledMax = 200;
int cycleCount = 0;
int fadeReset = 0;
int addv = 2;
int rgbsB1 = 0;
int rgbsB2 = 0;
int bps = 0;
int bpsPending = 0;
int key1c = 0;
int key2c = 0;
int buttonValue = 0;
int blinkCount = 0;
int reset = 0;
int blinkValue = 0;
int bBlink = 0;
int count = 0;
int b3cooldown = 0;
int BPMReset = 0;

// millis timer values
  // previousMillis
  unsigned long previousMillis = 0;
  unsigned long previousMillisB1 = 0;
  unsigned long previousMillisB2 = 0;
  unsigned long previousMillisB3 = 0;
  unsigned long previousMillisBPM = 0;
  unsigned long previousMillisBPS = 0;
  unsigned long previousMillisBlink = 0;
  unsigned long previousMillisCD = 0;
  unsigned long previousMillisBright = 0;
  // interval
  unsigned long interval = 10;
  unsigned long fast = 1;
  unsigned long intBPM = 1000;
  unsigned long intBPS = 5;
  unsigned long shortHold = 500;
  unsigned long longHold = 1500;
  unsigned long veryLongHold = 6000;
  unsigned long blinkShort = 50;
  unsigned long blinkLong = 100;
  unsigned long blinkReset = 200;
  unsigned long blinkResetLong = 350;
  unsigned long bpmTransition = 5;
  unsigned long cooldown = 200;

// button pins
const int key1 = 2; // left keyswitch
const int key2 = 4; // right keyswitch
const int key3 = 18; // side button

// LED values
int r1 = 0;
int g1 = 0;
int b1 = 0;
int r2 = 0;
int g2 = 0;
int b2 = 0;

int r1bpm = 0;
int g1bpm = 0;
int b1bpm = 0;
int r2bpm = 0;
int g2bpm = 0;
int b2bpm = 0;

// Brightness
struct RGB {
  byte r;
  byte g;
  byte b;
};

RGB led1 = { 0, 0, 0 };
RGB led2 = { 0, 0, 0 };

int bMin = 20;
int bMax = 100;
float bVal = 0;
int bPer = EEPROM.read(11);


Bounce button1 = Bounce();
Bounce button2 = Bounce();
int b3 = 0;

void setup() {
  // button 1
  pinMode(key1, INPUT_PULLUP);
  button1.attach(key1);
  button1.interval(8);
  
  // button 2
  pinMode(key2, INPUT_PULLUP);
  button2.attach(key2);
  button2.interval(8);
  
  // button 3
  pinMode(key3, INPUT);
  
  // LED
  LED.setOutput(outputPin);
  LED.setColorOrderGRB();
}

void loop() {
  // millis timer
  unsigned long currentMillis = millis();
  
  // EEPROM wipe
  if (EEPROM.read(10) != 0) {
    EEPROM.write(0, 0);
    EEPROM.write(1, 150);
    EEPROM.write(2, 0);
    EEPROM.write(3, 50);
    EEPROM.write(4, 50);
    EEPROM.write(5, 0);
    EEPROM.write(6, 150);
    EEPROM.write(7, 3);
    EEPROM.write(8, 3);
    EEPROM.write(9, 0);
    EEPROM.write(10, 0);
    EEPROM.write(11, 60);
  }
  
  // button 3 section
  if (analogRead(key3) > 1000) {
    b3 = 1;
  }
  if (analogRead(key3) < 1000) {
    previousMillisB3 = currentMillis;
    b3 = 0;
  }
  
  // button 3 press actions
  if (b3 == 1) {
    // ESC
    if ((currentMillis - previousMillisB3) > 10 && (currentMillis - previousMillisB3) < shortHold) {
      if (buttonValue == 0) {
        buttonValue = 1;
      }
    }
    // ledMode change
    if ((currentMillis - previousMillisB3) > shortHold && (currentMillis - previousMillisB3) < longHold) {
      if (buttonValue == 1) {
        buttonValue = 2;
        blinkValue = 1;
      }
    }
    
    // custom changer
    if (ledMode == custom) {
      if ((currentMillis - previousMillisB3) > longHold) {
        if (buttonValue == 2) {
          buttonValue = 3;
          blinkValue = 2;
        }
        if (!digitalRead(key1) || !digitalRead(key2)) {
          previousMillisB3 = currentMillis - longHold;
        }
      }
    }

    // brightness changer
    if (ledMode != custom) {
      if ((currentMillis - previousMillisB3) > longHold) {
        if (buttonValue == 2) {
          buttonValue = 5;
          blinkValue = 2;
        }
        if (!digitalRead(key1) || !digitalRead(key2)) {
          previousMillisB3 = currentMillis - longHold;
        }
      }
    }
    
    // remap to c and v (include blink)
    if ((currentMillis - previousMillisB3) > veryLongHold) {
      if (buttonValue == 2 || buttonValue == 3 || buttonValue == 5) {
      if (buttonValue == 2 || buttonValue == 3) {
        buttonValue = 4;
        blinkValue = 2;
      }
    }
  }

  if (buttonValue == 5) {
      led1.r = ledMax*bVal;
      led1.g = ledMax*bVal;
      led1.b = ledMax*bVal;
      led2.r = ledMax*bVal;
      led2.g = ledMax*bVal;
      led2.b = ledMax*bVal;
    if ((currentMillis - previousMillisBright) > interval*4) {
      if (!digitalRead(key1)) {
        bPer--;
      }
      if (!digitalRead(key2)) {
        bPer++;
      }
      previousMillisBright = currentMillis;
    }
  }

  if (bPer > bMax) {
    bPer = bMax;
  }
  if (bPer < bMin) {
    bPer = bMin;
  }
  Serial.println(bPer);
  
  // button 3 cooldown code
  if (b3cooldown == 0) {
    previousMillisCD = currentMillis;
  }
  
  if (b3cooldown == 1) {
    if ((currentMillis - previousMillisCD) > cooldown) {
      b3cooldown = 0;
    }
  }
  }
  //
  
  // button 3 release actions
  if (b3 == 0) {
    if (buttonValue == 1) {
      if (b3cooldown == 0) {
        Keyboard.press(key3p);
        delay(12);
        Keyboard.release(key3p);
        b3cooldown = 1;
      }
    }
    if (buttonValue == 2) {
      ledMode++;
      EEPROM.write(0, ledMode);
      bps = 0;
      if (ledMode >= 5) {
        ledMode = 0;
      }
      if (ledMode != custom) {
        r1 = 0;
        g1 = 0;
        b1 = 0;
        r2 = 0;
        g2 = 0;
        b2 = 0;
        cycleCount = 0;
      }
      if (ledMode == custom) {
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
    if (buttonValue == 3) {
      EEPROM.write(1, r1);
      EEPROM.write(2, g1);
      EEPROM.write(3, b1);
      EEPROM.write(4, r2);
      EEPROM.write(5, g2);
      EEPROM.write(6, b2);
      EEPROM.write(7, rgbsB1);
      EEPROM.write(8, rgbsB2);
    }
    if (buttonValue == 4) {
      buttonMapping++;
      if (buttonMapping >= 2) {
       buttonMapping = 0;
      }
      EEPROM.write(9, buttonMapping);
    }
    if (buttonValue == 5) {
      EEPROM.write(11,bPer);
    }
    buttonValue = 0;  
  }

  
  
  // blink code
  if (blinkValue != 0) {
    if (blinkValue == 1) {
      if ((currentMillis - previousMillisBlink) > blinkShort && (currentMillis - previousMillisBlink) < blinkLong) {
        bBlink = ledMin; 
      }
      if ((currentMillis - previousMillisBlink) > blinkLong && (currentMillis - previousMillisBlink) < blinkLong + blinkShort) {
        bBlink = ledMax*bVal;
      }
      if ((currentMillis - previousMillisBlink) > blinkLong + blinkShort) {
        bBlink = ledMin;
      }
      if ((currentMillis - previousMillisBlink) > blinkReset) {
        blinkValue = 0;
      }
    }
    if (blinkValue == 2) {
      if ((currentMillis - previousMillisBlink) > blinkShort && (currentMillis - previousMillisBlink) < blinkLong) {
        bBlink = ledMin; 
      }
      if ((currentMillis - previousMillisBlink) > blinkLong && (currentMillis - previousMillisBlink) < blinkLong + blinkShort) {
        bBlink = ledMax*bVal;
      }
      if ((currentMillis - previousMillisBlink) > blinkLong + blinkShort && (currentMillis - previousMillisBlink) < blinkLong + blinkShort*2) {
        bBlink = ledMin;
      }
      if ((currentMillis - previousMillisBlink) > blinkLong + blinkShort*2 && (currentMillis - previousMillisBlink) < blinkLong*2 + blinkShort*2) {
        bBlink = ledMax*bVal;
      }
      if ((currentMillis - previousMillisBlink) > blinkLong*2 + blinkShort*2) {
        bBlink = ledMin;
      }
      if ((currentMillis - previousMillisBlink) > blinkReset + blinkLong + blinkShort) {
        blinkValue = 0;
      }
    }
  }
  
  if (blinkValue == 0) {
    previousMillisBlink = currentMillis;
  }
  
  if (ledMode == custom) {
    if (count == 0) {
      r1 = EEPROM.read(1);
      g1 = EEPROM.read(2);
      b1 = EEPROM.read(3);
      r2 = EEPROM.read(4);
      g2 = EEPROM.read(5);
      b2 = EEPROM.read(6);
      rgbsB1 = EEPROM.read(7);
      rgbsB2 = EEPROM.read(8);
      count++;
    }
  }

  bVal = bPer*0.01;

  if (buttonValue != 5) {
  led1.r = r1*bVal;
  led1.g = g1*bVal;
  led1.b = b1*bVal;
  led2.r = r2*bVal;
  led2.g = g2*bVal;
  led2.b = b2*bVal;
  }

  
  
  // LED Code
  byte i = 0;

  if (blinkValue == 0) {
    while (i < LEDCount){
      if ((i % 3) == 0) {  // First LED, and every third after that
        value.b = led1.b; 
        value.g = led1.g; 
        value.r = led1.r; // RGB Value -> Red Only
        LED.set_crgb_at(i, value); // Set value at LED found at index 0
      }
      else if ((i % 3) == 1) { // Second LED, and every third after that
        value.b = led2.b; 
        value.g = led2.g; 
        value.r = led2.r; // RGB Value -> Green Only
        LED.set_crgb_at(i, value); // Set value at LED found at index 0
      }
      else  { // Third LED, and every third after that
        value.b = 0; 
        value.g = 0; 
        value.r = 0; // RGB Value -> Blue Only
        LED.set_crgb_at(i, value); // Set value at LED found at index 0
      }
      i++;
    }
  }
  
  
  
  if (blinkValue != 0) {
    while (i < LEDCount){
      if ((i % 3) == 0) {  // First LED, and every third after that
        value.b = bBlink; 
        value.g = bBlink; 
        value.r = bBlink; // RGB Value -> Red Only
        LED.set_crgb_at(i, value); // Set value at LED found at index 0
      }
      else if ((i % 3) == 1) { // Second LED, and every third after that
        value.b = bBlink; 
        value.g = bBlink; 
        value.r = bBlink; // RGB Value -> Green Only
        LED.set_crgb_at(i, value); // Set value at LED found at index 0
      }
      else  { // Third LED, and every third after that
        value.b = 0; 
        value.g = 0; 
        value.r = 0; // RGB Value -> Blue Only
        LED.set_crgb_at(i, value); // Set value at LED found at index 0
      }
      i++;
    }
  }

  

  LED.sync();

  if (ledMode > custom) {
    ledMode = cycle;
  }
  if (ledMode == cycle) {
    r2 = r1;
    g2 = g1;
    b2 = b1;
    if ((currentMillis - previousMillis) >= interval) {

      if (cycleCount == 0) {
        r1++;
        if (r1 >= ledMax){
          cycleCount++;
        } 
      }
      
      if (cycleCount == 1) {
        r1--;
        g1++;
        if (g1 >= ledMax){
          cycleCount++;
        }
      }
      
      if (cycleCount == 2) {
        g1--;
        b1++;
        if (b1 >= ledMax){
          cycleCount++;
        }
      }
      
      if (cycleCount == 3) {
        b1--;
        r1++;
        if (r1 >= ledMax){
          cycleCount=1;
        }
      }
      previousMillis = currentMillis; 
    }
  }

  
  
  if (ledMode == reactive) {
    if (fadeReset == 0) {
      r1 = 0;
      g1 = 0;
      b1 = 0;
      r2 = 0;
      g2 = 0;
      b2 = 0;
      fadeReset++;
    }
    if (!digitalRead(key1)){
      r1 = ledMax;
      g1 = ledMax;
      b1 = ledMax;
    }
    if (!digitalRead(key2)){
      r2 = ledMax;
      g2 = ledMax;
      b2 = ledMax;
    }
    if (b3 == 1){
      r1 = ledMax;
      g1 = ledMax;
      b1 = ledMax;
      r2 = ledMax;
      g2 = ledMax;
      b2 = ledMax;
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

  
  
  if (ledMode == pushOff) {
    if (digitalRead(key1)){
      r1 = ledMax;
      g1 = ledMax;
      b1 = ledMax;
    }
    if (digitalRead(key2)){
      r2 = ledMax;
      g2 = ledMax;
      b2 = ledMax;
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

  
  
  if (ledMode == custom) {
    if (buttonValue == 3) {
      if (!digitalRead(key1) && b3 == 1) {
        if ((currentMillis - previousMillisB1) >= interval) {
  
          if (rgbsB1 == 0) {
            r1++;
            if (r1 >= ledMax){
              rgbsB1++;
            }
          }
          
          if (rgbsB1 == 1) {
            r1--;
            g1++;
            if (g1 >= ledMax){
              rgbsB1++;
            }
          }
          
          if (rgbsB1 == 2) {
            g1--;
            b1++;
            if (b1 >= ledMax){
              rgbsB1++;
            }
          }
          
          if (rgbsB1 == 3) {
            b1--;
            r1++;
            if (r1 >= ledMax){
              rgbsB1=1;
            }
          }
          previousMillisB1 = currentMillis; 
        }
      }
  
      if (!digitalRead(key2) && b3 == 1) {
        if ((currentMillis - previousMillisB2) >= interval) {
  
          if (rgbsB2 == 0) {
            r2++;
            if (r2 >= ledMax){
              rgbsB2++;
            }
          }
          
          if (rgbsB2 == 1) {
            r2--;
            g2++;
            if (g2 >= ledMax){
              rgbsB2++;
            }
          }
          
          if (rgbsB2 == 2) {
            g2--;
            b2++;
            if (b2 >= ledMax){
              rgbsB2++;
            }
          }
          
          if (rgbsB2 == 3) {
            b2--;
            r2++;
            if (r2 >= ledMax){
              rgbsB2=1;
            }
          }
          previousMillisB2 = currentMillis; 
        }
      }
    }
  }

  
  
  if (ledMode == BPM) {
    if (BPMReset == 0) {
      bps = 0;
      BPMReset++;
    }
    if ((currentMillis - previousMillisBPM) > bpmTransition) {
      if (r1bpm < r1) {
        r1--;
      }
      if (r1bpm > r1) {
        r1++;
      }
      if (g1bpm < g1) {
        g1--;
      }
      if (g1bpm > g1) {
        g1++;
      }
      if (b1bpm < b1) {
        b1--;
      }
      if (b1bpm > b1) {
        b1++;
      }
      if (r2bpm < r2) {
        r2--;
      }
      if (r2bpm > r2) {
        r2++;
      }
      if (g2bpm < g2) {
        g2--;
      }
      if (g2bpm > g2) {
        g2++;
      }
      if (b2bpm < b2) {
        b2--;
      }
      if (b2bpm > b2) {
        b2++;
      }
      previousMillisBPM = currentMillis;
    }
    
    if ((currentMillis - previousMillisBPS) > intBPM) {
      bpsPending = bps;
      bps = 0;
      previousMillisBPS = currentMillis;
    }
    if (bpsPending == 0) {
      r1bpm = ledMin;
      g1bpm = ledMin;
      b1bpm = ledMin;
      r2bpm = ledMin;
      g2bpm = ledMin;
      b2bpm = ledMin;
    }
    
    if (bpsPending < 2 && bpsPending != 0) {
      r1bpm = ledMin;
      r2bpm = ledMin;
      g1bpm = ledMin;
      g2bpm = ledMin;
      b1bpm = ledMax/2;
      b2bpm = ledMax/2;    
    }
    
    if (bpsPending < 4 && bpsPending > 2) {
      r1bpm = ledMin;
      r2bpm = ledMin;
      g1bpm = ledMin;
      g2bpm = ledMin;
      b1bpm = ledMax;
      b2bpm = ledMax;    
    }
    
    if (bpsPending < 6 && bpsPending > 4) {
      r1bpm = ledMin;
      r2bpm = ledMin;
      g1bpm = ledMax/2;   
      g2bpm = ledMax/2;   
      b1bpm = ledMax/2;
      b2bpm = ledMax/2;    
    }
    
    if (bpsPending < 8 && bpsPending > 6) {
      r1bpm = ledMin;
      r2bpm = ledMin;
      g1bpm = ledMax;
      g2bpm = ledMax;
      b1bpm = ledMin;
      b2bpm = ledMin;
    }
    
    if (bpsPending < 10 && bpsPending > 8) {
      r1bpm = ledMax/2;
      r2bpm = ledMax/2;
      g1bpm = ledMax/2;
      g2bpm = ledMax/2;
      b1bpm = ledMin;
      b2bpm = ledMin;
    }
    
    if (bpsPending > 10) {
      r1bpm = ledMax;
      r2bpm = ledMax;
      g1bpm = ledMin;
      g2bpm = ledMin;
      b1bpm = ledMin;
      b2bpm = ledMin;
    }
    if (!digitalRead(key1)){
      
    }
    if (!digitalRead(key2)){
      
    }
  }
  
  
  
  // keyboard section
  button1.update();
  button2.update();

  if (buttonValue != 5) {
    if (b3 == 0) {
      if (buttonMapping == ZX) {
        if (button1.read() == LOW) {
          Keyboard.press(key1p);
          key1c = 1;
        }
        if (button1.read() == HIGH) {
          Keyboard.release(key1p);
          if (key1c == 1) {
            bps++;
            key1c = 0;
          }
        }
        if (button2.read() == LOW) {
          Keyboard.press(key2p);
          key2c = 1;
        }
        if (button2.read() == HIGH) {
          Keyboard.release(key2p);
          if (key2c == 1) {
            bps++;
            key2c = 0;
          }
        }
      }
      if (buttonMapping == CV) {
        if (button1.read() == LOW) {
          Keyboard.press(key1pA);
          key1c = 1;
        }
        if (button1.read() == HIGH) {
          Keyboard.release(key1pA);
          if (key1c == 1) {
            bps++;
            key1c = 0;
          }
        }
        if (button2.read() == LOW) {
          Keyboard.press(key2pA);
          key2c = 1;
        }
        if (button2.read() == HIGH) {
          Keyboard.release(key2pA);
          if (key2c == 1) {
            bps++;
            key2c = 0;
          }
        }
      }
    }
  }
}
