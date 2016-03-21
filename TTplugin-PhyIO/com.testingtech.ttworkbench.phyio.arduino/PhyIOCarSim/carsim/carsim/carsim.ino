#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>
#include <NewPing.h>

#define XSERIAL Serial // mySerial // Serial // to switch between Bluetooth and USB XSERIAL connection for communication

#define CV01   10
#define PE01   20
#define LED01  30
#define REL01  40
#define MM01   50
#define DR01   60
#define LS01   70
#define RF01   80

#define PE02   21
#define TD01   85 // Theft Detection (combination of button, rfid and led)

#define SETUP 1
#define SET 1
#define START 2
#define BLINK 2 
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

//Enable/Disable
//#define BT_PRESENT 1
//#define RFID_PRESENT 1
//#define PE_PRESENT 1


unsigned long readTimer; // holds the next read time;
unsigned int readSpeed = 500; // How frequently are we going to read the serial line
long randNumber; // random number for simulated parts

//Button also known as Door
#define Button1Pin 2
int BUTTON_ENABLED = 0;

//RFID 
unsigned int RFID_ENABLED = 0; // Tells if rfid is enabled for scanning
#define RFID_SS_PIN 10 
#define RFID_RST_PIN 9
MFRC522 mfrc522(RFID_SS_PIN, RFID_RST_PIN); // creates and holds mfrc522 data
  
//Theft Detection
int THEFT_ENABLED = 0; // Enables theft detection

//Ping Echo sensor setup
#define TRIGGER_PIN 6
#define ECHO_PIN  5
#define MAX_DISTANCE 300
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); //NewPing setup
#define PE_UNIT 1


bool pe_enabled = false;
unsigned long peTimer = 0;
#define PE_TIME_BETWEEN_SAMPLES 100 //Ping Frequency
#define PE_POINT_MEDIAN_ITERATIONS  1 //how many iterations used for median


void setup() {
  XSERIAL.begin(9600, SERIAL_8N1); // Open serial monitor at 9600 baud

  while (!XSERIAL) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  establishContact();  // send a byte to establish contact until receiver responds

//Button
#ifndef BT_PRESENT
  DEBUG_PRINTLN("#Simulating Button");
  randomSeed(analogRead(Button1Pin));
#endif

//RFID config
  SPI.begin();
  mfrc522.PCD_Init();
  
#ifndef RFID_PRESENT
  DEBUG_PRINTLN("#Simulating RFID");
  randomSeed(RFID_SS_PIN);
#endif

  //PE
  PingEchoFunctionSetup2();
  
  DEBUG_PRINTLN("#Entered loop");
}

void loop() {
  int id, functionType, command;
  if (millis() >= readTimer) {
    readTimer += readSpeed;      // Set the next ping time.
    
    // Serial Communication
    if (XSERIAL.available() > 0) {
      // Read in loop all data for one line
      id = XSERIAL.parseInt();
      functionType = XSERIAL.parseInt();
      command = XSERIAL.parseInt();

      if(THEFT_ENABLED){
        if(functionType == TD01 && command == STOP){
          TheftDetectionFunction(1,command);
          DEBUG_PRINTLN("#Theft Detection stopped.");
        }else{
          DEBUG_PRINTLN("#Please exit theft mode first.");
        }
       
      }
      else{
        switch (functionType) {
          case CV01:
            DEBUG_PRINTLN("#Not supported.");
            break;
          case PE01:
            DEBUG_PRINTLN("#Not supported. Use distance module");
            break;
          case PE02:
            PingEchoFunction2(id, command);
          break;
          case LED01:
            // LED
            LEDFunction(id, command);
            break;
          case REL01:
            DEBUG_PRINTLN("#Not supported.");
            break;
          case MM01:
            DEBUG_PRINTLN("#Not supported.");
            break;
          case RF01:
            // RFID Communication
            RFIDFunction(id, command);
            break;
          case DR01:
            // Door
            ButtonFunction(id, command);
            break;
         case TD01:
            // Theft Detection 
            TheftDetectionFunction(id, command);
            break;
        }
      }
      processCleanUp();
    }
  }
  ProcessHandling();
}

// -------- DoorFunction -----------
struct ButtonConfig{
  int ID;
  int state;
  int pin;

  ButtonConfig(int cID, int buttonPin){
    ID = cID;
    pin = buttonPin;
    pinMode(pin, INPUT);
  }
};

ButtonConfig Button1(4, Button1Pin);


void ButtonFunction(int id, int command){
  DEBUG_PRINT("#In ButtonFunction with function ");
  DEBUG_PRINTLN(command);
  switch(command){
    case START:
      BUTTON_ENABLED = 1;
      break;
    case STOP:
      BUTTON_ENABLED = 0;
      break;
    default:
      break;
  }
}

// Function returns 1 if the state changed, 0 if not
int ButtonFunctionProcess(struct ButtonConfig *bt){
  int current_state;

#ifdef BT_PRESENT
  current_state = digitalRead(bt->pin);
#else
  // Simulated button
  randNumber = random(100);
  if(randNumber < 1){
    current_state = bt->state^1;
  }else{
    current_state = bt->state;
  }
#endif

  // Only send if state changed
  if(bt->state != current_state){
    bt->state = current_state;
    //ID, DR01, R1, <pushed>
    XSERIAL.print(bt->ID); XSERIAL.print(",");
    XSERIAL.print(DR01); XSERIAL.print(",");
    XSERIAL.print(R1); XSERIAL.print(",");
    XSERIAL.println(bt->state);

    XSERIAL.flush();
    return 1;
  }
  return 0;
}

// -------- RFIDFunction -----------
// Needs to be refactored to support multiple rfid sensors using a rfid struct
// ID Karte: 194, 96, 196, 169 (DEZ)
// ID Chip: 4, 226, 92, 235 (DEZ)

int rfid_format = DEC;
int rfid_tag[4]; // Sollte noch in Struct umgewandelt werden
// bin = 2; dec = 10; oct = 8; hex = 16;

void RFIDFunctionTagClear(){
  for(int i = 0; i < 4; i++){
    rfid_tag[i] = 0;
  }
}

void RFIDFunction(int id, int command){
  DEBUG_PRINT("#In RFIDFunction with function ");
  DEBUG_PRINTLN(command);
  switch(command){
    case SETUP:
      RFIDFunctionTagClear();
      RFIDFunctionSetup();
      break;
    case START:
      RFID_ENABLED = 1; //start scanning for rfid card
      break;
    case STOP:
      RFID_ENABLED = 0; //stop scanning for rfid card
      RFIDFunctionTagClear();
      break;
    default:
      break;
  }
  
}

void RFIDFunctionSetup(){
  // ID, RF01, SETUP, <tag1>, <tag2>, <tag3>, <tag4>
  DEBUG_PRINTLN("#In RFIDFunctionSetup... ");
  DEBUG_PRINT("#Tag wird gelesen: ");
  for(int i = 0; i < 4; i++){
     rfid_tag[i] = XSERIAL.parseInt();
     DEBUG_PRINT(rfid_tag[i]); DEBUG_PRINT(" ");
  }
  DEBUG_PRINTLN("Tag erfolgreich gesetzt.");  
}

// Function returns 1 if the rfid read is matching the expected one, 0 if it does not match
// and only if in simulating mode it can return -1 when no simulated card was found nearby
int RFIDFunctionProcess(int id){
    int rfid_tag_tmp[4];

#ifdef RFID_PRESENT    
    if(!mfrc522.PICC_ReadCardSerial()){
      return -1; // nothing to read
    }

    for (byte i = 0; i < mfrc522.uid.size; i++){
      rfid_tag_tmp[i] = (int) mfrc522.uid.uidByte[i]; 
    }
#else
    randNumber = random(100);
    if(randNumber >= 2){
      return -1; // nothing to read
    }
    // Generate random rfid uid
    for (byte i = 0; i < 4; i++){
      if(randNumber == 0){
        rfid_tag_tmp[i] = random(256); 
      }else{
        rfid_tag_tmp[i] = rfid_tag[i];
      }
    }
#endif

    unsigned long time = millis();
    
    // ID, RF01, R1, <tag1:int>, <tag2:int>, <tag3:int>, <tag4:int> <timestamp:long>
    XSERIAL.print(3); XSERIAL.print(", ");
    XSERIAL.print(RF01); XSERIAL.print(", ");
    XSERIAL.print(R1); XSERIAL.print(", ");
      XSERIAL.print(time); XSERIAL.print(", ");
    for(int i = 0; i < 4; i++){
#ifdef RFID_PRESENT
      XSERIAL.print(mfrc522.uid.uidByte[i], rfid_format);
#else
      XSERIAL.print(rfid_tag_tmp[i], rfid_format);
#endif
      if(i < 3){
        XSERIAL.print(", ");
      }
    }
    
    XSERIAL.println("");
    XSERIAL.flush();

     if(rfid_tag[0] != 0 && rfid_tag[1] != 0 && rfid_tag[2] != 0 && rfid_tag[3] != 0){
      for(int i = 0; i < 4; i++){
        if(rfid_tag[i] != rfid_tag_tmp[i]){
          DEBUG_PRINT("#Tags stimmen nicht überein: ");
          DEBUG_PRINT(rfid_tag[i]); DEBUG_PRINT(" != ");
          DEBUG_PRINTLN(rfid_tag_tmp[i]);
          return 0;
        }
      }
      return 1;
    }
    return 0;

}


// -------- LEDFunciton ------------
#define Led1Pin 3 // pin for led

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

LEDFunctionConfig LED1(1, Led1Pin);

// Definitions of the needed functions
void LEDFunctionSet(int id, int state);
void LEDFunctionBlink(int id);
void LEDFunctionStop(int id);
void LEDToggling(LEDFunctionConfig led);
void LEDSwitch(LEDFunctionConfig led, int state);

unsigned long currentMillis; // keeps track of time to signal if blinking is needed

void LEDFunction( int id, int command) {
  // chooses the function for the led
  
  DEBUG_PRINT("#In the LEDFunction with function : '");
  DEBUG_PRINT(command);

  DEBUG_PRINT("' and with ID: ");
  DEBUG_PRINTLN(id);

  switch (command) {
    case SET:
      LEDFunctionSet(id, XSERIAL.parseInt());
      break;
    case BLINK:
      LEDFunctionBlink(id, XSERIAL.parseFloat());
      break;
    case STOP:
      LEDFunctionSet(id, 0);
      break;
    default:
      break;
  }
}


// LED should be refactored
void LEDFunctionBlink(int id, float freq) {
  // tells the given led, that it now has to blink
  //float freq = XSERIAL.parseFloat();

  switch (id){
    case 1:
      LED1.is_toggling = true;
      LED1.toggling_time = (1000/(freq*2));
      break;
  }
}

// 
void LEDshouldBlink(){
  // checks if a led has to blink
  currentMillis = millis();
  if(LED1.is_toggling){LEDToggling(&LED1);} 
}

// Function renamed toggling -> LEDToggling
void LEDToggling(struct LEDFunctionConfig *led){
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

// Function needs to be recoded as more generic
void LEDFunctionSet(int id, int state) {
  // sets the the corresponding led to the given state
  // and turns off toggling
  switch (id){
    case 1:
    LED1.is_toggling = false;
    LEDSwitch(&LED1, state);
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

// ----------------- Theft Detection -----------

struct TheftDetectionConfig{
  int ID;
  int bt_changed;
  unsigned long bt_time;
  int rfid_matched;
  //LEDFunctionConfig* led;
  //ButtonConfig* button;
  //RFIDConfig* struct

  TheftDetectionConfig(int cID){
    ID = cID;
    bt_changed = 0;
    bt_time = 0;
    rfid_matched = 0;
    //led = &cLED;
    //button = &cButton;
  }
};

TheftDetectionConfig Theft01(6);

//<ID>, <TD01>, START ,<int: tag1>,<int: tag2>,<int: tag3>,<int: tag4>
void TheftDetectionFunction(int id, int command){
  DEBUG_PRINT("#In the TheftDetectionFunction with function : '");
  DEBUG_PRINT(command);

  DEBUG_PRINT("' and with ID: ");
  DEBUG_PRINTLN(id);

  switch (command) {
    case START:
      RFIDFunctionSetup(); // Read and set rfid tag from serial
      RFID_ENABLED = 1;
      BUTTON_ENABLED = 1; // Enables button
      THEFT_ENABLED = 1;
      break;
    case STOP:
      THEFT_ENABLED = 0; // Disable Theft Mode
      BUTTON_ENABLED = 0; // Disable Button
      LEDFunctionSet(1,0); // Turn off LED
      RFID_ENABLED = 0;
      
      break;
    default:
      break;
 
  }
}

// ----------------- Ping Echo -----------------
struct PingEchoFunctionConfig
{
  int ID; // The ID
  unsigned int TimeBetweenSamples;
  unsigned long ReadTimestamp;
  bool enabled = false;
  
  int medianIterations;
  int maxDistance;
  int unit;
  long int distance ; // value depends on the unit set

  void Reset()
  {
    // Set default values in case there is nothing
    // stored in the eeprom yet.
    TimeBetweenSamples = PE_TIME_BETWEEN_SAMPLES;
    medianIterations = 1;
    maxDistance = MAX_DISTANCE;
    unit = 1;
    distance = 0;
  }
};

PingEchoFunctionConfig pePoint;

void PingEchoFunction2( int id, int command) {
  pePoint.ID = id;
  DEBUG_PRINT("#In the PingEchoFunction with function ");
  DEBUG_PRINTLN(command);

  switch (command) {
    case SETUP:
      PingEchoFunctionSetup2();
      break;
    case START:
      PingEchoFunctionStart2();
      break;
    case STOP:
      PingEchoFunctionStop2();
      break;
    case READ:
      PingEchoFunctionRead2();
      break;
    default:
      break;
  }
  // Read parameters
}

void PingEchoFunctionSetup2() {
  DEBUG_PRINTLN("#In the PingEchpFunctionSetup");
  // ID, PE01,SETUP,<timeBetweenSamples:unit16>[,<medianIterations:uint16 = 1>][,<maxDistance:uint16 = 200>][,<unit:string = cm>]
  //Configures the PingEcho Module - Hardcoded for testcases
  
  pePoint.TimeBetweenSamples = PE_TIME_BETWEEN_SAMPLES;
  DEBUG_PRINT("#parameters are ");
  DEBUG_PRINT(" time = ");
  DEBUG_PRINT(pePoint.TimeBetweenSamples);

  // [,<medianIterations:uint16 = 1>]
  pePoint.medianIterations = PE_POINT_MEDIAN_ITERATIONS;
  DEBUG_PRINT("# ; medianIterations= ");
  DEBUG_PRINTLN(pePoint.medianIterations);

  //[,<maxDistance:uint16 = 200>]
  pePoint.maxDistance = MAX_DISTANCE;
  DEBUG_PRINT("# ; maxDistance= ");
  DEBUG_PRINTLN(pePoint.maxDistance);

  // <unit:uint16= cm>]
  pePoint.unit = PE_UNIT;
  DEBUG_PRINT("# ; unit= ");
  DEBUG_PRINTLN(pePoint.unit);
}

void PingEchoFunctionStart2(){
  #ifdef DEBUG
  DEBUG_PRINTLN("#Started constanst distance measuring.");
  #endif
  pe_enabled = true;
}

void PingEchoFunctionStop2(){
  #ifdef DEBUG
  DEBUG_PRINTLN("#Stopped constanst distance measuring.");
  #endif
  pe_enabled = false;
}

void pintEchoSensorFix(){
  pinMode(ECHO_PIN,OUTPUT);
  digitalWrite(ECHO_PIN,LOW);
  pinMode(ECHO_PIN,INPUT);
}
void PingEchoFunctionRead2() {

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
#ifdef PE_PRESENT
      pePoint.distance = sonar.convert_cm(uS);
#else
      pePoint.distance = uS;
#endif
      break;
  }

  PingEchoFunctionR1();
}

void PingEchoFunctionR1() {
  // ID, PE01,R1,<timestamp:unit16>,<distance:uint16>
  XSERIAL.print(pePoint.ID); XSERIAL.print(", ");
  XSERIAL.print(PE02);  XSERIAL.print(", ");
  XSERIAL.print(R1);  XSERIAL.print(", ");
  XSERIAL.print(pePoint.ReadTimestamp);  XSERIAL.print(", ");
  XSERIAL.println(pePoint.distance);
  XSERIAL.flush();
}

void computeTimer(){
//  if ( pePoint.enabled && millis() >= (pePoint.TimeBetweenSamples + pePoint.ReadTimestamp){
//    PingEchoFunctionRead();
//  }

  if (pe_enabled && millis() >= peTimer){
    peTimer += PE_TIME_BETWEEN_SAMPLES;
    PingEchoFunctionRead2();
  }
}


// ----------------- PROCESS HANDLING ----------
// No real multi threading or processes involved

void ProcessHandling(){
  if(BUTTON_ENABLED){
      // Add all buttons to be processed here
      Theft01.bt_changed = ButtonFunctionProcess(&Button1);
      Theft01.bt_time = millis(); // currently unused
  }
#ifdef RFID_PRESENT
  if(RFID_ENABLED && mfrc522.PICC_IsNewCardPresent()){
#else
  if(RFID_ENABLED){  
#endif
     //Card detected, process data...
     int res = RFIDFunctionProcess(RF01);
     if(THEFT_ENABLED){
      Theft01.rfid_matched = res;
     }
     
  }
  if(THEFT_ENABLED){
    //194, 96, 196, 169
    if(Theft01.bt_changed){
      DEBUG_PRINT("#Process Handling   rfid_matched: ");
      DEBUG_PRINTLN(Theft01.rfid_matched);
      if(Theft01.rfid_matched <= 0){
        DEBUG_PRINTLN("#Process Handling   Alarm!");
        LEDFunctionBlink(1,0.5);
      }else{
        DEBUG_PRINTLN("#Car unlocked.");
        LEDFunctionSet(1,0);
      }
    }
  }
  LEDshouldBlink();
  computeTimer();  
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

