#ifndef CarSimPingEcho
#define CarSimPingEcho


#include <NewPing.h>
#include "Arduino.h"
#include "CarSimDefine.h"

class PingEcho {
	
	private:
	
		struct PingEchoFunctionConfig {
	
			int ID; // The ID
			unsigned int TimeBetweenSamples;
			unsigned long ReadTimestamp;
			bool enabled = false;
			  
			int medianIterations;
			int maxDistance;
			int unit;
			long int distance ; // value depends on the unit set

			void initialize(){
				//Configures the PingEcho Module - Hardcoded for testcases
			
				// [,<medianIterations:uint16 = 1>]
				medianIterations = PE_POINT_MEDIAN_ITERATIONS;
				// ID, PE01,SETUP,<timeBetweenSamples:unit16>
				TimeBetweenSamples = PE_TIME_BETWEEN_SAMPLES;
				//[,<maxDistance:uint16 = 200>]
				maxDistance = MAX_DISTANCE;
				// <unit:uint16 = cm>]
				unit = PE_UNIT;
			}
			
			void Reset() {
				// Set default values in case there is nothing
				// stored in the eeprom yet.
				TimeBetweenSamples = PE_TIME_BETWEEN_SAMPLES;
				medianIterations = 1;
				maxDistance = MAX_DISTANCE;
				unit = 1;
				distance = 0;
			}
		};
		
		NewPing sonar;
	
	public:
		
		bool pe_enabled;
		unsigned long peTimer;
		
		PingEchoFunctionConfig pePoint;
		
		PingEcho();
		void PingEchoFunction2( int id, int command);
		void PingEchoFunctionStart2();
		void PingEchoFunctionStop2();
		void pintEchoSensorFix();
		void PingEchoFunctionRead2();
		void PingEchoFunctionReadPrint();
		void PingEchoFunctionR1();
		void computeTimer();
		int PingEchoReturnDistance();

};


#endif