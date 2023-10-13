// OSC out: 04 - distanta WC, 19 - vibratie carpeta 2

#include "WiFiConfig.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
OSCErrorCode error;
const unsigned int outPort = 9904;          // remote port to receive OSC
const unsigned int localPort = 8804;        // local port to listen for OSC packets

const int TRIG_PIN = 15;       // D8
const int ECHO_PIN = 13;       // D7
const int vib1 = 14;     // D5
const int vib2 = 5;      // D1

unsigned int vib1state = LOW;
unsigned int vib2state = LOW;

long duration;
int distance;

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
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(vib1, INPUT);
  pinMode(vib2, INPUT);
}

void loop() {
  OSCBundle bundle;
  
  // send data
  vib1state = digitalRead(vib1);
  Serial.print(vib1state);
  vib2state = digitalRead(vib2);
  Serial.println(vib2state);
  
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.017;
  Serial.println(distance);

  bundle.empty();
  bundle.add("/04").add(distance);
  bundle.add("/19").add(vib1state).add(vib2state);
  Udp.beginPacket(host_ip, outPort);
  bundle.send(Udp);
  Udp.endPacket();
  bundle.empty();

  delay(20);
}
