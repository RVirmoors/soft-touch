// OSC out: 14 - senzor apa, 15 - fotorezistor gandac1, 21 - lumina gandac2 (i2c)

// https://circuitdigest.com/microcontroller-projects/arduino-pcf8591-adc-dac-module-interfacing

#include "WiFiConfig.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include <Wire.h>

#define PCF8591 (0x90 >> 1) // I2C bus address
#define AIn0 0x00

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
OSCErrorCode error;
const unsigned int outPort = 9914;          // remote port to send OSC to Max
const unsigned int localPort = 8814;        // local port to listen for OSC packets

const int gandacPin = 13;  // D7
const int gandacAnalogpin = A0;

unsigned int gandacState = LOW;
int gandacAnalog = 0;
int borcanAnalog14 = 0;


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
  Wire.begin();
  pinMode(gandacPin, INPUT);
}

void loop() {
  OSCBundle bundle;

  // send data
  gandacAnalog = analogRead(gandacAnalogpin);

  gandacState = digitalRead(gandacPin);

  Wire.beginTransmission(PCF8591);
  Wire.write(AIn0);
  Wire.endTransmission();
  Wire.requestFrom(PCF8591, 1);
  borcanAnalog14 = Wire.read();

  bundle.empty();
  bundle.add("/14").add(borcanAnalog14);
  bundle.add("/15").add(gandacState).add(gandacAnalog);

  Serial.print(gandacAnalog);Serial.print(" ");Serial.print(gandacState);Serial.print(" ");
  Serial.println(borcanAnalog14);
  
  Udp.beginPacket(host_ip, outPort);
  bundle.send(Udp);
  Udp.endPacket();
  bundle.empty();

  delay(10);
}
