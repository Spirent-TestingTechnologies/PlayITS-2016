#include "CarSimPingEcho.h"


PingEcho::PingEcho() : sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE){
	
	this->pe_enabled = false;
	this->peTimer = 0;
	this->pePoint.initialize();	
	
}


void PingEcho::PingEchoFunction2( int id, int command) {
	
	this->pePoint.ID = id;
	DEBUG_PRINT("#In the PingEchoFunction with function ");
	DEBUG_PRINTLN(command);

	switch (command) {
		case START:
			PingEchoFunctionStart2();
			break;
		case STOP:
			PingEchoFunctionStop2();
			break;
		case READ:
			PingEchoFunctionReadPrint();
			break;
		default:
			break;
	}
	// Read parameters
}

void PingEcho::PingEchoFunctionStart2(){
	#ifdef DEBUG
		DEBUG_PRINTLN("#Started constanst distance measuring.");
	#endif
		pePoint.enabled = true;
}

void PingEcho::PingEchoFunctionStop2(){
	#ifdef DEBUG
		DEBUG_PRINTLN("#Stopped constanst distance measuring.");
	#endif
		pePoint.enabled = false;
}

void PingEcho::pintEchoSensorFix(){
	pinMode(ECHO_PIN,OUTPUT);
	digitalWrite(ECHO_PIN,LOW);
	pinMode(ECHO_PIN,INPUT);
}


void PingEcho::PingEchoFunctionRead2() {

	DEBUG_PRINTLN("#In the PingEchoFunctionRead");

	#ifdef PE_PRESENT
		unsigned int uS = sonar.ping_median(this->pePoint.medianIterations);
	#else
		unsigned int uS = random(10000000);
	#endif

	this->pePoint.ReadTimestamp = millis();
	switch (pePoint.unit) {
		case 2:
			this->pePoint.distance = sonar.convert_in(uS);
			break;
		case 3:
			this->pePoint.distance = uS;
			break;
		default:
			#ifdef PE_PRESENT
				this->pePoint.distance = sonar.convert_cm(uS);
			#else
				this->pePoint.distance = uS;
			#endif
		break;
	}
}

void PingEcho::PingEchoFunctionR1() {
	
	#ifdef SAFETYSTRINGS
		XSERIAL.print(STARTSTRING);
	#endif
	
	// ID, PE01,R1,<timestamp:unit16>,<distance:uint16>
	XSERIAL.print(this->pePoint.ID); XSERIAL.print(", ");
	XSERIAL.print(PE02);  XSERIAL.print(", ");
	XSERIAL.print(R1);  XSERIAL.print(", ");
	// if the timestamp is ever needed again this is the code line one has to use
	//XSERIAL.print(this->pePoint.ReadTimestamp);  XSERIAL.print(", ");
	
	
	#ifdef SAFETYSTRINGS
		XSERIAL.print(this->pePoint.distance);
		XSERIAL.println(ENDSTRING);
	#else
		XSERIAL.println(this->pePoint.distance);
	#endif
}

void PingEcho::PingEchoFunctionReadPrint(){
	PingEchoFunctionRead2();
	
	#ifdef SAFETYSTRINGS
		XSERIAL.print(STARTSTRING);
		XSERIAL.print(this->pePoint.distance);
		XSERIAL.println(ENDSTRING);
	#else
		XSERIAL.println(this->pePoint.distance);
	#endif
}


int PingEcho::PingEchoReturnDistance(){
	PingEchoFunctionRead2();
	return this->pePoint.distance;
}

void PingEcho::computeTimer(){
//  if ( this->pePoint.enabled && millis() >= (this->pePoint.TimeBetweenSamples + this->pePoint.ReadTimestamp){
//    PingEchoFunctionRead();
//  }
	if(pePoint.enabled && 
	  (millis() - pePoint.ReadTimestamp >= pePoint.TimeBetweenSamples)){
		pePoint.ReadTimestamp = millis();
		PingEchoFunctionReadPrint();
	}
}