#ifndef CarSimLED
#define CarSimLED

#include "Arduino.h"
#include "CarSimDefine.h"

class LED {

	private:

		int LedCount;
	  
		unsigned long currentMillis;
		
		struct LEDFunctionConfig{
			// Struct that simulates and controls the corresponding diodes

			int ID; // The ID
			int state;
			int pin;
			bool is_toggling;
			long toggling_time;
			unsigned long previousMillis;

			LEDFunctionConfig() {
				pinMode(pin, OUTPUT);
				previousMillis = 0;
				toggling_time = 0;
				is_toggling = false;
				state = LOW;
			}

			void initialize(int theID, int ledPin){
				ID = theID;
				pin = ledPin;
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
		
		LEDFunctionConfig * Dioden;
	  
	public:
	
		LED(int LEDPins[], int LEDIDs[]);

		void LEDFunction( int id, int command);
		void LEDFunctionSet(int id, int state);
		void LEDFunctionBlink(int id, float freq);
		void LEDFunctionStop(int id);
		void LEDToggling(LED::LEDFunctionConfig *led);
		void LEDSwitch(LED::LEDFunctionConfig *led, int state);
		void LEDshouldBlink();
    
};

#endif
