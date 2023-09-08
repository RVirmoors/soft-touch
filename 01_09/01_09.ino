// OSC out: -
// OSC in: 01 - balansor unghi?, 09 - releu lampa on/off

#include "WiFiConfig.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
OSCErrorCode error;
const unsigned int outPort = 9901;          // remote port to receive OSC
const unsigned int localPort = 8801;        // local port to listen for OSC packets (actually not used for sending)
const int lampaPin = 2;   // D4

float balansor = 0.0;
unsigned int lampaState = LOW;

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

  pinMode(lampaPin, OUTPUT);
  digitalWrite(lampaPin, HIGH);
}

void bal(OSCMessage &msg) {
  balansor = msg.getFloat(0);
  Serial.print("Balansor: "); Serial.println(balansor);
  // TODO miscat balansor
}

void lampa(OSCMessage &msg) {
  lampaState = !msg.getInt(0);
  digitalWrite(lampaPin, lampaState);
  Serial.print("Lampa 09 (inversat): "); Serial.println(lampaState);
}

void loop() {
  OSCMessage msg;
  int size = Udp.parsePacket();

  if (size > 0) {
    while (size--) {
      msg.fill(Udp.read());
    }
    if (!msg.hasError()) {
      msg.dispatch("/01", bal);
      msg.dispatch("/09", lampa);
    } else {
      error = msg.getError();
      Serial.print("error: ");
      Serial.println(error);
    }
  }
}
