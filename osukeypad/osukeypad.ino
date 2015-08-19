#include <TrinketKeyboard.h> // keyboard library
#include <avr/power.h> // for running at 16mhz
#define KEY_1 0
#define KEY_2 2
#define KEY_3 1

#define KEYP_1 KEYCODE_Z
#define KEYP_2 KEYCODE_X
#define KEYP_3 KEYCODE_ESC

void setup()
{
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1); // 16mhz
  pinMode(KEY_1, INPUT);
  pinMode(KEY_2, INPUT);
  pinMode(KEY_3, INPUT);
  digitalWrite(KEY_1, HIGH); 
  digitalWrite(KEY_2, HIGH);
  digitalWrite(KEY_3, LOW);
  TrinketKeyboard.begin();
}

void loop(){
  TrinketKeyboard.poll();
  
  if(digitalRead(KEY_3)){
    TrinketKeyboard.pressKey(0, KEYP_3);  // side button gets precident
  }
  else if(!digitalRead(KEY_1) && !digitalRead(KEY_2)){
    TrinketKeyboard.pressKey(0, KEYP_1, KEYP_2);  // z and x can be pressed at the same time
  }
  else if(!digitalRead(KEY_1)){
    TrinketKeyboard.pressKey(0, KEYP_1);
  }
  else if(!digitalRead(KEY_2)){
    TrinketKeyboard.pressKey(0, KEYP_2);
  }
  else{
    TrinketKeyboard.pressKey(0,0);  // needed so key is not pressed forever
  }  
}
