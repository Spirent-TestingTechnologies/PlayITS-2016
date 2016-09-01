#include "CarSimTheft.h"

Theft::Theft(int cID, Button &button, LED &led, RFID &rfid) : button(button), led(led), rfid(rfid) {
	
	this->THEFT_ENABLED = 0;
	this->Theft01.initialize(cID);
}

//<ID>, <TD01>, START ,<int: tag1>,<int: tag2>,<int: tag3>,<int: tag4>
void Theft::TheftDetectionFunction(int id, int command){
	DEBUG_PRINT("#In the TheftDetectionFunction with function : '");
	DEBUG_PRINT(command);

	DEBUG_PRINT("' and with ID: ");
	DEBUG_PRINTLN(id);

	switch (command) {
		case START:
			rfid.RFIDFunctionSetup(); // Read and set rfid tag from serial
			rfid.RFID_ENABLED = 1;
			//button.BUTTON_ENABLED = 1; // Enables button
			this->THEFT_ENABLED = 1;
			break;
		case STOP:
			this->THEFT_ENABLED = 0; // Disable Theft Mode
			//button.BUTTON_ENABLED = 0; // Disable Button
			led.LEDFunctionSet(1,0); // Turn off LED
			rfid.RFID_ENABLED = 0;
		  
			break;
		default:
			break;
	 
	}
}

