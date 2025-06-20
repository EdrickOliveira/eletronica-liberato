const unsigned long interval1 = 5000;
unsigned long interval2 = 0;

unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;

bool dotHz = false, clk = false;

void setup() {
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(A5, INPUT);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis1 >= interval1) {
    if(!dotHz){
      dotHz = true;
      digitalWrite(2, HIGH);
    }
    else{
      dotHz = false;
      digitalWrite(2, LOW);
    }
    
    previousMillis1 = currentMillis;
  }

  // Segundo loop
  if (currentMillis - previousMillis2 >= interval2) {
    interval2 = map(analogRead(A5), 0, 1023, 2, 240);
    if(!clk){
      clk = true;
      digitalWrite(4, HIGH);
    }
    else{
      clk = false;
      digitalWrite(4, LOW);
    }
    
    previousMillis2 = currentMillis;
  }

}
