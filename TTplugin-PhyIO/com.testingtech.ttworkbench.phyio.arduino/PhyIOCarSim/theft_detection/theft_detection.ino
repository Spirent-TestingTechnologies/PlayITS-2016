#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>

//include <Time.h> // Use in future low power mode

#define XSERIAL Serial // mySerial // Serial // to switch between Bluetooth and USB XSERIAL connection for communication

#define CV01   10
#define PE01   20
#define LED01  30
#define REL01  40
#define MM01   50
#define DR01   60
#define LS01   70
#define RF01   80

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


unsigned long readTimer; // holds the next read time;
unsigned int readSpeed = 500; // How frequently are we going to read the serial line
long randNumber; // random number for simulated parts

//Button also known as Door
#define BT_PRESENT 1
#define Button1Pin 2
int BUTTON_ENABLED = 0;

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

ButtonConfig Button1(1, Button1Pin);

//RFID 
unsigned int RFID_ENABLED = 0; // Tells if rfid is enabled for scanning
#define RFID_SS_PIN 10 
#define RFID_RST_PIN 9
MFRC522 mfrc522(RFID_SS_PIN, RFID_RST_PIN); // creates and holds mfrc522 data


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
  DEBUG_PRINTLN("Simulating RFID");
  randomSeed(RFID_SS_PIN);
#endif


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

      switch (functionType) {
        case CV01:
          DEBUG_PRINTLN("#Not supported.");
          break;
        case PE01:
          DEBUG_PRINTLN("#Not supported. Use distance module");
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
          
      }
      processCleanUp();
    }
  }
  if(BUTTON_ENABLED){
      // Add all buttons to be processed here
      ButtonFunctionProcess(&Button1); 
  }
  if(RFID_ENABLED && mfrc522.PICC_IsNewCardPresent()){
     //Card detected, process data...
     RFIDFunctionProcess(RF01);
  }
  LEDshouldBlink();

}

// -------- DoorFunction -----------
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

void ButtonFunctionProcess(struct ButtonConfig *bt){
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
  }
}

// -------- RFIDFunction -----------
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
  DEBUG_PRINT("In RFIDFunction with function ");
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

void RFIDFunctionProcess(int id){
    DEBUG_PRINTLN("#In RFIDFunctionProcess...");
    int rfid_tag_tmp[4];

#ifdef RFID_PRESENT    
    if(!mfrc522.PICC_ReadCardSerial()){
      return; // nothing to read
    }

    for (byte i = 0; i < mfrc522.uid.size; i++){
      rfid_tag_tmp[i] = (int) mfrc522.uid.uidByte[i]; 
    }
#else
    randNumber = random(100);
    if(randNumber >= 5){
      return; // nothing to read
    }

    // Generate random rfid uid
    for (byte i = 0; i < 4; i++){
      rfid_tag_tmp[i] = random(256); 
    }
#endif

    unsigned long time = millis();
    
    // ID, RF01, R1, <tag1:int>, <tag2:int>, <tag3:int>, <tag4:int> <timestamp:long>
    XSERIAL.print(0); XSERIAL.print(", ");
    XSERIAL.print(RF01); XSERIAL.print(", ");
    XSERIAL.print(R1); XSERIAL.print(", ");
    for(int i = 0; i < 4; i++){
#ifdef RFID_PRESENT
      XSERIAL.print(mfrc522.uid.uidByte[i], rfid_format);
#else
      XSERIAL.print(rfid_tag_tmp[i], rfid_format);
#endif
      XSERIAL.print(", ");
    }
    
    XSERIAL.println(time);
    XSERIAL.flush();
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
      LEDFunctionBlink(id);
      break;
    case STOP:
      LEDFunctionSet(id, 0);
      break;
    default:
      break;
  }
}


// LED should be refactored
void LEDFunctionBlink(int id) {
  // tells the given led, that it now has to blink
  float freq = XSERIAL.parseFloat();

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

