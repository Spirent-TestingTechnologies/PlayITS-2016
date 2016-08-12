#ifndef CarSimButton
#define CarSimButton

#include "Arduino.h"
#include "CarSimDefine.h"

class Button  {

	private:
		
		struct ButtonConfig{
			int ID;
			int state;
			int pin;

			ButtonConfig(){	}
			
			void initialize(int cID, int buttonPin){
				ID = cID;
				pin = buttonPin;
				pinMode(pin, INPUT);
			}
		};
		
		
		long randNumber;		
		
	public:
		// folgendes w�hre ein Ansatz f�r die 
		// Gleichzeitige Nutzung mehrer Buttons:
		// ButtonConfig * Buttons;
		
		// allerdings wird nur einer genutzt, daher:
		ButtonConfig Button1;
		
		
		int BUTTON_ENABLED;
		
		// Konstruktor f�r mehrere Buttons:
		// Button(int pins[], int ids[]);
		// Konstruktor f�r einen Button:
		Button(int id, int pin);
		
		void ButtonFunction(int id, int command);
		int ButtonFunctionProcess(struct ButtonConfig *bt);
		

};

#endif