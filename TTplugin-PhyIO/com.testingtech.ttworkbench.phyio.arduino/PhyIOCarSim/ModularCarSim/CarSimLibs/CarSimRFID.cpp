#include "CarSimRFID.h"

RFID::RFID(){
	
	this->RFID_ENABLED = 0;
	mfrc522 = MFRC522(RFID_SS_PIN, RFID_RST_PIN);
	
	SPI.begin();
	mfrc522.PCD_Init();
  
	#ifndef RFID_PRESENT // changeable in CarSimDefine
		DEBUG_PRINTLN("#Simulating RFID");
		randomSeed(RFID_SS_PIN);
	#endif
	
}

void RFID::RFIDFunctionTagClear(){
	for(int i = 0; i < 4; i++){
		this->rfid_tag[i] = 0;
	}
}

void RFID::RFIDFunction(int id, int command){
	DEBUG_PRINT("#In RFIDFunction with function ");
	DEBUG_PRINTLN(command);
	switch(command){
		case SETUP:
			RFIDFunctionTagClear();
			RFIDFunctionSetup();
			break;
		case START:
			this->RFID_ENABLED = 1; //start scanning for rfid card
			break;
		case STOP:
			this->RFID_ENABLED = 0; //stop scanning for rfid card
			RFIDFunctionTagClear();
			break;
		default:
			break;
	}
}

void RFID::RFIDFunctionSetup(){
	// ID, RF01, SETUP, <tag1>, <tag2>, <tag3>, <tag4>
	DEBUG_PRINTLN("#In RFIDFunctionSetup... ");
	DEBUG_PRINT("#Tag wird gelesen: ");
	for(int i = 0; i < 4; i++){
		rfid_tag[i] = XSERIAL.parseInt();
		DEBUG_PRINT(rfid_tag[i]); DEBUG_PRINT(" ");
	}
	DEBUG_PRINTLN("Tag erfolgreich gesetzt.");  
}

// Function returns 1 if the rfid read is matching the expected one, 0 if it does not match
// and only if in simulating mode it can return -1 when no simulated card was found nearby
int RFID::RFIDFunctionProcess(int id){
    int rfid_tag_tmp[4];

	#ifdef RFID_PRESENT    
		if(!mfrc522.PICC_ReadCardSerial()){
			return -1; // nothing to read
		}

		for (byte i = 0; i < mfrc522.uid.size; i++){
			rfid_tag_tmp[i] = (int) mfrc522.uid.uidByte[i]; 
		}
	#else
		randNumber = random(100);
		if(randNumber >= 2){
			return -1; // nothing to read
		}
		// Generate random rfid uid
		for (byte i = 0; i < 4; i++){
			if(randNumber == 0){
				rfid_tag_tmp[i] = random(256); 
			}else{
				rfid_tag_tmp[i] = rfid_tag[i];
			}
		}
	#endif

    unsigned long time = millis();
    
    // ID, RF01, R1, <tag1:int>, <tag2:int>, <tag3:int>, <tag4:int> <timestamp:long>
    XSERIAL.print(3); XSERIAL.print(", ");
    XSERIAL.print(RF01); XSERIAL.print(", ");
    XSERIAL.print(R1); XSERIAL.print(", ");
    XSERIAL.print(time); XSERIAL.print(", ");
    
	for(int i = 0; i < 4; i++){
		#ifdef RFID_PRESENT
			XSERIAL.print(mfrc522.uid.uidByte[i], rfid_format);
		#else
			XSERIAL.print(rfid_tag_tmp[i], rfid_format);
		#endif
			if(i < 3){
				XSERIAL.print(", ");
			}
    }
    
    XSERIAL.println("");
    XSERIAL.flush();

    if(rfid_tag[0] != 0 && rfid_tag[1] != 0 && rfid_tag[2] != 0 && rfid_tag[3] != 0){
		for(int i = 0; i < 4; i++){
			if(rfid_tag[i] != rfid_tag_tmp[i]){
				DEBUG_PRINT("#Tags stimmen nicht überein: ");
				DEBUG_PRINT(rfid_tag[i]); DEBUG_PRINT(" != ");
				DEBUG_PRINTLN(rfid_tag_tmp[i]);
				return 0;
			}
		}
		return 1;
    }
    return 0;

}