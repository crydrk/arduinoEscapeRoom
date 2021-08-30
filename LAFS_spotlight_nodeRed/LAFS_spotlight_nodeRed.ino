#define PULSE_PIN D11
#define DIR_PIN D12
#define ENABLE_PIN D13

#define PULSE_WIDTH 250 // change this for speed, lower is faster
#define MAX_LOOPS_BEFORE_CHANGE 4000 // change this for travel distance, higher is further

// limit switch stuff
const int startButtonPin = 4;  
int startButtonState = 0;
const int endButtonPin = 5;  
int endButtonState = 0;

bool direction = HIGH;
int counter = 0;
bool readyToMove = true;

String mode = "standby";

void changeDirection() {
  direction = !direction;
}

void resetPosition() 
{
  readyToMove = false;
  mode = "reset";
}

bool checkForStart()
{
  if (digitalRead(startButtonPin) == HIGH)
  {
    mode = "standby";
    readyToMove = true;
  }
}

bool checkForEnd()
{
  if (digitalRead(endButtonPin) == HIGH)
  {
    resetPosition();
  }
}

void moveMotor(int distance)
{
  if (readyToMove == true)
  {
    counter = counter + distance;
    mode = "moving";
  }
}

void setReadyToMove(bool state)
{
  readyToMove = state;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(PULSE_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  
  digitalWrite(PULSE_PIN, LOW);
  digitalWrite(ENABLE_PIN, LOW);
  digitalWrite(DIR_PIN, LOW);
  
  Serial.begin(9600);
  Serial.println("init");
  
  digitalWrite(ENABLE_PIN, HIGH);
  delay(100);
  digitalWrite(ENABLE_PIN, LOW);

  pinMode(endButtonPin, INPUT);
  pinMode(startButtonPin, INPUT);

  resetPosition();
}

void loop() {
  //delay(1000);
  //moveMotor(200);
  
  //startButtonState = digitalRead(startButtonPin);
  
  // reset the counter when we hit max and alter direction
  //if (counter >= MAX_LOOPS_BEFORE_CHANGE)
  //{
  //  changeDirection();
  //  counter = 0;
  //}
  
  //digitalWrite(DIR_PIN, direction);

  //digitalWrite(PULSE_PIN, HIGH);
  //delayMicroseconds(PULSE_WIDTH);
  
  //digitalWrite(PULSE_PIN, LOW); 
  //delayMicroseconds(PULSE_WIDTH);

  //counter++;

  //Serial.print("start: ");
  //Serial.println(digitalRead(startButtonPin));
  //Serial.print("end: ");
  //Serial.println(digitalRead(endButtonPin));

  //return;

  if (mode == "reset")
  {
    digitalWrite(DIR_PIN, HIGH);
    
    digitalWrite(PULSE_PIN, HIGH);
    delayMicroseconds(PULSE_WIDTH);
    
    digitalWrite(PULSE_PIN, LOW); 
    delayMicroseconds(PULSE_WIDTH);

    checkForStart();

    //Serial.println("reset");
  }
  else if (mode == "standby")
  {
    digitalWrite(DIR_PIN, LOW);
    
    delay(500);
    moveMotor(5000);

    //Serial.println("standby");
  }
  else if (mode == "moving")
  {
    digitalWrite(DIR_PIN, LOW);
    
    digitalWrite(PULSE_PIN, HIGH);
    delayMicroseconds(PULSE_WIDTH);
    
    digitalWrite(PULSE_PIN, LOW); 
    delayMicroseconds(PULSE_WIDTH);

    checkForEnd();

    counter--;
    if (counter < 1)
    {
      mode = "standby";
    }

    //Serial.println("moving");
  }
}
