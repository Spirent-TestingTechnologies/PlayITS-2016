#include <SPI.h>
#include <MFRC522.h>

#include <SoftwareSerial.h>
#include <Time.h>

#define XSERIAL Serial // mySerial // Serial // to switch between Bluetooth and USB XSERIAL connection for communication

#define CV01   10
#define PE01   20
#define LED01  30
#define REL01  40
#define MM01   50

#define LS01   70
#define RF01   80

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

//RFID 
unsigned int RFID_ENABLED = 0; // Tells if rfid is enabled for scanning
#define RFID_SS_PIN 10 
#define RFID_RST_PIN 9
MFRC522 mfrc522(RFID_SS_PIN, RFID_RST_PIN); // creates and holds mfrc522 data

void setup() {
  XSERIAL.begin(9600, SERIAL_8N1); // Open serial monitor at 9600 baud

  //RFID config
  SPI.begin();
  mfrc522.PCD_Init();
  
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
    
    // Serial Communication
    if (XSERIAL.available() > 0) {
      // Read in loop all data for one line
      id = XSERIAL.parseInt();
      functionType = XSERIAL.parseInt();
      command = XSERIAL.parseInt();

      switch (functionType) {
        case CV01:
          DEBUG_PRINTLN("Not supported.");
          break;
        case PE01:
          DEBUG_PRINTLN("Not supported. Use distance module");
          break;
        case LED01:
          DEBUG_PRINTLN("Not supported. Use led module");
          break;
        case REL01:
          DEBUG_PRINTLN("Not supported.");
          break;
        case MM01:
          DEBUG_PRINTLN("Not supported.");break;
        case RF01:
          // RFID Communication
          RFIDFunction(id, command);
          break;
      }
      processCleanUp();
    }
  }
  if (RFID_ENABLED && mfrc522.PICC_IsNewCardPresent()){
     //Card detected, process data...
     RFIDFunctionProcess(RF01);
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
  DEBUG_PRINTLN("In RFIDFunctionSetup... ");
  DEBUG_PRINT("Tag wird gelesen: ");
  for(int i = 0; i < 4; i++){
     rfid_tag[i] = XSERIAL.parseInt();
     DEBUG_PRINT(rfid_tag[i]); DEBUG_PRINT(" ");
  }
  DEBUG_PRINTLN("Tag erfolgreich gesetzt.");  
}

void RFIDFunctionProcess(int id){
    DEBUG_PRINTLN("In RFIDFunctionProcess...");
    if(!mfrc522.PICC_ReadCardSerial()){
      return; // nothing to read
    }
    unsigned long time = millis();
    
    int rfid_tag_tmp[4];
    for (byte i = 0; i < mfrc522.uid.size; i++){
      rfid_tag_tmp[i] = (int) mfrc522.uid.uidByte[i]; 
    }
    
    if(rfid_tag[0] != 0 && rfid_tag[1] != 0 && rfid_tag[2] != 0 && rfid_tag[3] != 0){
      for(int i = 0; i < 4; i++){
        if(rfid_tag[i] != rfid_tag_tmp[i]){
          DEBUG_PRINT("Tags stimmen nicht überein: ");
          DEBUG_PRINT(rfid_tag[i]); DEBUG_PRINT(" != ");
          DEBUG_PRINTLN(rfid_tag_tmp[i]);
          return;
        }
      }
    }

    // ID, RF01, R1, <tag1:int>, <tag2:int>, <tag3:int>, <tag4:int> <timestamp:long>
    XSERIAL.print(0); XSERIAL.print(", ");
    XSERIAL.print(RF01); XSERIAL.print(", ");
    XSERIAL.print(R1); XSERIAL.print(", ");
    for(int i = 0; i < 4; i++){
      XSERIAL.print(mfrc522.uid.uidByte[i], rfid_format);
      XSERIAL.print(", ");
    }
    
    XSERIAL.println(time);
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

