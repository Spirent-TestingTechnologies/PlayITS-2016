#ifndef CarSimDefine
#define CarSimDefine

#define MAX_PARAM 5

#define XSERIAL Serial // mySerial // Serial // to switch between Bluetooth and USB XSERIAL connection for communication


//==========================================================================================


#define CV01   10 // Colour View      
#define PE01   20 // Echo Distance     
#define LED01  30 // LED               
#define REL01  40 // Relay             
#define MM01   50 // Motor             
#define DR01   60 // Button/toggleswitch
#define LS01   70 //
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


//==========================================================================================


// the following line declares if there will be debug lines
// important to note is the fact, that not all modules are capeable 
// to run at the same time, while debugging-print-outs are switched on,
// as it takes to much memory at once
// always switch this option off, while working with all modules at the same time

//#define DEBUG 1


//==========================================================================================


// the following defines which modules will be included during the compilation
// works great for easy testing on one module
#define Button_Module 1
//#define LED_Module 1				|
//#define RFID_Module 1
//#define Theft_Module 1 // button, led and rfid also have to be included for this module
//#define PingEcho_Module 1			|
//#define LighSensor_Module 1
//#define ColorView_Module 1		|
//#define Relay_Module 1
//#define Motor_Module 1			|
//#define Stripe_Module 1


//==========================================================================================


//Enable/Disable for Simulation of those Modules
#define BT_PRESENT 1
//#define RFID_PRESENT 1
#define PE_PRESENT 1
//#define LS_PRESENT 1
#define CVF_PRESENT 1
#define MOTOR_PRESENT 1
//#define RELAYS_PRESENT 1



//==========================================================================================


// The implementation of the debugging line
#ifdef DEBUG
#define DEBUG_PRINT(x) XSERIAL.print (x)
#define DEBUG_PRINTLN(x) XSERIAL.println (x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif


//==========================================================================================


// configurable defines for Button
#define Button1Pin A0
#define Button1ID 4


//==========================================================================================


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


//==========================================================================================


// configurable defines for echo ping
#define TRIGGER_PIN 12
#define ECHO_PIN  11
#define MAX_DISTANCE 300
#define PE_UNIT 1
#define PE_TIME_BETWEEN_SAMPLES 200 //Ping Frequency
#define PE_POINT_MEDIAN_ITERATIONS  1 //how many iterations used for median


//==========================================================================================


// configurable defines for RFID
#define RFID_SS_PIN 10 
#define RFID_RST_PIN 9


//==========================================================================================


// configurable defines for Theft-Detection
#define Theft1ID 6


//==========================================================================================


// configurable defines for LightSensor
#define LS1PIN A1


//==========================================================================================


// configurable defines for Colour View
#define cvf_ledpin 4


//==========================================================================================


// configurable defines for Motor Function
#define enablePin 5
#define in1Pin 6
#define in2Pin 7


//==========================================================================================


// configurable defines for Relay Functions
#define RELAY1PIN  A1
#define RELAY2PIN  A2


//==========================================================================================


/*
// 32 bit NEC IR remote control
// für das Schreiben eines Moduls für die Remote, wird folgender
// Import benötigt
//#include <IRremote.h>

//#define IR_PRESENT 1
#ifdef IR_PRESENT
// IR LED DATA on D9 (PWM)
IRsend irsend;
#endif

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
*/


//==========================================================================================


#endif
