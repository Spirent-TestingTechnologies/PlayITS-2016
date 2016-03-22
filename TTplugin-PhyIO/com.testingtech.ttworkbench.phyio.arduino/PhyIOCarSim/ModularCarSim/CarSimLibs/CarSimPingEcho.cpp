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
		case SETUP:
			PingEchoFunctionSetup2();
			break;
		case START:
			PingEchoFunctionStart2();
			break;
		case STOP:
			PingEchoFunctionStop2();
			break;
		case READ:
			PingEchoFunctionRead2();
			break;
		default:
			break;
	}
	// Read parameters
}

void PingEcho::PingEchoFunctionSetup2() {
	
	this->pePoint.initialize();
	
	#ifdef DEBUG
		DEBUG_PRINTLN("#In the PingEchpFunctionSetup");
	  
		DEBUG_PRINT("#parameters are ");
		DEBUG_PRINT(" time = ");
		DEBUG_PRINT(this->pePoint.TimeBetweenSamples);

		DEBUG_PRINT("# ; medianIterations= ");
		DEBUG_PRINTLN(this->pePoint.medianIterations);

		DEBUG_PRINT("# ; maxDistance= ");
		DEBUG_PRINTLN(this->pePoint.maxDistance);

		DEBUG_PRINT("# ; unit= ");
		DEBUG_PRINTLN(this->pePoint.unit);
	#endif	
	
}

void PingEcho::PingEchoFunctionStart2(){
	#ifdef DEBUG
		DEBUG_PRINTLN("#Started constanst distance measuring.");
	#endif
		pe_enabled = true;
}

void PingEcho::PingEchoFunctionStop2(){
	#ifdef DEBUG
		DEBUG_PRINTLN("#Stopped constanst distance measuring.");
	#endif
		pe_enabled = false;
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

	PingEchoFunctionR1();
}

void PingEcho::PingEchoFunctionR1() {
	
	// ID, PE01,R1,<timestamp:unit16>,<distance:uint16>
	XSERIAL.print(this->pePoint.ID); XSERIAL.print(", ");
	XSERIAL.print(PE02);  XSERIAL.print(", ");
	XSERIAL.print(R1);  XSERIAL.print(", ");
	XSERIAL.print(this->pePoint.ReadTimestamp);  XSERIAL.print(", ");
	XSERIAL.println(this->pePoint.distance);
	XSERIAL.flush();
	
}

void PingEcho::computeTimer(){
//  if ( this->pePoint.enabled && millis() >= (this->pePoint.TimeBetweenSamples + this->pePoint.ReadTimestamp){
//    PingEchoFunctionRead();
//  }

	if (pe_enabled && millis() >= peTimer){
		peTimer += PE_TIME_BETWEEN_SAMPLES;
		PingEchoFunctionRead2();
	}
}