// OSC out: 03 - atingere bormasina MPR121, 06 - distanta microscop, 08 - fotorezistor carte analog
// OSC in: relee on/off ventilator, mixer, bormasina, TVs

#include "WiFiConfig.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>

#include "Adafruit_MPR121.h"

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
OSCErrorCode error;
const unsigned int outPort = 9903;          // remote port to receive OSC
const unsigned int localPort = 8803;        // local port to listen for OSC packets

Adafruit_MPR121 cap = Adafruit_MPR121(); // D1 & D2
const int ventilatorPin = 0;  // D3
const int mixerPin = 2;       // D4
const int tvPin = 13;       // D7
const int bormasinaPin  = 15;  // D8

long duration;
int distance;

float touch = 0.0;
unsigned int venState = HIGH;
unsigned int mixState = HIGH;
unsigned int borState = HIGH;
unsigned int tvState = HIGH;
int carte = 0;

void setup() {
  Serial.begin(115200);
  delay(10);

  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");

  IPAddress local_IP(192, 168, 50, 95);
  IPAddress gateway(192, 168, 50, 155);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(local_IP,gateway,subnet);
  
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

  pinMode(ventilatorPin, OUTPUT);pinMode(mixerPin, OUTPUT);
  pinMode(bormasinaPin, OUTPUT);
  pinMode(tvPin, OUTPUT);
  digitalWrite(ventilatorPin, LOW);digitalWrite(mixerPin, LOW);
  digitalWrite(bormasinaPin, HIGH);
  digitalWrite(tvPin, LOW);
}

void relays(OSCMessage &msg) {
  venState = !msg.getInt(0); digitalWrite(ventilatorPin, venState);
  mixState = !msg.getInt(1); digitalWrite(mixerPin, mixState);
  borState = !msg.getInt(2); digitalWrite(bormasinaPin, borState);
  tvState = !msg.getInt(3); digitalWrite(tvPin, tvState);
  Serial.print("ven/mix/bor/TV (inversate): ");
  Serial.print(venState);
  Serial.print(mixState);
  Serial.print(borState);
  Serial.println(tvState);
}

void loop() {
  OSCBundle bundle;
  
  // get data
  int size = Udp.parsePacket();

  if (size > 0) {
    while (size--) {
      bundle.fill(Udp.read());
    }
    if (!bundle.hasError()) {
      bundle.dispatch("/03", relays);
    } else {
      error = bundle.getError();
      Serial.print("error: ");
      Serial.println(error);
    }
  }

  // send data
//  Serial.println(cap.touched(), HEX);
//  Serial.print(cap.filteredData(0)); Serial.print("\t");
//  Serial.print(cap.baselineData(0)); Serial.print("\t");
//  Serial.println();

  touch = cap.baselineData(0) - cap.filteredData(0);
  carte = analogRead(A0);

  Serial.print("Sending MPR: "); Serial.println(touch);
  Serial.print("Dist: "); Serial.println(distance);
  Serial.print("Carte: "); Serial.println(carte);

  bundle.empty();
  bundle.add("/03").add(touch);
  bundle.add("/08").add(carte);
  
  Udp.beginPacket(host_ip, outPort);
  bundle.send(Udp);
  Udp.endPacket();
  bundle.empty();

  delay(100);
}
