#ifndef CarSimColorView
#define CarSimColorView

#include "Adafruit_TCS34725.h"
#include "Arduino.h"
#include "CarSimDefine.h"

class ColorView {
	
	private:
	
		struct ColorViewFunctionConfig{
			int ID; // The ID
			unsigned long TimeBetweenSamples; // time when cvf read is taken
			unsigned int intLED; // uses the internal LED
			uint16_t clear, red, green, blue; // raw rgb values
			uint16_t red256, green256, blue256; // normed rgb values (0..255)
			uint16_t colorTemp, lux; // computed values
			unsigned long ReadTimestamp;

			void Reset(){
				// Set default values in case there is nothing
				// stored in the eeprom yet.
				TimeBetweenSamples = 500;
				intLED = 1;
			}
		};
		
		Adafruit_TCS34725 tcs;
	
	public : 
	
		ColorViewFunctionConfig cvfPoint;
	
		ColorView();
		void ColorViewFunction( int id, int command);
		void ColorViewSetup();
		void ColorViewFunctionSetup();
		void ColorViewFunctionRead();
		void ColorViewFunctionR1();	
		
	
};



#endif