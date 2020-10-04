// Networking libraries
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// RFID libraries
#include <SPI.h>
#include <MFRC522.h>

// Wifi Setup
#ifndef STASSID
#define STASSID "SGC Surveillance Van"
#define STAPSK  "fourwordsalluppercase374"
#endif

// Node red udp connection info
IPAddress nodeRedIP(192,168,0,160);
int nodeRedPort = 1883; 

// RFID setup
#define SS_PIN D8
#define RST_PIN D3
MFRC522 rfid(SS_PIN, RST_PIN);  // Create MFRC522 instance.

String lastID = "";
bool isConnected;

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1]; //buffer to hold incoming packet,
char  ReplyBuffer[] = "good";       // a string to send back

WiFiUDP Udp;

void setup() 
{
  
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("UDP server on port %d\n", nodeRedPort);
  Udp.begin(nodeRedPort);

  SPI.begin();
  rfid.PCD_Init();
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
      Serial.println(packetBuffer);

      if (strcmp(packetBuffer, "verify\n"))
      {
        Serial.println("sending verification");
        // send a reply, to the IP address and port that sent us the packet we received
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.write(ReplyBuffer);
        Udp.endPacket();
      }
  
      
    }

    delay(100);

    // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
    if ( ! rfid.PICC_IsNewCardPresent())
      return;
  
    // Verify if the NUID has been read
    if ( ! rfid.PICC_ReadCardSerial())
      return;
  
    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
    Serial.println(rfid.PICC_GetTypeName(piccType));
  
    // Check is the PICC of Classic MIFARE type
    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
      Serial.println(F("Your tag is not of type MIFARE Classic."));
      return;
    }
  
    Serial.print(F("In hex: "));
    String id = "";
    for (int i = 0; i < 4; i++)
    {
      Serial.print(rfid.uid.uidByte[i]);
      Serial.print(" ");
      id += rfid.uid.uidByte[i];
    }
    Serial.println();
    Serial.println(id);

    char charArray[12];
    id.toCharArray(charArray, 12);

    // send data to node-red
    Udp.beginPacket(nodeRedIP, nodeRedPort);
    Udp.write(charArray);
    Udp.endPacket();

    delay(1000);


}
