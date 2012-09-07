/*
  Sends sensor data to Arduino
  (needs SensorGraph and Amarino app installed and running on Android)
*/
 
#include <SoftwareSerial.h> 
#include <Obi.h>

Obi obi;
int sensor = 5;

void setup()  
{
  // use the baud rate your bluetooth module is configured to 
  // not all baud rates are working well, i.e. ATMEGA168 works best with 57600
  Serial.begin(57600); 
 
  // we initialize analog pin 5 as an input pin
  pinMode(sensor, INPUT);
}

void loop()
{
  obi.receive(); // you need to keep this in your loop() to receive events
  
  // read input pin and send result to Android
  obi.send(analogRead(sensor));
  
  // add a little delay otherwise the phone is pretty busy
  delay(100);
}


