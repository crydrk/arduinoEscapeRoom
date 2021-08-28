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
int nodeRedPort = 2004;

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1]; //buffer to hold incoming packet

WiFiUDP Udp;

bool isLocked = false;

void setup() 
{
  // Setup the pin and make sure it starts off
  pinMode(D15, OUTPUT);
  digitalWrite(D15, LOW);

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
      else if (!strcmp(packetBuffer, "on"))
      {
        Serial.println("turning on maglock");
        isLocked = true;
        digitalWrite(D15, HIGH);
        // send a reply, to the IP address and port that sent us the packet we received
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        char onReply[] = "on";
        Udp.write(onReply);
        Udp.endPacket();
      }
      else if (!strcmp(packetBuffer, "off"))
      {
        Serial.println("turning off maglock");
        isLocked = false;
        digitalWrite(D15, LOW);
        // send a reply, to the IP address and port that sent us the packet we received
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        char offReply[] = "off";
        Udp.write(offReply);
        Udp.endPacket();
      }
      else if (!strcmp(packetBuffer, "status"))
      {
        Serial.println("returning status");
        // send a reply, to the IP address and port that sent us the packet we received
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        
        
        if (isLocked == true)
        {
          char statusReply[] = "on";
          Udp.write(statusReply);
          Udp.endPacket();
        }
        else
        {
          char statusReply[] = "off";
          Udp.write(statusReply);
          Udp.endPacket();
        }
          
        
      }
  
      
    }

    delay(100);
}
