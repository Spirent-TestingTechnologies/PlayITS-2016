#include "CarSim.h"

Car::Car() 
			#if defined(LED_Module) || defined(Button_Module) || defined(Theft_Module) || defined(LighSensor_Module)
				: 
			#endif
			#ifdef LED_Module
				LEDIDs({Led1ID}),
				LEDPins({Led1Pin}),
				BiLEDIDs({BiLed1ID}),
				BiLEDPins1({BiLed1DPin}),
				BiLEDPins2({BiLed1APin}),
				
				led(LEDIDs, LEDPins, BiLEDIDs, BiLEDPins1, BiLEDPins2)
			#endif
			
			#ifdef Button_Module
				#ifdef LED_Module
					,
				#endif
				ButtonIDs({Switch1ID, Button1ID}),
				ButtonPins({Switch1Pin, Button1Pin}),
				button(ButtonIDs, ButtonPins)
			#endif
			
			#ifdef Theft_Module
				, theft(Theft1ID, button, led, rfid)
			#endif
			
			#ifdef LighSensor_Module
				#if defined(LED_Module) || defined(Button_Module) || defined(Theft_Module) 
					, 
				#endif
				ls(readTimer, readSpeed)
			#endif
{}


#ifdef ColorView_Module
	void Car::ColorViewFunction(int id, int command){
		this->cv.ColorViewFunction(id, command);
	}

	void Car::ColorViewSetup(){
		cv.ColorViewSetup();
	}
#endif


#ifdef PingEcho_Module
	void Car::PingEchoFunction2(int id, int command){
		this->echo.PingEchoFunction2(id, command);
	}
#endif


#ifdef LED_Module
void Car::LEDFunction(int id, int command){
	this->led.LEDFunction(id, command);
}
#endif


#ifdef Relay_Module
void Car::RelayFunction(int id, int command){
	this->relay.RelayFunction(id, command);
}
#endif


#ifdef Motor_Module
void Car::MotorFunction(int id, int command){
	this->motor.MotorFunction(id, command);
}
#endif


#ifdef RFID_Module
void Car::RFIDFunction(int id, int command){
	this->rfid.RFIDFunction(id, command);
}

void Car::RFIDSetup(){
	rfid.RFIDSetup();
}
#endif


#ifdef Button_Module // -----------------------------------------------------
void Car::ButtonFunction(int id, int command){
	this->button.ButtonFunction(id, command);
}
#endif


#ifdef Theft_Module
void Car::TheftDetectionFunction(int id, int command){
	this->theft.TheftDetectionFunction(id, command);
}

void Car::TheftHandling(int functionType, int command){
	if(theft.THEFT_ENABLED){
		if(functionType == TD01 && command == STOP){
			theft.TheftDetectionFunction(1,command);
			DEBUG_PRINTLN("#Theft Detection stopped.");
		}else{
			DEBUG_PRINTLN("#Please exit theft mode first.");
		}
	}
}
#endif


#ifdef LighSensor_Module
void Car::LightSensorFunction(int id, int command){
	this->ls.LightSensorFunction(id, command);
}
#endif

#ifdef Stripe_Module
void Car::StripeFunction(int command){
	this->remote.StripeFunction(command);
}
#endif


void Car::ProcessHandling(){
	
	#ifdef Theft_Module
		theft.Theft01.bt_changed = button.ButtonFunctionProcess(Switch1ID);
		theft.Theft01.bt_time = millis(); // currently unused	
	#endif
	
	#ifdef RFID_Module
		#ifdef RFID_PRESENT
			if(rfid.RFID_ENABLED && rfid.mfrc522.PICC_IsNewCardPresent()){
		#else
			if(rfid.RFID_ENABLED){  
		#endif
			//Card detected, process data...
			int res = rfid.RFIDFunctionProcess(RF01);
				#ifdef Theft_Module
					if(theft.THEFT_ENABLED){
					theft.Theft01.rfid_matched = res;
					}
				#endif
			}
	#endif	
	
	#ifdef Theft_Module
		if(theft.THEFT_ENABLED){
			//194, 96, 196, 169
			if(theft.Theft01.bt_changed){
				DEBUG_PRINT("#Process Handling   rfid_matched: ");
				DEBUG_PRINTLN(theft.Theft01.rfid_matched);
				if(theft.Theft01.rfid_matched <= 0){
					DEBUG_PRINTLN("#Process Handling   Alarm!");
					led.LEDFunctionBlink(1,0.5);
				}else{
					DEBUG_PRINTLN("#Car unlocked.");
					led.LEDFunctionSet(1,0);
				}
			}
		}
	#endif
	
	#ifdef Button_Module
		button.ButtonProcessing();
	#endif
	
	#ifdef LED_Module
		led.LEDshouldBlink();
	#endif
	
	#ifdef ColorView_Module
		cv.ColorViewScheduling();
	#endif
	
	#ifdef PingEcho_Module
		echo.computeTimer();  
	#endif
}