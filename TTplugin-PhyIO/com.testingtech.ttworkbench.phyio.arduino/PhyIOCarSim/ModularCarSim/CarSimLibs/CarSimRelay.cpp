#include "CarSimRelay.h"


Relay::Relay() : RelayPoint1(5, RELAY1PIN), RelayPoint2(6, RELAY2PIN) {

	pinMode(RELAY1PIN, OUTPUT);
	pinMode(RELAY2PIN, OUTPUT);

}

void Relay::RelayFunction(int id, int command) {
	DEBUG_PRINT("#In the RelayFunction with function ");
	DEBUG_PRINTLN(command);

	DEBUG_PRINT("#and with ID ");
	DEBUG_PRINTLN(id);

	switch (command) {
		case SET:
			RelayFunctionSet(id);
			break;
		default:
			break;
	}
}

void Relay::RelayFunctionSet(int id) {
	int oldState ;
	DEBUG_PRINTLN("#In the RelayFunctionSet");
	DEBUG_PRINT("# with id");
	DEBUG_PRINTLN(id);

	int state = XSERIAL.parseInt();
	DEBUG_PRINT("#parameters are ");
	DEBUG_PRINT(" state = ");
	DEBUG_PRINTLN(state);

	if (id == RelayPoint1.ID) {
		oldState = RelayPoint1.state ;
		RelayPoint1.state = state;
		digitalWrite(RelayPoint1.pin, state);
	} else if (id == RelayPoint2.ID) {
		oldState = RelayPoint2.state ;
		RelayPoint2.state = state;
		digitalWrite(RelayPoint2.pin, state);
	} else {
		DEBUG_PRINTLN("# DO NOT KNOW THIS ID");
		return;
	}
	
	DEBUG_PRINT("#Switching from ");
	DEBUG_PRINT(oldState) ;
	DEBUG_PRINT(" to ");
	DEBUG_PRINTLN(state);

}