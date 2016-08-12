#include <CarSim.h>

unsigned long readTimer; // holds the next read time;
unsigned int readSpeed = 500; // How frequently are we going to read the serial line
  
  
//Initialisierung des Moduls
Car car;
  
// Definitionen der Ino
void processCleanUp ();
void establishContact();


void setup(){
	XSERIAL.begin(9600, SERIAL_8N1); // Open serial monitor at 9600 baud

	#ifdef RFID_Module
		car.RFIDSetup();
	#endif
	
	#ifdef ColorView_Module
		car.ColorViewSetup();
	#endif
	
	while (!XSERIAL) {
		; // wait for serial port to connect. Needed for native USB port only
	}
	establishContact();  // send a byte to establish contact until receiver responds

	
	
	DEBUG_PRINTLN("#Entered loop");
}



void loop(){
	int id, functionType, command;
	
	if (millis() >= readTimer) {
		readTimer += readSpeed;      // Set the next ping time.
    
		// Serial Communication
		if (XSERIAL.available() > 0) {
			
			// Read in loop all data for one line
			id = XSERIAL.parseInt();
			functionType = XSERIAL.parseInt();
			command = XSERIAL.parseInt();
      
			#ifdef Theft_Module
				car.TheftHandling(functionType, command);
			#endif
			
			switch (functionType) {
				case CV01:
					// ColorView -- "Legacy module"
					#ifdef ColorView_Module	
						car.ColorViewFunction(id, command);
					#else
						DEBUG_PRINTLN("#Module Currently not supported. Make sure to include it in 'CarSimDefine.h'");
					#endif
					break;
				case PE01:
					//DEBUG_PRINTLN("#Not supported. Use distance module");
					break;
				case PE02:
					// Distance
					#ifdef PingEcho_Module	
						car.PingEchoFunction2(id, command);
					#else
						DEBUG_PRINTLN("#Module Currently not supported. Make sure to include it in 'CarSimDefine.h'");
					#endif
					break;
				case LED01:
					// LED
					#ifdef LED_Module
						car.LEDFunction(id, command);
					#else
						DEBUG_PRINTLN("#Module Currently not supported. Make sure to include it in 'CarSimDefine.h'");
					#endif
					break;
				case REL01:
					// Relay -- "Legacy module"
					#ifdef Relay_Module
						car.RelayFunction(id, command);
					#else
						DEBUG_PRINTLN("#Module Currently not supported. Make sure to include it in 'CarSimDefine.h'");
					#endif
					break;
				case MM01:
					//Motor -- "Legacy module"
					#ifdef Motor_Module
						car.MotorFunction(id, command);
					#else
						DEBUG_PRINTLN("#Module Currently not supported. Make sure to include it in 'CarSimDefine.h'");
					#endif
					break;
				case RF01:
					// RFID Communication
					#ifdef RFID_Module
						car.RFIDFunction(id, command);
					#else
						DEBUG_PRINTLN("#Module Currently not supported. Make sure to include it in 'CarSimDefine.h'");
					#endif
					break;
				case DR01:
					// Door
					#ifdef Button_Module
						car.ButtonFunction(id, command);
					#else
						DEBUG_PRINTLN("#Module Currently not supported. Make sure to include it in 'CarSimDefine.h'");
					#endif
					break;
				case TD01:
					// Theft Detection 
					#ifdef Theft_Module
						car.TheftDetectionFunction(id, command);
					#else
						DEBUG_PRINTLN("#Module Currently not supported. Make sure to include it in 'CarSimDefine.h'");
					#endif
					break;
				case LS01:
					// Light Sensor
					#ifdef LighSensor_Module
						car.LightSensorFunction(id, command);
					#else
						DEBUG_PRINTLN("#Module Currently not supported. Make sure to include it in 'CarSimDefine.h'");
					#endif
					break;
        case IR01:
          //Stripe remote
          #ifdef Stripe_Module
            car.StripeFunction(command);
          #else
            DEBUG_PRINTLN("#Module Currently not supported. Make sure to include it in 'CarSimDefine.h'");
          #endif
			}	
			processCleanUp();
		}
	}
	car.ProcessHandling();
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

