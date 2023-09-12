// OSC in: 07 - 2 relee TVs

#include "WiFiConfig.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
OSCErrorCode error;
const unsigned int outPort = 9907;          // remote port to receive OSC in Max
const unsigned int localPort = 8807;        // local port to listen for OSC packets (actually not used for sending)
const int tvPin = 2;    // D4
const int tv2Pin = 4;   // D2

unsigned int tvState = LOW;
unsigned int tv2State = LOW;

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

  pinMode(tvPin, OUTPUT);pinMode(tv2Pin, OUTPUT);
  digitalWrite(tvPin, HIGH);digitalWrite(tv2Pin, HIGH);
}

void tv(OSCMessage &msg) {
  tvState = !msg.getInt(0);
  tv2State = !msg.getInt(1);
  digitalWrite(tvPin, tvState);
  digitalWrite(tv2Pin, tv2State);
  Serial.print("TV 07 (inversat): "); Serial.print(tvState); Serial.println(tv2State);
}

void loop() {
  OSCMessage msg;
  int size = Udp.parsePacket();

  if (size > 0) {
    while (size--) {
      msg.fill(Udp.read());
    }
    if (!msg.hasError()) {
      msg.dispatch("/07", tv);
    } else {
      error = msg.getError();
      Serial.print("error: ");
      Serial.println(error);
    }
  }
}
