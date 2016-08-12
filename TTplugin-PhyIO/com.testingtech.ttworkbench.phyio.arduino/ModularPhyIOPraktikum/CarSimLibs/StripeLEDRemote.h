#ifndef StripeLEDRemote
#define	StripeLEDRemote

#include "Arduino.h"
#include "CarSimDefine.h"
#include <IRremote.h>

// 32 bit NEC IR remote control
//
#define XBPLUS  0xF700FF  // langsames Durchwechseln in die eine Richtung
#define XBMINUS 0xF7807F  // langsames Durchwechseln in die andere Richtung
#define XOFF    0xF740BF  // 
#define XON     0xF7C03F  // 

#define X1     0xF720DF  // --Red
#define X2     0xF710EF  // --orange
#define X3     0xF730CF  // --gelb
#define X4     0xF708F7  // --gelb grün
#define X5     0xF728D7  // --grün gelb
#define X6     0xF7A05F  // --Green
#define X7     0xF7906F  // --türkis
#define X8     0xF7B04F  // --himmelblau
#define X9     0xF78877  // --hellblau
#define X10    0xF7A857  // --ice blue
#define X11    0xF7609F  // --Blue
#define X12    0xF750AF  // --dunkles lila
#define X13    0xF7708F  // --helles lila
#define X14    0xF748B7  // --dunkleres rosa
#define X15    0xF76897  // --rosa
#define X16    0xF7E01F  // --weiß 
 
#define XSTROBE 0xF7D02F  // geht  schnell abgehackt durch alle farben
#define XFLASH  0xF7F00F  // geht langsam abgehackt durch alle farben
#define XFADE   0xF7C837  // geht gleichmäßig durch alle Farben
#define XSMOOTH 0xF7E817  // geht sehr langsam gleichmäßig durch alle Farben

#define COLOUR 1
#define STROBE 2
#define FLASH  3
#define FADE   4
#define SMOOTH 5

class LEDRemote{
	
	private:
		
		IRsend irsend;
		long colours[16];
		unsigned long curentMillis;
		
		struct StripeFunctionConfig{
			//Struct for the LED-Stripe
			
			long state;
			bool is_toggling;
			long toggle_time;
			unsigned long previousMillis;
			
			int toggling_size; // how many colours have to be cycled through
			int * toggling_colours;
			int toggling_index;
			
			StripeFunctionConfig(){
				state = XOFF;
				is_toggling = false;
				toggle_time = 0;
				previousMillis = 0;
				toggling_index = 0;
				toggling_colours = NULL;
				toggling_size = 0;
			}
			/*
			void set_colours(int amount){
				toggling_colours = new int[amount];
			}
			*/
			void Reset(){
				state = XOFF;
				is_toggling = false;
				toggle_time = 0;
				previousMillis = 0;
				toggling_index = 0;
				delete [] toggling_colours;
				toggling_colours = NULL;
				toggling_size = 0;
			}
		};
		
		StripeFunctionConfig stripe;
		
		void sending(long command);
		//man muss mal gucken, ob sending privat sein sollte
		long assignValue(int colour);
		// gibt den korrespondierenden HexWert zur Farbe wieder
		
	public:
		
		LEDRemote();
		void StripeFunction(int command);
		void StripeFunctionOn();
		void StripeFunctionOff();
		void StripeFunctionSTROBE();
		void StripeFunctionFLASH();
		void StripeFunctionFADE();
		void StripeFunctionSMOOTH();
		void StripeFunctionColour(int colour);
		void StripeFunctionSet(int colour);
		void StripeFunctionMode(int mode);
		void StripeFunctionBlink(); // kann vielleicht nicht an und aus blinken
		
		void StripeScheduler();
		
		
};

#endif



