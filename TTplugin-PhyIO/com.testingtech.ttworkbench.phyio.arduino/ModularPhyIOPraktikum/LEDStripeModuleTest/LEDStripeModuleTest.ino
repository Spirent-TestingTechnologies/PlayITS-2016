#include <StripeLEDRemote.h>

LEDRemote led;

int command;

void setup() {
  Serial.begin(9600, SERIAL_8N1);

}

void loop() {
  // put your main code here, to run repeatedly:
  command = Serial.parseInt();
  led.StripeFunction(command);
  led.StripeScheduler();
}
