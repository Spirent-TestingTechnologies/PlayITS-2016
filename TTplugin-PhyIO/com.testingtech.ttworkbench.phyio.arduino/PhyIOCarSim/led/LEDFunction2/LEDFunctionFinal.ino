//#include <SoftwareSerial.h>
//#include <Time.h>

#define XSERIAL Serial // mySerial // Serial // to switch between Bluetooth and USB XSERIAL connection for communication

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
void toggle();
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
  //if (millis() >= readTimer) {
    //readTimer += readSpeed;      // Set the next ping time.
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
    toggle();
  //}
}


// -------- LEDFunciton ------------
#define Led1Pin 10 // green
#define Led2Pin 11 // yellow
#define Led3Pin 12 // red

struct LEDFunctionConfig{
  
  int ID; // The ID
  int state;
  int pin;
  bool is_toggling;
  long toggle_time;
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
    toggle_time = 0;
    previousMillis = 0;
  }
};

LEDFunctionConfig LED1(1, Led1Pin);
LEDFunctionConfig LED2(2, Led2Pin);
LEDFunctionConfig LED3(3, Led3Pin);

void LEDFunctionSet(int id, int state);
void LEDtoggleblinken(int id);
void LEDFunctionStop(int id);
void toggling(LEDFunctionConfig led);
void Switch(LEDFunctionConfig led, int state);

unsigned long currentMillis;

void LEDFunction( int id, int command) {
  // wählt die Funktion aus, die mit der LED ausgeführt werden soll
  
  DEBUG_PRINT("\n#In the LEDFunction with function : '");
  DEBUG_PRINTLN(command);

  DEBUG_PRINT("' and with ID: ");
  DEBUG_PRINTLN(id);

  switch (command) {
    case SET:{
      int state = XSERIAL.parseInt();
      LEDFunctionSet(id, state);
      break;
      }
    case BLINK:
      LEDtoggleblinken(id);
      break;
    case STOP:
      LEDFunctionStop(id);
      break;
    default:
      break;
  }
}


void LEDtoggleblinken(int id) {
  // Gibt weiter, welche LED jetzt blinken soll
  float freq = XSERIAL.parseFloat();

  if(id == LED1.ID){
    LED1.is_toggling = true;
    LED1.toggle_time = (1000/(freq*2));
  }
    
  if(id == LED2.ID){
    LED2.is_toggling = true;
    LED2.toggle_time = (1000/(freq*2));  
  }
  
  if(id == LED3.ID){;
    LED3.is_toggling = true;
    LED3.toggle_time = (1000/(freq*2));
  }
}


void toggle(){
  // überprüft, ob eine der LED gerade blinken muss und schlatet je nach Stand um
  currentMillis = millis();

  if(LED1.is_toggling){toggling(&LED1);}
  if(LED2.is_toggling){toggling(&LED2);}
  if(LED3.is_toggling){toggling(&LED3);}  
}


void toggling(struct LEDFunctionConfig *led){
  
  if((currentMillis - led->previousMillis) >= led->toggle_time){
      led->previousMillis = currentMillis;
      if (led->state == LOW){
        Switch(led, 1);
        
      } else {
        Switch(led, 0);
      }
  }
}


void LEDFunctionSet(int id, int state) {
  // Schaltet die jeweilige LED basierend auf ihrer ID an oder aus
  
  if (id == LED1.ID) {
    Switch(&LED1, state);
    
  } else if (id == LED2.ID) {
    Switch(&LED2, state);
    
  } else if (id == LED3.ID) {
    Switch(&LED3, state);
  }
}


void Switch(struct LEDFunctionConfig *led, int state){
  // schlatet den übergeben pin mit 1 an und mit 0 aus
  // hilfsfunktion zum Wechseln der Zustände der LED
  
  if(state == 0){
    led->state = LOW;
    digitalWrite(led->pin, LOW);
        
  } else {
    led->state = HIGH;
    digitalWrite(led->pin, HIGH);
  }
}


void LEDFunctionStop(int id){  
  
  if (id == LED1.ID) {
    LED1.is_toggling = false;
    
  } else if (id == LED2.ID) {
    LED2.is_toggling = false;
    
  } else if (id == LED3.ID) {    
    LED3.is_toggling = false;
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


