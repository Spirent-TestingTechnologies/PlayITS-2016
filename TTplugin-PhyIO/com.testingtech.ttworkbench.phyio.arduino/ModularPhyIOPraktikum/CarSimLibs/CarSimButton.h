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
			bool is_enabled;

			ButtonConfig(){	
				is_enabled = false;
			}
			
			void initialize(int cID, int buttonPin){
				ID = cID;
				pin = buttonPin;
				pinMode(pin, INPUT);
			}
		};
		
		int button_index(int id);
		
		long randNumber;	
		int ButtonCount;	
		
	public:
		
		ButtonConfig * Buttons;
		
		// Konstruktor für mehrere Buttons:
		// Button(int pins[], int ids[]);
		// Konstruktor für einen Button:
		Button(int ButtonIDs[], int ButtonPins[]);
		
		void ButtonFunction(int id, int command);
		void ButtonFunctionStart(int id);
		void ButtonFunctionStop(int id);
		bool ButtonFunctionCheck(int id);
		int ButtonFunctionProcess(int id); // for one Button at a time
		void ButtonProcessing(); // for all buttons
		

};

#endif