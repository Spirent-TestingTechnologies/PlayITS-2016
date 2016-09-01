#include "CarSimLED.h"

/**
* Definition of the arrays is in the header-file
*/
LED::LED(int LEDIDs[], int LEDPins[], int BiLEDIDs[], int BiLEDPins1[], int BiLEDPins2[]){

	this->LedCount = (int)(sizeof(LEDIDs)+sizeof(BiLEDIDs)); 
  
	//erstelle neues Array von Structs
	this->Dioden = new LED::LEDFunctionConfig[LedCount];	
	
	// anzahl normaler LED
	int a = sizeof(LEDIDs);
	
	for(int i = 0; i < a; i++){
		// solange die ID noch nicht im Bidirektionalem Bereich des Arrays sind
		// springe in die erste Initialisierung, ansonsten in die zweite
		this->Dioden[i].initialize(LEDIDs[i], LEDPins[i], 0);
	}
	for(int i = 0; i < sizeof(BiLEDIDs); i++){
		// Im bidirektionalen Bereich des Arrays angekommen, also werden die LED jetzt 
		// anders initialisiert
		this->Dioden[i+a].initialize(BiLEDIDs[i], BiLEDPins1[i], BiLEDPins2[i]);
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
			LEDFunctionBlink(id, (1000/(XSERIAL.parseFloat()*2)), XSERIAL.parseInt());
			break;
		case STOP:
			LEDFunctionSet(id, 0);
			break;
		default:
			break;
  }
}

int LED::dioden_index(int id){
	// gibt den den korrespondierenden Platz zur ID in dem Array der Dioden wieder
	for(int i = 0; i < LedCount; i++){
		if(this->Dioden[i].ID == id){
			return i;
		}
	}
}


void LED::LEDFunctionBlink(int id, float on_time, int mode) {
	// tells the given led, that it now has to blink

	DEBUG_PRINT("\n# In the LEDFunctionBlink");
	DEBUG_PRINT("\n# With ID: ");
	DEBUG_PRINTLN(id);
	DEBUG_PRINT("\n# and with an on_time of: ");
	DEBUG_PRINTLN(on_time);

	this->Dioden[dioden_index(id)].is_toggling = true;
	this->Dioden[dioden_index(id)].toggling_time = on_time;
    this->Dioden[dioden_index(id)].toggle_mode = mode;
}


void LED::LEDshouldBlink(){
    // checks if a led has to blink

	// no debug print here, as it would clutter the readabillity
	//on the serial monitor

	this->currentMillis = millis();

	for(int i = 0; i < LedCount; i++){
		if(this->Dioden[i].is_toggling){LEDToggling(&this->Dioden[i]);}
	}

}


void LED::LEDToggling(LED::LEDFunctionConfig *led){
	// toggles between the on and off state of the given led

	// no debug print here, as it would clutter the readabillity
	//on the serial monitor

	if((this->currentMillis - led->previousMillis) >= led->toggling_time){
		led->previousMillis = this->currentMillis;
		if(led->pin2 == 0 || led->toggle_mode == 1){
			if (led->state == 0){
				LEDSwitch(led, 1);
			} else {
				LEDSwitch(led, 0);
			}
		}
		else if(led->toggle_mode == 2){
			if (led->state == 0){
				LEDSwitch(led, 2);
			} else {
				LEDSwitch(led, 0);
			}
		}
		else if(led->toggle_mode == 3){
			if (led->state == 2){
				LEDSwitch(led, 1);
			} else {
				LEDSwitch(led, 2);
			}
		}
		//DEBUG_PRINT("blink!");
	}
}


void LED::LEDFunctionSet(int id, int state) {
	// sets the the corresponding led to the given state
	// and turns off toggling
	/*
	DEBUG_PRINT("\n# In the LEDFunctionSet");
	DEBUG_PRINT("\n# With ID: ");
	DEBUG_PRINTLN(id);
	
	DEBUG_PRINT("\n# Setting the led to state: ");
	DEBUG_PRINTLN(state);
	*/
	this->Dioden[dioden_index(id)].is_toggling = false;
	LEDSwitch(&this->Dioden[dioden_index(id)], state);
}


void LED::LEDSwitch(LED::LEDFunctionConfig *led, int state){
	// switches the given led into the given state
	// 0 => off
	// 1 => first colour
	// 2 => second colour
	
	/*
	DEBUG_PRINT("#Anzahl der LED: ");
	DEBUG_PRINTLN(sizeof(this->Dioden));
	
	DEBUG_PRINT("#Der zweite Pin des zweiten Elementes: ");
	DEBUG_PRINTLN(this->Dioden[1].pin2);
	
	DEBUG_PRINT("\n# LED has the following Pins: ");
	DEBUG_PRINT(led->pin);
	DEBUG_PRINT(" , ");
	DEBUG_PRINTLN(led->pin2);
	*/

	if(state == 0){
		led->state = 0;
		digitalWrite(led->pin, LOW);
		if(led->pin2 != 0){
			digitalWrite(led->pin2, LOW);
		}	
	}
	else if(state == 1){
		led->state = 1;
		digitalWrite(led->pin, HIGH);
		if(led->pin2 != 0){
			digitalWrite(led->pin2, LOW);
		}
	}
	else if((state == 2) && (led->pin2 != 0)){
		led->state = 2;
		digitalWrite(led->pin, LOW);
		digitalWrite(led->pin2, HIGH);
	}
	else {
		XSERIAL.println("Diese Möglichkeit gibt es bei dieser LED nicht.");
	}
}


