// OSC out: 16 - fotorezistor valiza, 22 - vibratie lana

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
  
  pinMode(valizaPin, INPUT);
  pinMode(vib1, INPUT);
//  pinMode(ledPin, OUTPUT);

  WS2812B.begin();  // INITIALIZE WS2812B strip object (REQUIRED)
}

void loop() {
  OSCBundle bundle;
  
  // send data
  vib1state = digitalRead(vib1);
  Serial.print("Vib lana: ");Serial.println(vib1state);
  valiza = digitalRead(valizaPin);
  Serial.print("Valiza: ");Serial.println(valiza);


  if (valiza) {
    // 1 = valiza e inchisa
    WS2812B.clear();
    WS2812B.show();
    Serial.println(valiza);
  } else {
    for (int pixel = 0; pixel < NUM_PIXELS; pixel++) {           // for each pixel
      WS2812B.setPixelColor(pixel, WS2812B.Color(80, 60, 10));  // it only takes effect if pixels.show() is called
      WS2812B.show();                                           // send the updated pixel colors to the WS2812B hardware.
    }
    WS2812B.setBrightness(200);
    WS2812B.show();   
  }


  bundle.empty();
  bundle.add("/16").add(valiza);
  bundle.add("/22").add(vib1state);
  Udp.beginPacket(host_ip, outPort);
  bundle.send(Udp);
  Udp.endPacket();
  bundle.empty();

  delay(20);
}
