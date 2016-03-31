#include "CarSimColorView.h"

ColorView::ColorView() : tcs(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X){}

void ColorView::ColorViewSetup(){
	#ifdef CVF_PRESENT
		if (tcs.begin()) {
			DEBUG_PRINTLN("#Found CVF sensor");
		} else {
			DEBUG_PRINTLN("#No TCS34725 found ... check your connections");
			while (1); // halt!
		}
	
	// use these three pins to drive an LED
	pinMode(cvf_ledpin, OUTPUT);
	digitalWrite(cvf_ledpin, HIGH);
	#else
		DEBUG_PRINTLN("#Found simulated CVF sensor");
	#endif
}

void ColorView::ColorViewFunction( int id, int command) {
	cvfPoint.ID = id;
	DEBUG_PRINT("#In the ColorViewFunction with function ");
	DEBUG_PRINTLN(command);

	switch (command) {
		case SETUP:
			ColorViewFunctionSetup();
			break;
		case START:
			// ColorViewFunctionStart();
			break;
		case STOP:
			// ColorViewFunctionStop();
			break;
		case READ:
			ColorViewFunctionRead();
			break;
		default:
			break;
	}
  // Read parameters
}

void ColorView::ColorViewFunctionSetup() {
	DEBUG_PRINTLN("#In the ColorViewFunctionSetup");

	int timeBetweenSamples = 100;
	int intLED = 1; // default

	//ID,CV01,SETUP,<timeBetweenSamples:unit16>[,<LED ON/OFF = 1>]
	//Configures the Color View Module
	//<timeBetweenSamples:unit16>    - Time between two measurements in milliseconds
	//<LED OFF = 1>       - 0 to switch LED DEFAULT OFF, default=1 ON, other values ignored. optional

	cvfPoint.TimeBetweenSamples = XSERIAL.parseInt();
	DEBUG_PRINT("#parameters are ");
	DEBUG_PRINT(" time = ");
	DEBUG_PRINT(cvfPoint.TimeBetweenSamples);

	if (XSERIAL.peek() == 10 && XSERIAL.find("10")) {
		DEBUG_PRINT(" ; intLED<default>= ");
		DEBUG_PRINTLN(intLED);
		return; // no more optional params
	}

	cvfPoint.intLED = XSERIAL.parseInt();
	DEBUG_PRINT("# ; intLED= ");
	DEBUG_PRINTLN(cvfPoint.intLED);
}

void ColorView::ColorViewFunctionRead() {
	uint16_t clear, red, green, blue;

	DEBUG_PRINTLN("#In the ColorViewFunctionRead");

	#ifdef CVF_PRESENT
		tcs.setInterrupt(false);      // turn on LED
		delay(60);  // takes 50ms to read
		tcs.getRawData(&(cvfPoint.red), &(cvfPoint.green), &(cvfPoint.blue), &(cvfPoint.clear));
		tcs.setInterrupt(true);  // turn off LED
	#else
		cvfPoint.red = random(555);
		cvfPoint.green = random(555);
		cvfPoint.blue = random(555);
		cvfPoint.clear = random(655);
	#endif
		
	cvfPoint.ReadTimestamp = millis();
	cvfPoint.colorTemp = tcs.calculateColorTemperature(cvfPoint.red, cvfPoint.green, cvfPoint.blue);
	cvfPoint.lux = tcs.calculateLux(cvfPoint.red, cvfPoint.green, cvfPoint.blue);
  
	float rf, gf, bf;
  
	rf = cvfPoint.red;
	rf /= cvfPoint.clear;
	rf *= 256;

	gf = cvfPoint.green;
	gf /= cvfPoint.clear;
	gf *= 256;

	bf = cvfPoint.blue;
	bf /= cvfPoint.clear;
	bf *= 256;

	cvfPoint.red256 = rf;
	cvfPoint.green256 = gf;
	cvfPoint.blue256 = bf;

	ColorViewFunctionR1();
}

void ColorView::ColorViewFunctionR1() {
	// ID, CV01,R1,<timestamp:unit16>,<r:unit16>, <g:unit16>, <b:unit16>, <c:unit16>, <colorTemp:unit16>, <lux:unit16>
	XSERIAL.print(cvfPoint.ID); XSERIAL.print(", ");
	XSERIAL.print(CV01);  XSERIAL.print(", ");
	XSERIAL.print(R1);  XSERIAL.print(", ");
	XSERIAL.print(cvfPoint.ReadTimestamp);  XSERIAL.print(", ");
	XSERIAL.print(cvfPoint.red);  XSERIAL.print(", ");
	XSERIAL.print(cvfPoint.green);  XSERIAL.print(", ");
	XSERIAL.print(cvfPoint.blue);  XSERIAL.print(", ");
	XSERIAL.print(cvfPoint.clear);  XSERIAL.print(", ");
	XSERIAL.print(cvfPoint.colorTemp);  XSERIAL.print(", ");
	XSERIAL.print(cvfPoint.lux);  XSERIAL.print(", ");
	XSERIAL.print(cvfPoint.red256);  XSERIAL.print(", ");
	XSERIAL.print(cvfPoint.green256);  XSERIAL.print(", ");
	XSERIAL.println(cvfPoint.blue256);
	XSERIAL.flush();
}
