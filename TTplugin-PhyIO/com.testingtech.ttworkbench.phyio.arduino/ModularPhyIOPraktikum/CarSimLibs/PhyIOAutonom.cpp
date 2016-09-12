#include "PhyIOAutonom.h"

Autonom::Autonom(Car* car){
	this->c = car;
}


void Autonom::start(){
	// set the colour to white and reset distance
	c->remote.StripeFunctionOn();
	distance = 100;
	c->remote.StripeFunctionColour(16);
	
	update_time(); 
	time_check = false;
	
	// check if there is an object within reach of the ping echo distance
	c->echo.PingEchoFunctionStart2();
	while(distance > maxHeight && is_autonom()){
		c->echo.PingEchoFunctionRead2();
		distance = c->echo.PingEchoReturnDistance();
		
		// If there didn't happen anything for a certain amount of time, then
		// switch the box-light to a slow fade
		// the time check is for making sure, that it only switches once
		if (!time_check && ((millis() - current_time)/60000.0 > idleTime1)){
			c->remote.StripeFunctionFADE();
			time_check = true;
		}
		
		// if there didn't happen anything for another certain amount of time
		// then start the whole autonom mode with a random value
		if ((millis() - current_time)/60000.0 > idleTime2){
			distance = random(maxHeight);
		}
	}
	c->echo.PingEchoFunctionStop2();	
	

	// break and set everything back, if the PhyIO is no longer in autonomus mode
	if(!is_autonom()){
		c->remote.StripeFunctionOff();
		return;
	}
	
	// if so then set the speed of the motor relative to the distance 
	// of the hand to the sensor, also let the lights blink while spinning
	speed = (minSpeed - maxSpeed)*distance/maxHeight + maxSpeed;
	c->motor.MotorFunctionSet(speed);
	c->remote.StripeFunctionSTROBE();
	
	// let the motor speed up a bit and indicate readyness though colour of the bi-
	// directional led
	update_time(); 
	c->led.LEDFunctionSet(2, 2);
	while((millis() - current_time)/1000.0 < 4.5){
		// break and set everything back, if the PhyIO is no longer in autonomus mode
		if(!is_autonom()){
			c->led.LEDFunctionSet(2, 0);
			c->remote.StripeFunctionOff();
			c->motor.MotorFunctionBreak();
			return;
		}
	}
	// set the led to green, to indicate that the distance sensor is acive again
	c->led.LEDFunctionSet(2, 1);
	
	// check if the motor needs to be stoped
	// either through hand motion or through waiting
	distance = 100;
	update_time();
	c->echo.PingEchoFunctionStart2();
	
	while((distance > maxHeight) && ((millis() - current_time)/1000.0 < waitTime) && is_autonom()){
		c->echo.PingEchoFunctionRead2();
		distance = c->echo.PingEchoReturnDistance();
		delay(10);
	}
	c->echo.PingEchoFunctionStop2();
	
	
	
	// break
	// while waiting, turn on the blue led and set the stripe to a distinct colour (max 1 sec)
	c->led.LEDFunctionSet(2, 0);
	c->remote.StripeFunctionColour(8);
	c->led.LEDFunctionSet(1, 1);
	c->echo.PingEchoFunctionStop2();
	c->motor.MotorFunctionBreak();
	

	// break and set everything back, if the PhyIO is no longer in autonomus mode
	if(!is_autonom()){
		c->led.LEDFunctionSet(1, 0);
		c->remote.StripeFunctionOff();
		return;
	}
	
	// delay to make everything more smooth
	delay(750);
	
	// set everything back, if the PhyIO is no longer in autonomus mode
	if(!is_autonom()){
		c->led.LEDFunctionSet(1, 0);
		c->remote.StripeFunctionOff();
		return;
	}
	
	// read the colour on the backsite of the disk 
	c->cv.ColorViewFunctionRead();
	
	// change the colour of the box to either green or red depending on the 
	// position of the disk
	if(c->cv.ColorViewDecide()){
		c->remote.StripeFunctionColour(6);
	} else {
		c->remote.StripeFunctionColour(1);
	}
	c->led.LEDFunctionSet(1, 0);
	
	
	update_time();
	
	// wait a bit or break if nolonger autonomus
	while((millis() - current_time)/1000.0 < endingTime){
		
		if(!is_autonom()){
			c->remote.StripeFunctionOff();
			return;
		}
		delay(100);
	}
	
}


bool Autonom::is_autonom(){
	//XSERIAL.print("Ist noch Autonom?");
	delay(5); // don't know why, but without this delay, 
	// the whole autonomus modus doesn't work anymore
	if(c->button.ButtonFunctionCheck(1)){
	//	XSERIAL.println(" nein");
		return false;
	}
	//XSERIAL.println(" ja");
	return true;
}


void Autonom::update_time(){
	current_time = millis();
}

void Autonom::reset(){
		c->cv.ColorViewFunction(1, 3);
		c->led.LEDFunctionSet(1, 0);
		c->led.LEDFunctionSet(2, 0);
		c->button.ButtonFunction(1, 3);
		c->button.ButtonFunction(2, 3);
		c->echo.PingEchoFunctionStop2();
		c->motor.MotorFunctionBreak();
		c->remote.StripeFunctionOn();
		c->remote.StripeFunctionColour(16);
}