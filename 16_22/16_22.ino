// OSC in: on/off LED-uri valiza
// OSC out: 16 - vibratie valiza, 22 - vibratie lana

#include "WiFiConfig.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>

#include <Adafruit_NeoPixel.h>

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
OSCErrorCode error;
const unsigned int outPort = 9916;          // remote port to receive OSC
const unsigned int localPort = 8816;        // local port to listen for OSC packets

const int vib1 = 14;     // D5
const int valizaPin = 12;  // D6
const int ledPin = 13;   // D7

#define NUM_PIXELS     24
Adafruit_NeoPixel WS2812B(NUM_PIXELS, 13, NEO_GRB + NEO_KHZ800);

unsigned int vib1state = LOW;
unsigned int valiza = LOW;
unsigned int ledState = LOW;


void setup() {
  Serial.begin(115200);
  delay(10);

  IPAddress local_IP(192, 168, 50, 4);
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
  
  pinMode(valizaPin, INPUT);
  pinMode(vib1, INPUT);
//  pinMode(ledPin, OUTPUT);

  WS2812B.begin();  // INITIALIZE WS2812B strip object (REQUIRED)
}

void leduri(OSCMessage &msg) {
  ledState = !msg.getInt(0);
  Serial.print("leduri: "); Serial.println(ledState);
  if (ledState) {
    // 1 = valiza e inchisa
    WS2812B.clear();
    WS2812B.show();
  } else {
    for (int pixel = 0; pixel < NUM_PIXELS; pixel++) {           // for each pixel
      WS2812B.setPixelColor(pixel, WS2812B.Color(6, 4, 1));  // it only takes effect if pixels.show() is called
    }
    WS2812B.show();   
  }
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
      bundle.dispatch("/16", leduri);
    } else {
      error = bundle.getError();
      Serial.print("error: ");
      Serial.println(error);
    }
  }  
  
  // send data
  vib1state = digitalRead(vib1);
  Serial.print("Vib lana: ");Serial.println(vib1state);
  valiza = digitalRead(valizaPin);
  Serial.print("Valiza: ");Serial.println(valiza);

  bundle.empty();
  bundle.add("/16").add(valiza);
  bundle.add("/22").add(vib1state);
  Udp.beginPacket(host_ip, outPort);
  bundle.send(Udp);
  Udp.endPacket();
  bundle.empty();

  delay(20);
}
