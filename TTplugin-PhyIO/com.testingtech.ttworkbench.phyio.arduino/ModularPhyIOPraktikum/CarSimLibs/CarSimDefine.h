#ifndef CarSimDefine
#define CarSimDefine

#define MAX_PARAM 5

//#define XSERIAL Serial // mySerial // Serial // to switch between Bluetooth and USB XSERIAL connection for communication

 
#include <PhyIOCommunication.h>

#define Bluetooth 1

#define rxPin 3
#define txPin 2

// this is a declaration to make the Buetoothcommunication work
extern Communication XSERIAL;

//========================================================================================

// the next defines will decide if there are start and stop strings at the end 
// and start of each message. These strings will decide how a send message will
// look like and help to ensure a more stable communication over bluetooth

#define SAFETYSTRINGS 1
#define STARTSTRING "?"
#define ENDSTRING "!"

//========================================================================================


#define CV01   10 // Colour View      
#define PE01   20 // Echo Distance     
#define LED01  30 // LED               
#define REL01  40 // Relay             
#define MM01   50 // Motor             
#define DR01   60 // Button/toggleswitch
#define LS01   70 // Light sensor (doesn't work reliably at this state)
#define RF01   80 // RFID
#define IR01   90 // Infrarotsender für die LED-Leiste

#define PE02   21 // Echo Distance 
#define TD01   85 // Theft Detection (combination of button, rfid and led)

#define SETUP 1
#define SET 1
#define START 2
#define BLINK 2 
#define STOP 3
#define READ 4
#define R1 101

//========================================================================================

// These Defines set the values used in the Autonomus-Mode

#define maxHeight 50
#define minSpeed 40
#define maxSpeed 99

#define idleTime1 1 // in min
#define idleTime2 3 // in min
#define waitTime 8 // in sec
#define endingTime 5 // in sec

//========================================================================================

// this define will decide wheter the function "establishContact()" is executed when
// starting the input-mode of the PhyIO or if it shouldn't be used
// 		- the establishContact() function is the one that prints "0,0,0,time" into the
//		  serial connection
// currently there is no reaon to use this, therefore it is deactivated
// the testcases don't need the time to work correctly

//#define establishContact 1

//========================================================================================


// the following line declares if there will be debug lines
// important to note is the fact, that not all modules are capeable 
// to run at the same time, while debugging-print-outs are switched on,
// as it takes to much memory at once
// always switch this option off, while working with all modules at the same time

//#define XDEBUG 1


//========================================================================================


// the following defines which modules will be included during the compilation
// works great for easy testing on one module
#define Button_Module 1				|
#define LED_Module 1				|
//#define RFID_Module 1 // there is no rfid reader in the PhyIO at the moment
//#define Theft_Module 1 // button, led and rfid also have to be included for this module
#define PingEcho_Module 1			|
//#define LighSensor_Module 1
#define ColorView_Module 1		|
//#define Relay_Module 1 // can not be used at the moment
#define Motor_Module 1			|
#define Stripe_Module 1


//========================================================================================


//Enable/Disable for Simulation of those Modules
#define BT_PRESENT 1
//#define RFID_PRESENT 1
#define PE_PRESENT 1
//#define LS_PRESENT 1
#define CVF_PRESENT 1
#define MOTOR_PRESENT 1
//#define RELAYS_PRESENT 1



//========================================================================================


// The implementation of the debugging line
#ifdef XDEBUG
#define DEBUG_PRINT(x) XSERIAL.print (x)
#define DEBUG_PRINTF(x,y) XSERIAL.print (x,y)
#define DEBUG_PRINTLN(x) XSERIAL.println (x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTF(x,y)
#define DEBUG_PRINTLN(x)
#endif


//========================================================================================


// configurable defines for Button

#define ButtonAmount 2 //How many Buttons and Pins are there?

#define Switch1Pin 15 // pin liegt auf A1, wird allerdings digital genutzt, daher 15
// die analogen Pins sind für digitalen gebrauch doppelt belegt
#define Switch1ID 1

#define Button1Pin 16 
#define Button1ID 2 // pin liegt auf A2 (analog zu A1)


//========================================================================================


// configurable defines for LED
	// sollte man weitere LED hinzufügen, dann muss man
	// dies auch in "CarSim.cpp" ab Zeile 7 erweitern
	// !keine LED darf die ID 0 haben!
#define LedAmount 1
#define Led1Pin 10 // 3 in the finished testing version |
#define Led1ID 1

#define BiLedAmount 1 //Anzahl der Bidirektionlen LED
#define BiLed1APin A3 // Bidirektionale LED Analog pin
#define BiLed1DPin 8 // Bidirektionale LED Digital pin
#define BiLed1ID 2 // Bidirektionale LED ID; diese ID ist weiter fortgesetzt
	// von den bereits genutzten normalen LED ID


//========================================================================================


// configurable defines for echo ping
#define TRIGGER_PIN 12
#define ECHO_PIN  11
#define MAX_DISTANCE 300
#define PE_UNIT 1
#define PE_TIME_BETWEEN_SAMPLES 200 //Ping Frequency
#define PE_POINT_MEDIAN_ITERATIONS  7 //how many iterations used for median


//========================================================================================


// configurable defines for RFID
#define RFID_SS_PIN 10 
#define RFID_RST_PIN 9


//========================================================================================


// configurable defines for Theft-Detection
#define Theft1ID 6


//========================================================================================


// configurable defines for LightSensor
#define LS1PIN A1


//========================================================================================


// configurable defines for Colour View
#define cvf_ledpin 4


//========================================================================================


// configurable defines for Motor Function
#define enablePin 5
#define in1Pin 6
#define in2Pin 7


//========================================================================================


// currently there is no relay build into the Phy IO
// there are still 2 open wires conected to A6 and A7
// in case another Relay needs to be added 
// configurable defines for Relay Functions
#define RELAY1PIN  A6
#define RELAY2PIN  A7


#endif
