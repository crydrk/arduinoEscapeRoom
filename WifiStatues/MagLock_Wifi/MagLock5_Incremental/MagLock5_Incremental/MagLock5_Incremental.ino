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
int nodeRedPort = 2101;

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1]; //buffer to hold incoming packet

WiFiUDP Udp;

#define r1 D11
#define r2 D12
#define r3 D13
#define r4 D14
#define r5 D15

int lockIncrement = 1;

void setup() 
{
  // Setup the pin and make sure it starts of
  pinMode(r1, OUTPUT);
  pinMode(r2, OUTPUT);
  pinMode(r3, OUTPUT);
  pinMode(r4, OUTPUT);
  pinMode(r5, OUTPUT);
  digitalWrite(r1, HIGH);
  digitalWrite(r2, HIGH);
  digitalWrite(r3, HIGH);
  digitalWrite(r4, HIGH);
  digitalWrite(r5, HIGH);

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

void loop() 
{
    int packetSize = Udp.parsePacket();
    if (packetSize) 
    {
      Serial.printf("Received packet of size %d from %s:%d\n    (to %s:%d, free heap = %d B)\n",
                    packetSize,
                    Udp.remoteIP().toString().c_str(), Udp.remotePort(),
                    Udp.destinationIP().toString().c_str(), Udp.localPort(),
                    ESP.getFreeHeap());
  
      // read the packet into packetBufffer
      int n = Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
      packetBuffer[n] = 0;
      Serial.println("Contents:");
      Serial.printf(packetBuffer);
      Serial.println(" ");

      if (!strcmp(packetBuffer, "verify"))
      {
        Serial.println("sending verification");
        // send a reply, to the IP address and port that sent us the packet we received
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        char verificationReply[] = "good";
        Udp.write(verificationReply);
        Udp.endPacket();
      }
      else if (!strcmp(packetBuffer, "reset"))
      {
        Serial.println("turning on all maglocks");
        digitalWrite(r1, LOW);
        digitalWrite(r2, LOW);
        digitalWrite(r3, LOW);
        digitalWrite(r4, LOW);
        digitalWrite(r5, LOW);
        lockIncrement = 1;
      }
      else if (!strcmp(packetBuffer, "off"))
      {
        Serial.println("turning off maglock");
        int pins[] = {D11, D12, D13, D14, D15};
        for (int i = 0; i < lockIncrement; i++)
        {
          digitalWrite(pins[i], HIGH);
        }
        lockIncrement += 1;
      }
  
      
    }

    delay(100);
}
