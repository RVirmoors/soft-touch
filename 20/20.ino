// OSC out: 20 - vibratie din 4 colturi (borcan lut)

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

const int vib1 = 2;     // D4
const int vib2 = 14;    // D5
const int vib3 = 12;    // D6
const int vib4 = 13;    // D7

unsigned int vib1state = LOW;
unsigned int vib2state = LOW;
unsigned int vib3state = LOW;
unsigned int vib4state = LOW;

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
  
  pinMode(vib1, INPUT);pinMode(vib2, INPUT);pinMode(vib3, INPUT);pinMode(vib4, INPUT);
}

void loop() {
  OSCBundle bundle;
  
  // send data
  vib1state = digitalRead(vib1);
  vib2state = digitalRead(vib2);
  vib3state = digitalRead(vib3);
  vib4state = digitalRead(vib4);
  Serial.print(vib1state);Serial.print(vib2state);Serial.print(vib3state);Serial.println(vib4state);

  bundle.empty();
  bundle.add("/20").add(vib1state).add(vib2state).add(vib3state).add(vib4state);
  Udp.beginPacket(host_ip, outPort);
  bundle.send(Udp);
  Udp.endPacket();
  bundle.empty();
}
