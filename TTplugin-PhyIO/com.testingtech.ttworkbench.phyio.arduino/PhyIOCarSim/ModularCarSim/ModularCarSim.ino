#include "CarSim.h"

unsigned long readTimer; // holds the next read time;
unsigned int readSpeed = 500; // How frequently are we going to read the serial line
  
// alle debug ausgaben durch defines ausklammern
  

// Button Initialisierung
Button button(Button1ID, Button1Pin);

// LED Initialisierung
int LEDIDs[] = {Led1ID};
int LEDPins[] = {Led1Pin};
LED led(LEDPins, LEDIDs);

// RFID Initialisierung
RFID rfid;

// Theft-Detection Initialisierung
Theft theft(Theft1ID, button, led, rfid);

// Distance Initialisierung
PingEcho echo;


// LightSensor Initialisierung
LightSensor ls(readTimer, readSpeed);


void setup() {
	XSERIAL.begin(9600, SERIAL_8N1); // Open serial monitor at 9600 baud

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

			// vielleicht folgendes in die carsim.h packen
			// vielleicht sogar mit defines code ausklammern,
			// wenn die module nicht benutzt werden sollen
			if(theft.THEFT_ENABLED){
				if(functionType == TD01 && command == STOP){
					theft.TheftDetectionFunction(1,command);
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
						// Distance
						echo.PingEchoFunction2(id, command);
						break;
					case LED01:
						// LED
						led.LEDFunction(id, command);
						break;
					case REL01:
						DEBUG_PRINTLN("#Not supported.");
						break;
					case MM01:
						DEBUG_PRINTLN("#Not supported.");
						break;
					case RF01:
						// RFID Communication
						rfid.RFIDFunction(id, command);
						break;
					case DR01:
						// Door
						button.ButtonFunction(id, command);
						break;
					case TD01:
						// Theft Detection 
						theft.TheftDetectionFunction(id, command);
						break;
					case LS01:
						// Light Sensor
						ls.LightSensorFunction(id, command);
						break;
				}
			}
			processCleanUp();
		}
	}
	ProcessHandling();
}



// ----------------- PROCESS HANDLING ----------
// No real multi threading or processes involved

void ProcessHandling(){
	
	if(button.BUTTON_ENABLED){
		// Add all buttons to be processed here
		theft.Theft01.bt_changed = button.ButtonFunctionProcess(&button.Button1);
		theft.Theft01.bt_time = millis(); // currently unused
	}
	#ifdef RFID_PRESENT
		if(rfid.RFID_ENABLED && mfrc522.PICC_IsNewCardPresent()){
	#else
		if(rfid.RFID_ENABLED){  
	#endif
		//Card detected, process data...
		int res = rfid.RFIDFunctionProcess(RF01);
			if(theft.THEFT_ENABLED){
			theft.Theft01.rfid_matched = res;
			}
		}
	if(theft.THEFT_ENABLED){
		//194, 96, 196, 169
		if(theft.Theft01.bt_changed){
			DEBUG_PRINT("#Process Handling   rfid_matched: ");
			DEBUG_PRINTLN(theft.Theft01.rfid_matched);
			if(theft.Theft01.rfid_matched <= 0){
				DEBUG_PRINTLN("#Process Handling   Alarm!");
				led.LEDFunctionBlink(1,0.5);
			}else{
				DEBUG_PRINTLN("#Car unlocked.");
				led.LEDFunctionSet(1,0);
			}
		}
	}
	led.LEDshouldBlink();
	echo.computeTimer();  
}

// ----------------- GENERAL CONFIG ----------------
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

