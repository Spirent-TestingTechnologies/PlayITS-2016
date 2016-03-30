#include "CarSimLightSensor.h"

LightSensor::LightSensor(unsigned long &readTimer, unsigned int readSpeed) : readTimer(readTimer){
	this->readSpeed = readSpeed;
}


void LightSensor::LightSensorFunction( int id, int command){
	int variable = XSERIAL.parseInt();
  
	switch(command){
		case READ:
			if(variable == 0){
				LightFunctionPrint(id,analogRead(LS1PIN),millis());
			}
			else if(variable == 1){
				LightFunctionPrint2(id,0.0,millis());
			}
			break;
		
		case START:{
			int lightValue = analogRead(LS1PIN);
			LightFunctionStart(id,lightValue,variable);
			break;
		}
			
		case STOP:
			LightFunctionPrint(id,0.0,millis());
			break;
		
		default:
			break;
	}
  // ID, LS01, R1, <brightness:int>, <timestamp:long>
}

void LightSensor::LightFunctionPrint(int id,int value,unsigned long time){
    //Protokoll zum Uebermitteln der Helligkeit
    XSERIAL.print(id); XSERIAL.print(", ");
    XSERIAL.print(LS01);  XSERIAL.print(", ");
    XSERIAL.print(R1);  XSERIAL.print(", ");
    XSERIAL.print(value);  XSERIAL.print(", ");
    XSERIAL.println(time);
    XSERIAL.flush();
}

void LightSensor::LightFunctionPrint2(int id,float value,unsigned long time){
    //Protokoll zum Uebermitteln der Frequentz
    XSERIAL.print(id); XSERIAL.print(", ");
    XSERIAL.print(LS01);  XSERIAL.print(", ");
    XSERIAL.print(R1);  XSERIAL.print(", ");
    XSERIAL.print(value);  XSERIAL.print(", ");
    XSERIAL.println(time);
    XSERIAL.flush();
}

void LightSensor::LightFunctionStart(int id,int lightValue,int lightDiff){
    //wartet bis sich die Helligkeit um lightDiff geändert hat und beginnt zu messen
    int lightValueNow,top,down;
    //////////Schranken setzen/////////////
    if(lightValue - lightDiff < 0){
		down = 0;
    }
    else {
		down = lightValue - lightDiff;
    }
    top = lightValue + lightDiff;
    ///////////////////////////////////////
    while(true){
		lightValueNow = analogRead(LS1PIN);
		if(lightValueNow>=top){
			LightFunctionFrequenz(id,top,lightValue,true,millis(),millis());
			return;
		}
		else if(lightValueNow<=down){
			LightFunctionFrequenz(id,lightValue,down,false,millis(),millis());
			return;
		}
		if(Serial.available() > 0){
			if(readInput(0.0,false)==true){
			return;  
			}
		}
    }
}

void LightSensor::LightFunctionFrequenz(int id,int top,int down,boolean upOrDown,unsigned long startTime,unsigned long endTime){
	//zaehlt die Anzahl der Wechsel und überprüft ob die Wechsel den gleichen zeitlichen Abstand haben
	boolean error=false;
	boolean inPut = false;
	unsigned long timeDiff= 0;
	int counter = 1;
	int lightValueNow;
	float frequency;
   
	while(true){
		lightValueNow = analogRead(LS1PIN);
		if(lightValueNow>=top && upOrDown == false){
			upOrDown = true;
			counter++;
			if(timeDiff == 0 ){
				endTime = millis();
				timeDiff = endTime - startTime;
			}
			else{
				if(checkError(timeDiff,millis()-endTime)==false && inPut == false){
					error = true;
				}
				endTime = millis();
			}
			inPut = false;
		}
		else if(lightValueNow<=down && upOrDown == true){
			upOrDown = false;
			counter++;
			if(timeDiff == 0 ){
				endTime = millis();
				timeDiff = endTime - startTime;
			}
			else{
				if(checkError(timeDiff,millis()-endTime)==false && inPut == false){
					error = true;
				}
				endTime = millis();
			}
			inPut = false;
		}
		if(Serial.available()> 0 ) {
			inPut = true;
			if(readInput(getFrequency(endTime-startTime,counter),error)==true){
				return;  
			}
      
		}
	}
}
 
boolean LightSensor::checkError(unsigned long timeDiff,unsigned long timeDiffNow){
	//überprüft ob die Abstände gleich sind
	if(timeDiffNow > (timeDiff+(timeDiff*0.1))){
		return false;
	}
	else if(timeDiffNow < (timeDiff-(timeDiff*0.1))){
		return false;
	}
	else{
		return true;
	}
}
 
float LightSensor::getFrequency(unsigned long time,int counter){
	//berechnet die Frequentz
	return (((float)counter*1000.0)/time)/2.0;
}

boolean LightSensor::readInput(float frequency,boolean error){
	//ließt Input und bearbeitet ihn, liefert true wenn die Aufrufende Funktion gestoppt werden muss
	int id, functionType, command,variable;
	//division by zero return -1
	// todo test isnan or isinf
	if(isnan(frequency)){
		frequency = -1.0;
	}
	if(isinf(frequency)){
		frequency = -1.0;
	}
	//////////////////
	if (millis() >= readTimer) {
		readTimer += readSpeed;      // Set the next ping time.
		if (XSERIAL.available() > 0) {
			// Read in loop all data for one line
			id = XSERIAL.parseInt();
			functionType = XSERIAL.parseInt();
			command = XSERIAL.parseInt();
			variable = XSERIAL.parseInt();
			
			switch (functionType) {
				case LS01:
					switch (command){
						case START:
						LightFunctionStart(id,analogRead(LS1PIN),variable);
						return true;
						break;
						
						case READ:
							if(variable == 0){
								LightFunctionPrint(id,analogRead(LS1PIN),millis());
								return false;
							}
							else if(variable == 1 ){
								if(error==false){
									LightFunctionPrint2(id,frequency,millis());
								}
								else{
									LightFunctionPrint2(id,0.0,millis());
								}
								return false;
							}
							break;
						
						case STOP:
							if(error==false){
								LightFunctionPrint2(id,frequency,millis());
							}
							else{
								LightFunctionPrint2(id,0.0,millis());
							}
							return true;
							break;
            
				}
				break;
			}
		}
	}
}
