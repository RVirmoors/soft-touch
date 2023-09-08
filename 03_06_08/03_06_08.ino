// OSC out: 03 - atingere bormasina MPR121, 06 - fotorezistor microscop, 08 - fotorezistor carte
// OSC in: relee on/off ventilator, mixer, bormasina

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

Adafruit_MPR121 cap = Adafruit_MPR121();
const int ventilatorPin = 2;  // D4
const int mixerPin = 4;       // D2
const int bormasinaPin = 5;   // D1
const int microscopPin = 15;  // D8
const int cartePin = 13;      // D7

float touch = 0.0;
unsigned int venState = LOW;
unsigned int mixState = LOW;
unsigned int borState = LOW;
unsigned int micState = LOW;
unsigned int carState = LOW;

void setup() {
  Serial.begin(115200);
  delay(10);

//  if (!cap.begin(0x5A)) {
//    Serial.println("MPR121 not found, check wiring?");
//    while (1);
//  }
//  Serial.println("MPR121 found!");
  
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

  pinMode(ventilatorPin, OUTPUT);pinMode(mixerPin, OUTPUT);pinMode(bormasinaPin, OUTPUT);
  digitalWrite(ventilatorPin, HIGH);digitalWrite(mixerPin, HIGH);digitalWrite(bormasinaPin, HIGH);
  pinMode(microscopPin, INPUT);pinMode(cartePin, INPUT);
}

void relays(OSCMessage &msg) {
  venState = !msg.getInt(0); digitalWrite(ventilatorPin, venState);
  mixState = !msg.getInt(1); digitalWrite(mixerPin, mixState);
  borState = !msg.getInt(2); digitalWrite(bormasinaPin, borState);
  Serial.print("ven/mix/bor (inversat): ");
  Serial.print(venState);
  Serial.print(mixState);
  Serial.println(borState);
}

void loop() {
  // get data
  OSCBundle bundle;
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

  touch = cap.filteredData(0);
  micState = digitalRead(microscopPin);
  carState = digitalRead(cartePin);

//  Serial.print("Sending MPR: "); Serial.println(touch);

  bundle.empty();
  bundle.add("/03").add(touch);
  bundle.add("/06").add(micState);
  bundle.add("/08").add(carState);
  
  Udp.beginPacket(host_ip, outPort);
  bundle.send(Udp);
  Udp.endPacket();
  bundle.empty();

  delay(100);
}
