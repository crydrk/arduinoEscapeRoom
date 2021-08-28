#define PULSE_PIN D11
#define DIR_PIN D12
#define ENABLE_PIN D13

#define PULSE_WIDTH 250 // change this for speed, lower is faster
#define MAX_LOOPS_BEFORE_CHANGE 4000 // change this for travel distance, higher is further

bool direction = HIGH;
int counter = 0;

void changeDirection() {
  direction = !direction;
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
}

void loop() {
  // reset the counter when we hit max and alter direction
  if (counter >= MAX_LOOPS_BEFORE_CHANGE)
  {
    changeDirection();
    counter = 0;
  }
  
  digitalWrite(DIR_PIN, direction);

  digitalWrite(PULSE_PIN, HIGH);
  delayMicroseconds(PULSE_WIDTH);
  
  digitalWrite(PULSE_PIN, LOW); 
  delayMicroseconds(PULSE_WIDTH);

  counter++;
}
