// OSC out: 03 - atingere bormasina MPR121, 06 - distanta microscop, 08 - fotorezistor carte
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

Adafruit_MPR121 cap = Adafruit_MPR121(); // D1 & D2
const int ventilatorPin = 0;  // D3
const int mixerPin = 2;       // D4
const int bormasinaPin = 14;   // D5
const int cartePin = 12;  // D6
const int TRIG_PIN = 15;       // D8
const int ECHO_PIN = 13;       // D7

long duration;
int distance;

float touch = 0.0;
unsigned int venState = LOW;
unsigned int mixState = LOW;
unsigned int borState = LOW;
unsigned int carState = LOW;

void setup() {
  Serial.begin(115200);
  delay(10);

  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");
  
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
  pinMode(cartePin, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
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

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.017;

  touch = cap.baselineData(0) - cap.filteredData(0);
  carState = digitalRead(cartePin);

  Serial.print("Sending MPR: "); Serial.println(touch);
  Serial.print("Dist: "); Serial.println(distance);
  Serial.print("Carte: "); Serial.println(carState);

  bundle.empty();
  bundle.add("/03").add(touch);
  bundle.add("/06").add(distance);
  bundle.add("/08").add(carState);
  
  Udp.beginPacket(host_ip, outPort);
  bundle.send(Udp);
  Udp.endPacket();
  bundle.empty();

  delay(100);
}
