#ifndef PhyIOCommunication
#define PhyIOCommunication

#include "Arduino.h"
#include <SoftwareSerial.h>

class Communication {

	private:
	
		// for the Bluetoothconnection
		SoftwareSerial SoftSerial;

	public:
	
		Communication();
		int available();
		void begin(); // muss in die Setup function
		char peek();
		bool find(char c);
		char read();
		String readString();
		
		
		void print(int i);
		void print(char c);
		void print(String s);
		void print(float f);
		void print(long l);
		void print(long unsigned int i);
		void print(uint16_t u);
		
		void println(int i);
		void println(char c);
		void println(String s);
		void println(float f);
		void println(long l);
		void println(long unsigned int i);
		void println(uint16_t u);
		
		
		void print(int i, int m);
		void print(char c, int m);
		void print(float f, int m);
		void print(long l, int m);
		void print(long unsigned int i, int m);
		void print(uint16_t u, int m);
		
		void println(int i, int m);
		void println(char c, int m);
		void println(float f, int m);
		void println(long l, int m);
		void println(long unsigned int i, int m);
		void println(uint16_t u, int m);
		
		//void write();
		float parseFloat();
		int parseInt();
		

};


#endif