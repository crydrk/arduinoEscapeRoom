String incomingString = "";

void setup() {
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps

  pinMode(12, OUTPUT);
  digitalWrite(12, HIGH);  
}

void loop() {
  // send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingString = Serial.readString();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingString);

    if (incomingString == "on")
    {
      digitalWrite(12, LOW);   // turn the LED on (HIGH is the voltage level)
      delay(1000);                       // wait for a second
      digitalWrite(12, HIGH);    // turn the LED off by making the voltage LOW
      delay(1000);     
    }
  }
}
