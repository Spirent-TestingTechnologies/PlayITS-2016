
// constants won't change. They're used here to
// set pin numbers:
const int buttonPin1 = 15;
const int buttonPin2 = 16;

// variables will change:
int buttonState1 = 0;         // variable for reading the pushbutton status
int buttonState2 = 0;

void setup() {
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  Serial.begin(9600);
}

void loop() {
  // read the state of the pushbutton value:
  if(buttonState1 != digitalRead(buttonPin1)){
    buttonState1 = digitalRead(buttonPin1);
    Serial.print("Kippschalterstate: ");
    Serial.println(buttonState1);  
    delay(100);
  }
  if(buttonState2 != digitalRead(buttonPin2)){
    buttonState2 = digitalRead(buttonPin2);
    Serial.print("Pushbuttonstate: ");
    Serial.println(buttonState2);  
    delay(100);
  }
  
  delay(100);
}
