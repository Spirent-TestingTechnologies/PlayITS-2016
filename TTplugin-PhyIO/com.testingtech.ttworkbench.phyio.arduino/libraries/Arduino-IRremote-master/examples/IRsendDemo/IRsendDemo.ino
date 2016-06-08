/*
 * IRremote: IRsendDemo - demonstrates sending IR codes with IRsend
 * An IR LED must be connected to Arduino PWM pin 3.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 *
// 32 bit NEC IR remote control
#define ON 0xF7C03F
#define OFF 0xF740BF
#define RED 0xF720DF
#define GREEN 0xF7A05F
#define BLUE 0xF7609F
#define WHITE 0xF7E01F
#define FADE 0xF7C837
#define SMOOTH 0xF7E817
#define STROBE 0xF7D02F
#define FLASH 0xF7D02F


#include <IRremote.h>

IRsend irsend;

void setup()
{

  
}

void loop() {
	for (int i = 0; i < 3; i++) {
		irsend.sendNEC(ON, 32);
		delay(40);
	}
  delay(1000); //5 second delay between each signal burst
  for (int i = 0; i < 3; i++) {
    irsend.sendNEC(GREEN, 32);
    delay(40);
  }
  delay(1000); //5 second delay between each signal burst
  for (int i = 0; i < 3; i++) {
    irsend.sendNEC(WHITE, 32);
    delay(40);
  }
  delay(1000); //5 second delay between each signal burst
  for (int i = 0; i < 3; i++) {
    irsend.sendNEC(RED, 32);
    delay(40);
  }
  delay(1000); //5 second delay between each signal burst
  for (int i = 0; i < 3; i++) {
    irsend.sendNEC(FLASH, 32);
    delay(40);
  }
  delay(1000); //5 second delay between each signal burst
  for (int i = 0; i < 3; i++) {
    irsend.sendNEC(BLUE, 32);
    delay(40);
  }
  delay(1000); //5 second delay between each signal burst
  for (int i = 0; i < 3; i++) {
    irsend.sendNEC(OFF, 32);
    delay(40);
  }
  delay(1000); //5 second delay between each signal burst
}
