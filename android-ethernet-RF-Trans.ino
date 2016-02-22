/*
Written by Steve Lovejoy aka lovejoy777
This code is for the use with ethernet/rf base station
to control rf receiver relays.
*/
#include <RCSwitch.h>    // needed to sent rf code
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include <SoftwareSerial.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {0x90,0xA2,0xDA,0x0D,0x8B,0x8F};
IPAddress ip(192, 168, 0, 105);
EthernetServer server(8032);
unsigned int localPort = 8032; 
boolean incoming = 0;
// local port to listen on

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
char  ReplyBuffer[] = "acknowledged";       // a string to send back

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

RCSwitch mySwitch = RCSwitch();

void setup() {
  // start the Ethernet and UDP:
  Ethernet.begin(mac,ip);
  Udp.begin(localPort);
  Serial.begin(9600);
  // RF Transmitter data pin is connected to Arduino Pin #4  
  mySwitch.enableTransmit(4);
}

void loop() {
  
// if there's data available, read a packet
  int packetSize = Udp.parsePacket();

Serial.println(packetSize);
if(packetSize)
  {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remote = Udp.remoteIP();
    for (int i =0; i < 4; i++)
    {
      Serial.print(remote[i], DEC);
      if (i < 3)
      {
        Serial.print(".");
      }
    }
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    Udp.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);
    Serial.println("Contents:");
    Serial.println(packetBuffer);
    Serial.println(packetBuffer[0]);

  // function, if packet data from ethernet is 1 send 5392 with RF
    if(packetBuffer[0]=='1'){
     mySwitch.send(5392, 24);
     
  // function, if packet data from ethernet is 2 send 5393 with RF    
    }
    else if(packetBuffer[0]=='2')
    {
      mySwitch.send(5393, 24);

    }

    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(),Udp.remotePort());
    Udp.write("Received");
    Udp.endPacket();
  }
}
