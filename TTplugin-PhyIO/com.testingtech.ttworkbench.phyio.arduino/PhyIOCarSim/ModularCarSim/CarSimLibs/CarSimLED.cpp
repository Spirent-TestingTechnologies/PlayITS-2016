#include "CarSimLED.h"

/**
* both arrays have to be equal in size
*/
LED::LED(int LEDPins[], int LEDIDs[]){
	
	DEBUG_PRINT("\n# In CarSimLED-Constructor\n");

	this->LedCount = (int)( sizeof(LEDPins) / sizeof(LEDPins[0]));
  
	//erstelle neues Array von Structs
	this->Dioden = new LED::LEDFunctionConfig[LedCount];

	for(int i = 0; i < LedCount; i++){
		this->Dioden[i].initialize(LEDIDs[i], LEDPins[i]);
	}
}


void LED::LEDFunction(int id, int command) {
	// chooses the function for the led
  
	DEBUG_PRINT("\n# In the LEDFunction with function : '");
	DEBUG_PRINTLN(command);

	DEBUG_PRINT("# and with ID: ");
	DEBUG_PRINTLN(id);

	switch (command) {
		case SET:
			LEDFunctionSet(id, XSERIAL.parseInt());
			break;
		case BLINK:
			LEDFunctionBlink(id, (1000/(XSERIAL.parseFloat()*2)));
			break;
		case STOP:
			LEDFunctionSet(id, 0);
			break;
		default:
			break;
  }
}


void LED::LEDFunctionBlink(int id, float on_time) {
	// tells the given led, that it now has to blink

	DEBUG_PRINT("\n# In the LEDFunctionBlink");
	DEBUG_PRINT("\n# With ID: ");
	DEBUG_PRINTLN(id);
	DEBUG_PRINT("\n# and with an on_time of: ");
	DEBUG_PRINTLN(on_time);

	this->Dioden[(id - 1)].is_toggling = true;
	this->Dioden[(id - 1)].toggling_time = on_time;

}


void LED::LEDshouldBlink(){
    // checks if a led has to blink

	// no debug print here, as it would clutter the readybillity
	//on the serial monitor

	this->currentMillis = millis();

	for(int i = 0; i < LedCount; i++){
		if(this->Dioden[i].is_toggling){LEDToggling(&this->Dioden[i]);}
	}

}


void LED::LEDToggling(LED::LEDFunctionConfig *led){
	// toggles between the on and off state of the given led

	// no debug print here, as it would clutter the readybillity
	//on the serial monitor

	if((this->currentMillis - led->previousMillis) >= led->toggling_time){
		led->previousMillis = this->currentMillis;
		if (led->state == LOW){
			LEDSwitch(led, 1);
		} else {
			LEDSwitch(led, 0);
		}
		//DEBUG_PRINT("blink!");
	}
}


void LED::LEDFunctionSet(int id, int state) {
	// sets the the corresponding led to the given state
	// and turns off toggling
	
	DEBUG_PRINT("\n# In the LEDFunctionSet");
	DEBUG_PRINT("\n# With ID: ");
	DEBUG_PRINTLN(id);
	
	DEBUG_PRINT("\n# Setting the led to state: ");
	DEBUG_PRINTLN(state);
	

	this->Dioden[(id - 1)].is_toggling = false;
	LEDSwitch(&this->Dioden[(id - 1)], state);
}


void LED::LEDSwitch(LED::LEDFunctionConfig *led, int state){
	// switches the given led into the given state

	if(state == 0){
		led->state = LOW;
		digitalWrite(led->pin, LOW);
        
	} else {
		led->state = HIGH;
		digitalWrite(led->pin, HIGH);
	}
}

