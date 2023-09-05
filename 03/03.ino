// OSC out: 03 - atingere bormasina MPR121
// OSC in: relee on/off ventilator, mixer, bormasina

#include "WiFiConfig.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

#include "Adafruit_MPR121.h"

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
const unsigned int outPort = 9903;          // remote port to receive OSC
const unsigned int localPort = 8803;        // local port to listen for OSC packets (actually not used for sending)

Adafruit_MPR121 cap = Adafruit_MPR121();
float touch = 0.0;
const int ventilatorPin = 4;
const int mixerPin = 5;
const int bormasinaPin = 6;

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
  digitalWrite(ventilatorPin, LOW);digitalWrite(mixerPin, LOW);digitalWrite(bormasinaPin, LOW);
}


void loop() {
  Serial.println(cap.touched(), HEX);
  Serial.print(cap.filteredData(0)); Serial.print("\t");
  Serial.print(cap.baselineData(0)); Serial.print("\t");
  Serial.println();

  touch = cap.filteredData(0);
  
  OSCMessage msg("/03");
  msg.add(touch);
  Udp.beginPacket(host_ip, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();

  delay(100);
}
