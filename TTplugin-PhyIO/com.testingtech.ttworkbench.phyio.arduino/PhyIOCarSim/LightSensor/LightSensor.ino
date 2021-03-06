#include <SoftwareSerial.h>
#include <Wire.h>
//#include "Adafruit_TCS34725.h"
//#include <NewPing.h>
//#include <Time.h>
//#include <IRremote.h>
#include <math.h>

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

#define cvf_ledpin 4

//#define PE_PRESENT 1
#define TRIGGER_PIN 12
#define ECHO_PIN 11
#define MAX_DISTANCE 100


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

///////////defnition der Funktionen////////////////

void LightSensorFunction( int id, int command);
void LightFunctionPrint(int id,int value,unsigned long time);
void LightFunctionPrint2(int id,float value,unsigned long time);
void LightFunctionStart(int id,int lightValue,int lightDiff);
void LightFunctionFrequenz(int id,int top,int down,boolean upOrDown,unsigned long startTime,unsigned long endTime);
boolean checkError(unsigned long timeDiff,unsigned long timeDiffNow);
float getFrequency(unsigned long time,int counter);
boolean readInput(float frequency,boolean error);

//////////////////////////////////////////////////

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
        //  ColorViewFunction(id, command);
          break;
        case PE01:
         // PingEchoFunction(id, command);
          break;
        case LED01:
         // LEDFunction(id, command);
          break;
        case REL01:
         // RelayFunction(id, command);
          break;
        case MM01:
         // MotorFunction(id, command);
          break;
        case LS01:
          LightSensorFunction(id, command);
          break;
      }
      processCleanUp();
    }
  }
}
 

// -------- LightSensor ------------
void LightSensorFunction( int id, int command){
  int variable = XSERIAL.parseInt();
  
  switch(command){
    case READ:{
      if(variable == 0){
        LightFunctionPrint(id,analogRead(LS1PIN),millis());
      }
      else if(variable == 1){
        LightFunctionPrint2(id,0.0,millis());
      }
      break;
    }
    case START:{
      int lightValue = analogRead(LS1PIN);
      LightFunctionStart(id,lightValue,variable);
      break;
    }
    case STOP:{
      LightFunctionPrint(id,0.0,millis());
      break;
    }
    default:
      break;
  }
  // ID, LS01, R1, <brightness:int>, <timestamp:long>
}
void LightFunctionPrint(int id,int value,unsigned long time){
    //Protokoll zum Uebermitteln der Helligkeit
    XSERIAL.print(id); XSERIAL.print(", ");
    XSERIAL.print(LS01);  XSERIAL.print(", ");
    XSERIAL.print(R1);  XSERIAL.print(", ");
    XSERIAL.print(value);  XSERIAL.print(", ");
    XSERIAL.println(time);
    XSERIAL.flush();
  }
  void LightFunctionPrint2(int id,float value,unsigned long time){
    //Protokoll zum Uebermitteln der Frequentz
    XSERIAL.print(id); XSERIAL.print(", ");
    XSERIAL.print(LS01);  XSERIAL.print(", ");
    XSERIAL.print(R1);  XSERIAL.print(", ");
    XSERIAL.print(value);  XSERIAL.print(", ");
    XSERIAL.println(time);
    XSERIAL.flush();
  }
  void LightFunctionStart(int id,int lightValue,int lightDiff){
    //wartet bis sich die Helligkeit um lightDiff geändert hat und beginnt zu messen
    int lightValueNow,top,down;
    //////////Schranken setzen/////////////
    if(lightValue - lightDiff < 0){
      down = 0;
    }
    else {
      down = lightValue - lightDiff;
    }
    top = lightValue + lightDiff;
    ///////////////////////////////////////
    while(true){
      lightValueNow = analogRead(LS1PIN);
      if(lightValueNow>=top){
        LightFunctionFrequenz(id,top,lightValue,true,millis(),millis());
        return;
      }
      else if(lightValueNow<=down){
        LightFunctionFrequenz(id,lightValue,down,false,millis(),millis());
        return;
      }
      if(Serial.available() > 0){
         if(readInput(0.0,false)==true){
          return;  
         }
      }
    }

  
  }
 void LightFunctionFrequenz(int id,int top,int down,boolean upOrDown,unsigned long startTime,unsigned long endTime){
  //zaehlt die Anzahl der Wechsel und überprüft ob die Wechsel den gleichen zeitlichen Abstand haben
   boolean error=false;
   boolean inPut = false;
   unsigned long timeDiff= 0;
   int counter = 1;
   int lightValueNow;
   float frequency;
   
   while(true){
    lightValueNow = analogRead(LS1PIN);
    if(lightValueNow>=top && upOrDown == false){
        upOrDown = true;
        counter++;
        if(timeDiff == 0 ){
          endTime = millis();
          timeDiff = endTime - startTime;
        }
        else{
          if(checkError(timeDiff,millis()-endTime)==false && inPut == false){
            error = true;
          }
          endTime = millis();
        }
        inPut = false;
      }
    else if(lightValueNow<=down && upOrDown == true){
        upOrDown = false;
        counter++;
        if(timeDiff == 0 ){
          endTime = millis();
          timeDiff = endTime - startTime;
        }
        else{
          if(checkError(timeDiff,millis()-endTime)==false && inPut == false){
            error = true;
          }
          endTime = millis();
        }
        inPut = false;
      }
    if(Serial.available()> 0 ) {
      inPut = true;
      if(readInput(getFrequency(endTime-startTime,counter),error)==true){
        return;  
      }
      
    }
   }
 }
 
 boolean checkError(unsigned long timeDiff,unsigned long timeDiffNow){
  //überprüft ob die Abstände gleich sind
  if(timeDiffNow > (timeDiff+(timeDiff*0.1))){
    return false;
  }
  else if(timeDiffNow < (timeDiff-(timeDiff*0.1))){
    return false;
  }
  else{
    return true;
  }
 }
 
 float getFrequency(unsigned long time,int counter){
  //berechnet die Frequentz
  return (((float)counter*1000.0)/time)/2.0;
 }

 boolean readInput(float frequency,boolean error){
  //ließt Input und bearbeitet ihn, liefert true wenn die Aufrufende Funktion gestoppt werden muss
  int id, functionType, command,variable;
  //division by zero return -1
  // todo test isnan or isinf
  if(isnan(frequency)){
    frequency = 0.0;
  }
  if(isinf(frequency)){
    frequency = 0.0;
  }
  //////////////////
  if (millis() >= readTimer) {
    readTimer += readSpeed;      // Set the next ping time.
    if (XSERIAL.available() > 0) {
      // Read in loop all data for one line
      id = XSERIAL.parseInt();
      functionType = XSERIAL.parseInt();
      command = XSERIAL.parseInt();
      variable = XSERIAL.parseInt();
      
      switch (functionType) {
        case LS01:
          switch (command){
            case START:{
              LightFunctionStart(id,analogRead(LS1PIN),variable);
              return true;
              break;
            }
            case READ:{
              if(variable == 0){
                LightFunctionPrint(id,analogRead(LS1PIN),millis());
                return false;
              }
              else if(variable == 1 ){
                if(error==false){
                  LightFunctionPrint2(id,frequency,millis());
                }
                else{
                   LightFunctionPrint2(id,0.0,millis());
                }
                return false;
              }
              break;
            }
            case STOP:{
              if(error==false){
                  LightFunctionPrint2(id,frequency,millis());
                }
                else{
                   LightFunctionPrint2(id,0.0,millis());
                }
              return true;
              break;
            }
          }
          break;
      }
      processCleanUp();
    }
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

