#ifndef CarSim
#define CarSim

#include "Arduino.h"
#include "CarSimDefine.h"
#include <CarSimLED.h>
#include <CarSimButton.h>
#include <CarSimRFID.h>
#include <CarSimTheft.h>
#include <CarSimPingEcho.h>
#include <CarSimLightSensor.h>
#include <CarSimColorView.h>
#include <CarSimRelay.h>
#include <CarSimMotor.h>

//hier sollte am besten das process handling rein


class Car {
	
	private:
	
		//Variablen
		
		#ifdef LighSensor_Module
			//Variable für Lightsensor ... der nicht mal funktioniert
			unsigned long readTimer; // holds the next read time;
			unsigned int readSpeed = 500; // How frequently are we going to read the serial line
		#endif
	
		#ifdef LED_Module
			//Variablen für Initialisierung der Module
			//led vordefinitionen
			int LEDIDs[LedAmount];
			int LEDPins[LedAmount];
		#endif
		
		
		//Module
		
		#ifdef Button_Module
			Button button;
		#endif
		
		#ifdef LED_Module
			LED led;
		#endif
		
		#ifdef RFID_Module
			RFID rfid;
		#endif
		
		#ifdef Theft_Module
			Theft theft;
		#endif
		
		#ifdef PingEcho_Module
			PingEcho echo;
		#endif
		
		#ifdef LighSensor_Module
			LightSensor ls;
		#endif
		
		#ifdef ColorView_Module
			ColorView cv;
		#endif
		
		#ifdef Relay_Module
			Relay relay;
		#endif
		
		#ifdef Motor_Module
			Motor motor;
		#endif
		
		
	public:
	
		Car();
		//Modulfunktionalitäten
		#ifdef ColorView_Module
			void ColorViewFunction(int id, int command);
			void ColorViewSetup();
		#endif
		#ifdef PingEcho_Module
			void PingEchoFunction2(int id, int command);
		#endif	
		#ifdef LED_Module
			void LEDFunction(int id, int command);
		#endif	
		#ifdef Relay_Module
			void RelayFunction(int id, int command);
		#endif	
		#ifdef Motor_Module
			void MotorFunction(int id, int command);
		#endif	
		#ifdef RFID_Module
			void RFIDFunction(int id, int command);
			void RFIDSetup();
		#endif	
		#ifdef Button_Module
			void ButtonFunction(int id, int command);
		#endif	
		#ifdef Theft_Module
			void TheftDetectionFunction(int id, int command);
			void TheftHandling(int functionType, int command);
		#endif	
		#ifdef LighSensor_Module
			void LightSensorFunction(int id, int command);
		#endif
		
		//Quasi-Multithreading
		void ProcessHandling();
		
};


#endif
