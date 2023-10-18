// OSC in: releu bec UV
// OSC out: 05 - distanta WC

#include "WiFiConfig.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
OSCErrorCode error;
const unsigned int outPort = 9906;          // remote port to receive OSC
const unsigned int localPort = 8806;        // local port to listen for OSC packets

const int TRIG_PIN = 14;       // D5
const int ECHO_PIN = 12;       // D6

long duration;
int distance;
unsigned int relState = LOW;

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
}

void loop() {
  OSCBundle bundle;
  
  // send data
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.017;
  Serial.println(distance);

  bundle.empty();
  bundle.add("/06").add(distance);
  Udp.beginPacket(host_ip, outPort);
  bundle.send(Udp);
  Udp.endPacket();
  bundle.empty();

  delay(20);
}
