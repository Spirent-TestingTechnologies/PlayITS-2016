int a;
int b;
int c;


void setup() {
	// Open serial communications and wait for port to open:
	Serial.begin(9600);
}

void loop() { // run over and over
	if(Serial.available()){
		a = Serial.parseInt();
		b = Serial.parseInt();
		c = Serial.parseInt();
		
		Serial.print("a"); Serial.print(a);
		
		Serial.print("b"); Serial.print(b);
		
		Serial.print("c"); Serial.print(c);
		
		if(a == 1 && b == 2 && c == 3){
			Serial.print("true");
		} else {
			Serial.print("false");
		}
	}
}