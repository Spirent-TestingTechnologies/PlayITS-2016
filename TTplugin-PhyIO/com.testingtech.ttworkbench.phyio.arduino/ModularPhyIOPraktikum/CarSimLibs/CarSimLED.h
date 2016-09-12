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
			int pin2; // siehe in initialize für erläuterung
			bool is_toggling;
			long toggling_time;
			unsigned long previousMillis;
			int toggle_mode; // 1, togglet

			LEDFunctionConfig() {
				previousMillis = 0;
				toggling_time = 0;
				is_toggling = false;
				state = 0;  // 0 = off | 1 = first colour | 2 = second colour
						    // not all diodes can turn on a second colour
				toggle_mode = 1;
			}

			void initialize(int theID, int ledPin, int aledPin){
				// wenn pin2 mit 0 belegt wird, dann handelt es sich um eine normale LED
				// wenn pin2 mit einem Pin belegt wird, dann handelt es sich um eine
				// bidirektionale LED
				ID = theID;
				pin = ledPin;
				pin2 = aledPin;
				pinMode(pin, OUTPUT);
				if(pin2 != 0){
					pinMode(pin2, OUTPUT);
				}
			}

			void Reset()
			{
				// Set default values in case there is nothing stored
				state = 0;
				is_toggling = false;
				toggling_time = 0;
				previousMillis = 0;
			}
		
		};
		
		LEDFunctionConfig * Dioden;
		
		int dioden_index(int id);

		void LEDToggling(LED::LEDFunctionConfig *led);
		void LEDSwitch(LED::LEDFunctionConfig *led, int state);
	  
	public:
	
		LED(int LEDIDs[], int LEDPins[], int BiLEDIDs[], int BiLEDPins1[], int BiLEDPins2[]);

		void LEDFunction(int id, int command);
		void LEDFunctionSet(int id, int state);
		void LEDFunctionBlink(int id, float on_time, int mode);
		void LEDFunctionStop(int id);
		void LEDshouldBlink();
    
};

#endif
