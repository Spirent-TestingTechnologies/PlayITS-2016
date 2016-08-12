#ifndef CarSimRelay
#define CarSimRelay

#include "Arduino.h"
#include "CarSimDefine.h"

class Relay {
	
	private:
	
		struct RelayFunctionConfig{
			int ID; // The ID
			int state;
			int pin;

			RelayFunctionConfig(int theID, int rPin) {
				ID = theID;
				pin = rPin;
			}
			
			void Reset(){
				// Set default values in case there is nothing store
				state = HIGH;
			}
		};

		//RelayFunctionConfig RelayPoint1(5, RELAY1PIN);
		//RelayFunctionConfig RelayPoint2(6, RELAY2PIN);
	
		RelayFunctionConfig RelayPoint1;
		RelayFunctionConfig RelayPoint2;
	
	public:
	
		Relay();
		void RelayFunction(int id, int command);
		void RelayFunctionSet(int id);
		

};

#endif