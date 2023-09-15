// OSC out: 12 - IMU ghem

#include "WiFiConfig.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>

#include <Wire.h>

// MMA8452Q I2C address is 0x1C(28)
#define Addr 0x1C

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
OSCErrorCode error;
const unsigned int outPort = 9912;          // remote port to receive OSC
const unsigned int localPort = 8812;        // local port to listen for OSC packets

void setup() {
  Serial.begin(115200);
  delay(10);

  // I2C stuff
  Wire.begin();
  Serial.begin(9600);
  Wire.beginTransmission(Addr);
  Wire.write(0x2A); // Select control register
  Wire.write(0x00); // StandBy mode
  Wire.endTransmission();
  Wire.beginTransmission(Addr);
  Wire.write(0x2A);
  Wire.write(0x01);
  Wire.endTransmission();
  Wire.beginTransmission(Addr);
  Wire.write(0x0E);
  Wire.write(0x00); // Set range to +/- 2g
  Wire.endTransmission();
  delay(300);
  
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
}

void loop() {
  OSCBundle bundle;
  
  // send data
  unsigned int data[7];

  // Request 7 bytes of data
  Wire.requestFrom(Addr, 7);
  
  // Read 7 bytes of data
  // staus, xAccl lsb, xAccl msb, yAccl lsb, yAccl msb, zAccl lsb, zAccl msb
  if(Wire.available() == 7) 
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
    data[2] = Wire.read();
    data[3] = Wire.read();
    data[4] = Wire.read();
    data[5] = Wire.read();
    data[6] = Wire.read();
  }

  // Convert the data to 12-bits
  int xAccl = ((data[1] * 256) + data[2]) / 16;
  if (xAccl > 2047)
  {
    xAccl -= 4096;
  }
  int yAccl = ((data[3] * 256) + data[4]) / 16;
  if (yAccl > 2047)
  {
    yAccl -= 4096;
  }
  int zAccl = ((data[5] * 256) + data[6]) / 16;
  if (zAccl > 2047)
  {
    zAccl -= 4096;
  }

  // Output data to serial monitor
  Serial.print("Acceleration: ");
  Serial.print(xAccl); Serial.print(yAccl); Serial.println(zAccl);

  bundle.empty();
  bundle.add("/12").add(xAccl).add(yAccl).add(zAccl);
  
  Udp.beginPacket(host_ip, outPort);
  bundle.send(Udp);
  Udp.endPacket();
  bundle.empty();

  delay(20);  // 500?
}
