#include "StripeLEDRemote.h"


LEDRemote::LEDRemote() 
	: colours{X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16},
	stripe(){}
	


void LEDRemote::sending(long command){
	for (int i = 0; i < 3; i++) {
		irsend.sendNEC(command, 32);
		delay(30);
	}
}

long LEDRemote::assignValue(int colour){
	if(colour > 16 || colour < 1){
		return XON;
	}
	return colours[colour-1];
}


void LEDRemote::StripeFunction(int command){
	
	DEBUG_PRINT("\n# In the LEDFunction with function : '");
	DEBUG_PRINTLN(command);
	
	switch(command) {
		
		case SET:
			StripeFunctionSet(XSERIAL.parseInt());
			break;
		case BLINK:
			StripeFunctionBlink();
			break;			
		case STOP:
			StripeFunctionOff();
			stripe.Reset();
			break;
		default:
			break;
	}

}


void LEDRemote::StripeFunctionOn(){
	sending(XON);
	this->stripe.state = XON;
}
void LEDRemote::StripeFunctionOff(){
	sending(XOFF);
	this->stripe.state = XOFF;
}
void LEDRemote::StripeFunctionSTROBE(){
	sending(XSTROBE);
	this->stripe.state = XSTROBE;
}
void LEDRemote::StripeFunctionFLASH(){
	sending(XFLASH);
	this->stripe.state = XFLASH;
}
void LEDRemote::StripeFunctionFADE(){
	sending(XFADE);
	this->stripe.state = XFADE;
}
void LEDRemote::StripeFunctionSMOOTH(){
	sending(XSMOOTH);
	this->stripe.state = XSMOOTH;
}
void LEDRemote::StripeFunctionColour(int colour){
	long c = assignValue(colour); 
	sending(c);
	this->stripe.state = c;
}


void LEDRemote::StripeFunctionSet(int mode){
	if(stripe.state == XOFF){
		StripeFunctionOn();
	}
	stripe.Reset();
	switch(mode){
		case COLOUR:
			StripeFunctionColour(XSERIAL.parseInt());
			break;
		case STROBE:
			StripeFunctionSTROBE();
			break;
		case FLASH:
			StripeFunctionFLASH();
			break;
		case FADE:
			StripeFunctionFADE();
			break;
		case SMOOTH:
			StripeFunctionSMOOTH();
			break;
	}
}


void LEDRemote::StripeFunctionBlink(){
	float freq = XSERIAL.parseFloat();
	int colour_amount = XSERIAL.parseInt();
	
	
	if(stripe.state == XOFF){
		StripeFunctionOn();
	}
	stripe.Reset();
	
	this->stripe.toggling_colours = new int[colour_amount];
	this->stripe.toggling_size = colour_amount;
	
	
	for (int i = 0; i < colour_amount; i++){
		this->stripe.toggling_colours[i] = XSERIAL.parseInt();
	}
	
	this->stripe.toggle_time = 1000/freq;
	
	
	
	this->stripe.is_toggling = true;
	
} // kann vielleicht nicht an und aus blinken



void LEDRemote::StripeScheduler(){
	if(!this->stripe.is_toggling){
		return;
	}
	if(!((millis() - this->stripe.previousMillis) >= this->stripe.toggle_time)){
		return;
	}
	StripeFunctionColour(this->stripe.toggling_colours[this->stripe.toggling_index]);
	this->stripe.previousMillis = millis();
	
	int temp = this->stripe.toggling_index + 1;
	this->stripe.toggling_index = temp%this->stripe.toggling_size;
}