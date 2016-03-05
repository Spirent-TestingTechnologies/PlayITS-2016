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
//void establishContact();
void processCleanUp ();
void toggle();
void LEDFunction( int id, int command);


void setup() {
  XSERIAL.begin(9600, SERIAL_8N1); // Open serial monitor at 9600 baud
  while (!XSERIAL) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  // ECHO PING Config in Constructor of SONAR
  // BT / Softserial Config in Constructor of SoftSerial

  //establishContact();  // send a byte to establish contact until receiver responds

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
void LEDFunctionBlink(int id);
void LEDtoggleblinken(int id);
void LEDFunctionStop(int id);
void Blink(int LED, float freq);
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
      //LEDFunctionBlink(id);
      LEDtoggleblinken(id);
      break;
    case STOP:
      LEDFunctionStop(id);
      //LEDFunctionSet(id, 0);
      break;
    default:
      break;
  }
}


void LEDFunctionBlink(int id) {
  // lässt eine LED basierend auf ihrer Id in einer bestimmten Frequenz blinken
  
  DEBUG_PRINTLN("# In the LEDFunctionBlink");
  DEBUG_PRINT("# with id");
  DEBUG_PRINTLN(id);
  
  float freq = XSERIAL.parseFloat();
  DEBUG_PRINT("# with frequency");
  DEBUG_PRINTLN(freq);


  if(id == LED1.ID){Blink(LED1.pin, freq);}
  if(id == LED2.ID){Blink(LED2.pin, freq);}
  if(id == LED3.ID){Blink(LED3.pin, freq);}
}


void Blink(int pin, float freq){
  // lässt die an dem übergebenen Pin angelegte LED in der angegebenen Frequenz blinken
  // (in Herz)
  // Hilfsfunktion von LEDFunctionBlink
  
  int ontime = (int)(1000/(freq*2));

  DEBUG_PRINTLN("# In the Blink sub-function");
  DEBUG_PRINTLN("# The Time Period the LED will be turned on and off again ");
  DEBUG_PRINTLN(ontime);
  
  while(Serial.available() == 0){
    digitalWrite(pin, HIGH);
    delay(ontime);
    digitalWrite(pin, LOW);
    delay(ontime);
  }
}


void LEDtoggleblinken(int id) {
  DEBUG_PRINTLN("# In the LEDFunctionBlink");
  DEBUG_PRINT("# with id");
  DEBUG_PRINTLN(id);
  
  float freq = XSERIAL.parseFloat();
  DEBUG_PRINT("# with frequency");
  DEBUG_PRINTLN(freq);
  DEBUG_PRINTLN((1000/(freq*2)));

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
  //DEBUG_PRINTLN("------ TOGGLING------");
  //DEBUG_PRINTLN("#LED hat jetzt folgenden Zusatand : ");
  //DEBUG_PRINTLN(LED1.state);
  //DEBUG_PRINTLN(LED2.state);
  //DEBUG_PRINTLN(LED3.state);
  
  currentMillis = millis();

  if(LED1.is_toggling){toggling(&LED1);}
  if(LED2.is_toggling){toggling(&LED2);}
  if(LED3.is_toggling){toggling(&LED3);}

  //DEBUG_PRINTLN("------ TOGGLING END------");
  //DEBUG_PRINTLN("#LED hat jetzt folgenden Zusatand : ");
  //DEBUG_PRINTLN(LED1.state);
  //DEBUG_PRINTLN(LED2.state);
  //DEBUG_PRINTLN(LED3.state);
  
}


// vielleicht beim toggling auf die switch funktion verzichten
// vielleicht wird hir lediglich ein Pointer übergeben (was absolut keinen Sinn ergibt)
void toggling(struct LEDFunctionConfig *led){
  DEBUG_PRINTLN("# In toggling() Am Anfang");
  
  DEBUG_PRINTLN("#LED hat jetzt folgenden Zusatand : ");
  DEBUG_PRINTLN(led->state);

  DEBUG_PRINTLN("currentMillis : ");
  DEBUG_PRINTLN(currentMillis);

  DEBUG_PRINTLN("led.previousMillis : ");
  DEBUG_PRINTLN(led->previousMillis);

  DEBUG_PRINTLN("currentMillis - led.previousMillis : ");
  DEBUG_PRINTLN(currentMillis - led->previousMillis);

  DEBUG_PRINTLN("led.toggle_time : ");
  DEBUG_PRINTLN(led->toggle_time);
  
  if((currentMillis - led->previousMillis) >= led->toggle_time){
      DEBUG_PRINTLN("Zeit überschritten muss switchen");
      led->previousMillis = currentMillis;
      if (led->state == LOW){

        DEBUG_PRINTLN("macht LED jetzt an");
        Switch(led, 1);
        
      } else {

        DEBUG_PRINTLN("macht LED jetzt aus");
        Switch(led, 0);
      }
  }
  DEBUG_PRINTLN("# In toggling() Am Ende");
  DEBUG_PRINTLN("#LED hat jetzt folgenden Zusatand : ");
  DEBUG_PRINTLN(led->state);
}


void LEDFunctionSet(int id, int state) {
  // Schaltet die jeweilige LED basierend auf ihrer ID an oder aus
  
  DEBUG_PRINTLN("# In the LEDFunctionSetup");
  DEBUG_PRINT("# with id");
  DEBUG_PRINTLN(id);

  DEBUG_PRINT("#parameters are ");
  DEBUG_PRINT(" state = ");
  DEBUG_PRINTLN(state);
  
  if (id == LED1.ID) {
    DEBUG_PRINT("#LED1 is now in state ");
    Switch(&LED1, state);
    DEBUG_PRINT(LED1.state);
    
  } else if (id == LED2.ID) {
    DEBUG_PRINT("#LED2 is now in state ");
    Switch(&LED2, state);
    DEBUG_PRINT(LED2.state);
    
  } else if (id == LED3.ID) {
    DEBUG_PRINT("#LED3 is now in state ");
    Switch(&LED3, state);
    DEBUG_PRINT(LED3.state);
  }
}


void Switch(struct LEDFunctionConfig *led, int state){
  // schlatet den übergeben pin mit 1 an und mit 0 aus
  // hilfsfunktion für LED FunctionSet
  DEBUG_PRINTLN("# In Switch()");
  DEBUG_PRINTLN("# folgende LED wurde ausgewählt:");
  DEBUG_PRINTLN(led->pin);
  DEBUG_PRINTLN("#led hat derzeit Zusatand : ");
  DEBUG_PRINTLN(led->state);
  DEBUG_PRINTLN("#LED soll folgenden Zusatand annehmen : ");
  DEBUG_PRINTLN(state);
  
  if(state == 0){
    led->state = LOW;
    digitalWrite(led->pin, LOW);
        
  } else {
    led->state = HIGH;
    digitalWrite(led->pin, HIGH);
  }

  DEBUG_PRINTLN("#LED hat jetzt folgenden Zusatand : ");
  DEBUG_PRINTLN(led->state);
}


void LEDFunctionStop(int id){
  // Stoppt das Blinken der LED mit der übergebenen ID
  
  DEBUG_PRINTLN("# In the LEDFunctionStop");
  DEBUG_PRINT("# with id");
  DEBUG_PRINTLN(id);
  
  
  if (id == LED1.ID) {
    DEBUG_PRINT("LED1 toggling status vor dem Stoppen : ");
    DEBUG_PRINT(LED1.is_toggling);
    
    LED1.is_toggling = false;
    
    DEBUG_PRINT("LED1 toggling status nach dem Stoppen : ");
    DEBUG_PRINT(LED1.is_toggling);
    
  } else if (id == LED2.ID) {
    DEBUG_PRINT("LED2 toggling status vor dem Stoppen : ");
    DEBUG_PRINT(LED2.is_toggling);
    
    LED2.is_toggling = false;
    
    DEBUG_PRINT("LED2 toggling status nach dem Stoppen : ");
    DEBUG_PRINT(LED2.is_toggling);
    
  } else if (id == LED3.ID) {
    DEBUG_PRINT("LED3 toggling status vor dem Stoppen : ");
    DEBUG_PRINT(LED3.is_toggling);
    
    LED3.is_toggling = false;
    
    DEBUG_PRINT("LED3 toggling status nach dem Stoppen : ");
    DEBUG_PRINT(LED3.is_toggling);
  }
}


// ----------------- GENERAL CONFIG ------------
/*void establishContact() {
  while (XSERIAL.available() <= 0) {
    XSERIAL.print("0,0,0,");   // send an initial string
    XSERIAL.println(millis());
    delay(1000);
  }
}
*/


void processCleanUp () {
  // If we are here rest ist rubbish
  int i;
  DEBUG_PRINTLN("#CleanUp");
  while (XSERIAL.available() > 0) {
    i = XSERIAL.read();
  }
}


