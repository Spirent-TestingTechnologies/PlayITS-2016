#ifndef CarSimMotor
#define CarSimMotor

#include "Arduino.h"
#include "CarSimDefine.h"

class Motor {

	private:
	
		struct MotorFunctionConfig{
			int ID; // The ID
			int velocity; // +% of Max for right, -% of Max for left

			void Reset()  {
				// Set default values in case there is nothing
				// stored in the eeprom yet.
				velocity = 0;
			}
		};
		
		MotorFunctionConfig motor;

	public:
	
		Motor();
		void MotorFunction(int id, int command);
		void MotorFunctionSet(int vel);
		void MotorFunctionBreak();

};

#endif