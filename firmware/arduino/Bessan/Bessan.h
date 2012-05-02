/*
  Bessan.h - Arduino Library for bluetooth communication
  Copyright (c) 2012 Free Beachler.  All right reserved.
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 3.0 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License v3.0 for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Acknowledgements:
  This library is based on the Amarino library.  Amarino is based
  on the SerialHandler library from Madrang of The
  Warrent Team.

 Following changes were made:
      - Full support for SoftwareSerial or NewSoftSerial.  If you haven't
             updated your Arduino IDE this will break for you, but
             you should have already updated.  If not, install 
             the NewSoftSerial library from http://arduiniana.org/libraries/newsoftserial/.
	  - ByteBufferLength is named properly - increased from 64 to 128

  last modified by Longevity Software LLC 5/1/2012
*/

#ifndef Bessan_h
#define Bessan_h

#define ByteBufferLength 128
#define FunctionBufferLenght 75 // 48-122 (in ascii: 0 - z)
#define FunctionBufferOffset 48  // offset to calc the position in the function buffer ('0' should be stored in intFunc[0])
#define _BESSAN_VERSION 1 

#include <inttypes.h>
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#include "/Applications/Arduino.app/Contents/Resources/Java/libraries/SoftwareSerial/SoftwareSerial.h"
#else
#include "WProgram.h"
#include "WConstants.h"
#include "NewSoftSerial.h"
#endif

#ifndef AMARINO_RX
#define AMARINO_RX 0
#endif
#ifndef AMARINO_TX
#define AMARINO_TX 1
#endif


/******************************************************************************
* Definitions
******************************************************************************/

class Bessan

{
private:
    uint8_t rx;
    uint8_t tx;
	// per object data
	uint8_t bufferCount;
	uint8_t buffer[ByteBufferLength];
	
	int numberOfValues;
	
	char abord;
	char ack;
	char delimiter;
	char startFlag; // used to communicate with Android (leads each msg to Android)
	
	bool customErrorFunc;

	typedef void (*H_voidFuncPtr)(uint8_t, uint8_t);
	H_voidFuncPtr intFunc[FunctionBufferLenght];
	H_voidFuncPtr errorFunc;
    SoftwareSerial mySerial;

	// static data

	// private methods
	void processCommand(void);
	void init(void);
	int getArrayLength();

public: 
	// public methods
	Bessan(H_voidFuncPtr err);
	Bessan(void);
    Bessan(uint8_t, uint8_t);
	
	void flush(void);
	bool receive(void);
	void registerFunction(void(*)(uint8_t, uint8_t),uint8_t);
	void unregisterFunction(uint8_t);
	int bufferLength(){return bufferCount;} // buffer withouth ACK
	int stringLength(){return bufferCount;} // string without flag but '/0' at the end
	void getBuffer(uint8_t[]);
	
	void getString(char[]);
	int getInt();
	long getLong();
	float getFloat();
	double getDouble();
	void getIntValues(int[]);
	void getFloatValues(float[]);
	void getDoubleValues(float[]); // in Arduino double and float are the same
	
	void write(uint8_t);

	void send(char);
    void send(const char[]);
    void send(uint8_t);
    void send(int);
    void send(unsigned int);
    void send(long);
    void send(unsigned long);
    void send(long, int);
    void send(double);
    void sendln(void);


	uint16_t waitTime;
	
	static int library_version() { 
		return _BESSAN_VERSION;} 
};

// Arduino 0012 workaround
#undef int
#undef char
#undef long
#undef byte
#undef float
#undef abs
#undef round 

#endif
