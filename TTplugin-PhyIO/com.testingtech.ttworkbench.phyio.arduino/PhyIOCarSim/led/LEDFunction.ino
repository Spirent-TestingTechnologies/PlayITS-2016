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


// definitionen der funktionen
void establishContact();
void processCleanUp ();
void LEDshouldBlink();
void LEDFunction( int id, int command);


void setup() {
  XSERIAL.begin(9600, SERIAL_8N1); // Open serial monitor at 9600 baud
  while (!XSERIAL) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
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
  LEDshouldBlink();
}


// -------- LEDFunciton ------------
#define Led1Pin 10 // green
#define Led2Pin 11 // yellow
#define Led3Pin 12 // red



struct LEDFunctionConfig{
  // Struct that simulates and controlls the corresponding diods
  
  int ID; // The ID
  int state;
  int pin;
  bool is_toggling;
  long toggling_time;
  unsigned long previousMillis;

  LEDFunctionConfig(int theID, int ledPin) {
    ID = theID;
    pin = ledPin;
    pinMode(pin, OUTPUT);
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

LEDFunctionConfig LED1(1, Led1Pin); // green
LEDFunctionConfig LED2(2, Led2Pin); // yellow
LEDFunctionConfig LED3(3, Led3Pin); // red

// Definitions of the needed functions
void LEDFunctionSet(int id, int state);
void LEDFunctionBlink(int id);
void LEDFunctionStop(int id);
void toggling(LEDFunctionConfig led);
void LEDSwitch(LEDFunctionConfig led, int state);

unsigned long currentMillis; // keeps track of time to signal if blinking is needed

void LEDFunction( int id, int command) {
  // chooses the function for the led
  
  DEBUG_PRINT("\n#In the LEDFunction with function : '");
  DEBUG_PRINTLN(command);

  DEBUG_PRINT("' and with ID: ");
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
  float freq = XSERIAL.parseFloat();

  switch (id){
    case 1:
      LED1.is_toggling = true;
      LED1.toggling_time = (1000/(freq*2));
      break;
      
    case 2:
      LED2.is_toggling = true;
      LED2.toggling_time = (1000/(freq*2));  
      break;
    
    case 3:
      LED3.is_toggling = true;
      LED3.toggling_time = (1000/(freq*2));
      break;
  }
}


void LEDshouldBlink(){
  // checks if a led has to blink
  currentMillis = millis();

  if(LED1.is_toggling){toggling(&LED1);}
  if(LED2.is_toggling){toggling(&LED2);}
  if(LED3.is_toggling){toggling(&LED3);}  
}


void toggling(struct LEDFunctionConfig *led){
  // toggles between the on and off state of the given led
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
  switch (id){
    case 1:
    LED1.is_toggling = false;
    LEDSwitch(&LED1, state);
    break;
    
    case 2: 
    LED2.is_toggling = false;
    LEDSwitch(&LED2, state);
    break;
    
    case 3: 
    LED3.is_toggling = false;
    LEDSwitch(&LED3, state);
    break;
  }
}


void LEDSwitch(struct LEDFunctionConfig *led, int state){
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


