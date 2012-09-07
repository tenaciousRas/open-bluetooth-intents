/*
  SpeakToArduino (works with Amarino 2.0 and the SpeakToArduino Android app)
  
  - counterpart of the Amarino 2.0 SpeakToArduino app
  - receives events from Amarino turning lights on and off and changes
    its color using speech recognision
  
  author: Free Beachler - February 2012
*/
 
#include <SoftwareSerial.h> 
#include <Obi.h>

// declare obi so that you can call functions with it
Obi obi;

int inbuiltLED = 13;   

void setup()  
{
  // use the baud rate your bluetooth module is configured to 
  // not all baud rates are working well, i.e. ATMEGA168 works best with 57600
  Serial.begin(57600); 
  
  // register callback functions, which will be called when an associated event occurs.
  obi.registerFunction(ledOn, 'o');
  obi.registerFunction(ledOff, 'x');

  // set all color leds as output pins
  pinMode(inbuiltLED, OUTPUT);
  
  // flash LED low so that we see startup
  digitalWrite(inbuiltLED, LOW);
  digitalWrite(inbuiltLED, HIGH);
  digitalWrite(inbuiltLED, LOW);
  digitalWrite(inbuiltLED, HIGH);
  digitalWrite(inbuiltLED, LOW);
  digitalWrite(inbuiltLED, HIGH);
}

void loop()
{
  obi.receive(); // you need to keep this in your loop() to receive events
}

void ledOn(byte flag, byte numOfValues)
{
	digitalWrite(inbuiltLED, HIGH);
}

void ledOff(byte flag, byte numOfValues)
{
	digitalWrite(inbuiltLED, LOW);
}

