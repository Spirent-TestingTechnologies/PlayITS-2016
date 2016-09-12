#include "PhyIOCommunication.h"


Communication::Communication() : SoftSerial(3, 2){}



int Communication::available(){
	return Serial.available() + SoftSerial.available();
}

void Communication::begin(){
	// muss in die Setup function
	// setzt automatisch auf die Baudrate von 9600, weil das Bluetooth-Modul so
	// konfiguriert ist
	Serial.begin(9600);
	SoftSerial.begin(9600);
} 

char Communication::peek(){
	if(Serial.peek() != -1){
		return Serial.peek();
	}
	if(SoftSerial.peek() != -1){
		return SoftSerial.peek();
	}
	return -1;
}

bool Communication::find(char c){
	if(Serial.available() > 0){
		return Serial.find(c);
	}
	return SoftSerial.find(c);
}


char Communication::read(){
	if(Serial.available() > 0){
		return Serial.read();
	}
	return SoftSerial.read();
}


void Communication::print(int i){
	Serial.print(i);
	SoftSerial.print(i);
}

void Communication::print(char c){
	Serial.print(c);
	SoftSerial.print(c);
}

void Communication::print(String s){
	Serial.print(s);
	SoftSerial.print(s);
}

void Communication::print(float f){
	Serial.print(f);
	SoftSerial.print(f);
}

void Communication::print(long l){
	Serial.print(l);
	SoftSerial.print(l);
}

void Communication::print(long unsigned int i){
	Serial.print(i);
	SoftSerial.print(i);
}

void Communication::print(uint16_t u){
	Serial.print(u);
	SoftSerial.print(u);
}


void Communication::println(int i){
	Serial.println(i);
	SoftSerial.println(i);
}

void Communication::println(char c){
	Serial.println(c);
	SoftSerial.println(c);
}

void Communication::println(String s){
	Serial.println(s);
	SoftSerial.println(s);
}

void Communication::println(float f){
	Serial.println(f);
	SoftSerial.println(f);
}

void Communication::println(long l){
	Serial.println(l);
	SoftSerial.println(l);
}

void Communication::println(long unsigned int i){
	Serial.println(i);
	SoftSerial.println(i);
}

void Communication::println(uint16_t u){
	Serial.println(u);
	SoftSerial.println(u);
}



void Communication::print(int i, int m){
	Serial.print(i, m);
	SoftSerial.print(i, m);
}

void Communication::print(char c, int m){
	Serial.print(c, m);
	SoftSerial.print(c, m);
}

void Communication::print(float f, int m){
	Serial.print(f, m);
	SoftSerial.print(f, m);
}

void Communication::print(long l, int m){
	Serial.print(l, m);
	SoftSerial.print(l, m);
}

void Communication::print(long unsigned int i, int m){
	Serial.print(i, m);
	SoftSerial.print(i, m);
}

void Communication::print(uint16_t u, int m){
	Serial.print(u, m);
	SoftSerial.print(u, m);
}


void Communication::println(int i, int m){
	Serial.println(i, m);
	SoftSerial.println(i, m);
}

void Communication::println(char c, int m){
	Serial.println(c, m);
	SoftSerial.println(c, m);
}

void Communication::println(float f, int m){
	Serial.println(f, m);
	SoftSerial.println(f, m);
}

void Communication::println(long l, int m){
	Serial.println(l, m);
	SoftSerial.println(l, m);
}

void Communication::println(long unsigned int i, int m){
	Serial.println(i, m);
	SoftSerial.println(i, m);
}

void Communication::println(uint16_t u, int m){
	Serial.println(u, m);
	SoftSerial.println(u, m);
}



float Communication::parseFloat(){
	if(Serial.available() > 0){
		return Serial.parseFloat();
	}
	if(SoftSerial.available() > 0){
		return SoftSerial.parseFloat();
	}
	
}

int Communication::parseInt(){
	if(Serial.available() > 0){
		return Serial.parseInt();
	}
	if(SoftSerial.available() > 0){
		return SoftSerial.parseInt();
	}
}