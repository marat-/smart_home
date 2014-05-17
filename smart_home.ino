/*
 * Smart Home
 */
 
#include "IRremote.h"
#include "Ethernet.h"
#include "SPI.h"
#include "DHT.h"
#include "D:/Development/Arduino/Constants.h"

#define DHTPIN 2
#define DHTTYPE DHT22 

DHT dht(DHTPIN, DHTTYPE);
float humidity;
float temp;
boolean dhtDegug = true;

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
const unsigned long postingInterval = 10;  // delay between updates, in seconds
unsigned long lastConnectionTime = 0;          // last time you connected to the server, in seconds
 
void setup()
{
  Serial.begin(9600);
  initEthernet();
  initDHT22();
}
// volume down
   //unsigned int S_vdown[68]={4600,4350,700,1550,650,1550,700,1500,700,450,650,450,700,400,700,400,700,400,700,1550,700,1500,700,1550,700,400,700,400,700,400,700,450,650,450,650,1550,700,1500,700,450,650,1550,700,400,700,400,700,450,700,400,700,400,700,400,700,1550,700,400,700,1500,700,1500,700,1550,700,1500,700};
//  unsigned int S_vdown[68]={4450,4450,500,1700,450,1750,500,1700,550,550,550,550,550,550,550,550,550,600,500,1700,450,1750,500,1700,550,550,550,400,700,400,700,450,650,450,650,1550,700,1500,700,450,650,1550,700,400,700,400,700,450,700,400,700,400,700,400,700,1550,700,400,700,1500,700,1500,700,1550,700,1500,700};
 
 
//unsigned int  S_vdown[68]={14120,4400,4450,600,1650,550,1650,600,1600,600,500,500,600,600,500,550,550,550,550,600,1600,600,1600,600,1650,550,550,550,550,550,550,550,550,550,550,600,1600,600,1600,600,550,550,1650,600,500,550,550,550,550,600,500,550,550,600,500,600,1600,600,550,500,1700,550,1650,550,1650,600,1600,600};
 
 
 
void loop() {
  if (!client.connected() && (lastConnectionTime == 0 || (millis()/1000 - lastConnectionTime > postingInterval))) {
    getTempAndHumidity();    
    SendDataToServer();
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

void getTempAndHumidity() {  
  humidity = dht.readHumidity();
  temp = dht.readTemperature();
  if (isnan(temp) || isnan(humidity)) {
    Serial.println("Failed to read from DHT");
  } else {
    if(dhtDegug) {
      Serial.print("Humidity: "); 
      Serial.print(humidity);
      Serial.print(" %\t");
      Serial.print("Temperature: "); 
      Serial.print(temp);
      Serial.println(" *C");
    }
  }
}

void sendSignalToSamsungTV(char* commandCode) {
  unsigned int command[68];
  if(strcmp(commandCode, "tvsmsngvlmdwn") == 0) {
    memcpy(S_vdown, command, 68);
  }
  irsend.sendRaw(command,68,38);
}

boolean SendDataToServer() {   
  if (client.connect(serverName, 80)) {    
    Serial.println("Sending information to weather server");
    Serial.println(client);
    int temp_frac= (temp - (int)temp) * 100;
    int humidity_frac= (humidity - (int)humidity) * 100;
    // Make a HTTP request:
    char buf[80];
    sprintf(buf, "GET /main/setDataFromArduino?temp=%0d.%d&humidity=%0d.%d HTTP/1.0", (int)temp, abs(temp_frac), (int)humidity, abs(humidity_frac));        
    client.println(buf);
    client.println("Host: www.doors-eco.ru");
    client.println("Connection: close");
    client.println();
    
    lastConnectionTime = millis()/1000;
    
    return true;
  } 
  else {
    // if you didn't get a connection to the server:
    lastConnectionTime = millis()/1000;
    Serial.println("Connection to weather server failed");
    //client.stop();
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

void initDHT22() {    
  dht.begin();
}
