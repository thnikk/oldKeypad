#include <Bounce2.h> // bounce library for keboard functionality
#include <EEPROM.h> // EEPROM library for saving

// button mapping
#define key1p 122
#define key2p 120
#define key3p KEY_ESC

// button pins
const int key1 = 2; // left keyswitch
const int key2 = 3; // right keyswitch
const int key3 = 4; // side button

// LED pins
const int led_1 = 5; // left LED
const int led_2 = 6; // right LED

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
unsigned long previousMillisCD = 0;
unsigned long cooldown = 200;
int interval = 5;
int interval2 = 50;
int interval3 = 10000;
int intbs = 500;
int swbs = 0;
int bncn = 0;
int intbn = 75;
int b3cooldown = 0;

// values
int buttonState = EEPROM.read(0);
int led1 = 0;
int led2 = 0;
int ledmax = 50;
int ledmin = 0;
int b3 = 0;

// flipper for breathe effect
int mod = 1;

Bounce button1 = Bounce();
Bounce button2 = Bounce();

void setup()
{
  pinMode(key1, INPUT_PULLUP);
  button1.attach(key1);
  button1.interval(8);
  
  pinMode(key2, INPUT_PULLUP);
  button2.attach(key2);
  button2.interval(8);
  
  pinMode(key3, INPUT_PULLUP);
  pinMode(led_1, OUTPUT);
  pinMode(led_2, OUTPUT);
}

void loop()
{ 
  unsigned long currentMillis = millis();
    
  // buttonstate counter
  if (EEPROM.read(0) > 5) {
    EEPROM.write(0, 0);
  }
  
  unsigned long cmbs = millis();
  if (!digitalRead(key3)){
    swbs = 1;
      if ((unsigned long)(cmbs - pmbs) >= intbs) {
      analogWrite(led_1, 10);
      analogWrite(led_2, 10);
      led1 = 10;
      led2 = 10;
      swbs = 2;
    }
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
  
  if (digitalRead(key3)){
    if (swbs == 2) {
      swbs = 0;
      bncn = 1;
    }
    if (swbs == 1) {
      if ((unsigned long)(cmbs - pmbs) < intbs && (unsigned long)(cmbs - pmbs) !=0) {
        if (b3cooldown == 0) {
        Keyboard.press(key3p);
        delay(12);
        Keyboard.release(key3p);
        b3cooldown = 1;
        swbs = 0;
      }
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
      if (buttonState > 5) {
        buttonState = 1;
      }
      EEPROM.write(0, buttonState);
    }
  }

  // buttonstates
  analogWrite(led_1, led1);
  analogWrite(led_2, led2);

if (bncn == 0){      
  if (buttonState == ledfade) {
    if (!digitalRead(key1)){
      led1 = ledmax;
    }
    if (!digitalRead(key2)){
      led2 = ledmax;
    }
    if (!digitalRead(key3)){
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
    if (!digitalRead(key1)){
      led1 = 0;
    }
    if (!digitalRead(key2)){
      led2 = 0;
    }
    if (!digitalRead(key3)){
      led1 = 0;
      led2 = 0;
    }
    if (digitalRead(key1)){
      led1 = ledmax;
    }
    if (digitalRead(key2)){
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
  button1.update();
  button2.update();
  
  if (button1.read() == LOW) {
    Keyboard.press(key1p);
  }
  if (button1.read() == HIGH) {
    Keyboard.release(key1p);
  }
  if (button2.read() == LOW) {
    Keyboard.press(key2p);
  }
  if (button2.read() == HIGH) {
    Keyboard.release(key2p);
  }

  /*
  if (digitalRead(key3)){
    Serial.println("1");
  }
  if (!digitalRead(key3)){
    Serial.println("0");
  }
  */
  Serial.println(swbs);

}
