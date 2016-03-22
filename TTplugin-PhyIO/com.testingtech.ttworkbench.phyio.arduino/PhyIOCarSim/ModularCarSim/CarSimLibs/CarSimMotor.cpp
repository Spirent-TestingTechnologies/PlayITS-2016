#include "CarSimMotor.h"

Motor::Motor(){
	
	pinMode(enablePin, OUTPUT);
	pinMode(in1Pin, OUTPUT);
	pinMode(in2Pin, OUTPUT);
	
}

void Motor::MotorFunction( int id, int command) {
	mPoint.ID = id;
	DEBUG_PRINT("#In the MotorFunction with function ");
	DEBUG_PRINTLN(command);

	switch (command) {
		case SET:
			MotorFunctionSet();
			break;
		default:
			break;
	}
}	

void Motor::MotorFunctionSet() {
	DEBUG_PRINTLN("#In the MotorFunctionSetup");

	//ID, MM01, SET, <velocity:uint16>
	//Configures the Motor Function

	mPoint.velocity = XSERIAL.parseInt();
	DEBUG_PRINT("#parameters are ");
	DEBUG_PRINT(" velocity = ");
	DEBUG_PRINTLN(mPoint.velocity);

	int  vel = (abs(mPoint.velocity) % 100) * 2.55;
	int reverse = mPoint.velocity > 0 ? 1 : 0;

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