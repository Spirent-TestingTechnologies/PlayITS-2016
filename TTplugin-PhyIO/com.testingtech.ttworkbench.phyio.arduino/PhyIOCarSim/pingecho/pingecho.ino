//How to:
#define PE_PRESENT  1       // set 0 or comment out
//ECHO_PIN = 8, TRIGGER_PIN = 10
//send "1,21,4" per serial
//receive "id,module,command,timestamp,DISTANCE"

#include <SoftwareSerial.h>
#include <NewPing.h>
//#include <Time.h>

#define XSERIAL Serial // mySerial // Serial // to switch between Bluetooth and USB XSERIAL connection for communication

#define CV01   10
#define PE01   20
#define LED01  30
#define REL01  40
#define MM01   50

#define PE02   21
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

//Ping Echo sensor setup
#define TRIGGER_PIN 10
#define ECHO_PIN  8
#define MAX_DISTANCE 300
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); //NewPing setup
#define PE_UNIT 1


bool pe_enabled = false;
unsigned long peTimer = 0;
#define PE_TIME_BETWEEN_SAMPLES 100 //Ping Frequency
#define PE_POINT_MEDIAN_ITERATIONS  1 //how many iterations used for median

//Serial
unsigned long readTimer; // holds the next read time;
unsigned int readSpeed = 500; // How frequently are we going to read the serial line


void setup() {
  XSERIAL.begin(9600, SERIAL_8N1); // Open serial monitor at 9600 baud
  while (!XSERIAL) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  establishContact();  // send a byte to establish contact until receiver responds

  //Easy testing
  PingEchoFunctionSetup2();
  //Easy testing
  
  DEBUG_PRINTLN("#Entered loop");
  XSERIAL.flush();
}

void loop() {
  int id, functionType, command;
  
  if (millis() >= readTimer) {
    readTimer += readSpeed;      // Set the next read time.
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
        case PE02:
         PingEchoFunction2(id, command);
          break;
        case LED01:
          //LEDFunction(id, command);
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
  //TODO add all timer related calls here
  computeTimer();
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
      pePoint.distance = sonar.convert_cm(uS);
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

