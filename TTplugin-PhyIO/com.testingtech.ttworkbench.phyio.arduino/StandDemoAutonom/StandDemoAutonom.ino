
#include <NewPing.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <IRremote.h>

#define enablePin 5
#define in1Pin 6
#define in2Pin 7

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 120 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

#define MINUTE 30

// A6,A7 buttons - not working for now

//#define XDEBUG 1

#define XSERIAL Serial // mySerial // Serial // to switch between Bluetooth and USB XSERIAL connection for communication

#ifdef XDEBUG
#define DEBUG_PRINT(x) XSERIAL.print (x)
#define DEBUG_PRINTF(x,y) XSERIAL.print (x,y)
#define DEBUG_PRINTLN(x) XSERIAL.println (x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTF(x,y)
#define DEBUG_PRINTLN(x)
#endif

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

unsigned int lastDistance = 0;

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

  digitalWrite(8, LOW);
  digitalWrite(A3, LOW);
  digitalWrite(10, LOW);
  setLamp(LOW);
  sendIR(IR_ON);
  sendIR(IR_WHITE);
  unsigned int count = 0;

  XSERIAL.begin(9600, SERIAL_8N1); // Open serial monitor at 9600 baud
  while (count++ < 5 && !tcs.begin()) {
    digitalWrite(8, LOW);
    digitalWrite(A3, HIGH);
    DEBUG_PRINTLN("No colo sensor TCS34725 found ... check your connections");
    delay(2000);
  }
  if (tcs.begin()) {
    digitalWrite(8, HIGH);
    digitalWrite(A3, LOW);
    DEBUG_PRINTLN("Found color sensor TCS34725");
  } else {
    DEBUG_PRINTLN("Abort searching for color sensor TCS34725");
  }
}

bool readColor() {
  uint16_t r, g, b, c, colorTemp, lux;

  digitalWrite(4, HIGH);
  digitalWrite(10, HIGH);
  tcs.getRawData(&r, &g, &b, &c);
  digitalWrite(4, LOW);
  digitalWrite(10, LOW);
  colorTemp = tcs.calculateColorTemperature(r, g, b);
  lux = tcs.calculateLux(r, g, b);

  float rf, gf, bf;
  rf = r; rf /= c; rf *= 256;
  gf = g; gf /= c; gf *= 256;
  bf = b; bf /= c; bf *= 256;

  DEBUG_PRINT("Color Temp: "); DEBUG_PRINTF(colorTemp, DEC); DEBUG_PRINT(" K - ");
  DEBUG_PRINT("Lux: "); DEBUG_PRINTF(lux, DEC); DEBUG_PRINT(" - ");
  DEBUG_PRINT("R: "); DEBUG_PRINTF(rf,0); DEBUG_PRINT(" ");
  DEBUG_PRINT("G: "); DEBUG_PRINTF(gf,0); DEBUG_PRINT(" ");
  DEBUG_PRINT("B: "); DEBUG_PRINTF(bf,0); DEBUG_PRINT(" ");
 // DEBUG_PRINT("C: "); DEBUG_PRINTF(c, DEC); DEBUG_PRINT(" ");
  DEBUG_PRINTLN(" ");

  return redTestRGBToHsvRedTest((byte)rf, (byte)gf, (byte)bf);
}

boolean redTestRGBToHsvRedTest(byte r, byte g, byte b) {
    double rd = (double) r/255;
    double gd = (double) g/255;
    double bd = (double) b/255;
    double mx = max(rd, max(gd, bd));
    double mn = min(rd, min(gd, bd));
    double h, s, v = mx;

    double d = mx - mn;
    s = mx == 0 ? 0 : d / mx;

    if (mx == mn) { 
        h = 0; // achromatic
    } else {
        if (mx == rd) {
            h = (gd - bd) / d + (gd < bd ? 6 : 0);
        } else if (mx == gd) {
            h = (bd - rd) / d + 2;
        } else if (mx == bd) {
            h = (rd - gd) / d + 4;
        }
        h /= 6;
    }

  DEBUG_PRINT("H: "); DEBUG_PRINT(h); DEBUG_PRINT(" ");
  DEBUG_PRINT("S: "); DEBUG_PRINT(s); DEBUG_PRINT(" ");
  DEBUG_PRINT("V: "); DEBUG_PRINT(v); DEBUG_PRINT(" ");

//Color Temp: 1647 K - Lux: 0 - R: 171 G: 85 B: 0  
//H: 0.08 S: 1.00 V: 0.67 WH|ITE (back is YELLOW)

//Color Temp: 0 K - Lux: 0 - R: 0 G: 0 B: 0  
//H: 0.00 S: 0.00 V: 0.00 RED (back is BLUE - detected black)

//  bool res =  (s < 0.5);
  bool res =  (0.5 > h && h < 0.7);

  DEBUG_PRINTLN(res ? "WHITE" : "RED");
  // actually  WHITE is on back YELLOW
  //             RED is on back BLUE

  return res;
}

// the loop function runs over and over again forever
void loop() {
  checkDistance();
/*
  if (count > MINUTE){
    count = 0;
    setLamp(HIGH);
  } else if (count > 3 && count < 5) {
    setLamp(LOW);
    //setMotor(80, false);
  } else if (count > 10 && count < 13) {
    //setMotor(0, false);
  }

  //analogWrite(enablePin, 0);
  digitalWrite(8, HIGH);
  digitalWrite(A3, LOW);
  delay(1000);
  checkDistance();
  //  setMotor(100, false);
  digitalWrite(8, LOW);
  digitalWrite(A3, HIGH);
  delay(1000);


  //readColor();
  count++;*/
}

void checkDistance() {
  unsigned int t = sonar.ping_median(10);
  unsigned int cm = sonar.convert_cm(t);

  delay(50);                     // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  if (lastDistance != cm) {
    DEBUG_PRINT("Ping: ");
    DEBUG_PRINT(cm); // Send ping, get distance in cm and print result (0 = outside set distance range)
    DEBUG_PRINTLN("cm");
    lastDistance = cm;
  }

  if (cm < 1) {
    // 0 = outside set distance range - ignore
    delay(50);
  } else if (cm < 5) {
    setMotor(0, false);
    blinkLEDS();
    blinkLEDS();
    blinkLEDS();
    setMotor(0, false);
    blinkLEDS();
    bool isRed = readColor();
    sendIR(IR_ON);
    if (isRed) {
      setLamp(true);
      sendIR(IR_RED);
    } else {
      sendIR(IR_GREEN);
    }
    digitalWrite(8, LOW);
    digitalWrite(A3, LOW);
    delay(5000);
    setLamp(false);
    sendIR(IR_WHITE);
  } else if (cm > 35) {
    setMotor(0, false);
    digitalWrite(8, LOW);
    digitalWrite(A3, LOW);
  } else {
    digitalWrite(8, LOW);
    digitalWrite(A3, HIGH);
    setMotor(150-cm*150/80, false);
  }
}

void blinkLEDS() {
    delay(1000);
    digitalWrite(8, HIGH);
    digitalWrite(A3, LOW);
    delay(1000);
    digitalWrite(8, LOW);
    digitalWrite(A3, HIGH);
}

void setMotor(int speed, boolean reverse)
{
  analogWrite(enablePin, speed);
  digitalWrite(in1Pin, ! reverse);
  digitalWrite(in2Pin, reverse);
}


void setLamp(boolean on)
{
  digitalWrite(A1, on);
  digitalWrite(A2, !on);
}


void sendIR(unsigned long data) {
  for (int i = 0; i < 3; i++) {
    irsend.sendNEC(data, 32);
    delay(40);
  }
}
