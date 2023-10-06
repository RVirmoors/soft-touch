// OSC out: 20 - 10 magnetometri (senzori Hall)

#include "WiFiConfig.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
OSCErrorCode error;
const unsigned int outPort = 9920;          // remote port to send OSC to Max
const unsigned int localPort = 8820;        // local port to listen for OSC packets

const int mag1 = 2;     
const int mag2 = 4;    
const int mag3 = 6;    
const int mag4 = 8;    
const int mag5 = 10;
const int mag1a = 3;     
const int mag2a = 5;    
const int mag3a = 7;    
const int mag4a = 9;    
const int mag5a = 11;

unsigned int mag1state = LOW;
unsigned int mag2state = LOW;
unsigned int mag3state = LOW;
unsigned int mag4state = LOW;
unsigned int mag5state = LOW;
unsigned int mag1astate = LOW;
unsigned int mag2astate = LOW;
unsigned int mag3astate = LOW;
unsigned int mag4astate = LOW;
unsigned int mag5astate = LOW;

void setup() {
  Serial.begin(115200);
  delay(10);
  
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID_name);
  WiFi.begin(SSID_name, passw);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
#ifdef ESP32
  Serial.println(localPort);
#else
  Serial.println(Udp.localPort());
#endif

  delay(500);
  
  pinMode(mag1, INPUT);pinMode(mag2, INPUT);pinMode(mag3, INPUT);pinMode(mag4, INPUT);pinMode(mag5, INPUT);
  pinMode(mag1a, INPUT);pinMode(mag2a, INPUT);pinMode(mag3a, INPUT);pinMode(mag4a, INPUT);pinMode(mag5a, INPUT);
}

void loop() {
  OSCBundle bundle;
  
  // send data
  mag1state = digitalRead(mag1);
  mag2state = digitalRead(mag2);
  mag3state = digitalRead(mag3);
  mag4state = digitalRead(mag4);
  mag5state = digitalRead(mag5);
  mag1astate = digitalRead(mag1a);
  mag2astate = digitalRead(mag2a);
  mag3astate = digitalRead(mag3a);
  mag4astate = digitalRead(mag4a);
  mag5astate = digitalRead(mag5a);
  Serial.print(mag1state);Serial.print(mag1astate);Serial.print("-");
  Serial.print(mag2state);Serial.print(mag2astate);Serial.print("-");
  Serial.print(mag3state);Serial.print(mag3astate);Serial.print("-");
  Serial.print(mag4state);Serial.print(mag4astate);Serial.print("-");
  Serial.print(mag5state);Serial.println(mag5astate);
  
  bundle.empty();
  bundle.add("/20").add(mag1state).add(mag2state).add(mag3state).add(mag4state);add(mag5state);
  bundle.add(mag1astate).add(mag2astate).add(mag3astate).add(mag4astate);add(mag5astate);
  Udp.beginPacket(host_ip, outPort);
  bundle.send(Udp);
  Udp.endPacket();
  bundle.empty();
}
