#ifndef CarSimRFID
#define CarSimRFID

#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>
#include "Arduino.h"
#include "CarSimDefine.h"

class RFID {
	
	private:
	
		long randNumber;
	
		// Needs to be refactored to support multiple rfid sensors using a rfid struct
		// ID Karte: 194, 96, 196, 169 (DEZ)
		// ID Chip: 4, 226, 92, 235 (DEZ)

		int rfid_format = DEC;
		int rfid_tag[4]; // Sollte noch in Struct umgewandelt werden
		// bin = 2; dec = 10; oct = 8; hex = 16;
	
	public:
		
		MFRC522 mfrc522; // creates and holds mfrc522 data
		int RFID_ENABLED;
		
		RFID();
		
		void RFIDFunctionTagClear();
		void RFIDFunction(int id, int command);
		void RFIDSetup();
		void RFIDFunctionSetup();
		int RFIDFunctionProcess(int id);
		
};


#endif