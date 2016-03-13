#include <SoftwareSerial.h>
#include <Time.h>

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
#define R1 101

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


//#define LEDS_PRESENT 1
//D8+A3 LED bicolor
#define LED1PIN1  8 // HIGH = red
#define LED1PIN2  A3 // HIGH = green
#define LED2PIN   10 // blue



void setup() {
  XSERIAL.begin(9600, SERIAL_8N1); // Open serial monitor at 9600 baud
  while (!XSERIAL) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // LED Config
  pinMode(LED1PIN1, OUTPUT);
  pinMode(LED1PIN2, OUTPUT);
  pinMode(LED2PIN, OUTPUT);


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
          //ColorViewFunction(id, command);
          break;
        case PE01:
          //PingEchoFunction(id, command);
          break;
        case LED01:
          LEDFunction(id, command);
          break;
        case REL01:
          //RelayFunction(id, command);
          break;
        case MM01:
          //MotorFunction(id, command);
          break;
      }
      processCleanUp();
    }
  }
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

