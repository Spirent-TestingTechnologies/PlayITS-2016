#ifndef CarSimTheft
#define CarSimTheft


#include <CarSimLED.h>
#include <CarSimButton.h>
#include <CarSimRFID.h>


class Theft {

	private:
		
		struct TheftDetectionConfig{
			int ID;
			int bt_changed;
			unsigned long bt_time;
			int rfid_matched;
			//LEDFunctionConfig* led;
			//ButtonConfig* button;
			//RFIDConfig* struct

			TheftDetectionConfig(){}
			
			void initialize(int cID){
				ID = cID;
				bt_changed = 0;
				bt_time = 0;
				rfid_matched = 0;
				//led = &cLED;
				//button = &cButton;
			}
		};
		
		Button &button;
		LED &led;
		RFID &rfid;
	
	public:
	
		int THEFT_ENABLED;
		TheftDetectionConfig Theft01;
		
		Theft(int cID, Button &button, LED &led, RFID &rfid);
		void TheftDetectionFunction(int id, int command);

};



#endif