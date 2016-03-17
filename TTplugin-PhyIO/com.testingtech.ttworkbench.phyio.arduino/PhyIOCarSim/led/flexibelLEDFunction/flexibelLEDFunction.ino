//#include <SoftwareSerial.h>
//#include <Time.h>

#define XSERIAL Serial // mySerial // Serial // to LEDSwitch between Bluetooth and USB XSERIAL connection for communication

#define CV01   10
#define PE01   20
#define LED01  30
#define REL01  40
#define MM01   50

#define LS01   70

#define SETUP 1
#define SET 1
#define BLINK 2
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


// definitionen der Funktionen:

void processCleanUp ();
void LEDshouldBlink();
void LEDFunction( int id, int command);
void LEDFunctionSetup();


void setup() {
  XSERIAL.begin(9600, SERIAL_8N1); // Open serial monitor at 9600 baud
  while (!XSERIAL) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  LEDFunctionSetup();  // initializes the led's
                        // there pins and number may be changed in the led section

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
  LEDshouldBlink();
}


// -------- LEDFunciton ------------

/*
 * The following five lines set up the pins and id for the diods
 * to change the code to your wiring, just change the following five lines
 *
 */

#define Led1Pin 10 // green
#define Led2Pin 11 // yellow
#define Led3Pin 12 // red
#define LedCount 3
int LEDPins[] = {Led1Pin, Led2Pin, Led3Pin};

/* the following is an example for just one led on pin 5
 *

#define LedPin 5
#define LedCount 1
int LEDPins[] = {LedPin}

 *
 * the define of the pin slot isn't used anywhere else, so you may change the name
 * in the define and array definition
 *
 */


typedef struct LEDFunctionConfig{
  // Struct that simulates and controlls the corresponding diods

  int ID; // The ID
  int state;
  int pin;
  bool is_toggling;
  long toggling_time;
  unsigned long previousMillis;

  LEDFunctionConfig() {
    pinMode(pin, OUTPUT);
    previousMillis = 0;
    toggling_time = 0;
    is_toggling = false;
    state = LOW;
  }

  void initialize(int theID, int ledPin){
    ID = theID;
    pin = ledPin;
  }

  void Reset()
  {
    // Set default values in case there is nothing store
    state = LOW;
    is_toggling = false;
    toggling_time = 0;
    previousMillis = 0;
  }
};

// Definitions of the needed functions
void LEDFunctionSet(int id, int state);
void LEDFunctionBlink(int id);
void LEDFunctionStop(int id);
void LEDToggling(LEDFunctionConfig led);
void LEDSwitch(LEDFunctionConfig led, int state);
LEDFunctionConfig Dioden[LedCount];  // <-- array von Stracts scheint falsch zu sein

unsigned long currentMillis; // keeps track of time to signal if blinking is needed


void LEDFunctionSetup(){

  DEBUG_PRINT("\n# In the LEDFunctionSetup");

  for(int i = 0; i < LedCount; i++){
    Dioden[i].initialize((i+1), LEDPins[i]);
  }
}


void LEDFunction( int id, int command) {
  // chooses the function for the led
  
  DEBUG_PRINT("\n# In the LEDFunction with function : '");
  DEBUG_PRINTLN(command);

  DEBUG_PRINT("# and with ID: ");
  DEBUG_PRINTLN(id);

  switch (command) {
    case SET:
      LEDFunctionSet(id, XSERIAL.parseInt());
      break;
    case BLINK:
      LEDFunctionBlink(id);
      break;
    case STOP:
      LEDFunctionSet(id, 0);
      break;
    default:
      break;
  }
}


void LEDFunctionBlink(int id) {

  // tells the given led, that it now has to blink

  DEBUG_PRINT("\n# In the LEDFunctionBlink");
  DEBUG_PRINT("\n# With ID: ");
  DEBUG_PRINTLN(id);

  float freq = XSERIAL.parseFloat();

  DEBUG_PRINT("\n# and with frequency: ");
  DEBUG_PRINTLN(freq);

  Dioden[(id - 1)].is_toggling = true;
  Dioden[(id - 1)].toggling_time = (1000/(freq*2));

}


void LEDshouldBlink(){
    // checks if a led has to blink

  // no debug print here, as it would clutter the readybillity
  //on the serial monitor

  currentMillis = millis();

  for(int i = 0; i < LedCount; i++){
    if(Dioden[i].is_toggling){LEDToggling(&Dioden[i]);}
  }

}


void LEDToggling(struct LEDFunctionConfig *led){
  // toggles between the on and off state of the given led

  // no debug print here, as it would clutter the readybillity
  //on the serial monitor

  if((currentMillis - led->previousMillis) >= led->toggling_time){
    led->previousMillis = currentMillis;
    if (led->state == LOW){
      LEDSwitch(led, 1);
        
    } else {
      LEDSwitch(led, 0);
    }
  }
}


void LEDFunctionSet(int id, int state) {
  // sets the the corresponding led to the given state
  // and turns off toggling

  DEBUG_PRINT("\n# In the LEDFunctionSet");
  DEBUG_PRINT("\n# With ID: ");
  DEBUG_PRINTLN(id);

  DEBUG_PRINT("\n# Setting the led to state: ");
  DEBUG_PRINTLN(state);


  Dioden[(id - 1)].is_toggling = false;
  LEDSwitch(&Dioden[(id - 1)], state);
}


void LEDSwitch(LEDFunctionConfig *led, int state){
  // switches the given led into the given state

  if(state == 0){
    led->state = LOW;
    digitalWrite(led->pin, LOW);
        
  } else {
    led->state = HIGH;
    digitalWrite(led->pin, HIGH);
  }
}


// ----------------- GENERAL CONFIG ------------

void processCleanUp () {
  // If we are here rest ist rubbish
  int i;
  DEBUG_PRINTLN("#CleanUp");
  while (XSERIAL.available() > 0) {
    i = XSERIAL.read();
  }
}


