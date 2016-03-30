#include "CarSimButton.h"

Button::Button(int id, int pin){
	#ifndef BT_PRESENT
	DEBUG_PRINTLN("#Simulating Button");
	randomSeed(analogRead(Button1Pin));
	#endif
	
	this->Button1.initialize(id, pin);
	
	BUTTON_ENABLED = 0;
}

void Button::ButtonFunction(int id, int command){
	DEBUG_PRINT("#In ButtonFunction with function ");
	DEBUG_PRINTLN(command);
	switch(command){
		case START:
			BUTTON_ENABLED = 1;
			break;
		case STOP:
			BUTTON_ENABLED = 0;
			break;
		default:
			break;
	}
}

int Button::ButtonFunctionProcess(struct ButtonConfig *bt){
	int current_state;

	#ifdef BT_PRESENT
		current_state = digitalRead(bt->pin);
	#else
		// Simulated button
		this->randNumber = random(100);
		if(this->randNumber < 1){
			current_state = bt->state^1;
		}else{
			current_state = bt->state;
		}
	#endif

	// Only send if state changed
	if(bt->state != current_state){
		bt->state = current_state;
		//ID, DR01, R1, <pushed>
		XSERIAL.print(bt->ID); XSERIAL.print(",");
		XSERIAL.print(DR01); XSERIAL.print(",");
		XSERIAL.print(R1); XSERIAL.print(",");
		XSERIAL.println(bt->state);

		XSERIAL.flush();
		return 1;
	}
	return 0;
}