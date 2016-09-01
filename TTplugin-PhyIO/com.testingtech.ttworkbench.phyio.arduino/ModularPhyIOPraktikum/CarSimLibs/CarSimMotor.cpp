#include "CarSimMotor.h"

Motor::Motor(){
	
	pinMode(enablePin, OUTPUT);
	pinMode(in1Pin, OUTPUT);
	pinMode(in2Pin, OUTPUT);
	
}

void Motor::MotorFunction(int id, int command) {
	motor.ID = id;
	DEBUG_PRINT("#In the MotorFunction with function ");
	DEBUG_PRINTLN(command);

	switch (command) {
		case SET:
			MotorFunctionSet();
			break;
			
		case STOP:
			MotorFunctionBreak();
			break;
		
		default:
			break;
	}
}	

void Motor::MotorFunctionSet() {
	DEBUG_PRINTLN("#In the MotorFunctionSetup");

	//ID, MM01, SET, <velocity:uint16>
	//Configures the Motor Function

	motor.velocity = XSERIAL.parseInt();
	DEBUG_PRINT("#parameters are ");
	DEBUG_PRINT(" velocity = ");
	DEBUG_PRINTLN(motor.velocity);

	int  vel = (abs(motor.velocity) % 100) * 2.55;
	int reverse = motor.velocity > 0 ? 1 : 0;

	#ifdef MOTOR_PRESENT
		analogWrite(enablePin, vel);
		digitalWrite(in1Pin, ! reverse);
		digitalWrite(in2Pin, reverse);
	
	#endif
		
	DEBUG_PRINT("#Rotating with a velocity of ");
	DEBUG_PRINTLN(vel);
	DEBUG_PRINT("#Into the direction of ");
	DEBUG_PRINTLN(reverse);
}


void Motor::MotorFunctionBreak(){
	
	int x = abs(motor.velocity);
	
	int  vel = (x % 100) * 2.55;
	int reverse = motor.velocity > 0 ? 1 : 0;
	
	int delay_time;
	
	//delay_time = (XSERIAL.parseInt());
	
	DEBUG_PRINTLN(x);
	
	if(x < 20){
		delay_time = 2;
	}
	else if(20 <= x && x <= 30){
		delay_time = x;
	}
	else if(30 < x && x < 65){
		delay_time = (int)((-1/2.0)*x +48);
	}
	else{
		delay_time = (int)((-1/5.0)*x +28);
	}
	
	//int delay_time = (int)(9);
	DEBUG_PRINTLN(delay_time);
	
	for(int i = 1; i <= vel; i++){
		
		analogWrite(enablePin, vel-i);
		digitalWrite(in1Pin, reverse);
		digitalWrite(in2Pin, ! reverse);

		
		delay(delay_time);
	
	}
		
	motor.velocity = 0;
}

/*
void Motor::MotorFunctionRead(){
	
}
*/
