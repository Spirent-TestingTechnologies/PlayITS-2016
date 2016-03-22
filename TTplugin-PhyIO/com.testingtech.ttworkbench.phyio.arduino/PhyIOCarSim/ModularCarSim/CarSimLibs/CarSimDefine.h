#ifndef CarSimDefine
#define CarSimDefine


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


//Enable/Disable for Simulation of those Modules
//#define BT_PRESENT 1
//#define RFID_PRESENT 1
//#define PE_PRESENT 1

// configurable defines for Button
#define Button1Pin 2
#define Button1ID 4


// configurable defines for LED
#define Led1Pin 3
#define Led1ID 1

// configurable defines for echo ping
#define TRIGGER_PIN 6
#define ECHO_PIN  5
#define MAX_DISTANCE 300
#define PE_UNIT 1
#define PE_TIME_BETWEEN_SAMPLES 100 //Ping Frequency
#define PE_POINT_MEDIAN_ITERATIONS  1 //how many iterations used for median

// configurable defines for RFID
#define RFID_SS_PIN 10 
#define RFID_RST_PIN 9

// configurable defines for Theft-Detection
#define Theft1ID 6



#endif
