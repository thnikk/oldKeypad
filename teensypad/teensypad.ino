#include <Bounce.h> // bounce library for keboard functionality

// button mapping
#define KEYP_1 KEY_Z
#define KEYP_2 KEY_X
#define KEYP_3 KEY_ESC

// button pins
const int key_1 = 0; // left keyswitch
const int key_2 = 1; // right keyswitch
const int key_3 = 2; // side button

// LED pins
const int led_1 = 9; // left LED
const int led_2 = 10; // right LED

// LED buttonstates
const int ledfade = 1;
const int ledpushoff = 2;
const int ledon = 4;
const int ledoff = 5;
const int ledbreathe = 3;

// counters
unsigned long previousMillis=0;
unsigned long previousMillis2=0;
unsigned long pmbs=0;
unsigned long pmbn=0;
int interval = 5;
int interval2 = 50;
int interval3 = 10000;
int intbs = 500;
int swbs = 0;
int bncn = 0;
int intbn = 75;

// values
int buttonState = 1;
int led1 = 0;
int led2 = 0;
int ledmax = 50;
int ledmin = 0;

// flipper for breathe effect
int mod = 1;

Bounce button0 = Bounce(0, 8);
Bounce button1 = Bounce(1, 8);

void setup()
{
  pinMode(key_1, INPUT_PULLUP);
  pinMode(key_2, INPUT_PULLUP);
  pinMode(key_3, INPUT_PULLUP);
  pinMode(led_1, OUTPUT);
  pinMode(led_2, OUTPUT);
}

void loop()
{ 
  // buttonstate counter
  if (buttonState > 5){
    buttonState = 1;
  }
  
  unsigned long cmbs = millis();
  if (!digitalRead(key_3)){
    swbs = 1;
      if ((unsigned long)(cmbs - pmbs) >= intbs) {
      analogWrite(led_1, 10);
      analogWrite(led_2, 10);
      led1 = 10;
      led2 = 10;
      swbs = 2;
    }
  }
  
  if (digitalRead(key_3)){
    if (swbs == 2) {
      swbs = 0;
      bncn = 1;
    }
    if (swbs == 1) {
      if ((unsigned long)(cmbs - pmbs) < intbs && (unsigned long)(cmbs - pmbs) > 8) {
        Keyboard.press(KEYP_3);
        delay(10);
        Keyboard.release(KEYP_3);
        swbs = 0;
      }
    }
    if (swbs == 0) {
      pmbs = cmbs;
    }  
  }
  
  unsigned long bncm = millis();
  if (bncn == 1) {
    analogWrite(led_1, ledmax);
    analogWrite(led_2, ledmax);
    led1 = ledmax;
    led2 = ledmax;
    if ((unsigned long)(bncm - pmbn) >= intbn) {
      bncn++;
      pmbn = bncm;
    }
  }
  if (bncn == 2) {
    analogWrite(led_1, 0);
    analogWrite(led_2, 0);
    led1 = 0;
    led2 = 0;
    if ((unsigned long)(bncm - pmbn) >= intbn) {
      bncn++;
      pmbn = bncm;
    }
  }
  if (bncn == 3) {
    analogWrite(led_1, ledmax);
    analogWrite(led_2, ledmax);
    led1 = ledmax;
    led2 = ledmax;
    if ((unsigned long)(bncm - pmbn) >= intbn) {
      bncn++;
      pmbn = bncm;
    }
  }
    if (bncn == 4) {
    analogWrite(led_1, 0);
    analogWrite(led_2, 0);
    led1 = 0;
    led2 = 0;
    if ((unsigned long)(bncm - pmbn) >= intbn) {
      bncn++;
      pmbn = bncm;
    }
  }
  if (bncn == 5) {
    analogWrite(led_1, ledmax);
    analogWrite(led_2, ledmax);
    led1 = ledmax;
    led2 = ledmax;
    if ((unsigned long)(bncm - pmbn) >= intbn) {
      bncn++;
      pmbn = bncm;
    }
  }
  if (bncn == 6) {
    analogWrite(led_1, 0);
    analogWrite(led_2, 0);
    led1 = 0;
    led2 = 0;
    if ((unsigned long)(bncm - pmbn) >= intbn) {
      bncn = 0;
      pmbn = bncm;
      buttonState++;
    }
  }

  // buttonstates
  unsigned long currentMillis = millis();
  analogWrite(led_1, led1);
  analogWrite(led_2, led2);

if (bncn == 0){      
  if (buttonState == ledfade) {
    if (!digitalRead(key_1)){
      led1 = ledmax;
    }
    if (!digitalRead(key_2)){
      led2 = ledmax;
    }
    if (!digitalRead(key_3)){
      led1 = ledmax;
      led2 = ledmax;
    }
    if ((unsigned long)(currentMillis - previousMillis) >= interval) {
      if (led1 > 0){        
        led1--;          
      }
      if (led2 > 0){
        led2--;
      }
      previousMillis = currentMillis;      
    }
  }
  
  if (buttonState == ledpushoff) {
    if (!digitalRead(key_1)){
      led1 = 0;
    }
    if (!digitalRead(key_2)){
      led2 = 0;
    }
    if (!digitalRead(key_3)){
      led1 = 0;
      led2 = 0;
    }
    if (digitalRead(key_1)){
      led1 = ledmax;
    }
    if (digitalRead(key_2)){
      led2 = ledmax;
    }
  }
  
  if (buttonState == ledon) {
    led1 = ledmax;
    led2 = ledmax;
  }
  
  if (buttonState == ledoff) {
    led1 = 0;
    led2 = 0;
  }
}
  
  unsigned long currentMillis2 = millis();
  if (buttonState == ledbreathe){
    if ((unsigned long)(currentMillis - previousMillis) >= interval2) {
      analogWrite(led_1, led1);
      analogWrite(led_2, led2);
      led1+=mod;
      led2+=mod;
      if(led1 >= ledmax && led2 >= ledmax) mod*=-1;
      if(led1 <= ledmin && led2 <= ledmin) {
        led1=0;
        led2=0;
        if ((unsigned long)(currentMillis2 - previousMillis2) >= interval3) {
          mod*= -1;
          previousMillis2 = currentMillis2;
        }
      }
      previousMillis = currentMillis;   
    }   
  }   
  
// keyboard section

  button0.update();
  button1.update();

  if (button0.fallingEdge()) {
    Keyboard.press(KEYP_1);
  }
  else if (button0.risingEdge()) {
    Keyboard.release(KEYP_1);
  }
  if (button1.fallingEdge()) {
    Keyboard.press(KEYP_2);
  }
  else if (button1.risingEdge()) {
    Keyboard.release(KEYP_2);
  }

}
