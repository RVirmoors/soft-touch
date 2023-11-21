// serial out: 20 - 10 magnetometri (senzori Hall)

const int mag1 = 2;     
const int mag2 = 4;    
const int mag3 = 6;    
const int mag4 = 8;    
const int mag5 = 10;
const int mag1a = 3;     
const int mag2a = 5;    
const int mag3a = 7;    
const int mag4a = 9;    
const int mag5a = 11;

const int LEDpin = 53;
int incomingByte;      // a variable to read incoming serial data into

unsigned int mag1state = LOW;
unsigned int mag2state = LOW;
unsigned int mag3state = LOW;
unsigned int mag4state = LOW;
unsigned int mag5state = LOW;
unsigned int mag1astate = LOW;
unsigned int mag2astate = LOW;
unsigned int mag3astate = LOW;
unsigned int mag4astate = LOW;
unsigned int mag5astate = LOW;

void setup() {
  Serial.begin(9600);
  delay(10);

  pinMode(mag1, INPUT);pinMode(mag2, INPUT);pinMode(mag3, INPUT);pinMode(mag4, INPUT);pinMode(mag5, INPUT);
  pinMode(mag1a, INPUT);pinMode(mag2a, INPUT);pinMode(mag3a, INPUT);pinMode(mag4a, INPUT);pinMode(mag5a, INPUT);
  pinMode(LEDpin, OUTPUT);pinMode(LEDpin-2, OUTPUT);pinMode(LEDpin-8, OUTPUT);pinMode(LEDpin-4, OUTPUT);pinMode(LEDpin-6, OUTPUT);
}

void loop() {
  // see if there's incoming serial data:
  if (Serial.available() > 0) {
    // read the oldest byte in the serial buffer:
    incomingByte = Serial.read();
    // if it's a capital H (ASCII 72), turn on the LEDs:
    if (incomingByte == 'H') {
      digitalWrite(LEDpin, HIGH);digitalWrite(LEDpin-2, HIGH);digitalWrite(LEDpin-4, HIGH);digitalWrite(LEDpin-6, HIGH);digitalWrite(LEDpin-8, HIGH);
    }
    // if it's an L (ASCII 76) turn off the LEDs:
    if (incomingByte == 'L') {
      digitalWrite(LEDpin, LOW);digitalWrite(LEDpin-2, LOW);digitalWrite(LEDpin-4, LOW);digitalWrite(LEDpin-6, LOW);digitalWrite(LEDpin-8, LOW);
    }
  }
  
  // send data
  mag1state = digitalRead(mag1);
  mag2state = digitalRead(mag2);
  mag3state = digitalRead(mag3);
  mag4state = digitalRead(mag4);
  mag5state = digitalRead(mag5);
  mag1astate = digitalRead(mag1a);
  mag2astate = digitalRead(mag2a);
  mag3astate = digitalRead(mag3a);
  mag4astate = digitalRead(mag4a);
  mag5astate = digitalRead(mag5a);
  Serial.print("mag ");
  Serial.print(mag1state);Serial.print(" ");Serial.print(mag1astate);Serial.print(" ");
  Serial.print(mag2state);Serial.print(" ");Serial.print(mag2astate);Serial.print(" ");
  Serial.print(mag3state);Serial.print(" ");Serial.print(mag3astate);Serial.print(" ");
  Serial.print(mag4state);Serial.print(" ");Serial.print(mag4astate);Serial.print(" ");
  Serial.print(mag5state);Serial.print(" ");Serial.print(mag5astate);Serial.println(" ");

  int gandac = analogRead(A14);
  int borcan1 = analogRead(A15);
  int borcan2 = analogRead(A0);
  Serial.print("analog ");
  Serial.print(gandac);Serial.print(" ");
  Serial.print(borcan1);Serial.print(" ");
  Serial.print(borcan2);Serial.println(" ");
  delay(10);
}
