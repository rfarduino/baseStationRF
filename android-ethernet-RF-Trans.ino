/*
Written by Steve Lovejoy aka lovejoy777
This code is for the use with ethernet/rf base station
to control rf receiver relays.
*/
#include <Ethernet.h>
#include <EthernetUdp.h>

#include <SoftwareSerial.h>
#include <RCSwitch.h>
#include <SD.h>
#include <SPI.h>

byte mac[] = {
  0x90, 0xA2, 0xDA, 0x0D, 0x8B, 0x8F
};
IPAddress ip(192, 168, 0, 136);
EthernetServer server(8032);
File myFile;
unsigned int localPort = 8032;
boolean incoming = 0;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
char  ReplyBuffer[] = "acknowledged";
int pin8 = 8;
int pin9 = 9;
int setupMode = 0;
byte ip1[] = {192, 168, 0, 107};
String strIP;
String BTdata;

//SoftwareSerial mySerial(2, 3); // RX, TX
RCSwitch mySwitch = RCSwitch();
EthernetUDP Udp;

void setup() {
  
  pinMode(pin8, OUTPUT);
  pinMode(pin9, INPUT);
  digitalWrite (pin8, HIGH);
  mySwitch.enableTransmit(7);
  strIP = "192.168.0.105";
  
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  
  // SETUP MODE
  setupMode = digitalRead(pin9);
  if (setupMode == HIGH) {
    while (Serial.available() == 0) { // if the data came
    }
    BTdata = Serial.readString(); // read byte
    Serial.println(BTdata);
    Serial.println("SETUP MODE");
    if (SD.exists("ipa.txt")) {
      // delete the file:
      SD.remove("ipa.txt");
    } else {
      Serial.println("ipa.txt doesn't exist.");
    }
    myFile = SD.open("ipa.txt", FILE_WRITE);
    if (myFile) {
      myFile.println(BTdata); // write to sdcard
      myFile.close();
      Serial.println("done.");
    } else {
      Serial.println("error opening ipa.txt");
    }
    myFile = SD.open("ipa.txt");
    if (myFile) {
      while (myFile.available()) {
        String strIP = myFile.readStringUntil('\n');
        char buf[16];
        strIP.toCharArray(buf, 15);
        sscanf(buf, "%d.%d.%d.%d", &ip1[0], &ip1[1], &ip1[2], &ip1[3]);
      }
      myFile.close();
    } else {
      Serial.println("error opening ipa.txt");
    }
  } else {
    
    // USER MODE
    Serial.println("USER MODE");
    myFile = SD.open("ipa.txt");
    if (myFile) {
      while (myFile.available()) {
        String strIP = myFile.readStringUntil('\n');
        char buf[16];
        strIP.toCharArray(buf, 15);
        sscanf(buf, "%d.%d.%d.%d", &ip1[0], &ip1[1], &ip1[2], &ip1[3]);
      }
      myFile.close();
    } else {
      Serial.println("error opening ipa.txt");
    }
  }
  
  // START ETHERNET
  Ethernet.begin(mac, ip1);
  Udp.begin(localPort);
}

void loop() {
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    int i = atoi(&packetBuffer[0]);
    if (i > 0) {
      mySwitch.send(i, 24);
    }
    else {
      Serial.println("Unknown Code");
    }
  }
}
