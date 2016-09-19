#ifndef CarSim
#define CarSim

#include "Arduino.h"
#include "CarSimDefine.h"

#ifdef LED_Module
	#include <CarSimLED.h>
#endif
#ifdef Button_Module
	#include <CarSimButton.h>
#endif
#ifdef RFID_Module
	#include <CarSimRFID.h>
#endif
#ifdef Theft_Module
	#include <CarSimTheft.h>
#endif
#ifdef PingEcho_Module
	#include <CarSimPingEcho.h>
#endif
#ifdef LighSensor_Module
	#include <CarSimLightSensor.h>
#endif
#ifdef ColorView_Module
	#include <CarSimColorView.h>
#endif
#ifdef Relay_Module
	#include <CarSimRelay.h>
#endif
#ifdef Motor_Module
	#include <CarSimMotor.h>
#endif
#ifdef Stripe_Module
	#include <StripeLEDRemote.h>
#endif
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
			int BiLEDIDs[BiLedAmount];
			int BiLEDPins1[BiLedAmount];
			int BiLEDPins2[BiLedAmount];
		#endif
			
		#ifdef Button_Module
			int ButtonIDs[ButtonAmount];
			int ButtonPins[ButtonAmount];
		#endif
		
		
		//Module
		

		
	public:
		
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

		#ifdef Stripe_Module
			LEDRemote remote;
		#endif
	
	
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
		#ifdef Stripe_Module
			void StripeFunction(int command);
		#endif
		
		//Quasi-Multithreading
		void ProcessHandling();
		
};


#endif
