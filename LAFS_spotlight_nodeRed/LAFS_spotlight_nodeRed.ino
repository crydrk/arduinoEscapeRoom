// Networking libraries
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// Wifi Setup
#ifndef STASSID
#define STASSID "SGC Surveillance Van"
#define STAPSK  "fourwordsalluppercase374"
#endif

// Node red udp connection info
IPAddress nodeRedIP(192,168,0,160);
int nodeRedPort = 2103; // 2102 for right, 2103 for left

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1]; //buffer to hold incoming packet

WiFiUDP Udp;

#define PULSE_PIN D11
#define DIR_PIN D12
#define ENABLE_PIN D13

#define PULSE_WIDTH 250 // change this for speed, lower is faster
#define MAX_LOOPS_BEFORE_CHANGE 4000 // change this for travel distance, higher is further

// limit switch stuff
const int startButtonPin = 5;  
int startButtonState = 0;
const int endButtonPin = 4;  
int endButtonState = 0;

int moveDistance = 2000;

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

void setStandby()
{
  readyToMove = false;
  mode = "standby";
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
    Serial.println("ending");
    //resetPosition();
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    char completeReply[] = "complete";
    Udp.write(completeReply);
    Udp.endPacket();
    setStandby();
    delay(200);
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

  setStandby();

  // Wait to connect to WiFi
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print('.');
    delay(500);
  }
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("UDP server on port %d\n", nodeRedPort);
  Udp.begin(nodeRedPort);
}

void loop() {

  int packetSize = Udp.parsePacket();
  if (packetSize) 
  {
    // read the packet into packetBufffer
    int n = Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    packetBuffer[n] = 0;

    if (!strcmp(packetBuffer, "verify"))
    {
      setStandby();
      Serial.println("sending verification");
      // send a reply, to the IP address and port that sent us the packet we received
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      char verificationReply[] = "good";
      Udp.write(verificationReply);
      Udp.endPacket();
    }
    else if (!strcmp(packetBuffer, "setStandby"))
    {
      setStandby();
    }
    else if (!strcmp(packetBuffer, "reset"))
    {
      resetPosition();
    }
    else if (!strcmp(packetBuffer, "move"))
    {
      moveMotor(moveDistance);
    }
    else if (!strcmp(packetBuffer, "moveIncrease"))
    {
      moveDistance += 100;
    }
    else if (!strcmp(packetBuffer, "moveDecrease"))
    {
      if (moveDistance > 100)
      {
        moveDistance -= 100;
      }
      
    }
    
  }
  

  if (mode == "reset")
  {
    digitalWrite(DIR_PIN, LOW);
    
    digitalWrite(PULSE_PIN, HIGH);
    delayMicroseconds(PULSE_WIDTH);
    
    digitalWrite(PULSE_PIN, LOW); 
    delayMicroseconds(PULSE_WIDTH);

    checkForStart();

    //Serial.println("reset");
  }
  else if (mode == "standby")
  {
    digitalWrite(DIR_PIN, HIGH);
    
    //Serial.println("standby");
  }
  else if (mode == "moving")
  {
    digitalWrite(DIR_PIN, HIGH);
    
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
