
 #include <NewPing.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <IRremote.h>

#define enablePin 5
#define in1Pin 6
#define in2Pin 7

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

int count = 0;
#define MINUTE 30
#define DEBUG 1

#define HDELTA 0.05

// 32 bit NEC IR remote control
#define IR_ON 0xF7C03F
#define IR_OFF 0xF740BF
#define IR_RED 0xF720DF
#define IR_GREEN 0xF7A05F
#define IR_BLUE 0xF7609F
#define IR_WHITE 0xF7E01F
#define IR_FADE 0xF7C837
#define IR_SMOOTH 0xF7E817
#define IR_STROBE 0xF7D02F
#define IR_FLASH 0xF7D02F
IRsend irsend;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(8, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);

  digitalWrite(10, LOW);
  digitalWrite(8, LOW);
  digitalWrite(A3,  LOW);
  digitalWrite(4, LOW);
  setLamp(HIGH);
  count = 0;

  Serial.begin(9600);
  /*
  if (tcs.begin()) {
    digitalWrite(8, HIGH);
    //Serial.println("Found sensor");
  } else {
    digitalWrite(8, HIGH);
    digitalWrite(A3,  HIGH);
    //Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }*/
}

// the loop function runs over and over again forever
void loop() {
  if (count > MINUTE){
    count = 0;
  } else if (count > 3 && count < 5) {
    setLamp(LOW);
    digitalWrite(4, LOW);
    setMotor(100, false);
  } else if (count > 10 && count < 13) {
    setMotor(0, false);
    sendIR(IR_ON);
    sendIR(IR_RED);
  } else if (count > 13 && count < 15) {
    sendIR(IR_OFF);
  } else if (count > 25) {
    setLamp(HIGH);
  }
  digitalWrite(4, HIGH);

  //analogWrite(enablePin, 0);
  digitalWrite(8, HIGH);
  digitalWrite(A3,  LOW);
  digitalWrite(10, LOW);
  delay(1000);
    setMotor(100, false);
  digitalWrite(8, LOW);
  digitalWrite(A3,  HIGH);
  digitalWrite(10, HIGH);
  delay(1000);

  count++;
}

void setMotor(int speed, boolean reverse)
{
  analogWrite(enablePin, speed);
  digitalWrite(in1Pin, ! reverse);
  digitalWrite(in2Pin, reverse);
}

void sendIR(unsigned long data) {
  for (int i = 0; i < 3; i++) {
    irsend.sendNEC(data, 32);
    delay(40);
  }
}

void setLamp(boolean on) {
  sendIR(on ? IR_ON : IR_OFF);
  sendIR(IR_BLUE);
  digitalWrite(A1, on);
  digitalWrite(A2, !on);
}

