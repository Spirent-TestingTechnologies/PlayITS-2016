#include "CarSimButton.h"

Button::Button(int ButtonIDs[], int ButtonPins[]){
	#ifndef BT_PRESENT
		DEBUG_PRINTLN("#Simulating Button");
		randomSeed(analogRead(Button1Pin));
	#endif
	
	this->ButtonCount = (int)sizeof(ButtonIDs);
	
	//erstele neues Array von Structs
	this->Buttons = new Button::ButtonConfig[ButtonCount];
	
	for(int i = 0; i < ButtonCount; i++){
		this->Buttons[i].initialize(ButtonIDs[i], ButtonPins[i]);
	}
	
	
}

void Button::ButtonFunction(int id, int command){
	DEBUG_PRINT("#In ButtonFunction with function ");
	DEBUG_PRINTLN(command);
	
	DEBUG_PRINT("# ID: "); DEBUG_PRINT(id); DEBUG_PRINT(", gehört zum Index ");
	DEBUG_PRINTLN(i);
	
	
	switch(command){
		case START:
			ButtonFunctionStart(id);
			break;
		case STOP:
			ButtonFunctionStop(id);
			break;
		case READ:
			ButtonFunctionRead(id);
			break;
		default:
			break;
	}
}

void Button::ButtonFunctionStart(int id){
	this->Buttons[button_index(id)].is_enabled = true;
}

void Button::ButtonFunctionStop(int id){
	this->Buttons[button_index(id)].is_enabled = false;
}



int Button::button_index(int id){
	// gibt den den korrespondierenden Platz zur ID in dem Array der Buttons wieder
	for(int i = 0; i < ButtonCount; i++){
		if(this->Buttons[i].ID == id){
			return i;
		}
	}
}


bool Button::ButtonFunctionCheck(int id){
	
	int current_state;
	
	#define bt this->Buttons[button_index(id)]
	
	DEBUG_PRINT("# ButtonFunctionCheck, mit ID: ");
	DEBUG_PRINTLN(id);
	
	DEBUG_PRINT("# Zugehöriger Pin zur ID: ");
	DEBUG_PRINTLN(bt.pin);

	#ifdef BT_PRESENT
		current_state = digitalRead(bt.pin);
	#else
		// Simulated button
		DEBUG_PRINT("# randomize ");
		this->randNumber = random(100);
		if(this->randNumber < 1){
			current_state = bt.state^1;
		}else{
			current_state = bt.state;
		}
	#endif
		
	bt.state = current_state;
	
	return bt.state;
			
	#undef bt
}


int Button::ButtonFunctionProcess(int id){
	int current_state;
	
	#define bt this->Buttons[button_index(id)]

	#ifdef BT_PRESENT
		current_state = digitalRead(bt.pin);
	#else
		// Simulated button
		this->randNumber = random(100);
		if(this->randNumber < 1){
			current_state = bt.state^1;
		}else{
			current_state = bt.state;
		}
	#endif

	// Only send if state changed
	if(bt.state != current_state){
		bt.state = current_state;
		//ID, DR01, R1, <pushed>
		
		#ifdef SAFETYSTRINGS
			XSERIAL.print(STARTSTRING);
		#endif
		
		XSERIAL.print(bt.ID); XSERIAL.print(",");
		XSERIAL.print(DR01); XSERIAL.print(",");
		XSERIAL.print(R1); XSERIAL.print(",");
		
		
		#ifdef SAFETYSTRINGS
			XSERIAL.print(bt.state);
			XSERIAL.println(ENDSTRING);
		#else
			XSERIAL.println(bt.state);
		#endif

		return 1;
	}
	return 0;
	
	#undef bt
}

void Button::ButtonProcessing(){
	for(int i = 0; i < ButtonCount; i++){
		if(this->Buttons[i].is_enabled){
			ButtonFunctionProcess(this->Buttons[i].ID);
			//DEBUG_PRINTLN("# Button wurde durch Processing gecheckt");
		}
		/*
		DEBUG_PRINT("# Iteration: "); DEBUG_PRINT(i); DEBUG_PRINT(", ");
		DEBUG_PRINT(this->Buttons[i].ID); DEBUG_PRINT(", enabled: "); 
		DEBUG_PRINTLN(this->Buttons[i].is_enabled);
		*/
	}
}

void Button::ButtonFunctionRead(int id){
	ButtonFunctionCheck(id);
	
	#ifdef SAFETYSTRINGS
		XSERIAL.print(STARTSTRING);
		XSERIAL.print(this->Buttons[button_index(id)].state);
		XSERIAL.println(ENDSTRING);
	#else
		XSERIAL.println(this->Buttons[button_index(id)].state);
	#endif
}