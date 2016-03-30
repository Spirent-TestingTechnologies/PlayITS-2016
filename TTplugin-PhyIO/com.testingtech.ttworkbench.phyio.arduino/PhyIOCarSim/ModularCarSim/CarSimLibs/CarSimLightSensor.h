#ifndef CarSimLightSensor
#define CarSimLightSensor

#include "Arduino.h"
#include "CarSimDefine.h"

class LightSensor {
	
	private:

		unsigned long& readTimer;
		unsigned int readSpeed;
	
	public:
	
		LightSensor(unsigned long& readTimer, unsigned int readSpeed);
		void LightSensorFunction( int id, int command);
		void LightFunctionPrint(int id,int value,unsigned long time);
		void LightFunctionPrint2(int id,float value,unsigned long time);
		void LightFunctionStart(int id,int lightValue,int lightDiff);
		void LightFunctionFrequenz(int id,int top,int down,boolean upOrDown,unsigned long startTime,unsigned long endTime);
		boolean checkError(unsigned long timeDiff,unsigned long timeDiffNow);
		float getFrequency(unsigned long time,int counter);
		boolean readInput(float frequency,boolean error);
	
};

#endif