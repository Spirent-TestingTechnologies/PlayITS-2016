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
			
			bool scheduled; // tells if the module prints information on XSERIAL

			void Reset(){
				// Set default values in case there is nothing
				// stored in the eeprom yet.
				TimeBetweenSamples = 500;
				intLED = 1;
			}
		};
		
		Adafruit_TCS34725 tcs;
	
	public : 
	
		// sollte man mehrere CV-Sensoren haben, so kann man wie im LED-Modul vorgehen
		// die bisherige Struktur sollte einene relativ einfachen Umstieg auf Arsays
		// ermöglichen. Wenn die nächste Zeile zum Array mit Pointern umgeschrieben wird,
		// dann ist die Header-Datei bereits komplett abgesschlossen. Man siehe:
		// ColorViewFunctionConfig * cvfPoints;
	
		ColorViewFunctionConfig cvfPoint;
		
		ColorView();
		void ColorViewFunction( int id, int command);
		void ColorViewSetup();
		void ColorViewFunctionSetup();
		void ColorViewFunctionRead();
		void ColorViewFunctionR1();	
		void ColorViewFunctionR2();
		void ColorViewScheduling();
		bool ColorViewDecide();
				
	
};



#endif