#include <CarSimMotor.h>

Motor m;

int command;

void setup() {
  Serial.begin(9600, SERIAL_8N1);

}

void loop() {
  // put your main code here, to run repeatedly:
  command = Serial.parseInt();
  m.MotorFunction(1, command);
  
}
