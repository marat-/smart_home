/*
 * Smart Home
 */
 
#include "IRremote.h"
#include "Ethernet.h"
#include "SPI.h"
#include "D:/Development/Arduino/Constants.h"

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte gateway[] = {192, 168, 1, 1 };
byte dnsServer[] = {192, 168, 1, 1 };
byte subnet[] = {255, 255, 255, 0 };
// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192,168,1,120);

//byte server[] = {192, 168, 1, 17 };
char serverName[] = "www.doors-eco.ru"; 
 
IRsend irsend;
// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

boolean lastConnected = false;  
const unsigned long postingInterval = 10*1000;  // delay between updates, in milliseconds
unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
 
void setup()
{
  Serial.begin(9600);
  initEthernet();
}
// volume down
   //unsigned int S_vdown[68]={4600,4350,700,1550,650,1550,700,1500,700,450,650,450,700,400,700,400,700,400,700,1550,700,1500,700,1550,700,400,700,400,700,400,700,450,650,450,650,1550,700,1500,700,450,650,1550,700,400,700,400,700,450,700,400,700,400,700,400,700,1550,700,400,700,1500,700,1500,700,1550,700,1500,700};
//  unsigned int S_vdown[68]={4450,4450,500,1700,450,1750,500,1700,550,550,550,550,550,550,550,550,550,600,500,1700,450,1750,500,1700,550,550,550,400,700,400,700,450,650,450,650,1550,700,1500,700,450,650,1550,700,400,700,400,700,450,700,400,700,400,700,400,700,1550,700,400,700,1500,700,1500,700,1550,700,1500,700};
 
 
//unsigned int  S_vdown[68]={14120,4400,4450,600,1650,550,1650,600,1600,600,500,500,600,600,500,550,550,550,550,600,1600,600,1600,600,1650,550,550,550,550,550,550,550,550,550,550,600,1600,600,1600,600,550,550,1650,600,500,550,550,550,550,600,500,550,550,600,500,600,1600,600,550,500,1700,550,1650,550,1650,600,1600,600};
 
 
 
void loop() {
 // if (Serial.read() != -1) {
    
  //  Serial.print("loop ");
   // for (int i = 0; i < 3; i++) {
   //   irsend.sendSony(0xa90, 12); // Sony TV power code
    //  irsend.send
   //   delay(100);
  //  }
 // }

  if (!client.connected() && (lastConnectionTime == 0 || (millis() - lastConnectionTime > postingInterval))) {
    SendDataToServer(0, 0, 0);
  } 
  
  // if there are incoming bytes available 
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
  
  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
  lastConnected = client.connected();
}

void sendSignalToSamsungTV(char* commandCode) {
  unsigned int command[68];
  if(strcmp(commandCode, "tvsmsngvlmdwn") == 0) {
    memcpy(S_vdown, command, 68);
  }
  irsend.sendRaw(command,68,38);
}

boolean SendDataToServer(int d_stat, float temp, int humidity) {   
  if (client.connect(serverName, 80)) {
    //char buf[80];
    int temp1 = (temp - (int)temp) * 100;
    int humidityl = (humidity - (int)humidity) * 100;
    Serial.println("Sending information to weather server");
    // Make a HTTP request:
    //sprintf(buf, "GET /meteo.php?S=%d&T=%0d.%d&H=%0d.%d&CS=%d HTTP/1.1", (int)d_stat, (int)temp, abs(temp1), (int)humidity, abs(humidityl), last_condey_status);    
    Serial.println(client);
    //client.println("GET /meteo.php?a=1 HTTP/1.1");
    client.println("GET /main/setDataFromArduino?temp=25 HTTP/1.0");
    client.println("Host: www.doors-eco.ru");
    client.println("Connection: close");
    client.println();
    
    lastConnectionTime = millis();
    
    return true;
  } 
  else {
    // if you didn't get a connection to the server:
    Serial.println("Connection to weather server failed");
    client.stop();
    return false;
  } 
}

void initEthernet() {
  Serial.println("Try to configure Ethernet.");
  // start the Ethernet connection:  
  Ethernet.begin(mac, ip, dnsServer, gateway, subnet);
  Serial.println("Ethernet configuration succesfully completed.");
  Serial.println(Ethernet.dnsServerIP());
  Serial.println(Ethernet.localIP()); 
  // give the Ethernet shield a second to initialize:
  delay(2000);
}
