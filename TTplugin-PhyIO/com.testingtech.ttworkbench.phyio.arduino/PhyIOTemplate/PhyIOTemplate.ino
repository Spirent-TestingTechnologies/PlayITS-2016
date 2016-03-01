#include <SoftwareSerial.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <NewPing.h>
#include <Time.h>
#include <IRremote.h>


#define MAX_PARAM 5

#define XSERIAL Serial // mySerial // Serial // to switch between Bluetooth and USB XSERIAL connection for communication

#define CV01   10
#define PE01   20
#define LED01  30
#define REL01  40
#define MM01   50
#define LS01   70

#define SETUP 1
#define SET 1
#define START 2
#define STOP 3
#define READ 4
#define R1 101 //Result

#define DEBUG 1

#ifdef DEBUG
#define DEBUG_PRINT(x) XSERIAL.print (x)
#define DEBUG_PRINTLN(x) XSERIAL.println (x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif

unsigned long readTimer; // holds the next read time;
unsigned int readSpeed = 500; // How frequently are we going to read the serial line

//#define CVF_PRESENT 1
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
#define cvf_ledpin 4

//#define PE_PRESENT 1
#define TRIGGER_PIN 12
#define ECHO_PIN 11
#define MAX_DISTANCE 100

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

//#define MOTOR_PRESENT 1
#define enablePin 5
#define in1Pin 6
#define in2Pin 7

//#define LEDS_PRESENT 1
//D8+A3 LED bicolor
#define LED1PIN1  8 // HIGH = red
#define LED1PIN2  A3 // HIGH = green
#define LED2PIN   10 // blue

//#define RELAYS_PRESENT 1
#define RELAY1PIN  A1
#define RELAY2PIN  A2

// #define BT_PRESENT 1
#define BT_RX 2
#define BT_TX 3

//#define LS_PRESENT 1
#define LS1PIN A1

//#ifdef BT_PRESENT
SoftwareSerial mySerial(BT_RX, BT_TX); // RX, TX
//#endif

//#define IR_PRESENT 1
#ifdef IR_PRESENT

// IR LED DATA on D9 (PWM)

IRsend irsend;
#endif

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

void setup() {
  XSERIAL.begin(9600, SERIAL_8N1); // Open serial monitor at 9600 baud
  while (!XSERIAL) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

#ifdef CVF_PRESENT
  if (tcs.begin()) {
    DEBUG_PRINTLN("#Found CVF sensor");
  } else {
    DEBUG_PRINTLN("#No TCS34725 found ... check your connections");
    while (1); // halt!
  }
  // use these three pins to drive an LED
  pinMode(cvf_ledpin, OUTPUT);
  digitalWrite(cvf_ledpin, HIGH);
#else
  DEBUG_PRINTLN("#Found simulated CVF sensor");
#endif

  // LED Config
  pinMode(LED1PIN1, OUTPUT);
  pinMode(LED1PIN2, OUTPUT);
  pinMode(LED2PIN, OUTPUT);

  // Motor Config
  pinMode(enablePin, OUTPUT);
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);

  // Relay Config
  pinMode(RELAY1PIN, OUTPUT);
  pinMode(RELAY2PIN, OUTPUT);


  // ECHO PING Config in Constructor of SONAR
  // BT / Softserial Config in Constructor of SoftSerial


  establishContact();  // send a byte to establish contact until receiver responds

  DEBUG_PRINTLN("#Entered loop");
}

void loop() {
  int id, functionType, command;
  if (millis() >= readTimer) {
    readTimer += readSpeed;      // Set the next ping time.
    if (XSERIAL.available() > 0) {
      // Read in loop all data for one line
      id = XSERIAL.parseInt();
      functionType = XSERIAL.parseInt();
      command = XSERIAL.parseInt();

      switch (functionType) {
        case CV01:
          ColorViewFunction(id, command);
          break;
        case PE01:
          PingEchoFunction(id, command);
          break;
        case LED01:
          LEDFunction(id, command);
          break;
        case REL01:
          RelayFunction(id, command);
          break;
        case MM01:
          MotorFunction(id, command);
          break;
        case LS01:
          LightSensorFunction(id, command);
          break;
      }
      processCleanUp();
    }
  }
}

// -------- ColorViewFunction ------------
struct ColorViewFunctionConfig
{
  int ID; // The ID
  unsigned long TimeBetweenSamples; // time when cvf read is taken
  unsigned int intLED; // uses the internal LED
  uint16_t clear, red, green, blue; // raw rgb values
  uint16_t red256, green256, blue256; // normed rgb values (0..255)
  uint16_t colorTemp, lux; // computed values
  unsigned long ReadTimestamp;

  void Reset()
  {
    // Set default values in case there is nothing
    // stored in the eeprom yet.
    TimeBetweenSamples = 500;
    intLED = 1;
  }
};

unsigned long cvfTimer;     // Holds the next cvftime.

ColorViewFunctionConfig cvfPoint;

void ColorViewFunction( int id, int command) {
  cvfPoint.ID = id;
  DEBUG_PRINT("#In the ColorViewFunction with function ");
  DEBUG_PRINTLN(command);

  switch (command) {
    case SETUP:
      ColorViewFunctionSetup();
      break;
    case START:
      // ColorViewFunctionStart();
      break;
    case STOP:
      // ColorViewFunctionStop();
      break;
    case READ:
      ColorViewFunctionRead();
      break;
    default:
      break;
  }
  // Read parameters
}

void ColorViewFunctionSetup() {
  DEBUG_PRINTLN("#In the ColorViewFunctionSetup");

  int timeBetweenSamples = 100;
  int intLED = 1; // default

  //ID,CV01,SETUP,<timeBetweenSamples:unit16>[,<LED ON/OFF = 1>]
  //Configures the Color View Module
  //<timeBetweenSamples:unit16>    - Time between two measurements in milliseconds
  //<LED OFF = 1>       - 0 to switch LED DEFAULT OFF, default=1 ON, other values ignored. optional

  cvfPoint.TimeBetweenSamples = XSERIAL.parseInt();
  DEBUG_PRINT("#parameters are ");
  DEBUG_PRINT(" time = ");
  DEBUG_PRINT(cvfPoint.TimeBetweenSamples);

  if (XSERIAL.peek() == 10 && XSERIAL.find(10)) {
    DEBUG_PRINT(" ; intLED<default>= ");
    DEBUG_PRINTLN(intLED);
    return; // no more optional params
  }

  cvfPoint.intLED = XSERIAL.parseInt();
  DEBUG_PRINT("# ; intLED= ");
  DEBUG_PRINTLN(cvfPoint.intLED);
}

void ColorViewFunctionRead() {
  uint16_t clear, red, green, blue;

  DEBUG_PRINTLN("#In the ColorViewFunctionRead");

#ifdef CVF_PRESENT
  tcs.setInterrupt(false);      // turn on LED
  delay(60);  // takes 50ms to read
  tcs.getRawData(&(cvfPoint.red), &(cvfPoint.green), &(cvfPoint.blue), &(cvfPoint.clear));
  tcs.setInterrupt(true);  // turn off LED
#else
  cvfPoint.red = random(555);
  cvfPoint.green = random(555);
  cvfPoint.blue = random(755);
  cvfPoint.clear = random(655);
#endif
  cvfPoint.ReadTimestamp = millis();
  cvfPoint.colorTemp = tcs.calculateColorTemperature(cvfPoint.red, cvfPoint.green, cvfPoint.blue);
  cvfPoint.lux = tcs.calculateLux(cvfPoint.red, cvfPoint.green, cvfPoint.blue);
  
  float rf, gf, bf;
  
  rf = cvfPoint.red;
  rf /= cvfPoint.clear;
  rf *= 256;

  gf = cvfPoint.green;
  gf /= cvfPoint.clear;
  gf *= 256;

  bf = cvfPoint.blue;
  bf /= cvfPoint.clear;
  bf *= 256;

  cvfPoint.red256 = rf;
  cvfPoint.green256 = gf;
  cvfPoint.blue256 = bf;

  ColorViewFunctionR1();
}

void ColorViewFunctionR1() {
  // ID, CV01,R1,<timestamp:unit16>,<r:unit16>, <g:unit16>, <b:unit16>, <c:unit16>, <colorTemp:unit16>, <lux:unit16>
  XSERIAL.print(cvfPoint.ID); XSERIAL.print(", ");
  XSERIAL.print(CV01);  XSERIAL.print(", ");
  XSERIAL.print(R1);  XSERIAL.print(", ");
  XSERIAL.print(cvfPoint.ReadTimestamp);  XSERIAL.print(", ");
  XSERIAL.print(cvfPoint.red);  XSERIAL.print(", ");
  XSERIAL.print(cvfPoint.green);  XSERIAL.print(", ");
  XSERIAL.print(cvfPoint.blue);  XSERIAL.print(", ");
  XSERIAL.print(cvfPoint.clear);  XSERIAL.print(", ");
  XSERIAL.print(cvfPoint.colorTemp);  XSERIAL.print(", ");
  XSERIAL.print(cvfPoint.lux);  XSERIAL.print(", ");
  XSERIAL.print(cvfPoint.red256);  XSERIAL.print(", ");
  XSERIAL.print(cvfPoint.green256);  XSERIAL.print(", ");
  XSERIAL.println(cvfPoint.blue256);
  XSERIAL.flush();
}
// ----------- PING ECHO Function ----------------------
struct PingEchoFunctionConfig
{
  int ID; // The ID
  unsigned int TimeBetweenSamples; // time when pe read is taken
  unsigned long ReadTimestamp;

  int medianIterations;
  int maxDistance;
  int unit;
  long int distance ; // value depends on the unit set

  void Reset()
  {
    // Set default values in case there is nothing
    // stored in the eeprom yet.
    TimeBetweenSamples = 500;
    medianIterations = 1;
    maxDistance = MAX_DISTANCE;
    unit = 1;
    distance = 0;
  }
};

unsigned long peTimer;     // Holds the next petime.

PingEchoFunctionConfig pePoint;

void PingEchoFunction( int id, int command) {
  pePoint.ID = id;
  DEBUG_PRINT("#In the PingEchoFunction with function ");
  DEBUG_PRINTLN(command);

  switch (command) {
    case SETUP:
      PingEchoFunctionSetup();
      break;
    case START:
      // ColorViewFunctionStart();
      break;
    case STOP:
      // ColorViewFunctionStop();
      break;
    case READ:
      PingEchoFunctionRead();
      break;
    default:
      break;
  }
  // Read parameters
}

void PingEchoFunctionSetup() {
  DEBUG_PRINTLN("#In the PingEchpFunctionSetup");

  int timeBetweenSamples = 100;
  // ID, PE01,SETUP,<timeBetweenSamples:unit16>[,<medianIterations:uint16 = 1>][,<maxDistance:uint16 = 200>][,<unit:string = cm>]

  //Configures the PingEcho Module
  //<timeBetweenSamples:unit16>    - Time between two measurements in milliseconds
  pePoint.TimeBetweenSamples = XSERIAL.parseInt();
  DEBUG_PRINT("#parameters are ");
  DEBUG_PRINT(" time = ");
  DEBUG_PRINT(pePoint.TimeBetweenSamples);

  // [,<medianIterations:uint16 = 1>]
  if (XSERIAL.peek() == 10 && XSERIAL.find(10)) {
    DEBUG_PRINT(" ; medianIterations<default>= ");
    pePoint.medianIterations = 1;
    DEBUG_PRINTLN(pePoint.medianIterations);
    return; // no more optional params
  }
  pePoint.medianIterations = XSERIAL.parseInt();
  DEBUG_PRINT("# ; medianIterations= ");
  DEBUG_PRINTLN(pePoint.medianIterations);

  //[,<maxDistance:uint16 = 200>]

  if (XSERIAL.peek() == 10 && XSERIAL.find(10)) {
    DEBUG_PRINT("# ; maxDistance<default>= ");
    pePoint.maxDistance = MAX_DISTANCE;
    DEBUG_PRINTLN(pePoint.maxDistance);
    return; // no more optional params
  }
  pePoint.maxDistance = XSERIAL.parseInt();
  DEBUG_PRINT("# ; maxDistance= ");
  DEBUG_PRINTLN(pePoint.maxDistance);

  // <unit:uint16= cm>]
  if (XSERIAL.peek() == 10 && XSERIAL.find(10)) {
    DEBUG_PRINT("# ; unit<default>= ");
    pePoint.unit = 1;
    DEBUG_PRINTLN(pePoint.unit);
    return; // no more optional params
  }
  pePoint.unit = XSERIAL.parseInt();
  DEBUG_PRINT("# ; unit= ");
  DEBUG_PRINTLN(pePoint.unit);


}

void PingEchoFunctionRead() {

  DEBUG_PRINTLN("#In the PingEchoFunctionRead");


#ifdef PE_PRESENT
  unsigned int uS = sonar.ping_median(pePoint.medianIterations);
#else
  unsigned int uS = random(10000000);
#endif

  pePoint.ReadTimestamp = millis();
  switch (pePoint.unit) {
    case 2:
      pePoint.distance = sonar.convert_in(uS);
      break;
    case 3:
      pePoint.distance = uS;
      break;
    default:
      pePoint.distance = sonar.convert_cm(uS);
      break;
  }

  PingEchoFunctionR1();
}

void PingEchoFunctionR1() {
  // ID, PE01,R1,<timestamp:unit16>,<distance:uint16>
  XSERIAL.print(pePoint.ID); XSERIAL.print(", ");
  XSERIAL.print(PE01);  XSERIAL.print(", ");
  XSERIAL.print(R1);  XSERIAL.print(", ");
  XSERIAL.print(pePoint.ReadTimestamp);  XSERIAL.print(", ");
  XSERIAL.println(pePoint.distance);
  XSERIAL.flush();
}


// -------- LEDFunciton ------------
#define LED1PIN  8
#define LED2PIN  9
struct LEDFunctionConfig
{
  int ID; // The ID
  int state;
  int pin;

  LEDFunctionConfig(int theID, int ledPin) {
    ID = theID;
    pin = ledPin;
  }
  void Reset()
  {
    // Set default values in case there is nothing store
    state = LOW;
  }
};

LEDFunctionConfig LEDPoint1(1, LED1PIN);
LEDFunctionConfig LEDPoint2(2, LED2PIN);

void LEDFunction( int id, int command) {
  DEBUG_PRINT("#In the LEDFunction with function ");
  DEBUG_PRINTLN(command);

  DEBUG_PRINT("#and with ID ");
  DEBUG_PRINTLN(id);

  switch (command) {
    case SET:
      LEDFunctionSet(id);
      break;
    default:
      break;
  }
}

void LEDFunctionSet(int id) {
  int oldState;
  DEBUG_PRINTLN("#In the LEDFunctionSetup");
  DEBUG_PRINT("# with id");
  DEBUG_PRINTLN(id);

  int state = XSERIAL.parseInt();
  DEBUG_PRINT("#parameters are ");
  DEBUG_PRINT(" state = ");
  DEBUG_PRINTLN(state);

  if (id == LEDPoint1.ID) {
    oldState = LEDPoint1.state ;
    LEDPoint1.state = state;
    digitalWrite(LEDPoint1.pin, state);
  } else if (id == LEDPoint2.ID) {
    oldState = LEDPoint2.state ;
    LEDPoint2.state = state;
    digitalWrite(LEDPoint2.pin, state);
  }  else {
    DEBUG_PRINTLN("# DO NOT KNOW THIS ID");
    return;
  }
  DEBUG_PRINT("#Switching from ");
  DEBUG_PRINT(oldState) ;
  DEBUG_PRINT(" to ");
  DEBUG_PRINTLN(state);
}

// -------- RelayFunciton ------------

struct RelayFunctionConfig
{
  int ID; // The ID
  int state;
  int pin;

  RelayFunctionConfig(int theID, int rPin) {
    ID = theID;
    pin = rPin;
  }
  void Reset()
  {
    // Set default values in case there is nothing store
    state = HIGH;
  }
};

RelayFunctionConfig RelayPoint1(5, RELAY1PIN);
RelayFunctionConfig RelayPoint2(6, RELAY2PIN);

void RelayFunction(int id, int command) {
  DEBUG_PRINT("#In the RelayFunction with function ");
  DEBUG_PRINTLN(command);

  DEBUG_PRINT("#and with ID ");
  DEBUG_PRINTLN(id);

  switch (command) {
    case SET:
      RelayFunctionSet(id);
      break;
    default:
      break;
  }
}

void RelayFunctionSet(int id) {
  int oldState ;
  DEBUG_PRINTLN("#In the RelayFunctionSet");
  DEBUG_PRINT("# with id");
  DEBUG_PRINTLN(id);

  int state = XSERIAL.parseInt();
  DEBUG_PRINT("#parameters are ");
  DEBUG_PRINT(" state = ");
  DEBUG_PRINTLN(state);

  if (id == RelayPoint1.ID) {
    oldState = RelayPoint1.state ;
    RelayPoint1.state = state;
    digitalWrite(RelayPoint1.pin, state);
  } else if (id == RelayPoint2.ID) {
    oldState = RelayPoint2.state ;
    RelayPoint2.state = state;
    digitalWrite(RelayPoint2.pin, state);
  } else {
    DEBUG_PRINTLN("# DO NOT KNOW THIS ID");
    return;
  }
  DEBUG_PRINT("#Switching from ");
  DEBUG_PRINT(oldState) ;
  DEBUG_PRINT(" to ");
  DEBUG_PRINTLN(state);

}


// -------- MotorFunction ------------
struct MotorFunctionConfig
{
  int ID; // The ID
  int velocity; // +% of Max for right, -% of Max for left

  void Reset()
  {
    // Set default values in case there is nothing
    // stored in the eeprom yet.
    velocity = 0;
  }
};

MotorFunctionConfig mPoint;

void MotorFunction( int id, int command) {
  mPoint.ID = id;
  DEBUG_PRINT("#In the MotorFunction with function ");
  DEBUG_PRINTLN(command);

  switch (command) {
    case SET:
      MotorFunctionSet();
      break;
    default:
      break;
  }
}

void MotorFunctionSet() {
  DEBUG_PRINTLN("#In the MotorFunctionSetup");

  //ID, MM01, SET, <velocity:uint16>
  //Configures the Motor Function

  mPoint.velocity = XSERIAL.parseInt();
  DEBUG_PRINT("#parameters are ");
  DEBUG_PRINT(" velocity = ");
  DEBUG_PRINTLN(mPoint.velocity);

  int  vel = (abs(mPoint.velocity) % 100) * 2.55;
  int reverse = mPoint.velocity > 0 ? 1 : 0;

#ifdef MOTOR_PRESENT
  analogWrite(enablePin, vel);
  digitalWrite(in1Pin, ! reverse);
  digitalWrite(in2Pin, reverse);
#endif
  DEBUG_PRINT("#Rotating with a velocity of ");
  DEBUG_PRINTLN(vel);
  DEBUG_PRINT("#Into the direction of ");
  DEBUG_PRINTLN(reverse);
}

// -------- LightSensor ------------
void LightSensorFunction( int id, int command){
  //TODO Debug output
  
  int value = analogRead(LS1PIN);
  
  // ID, LS01, R1, <brightness:uint16>
  XSERIAL.print(id); XSERIAL.print(", ");
  XSERIAL.print(LS01);  XSERIAL.print(", ");
  XSERIAL.print(R1);  XSERIAL.print(", ");
  XSERIAL.println(value);
  XSERIAL.flush();
}

// ----------------- GENERAL CONFIG ------------
void establishContact() {
  while (XSERIAL.available() <= 0) {
    XSERIAL.print("0,0,0,");   // send an initial string
    XSERIAL.println(millis());
    delay(1000);
  }
}

void processCleanUp () {
  // If we are here rest ist rubbish
  int i;
  DEBUG_PRINTLN("#CleanUp");
  while (XSERIAL.available() > 0) {
    i = XSERIAL.read();
  }
}

