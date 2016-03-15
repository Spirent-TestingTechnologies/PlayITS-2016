#include <SoftwareSerial.h>
#include <Time.h>

#define XSERIAL Serial // mySerial // Serial // to switch between Bluetooth and USB XSERIAL connection for communication

#define CV01   10
#define PE01   20
#define LED01  30
#define REL01  40
#define MM01   50
#define DR01   60
#define LS01   70
#define RF01   80

#define TD01   85

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

//Button also known as Door
#define BT_PRESENT 1
#ifndef BT_PRESENT
long randNumber;
#endif

int BUTTON_ENABLED = 0;
#define Button1Pin 2

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

void setup() {
  XSERIAL.begin(9600, SERIAL_8N1); // Open serial monitor at 9600 baud

  while (!XSERIAL) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  establishContact();  // send a byte to establish contact until receiver responds

//Button
#ifndef BT_PRESENT
  DEBUG_PRINTLN("Simulating Button");
  randomSeed(analogRead(Button1Pin));
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
          DEBUG_PRINTLN("Not supported. Use rfid module");
          break;
        case DR01:
          // Door
          ButtonFunction(id, command);
      }
      processCleanUp();
    }
  }
  if(BUTTON_ENABLED){
      // Add all buttons to be processed here
      ButtonFunctionProcess(&Button1); 
  }

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
  if(randNumber < 5){
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

