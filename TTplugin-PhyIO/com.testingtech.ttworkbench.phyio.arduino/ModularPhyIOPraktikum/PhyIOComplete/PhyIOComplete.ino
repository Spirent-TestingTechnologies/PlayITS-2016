#include <CarSim.h>
#include <PhyIOAutonom.h>

unsigned long readTimer; // holds the next read time;
unsigned int readSpeed = 100; // How frequently are we going to read the serial line
  
  
//Initialisierung des Moduls
Car car;
Communication XSERIAL;
Autonom a(&car);
  
// Definitionen der Ino
void processCleanUp ();
void establishContact();
int id, functionType, command;
bool just_switched;


void setup(){
	XSERIAL.begin(); // Open serial monitor at 9600 baud

	just_switched = true;

	#ifdef RFID_Module
		car.RFIDSetup();
	#endif
	
	#ifdef ColorView_Module
		car.ColorViewSetup();
	#endif
	
	
	DEBUG_PRINTLN("#Entered loop");
	
}



void loop(){
	if(a.is_autonom()){
		a.reset();
		a.start();
		just_switched = true;
	}
	
	
	if(!a.is_autonom() && just_switched){
		XSERIAL.println("Entering Input Mode. Welcome!");
		
		#ifdef establishContact
			establishContact();  // send a byte to establish contact until receiver responds
		#endif
		
		car.echo.PingEchoFunctionStop2();
		car.cv.ColorViewFunctionStop();
		
		// following has to be changed depending on how many Buttons are installed
		car.button.ButtonFunctionStop(1);
		car.button.ButtonFunctionStop(2);
		
		
		just_switched = false;
	}
	
	
	if(!a.is_autonom() && !just_switched){
		
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
						break;
					default:
						DEBUG_PRINTLN("#No Valid Command, try again");
						break;
					}	
				processCleanUp();
			}
		}
		car.ProcessHandling();
	}
}

// ----------------- GENERAL CONFIG ----------------

#ifdef establishContact
	void establishContact() {
		long unsigned time_check = millis();
		while (XSERIAL.available() <= 0) {
		
			XSERIAL.print("0,0,0,");   // send an initial string
			XSERIAL.println(millis());
			// activly wait istead of delay
			time_check = millis();
			while((millis() - time_check)/1000.0 < 1){
				// break, if the PhyIO is switching to autonomus mode
				if(a.is_autonom()){
					return;
				}
			}
		}
	}
#endif

void processCleanUp () {
	// If we are here rest ist rubbish
	int i;
	DEBUG_PRINTLN("#CleanUp");
	while (XSERIAL.available() > 0) {
		i = XSERIAL.read();
	}
}

