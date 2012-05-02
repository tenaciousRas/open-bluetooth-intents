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
*/

// Includes
#include "Bessan.h"

extern "C" {
#include <stdlib.h>
}



// Private methods
void Bessan::processCommand(){
	if(buffer[0]-FunctionBufferOffset < FunctionBufferLenght){
		void (*H_FuncPtr)(uint8_t, uint8_t) = intFunc[buffer[0]-FunctionBufferOffset];
		if (H_FuncPtr != 0) {
			H_FuncPtr(buffer[0], getArrayLength());
		}
		else {
			send("Flag not registered: ");
			send(buffer[0]);
		}
	}
	else {
		if (customErrorFunc)
			errorFunc(buffer[0], getArrayLength());
		else {
			send("Flag out of bounds: ");
			send(buffer[0]);
		}
	}
}


void Bessan::init()
{
	waitTime = 30;
	startFlag = 18;
	ack = 19;
	abord = 27;
	delimiter = 59; //';'

	numberOfValues = 0;
	
	for(int a = 0;a < FunctionBufferLenght;a++){
		intFunc[a] = errorFunc;
	}
}


// public methods
Bessan::Bessan() : mySerial(0, 1, false)
{
    // it is hard to use member function pointer together with normal function pointers.
    customErrorFunc = false;
	errorFunc = 0;
	init();
}

// Constructur for use with HardwareSerial library
Bessan::Bessan(H_voidFuncPtr err) : mySerial(0, 1, false)
{
    customErrorFunc = true;
	errorFunc = err;
	init();
}

void Bessan::registerFunction(void(*userfunction)(uint8_t, uint8_t),uint8_t command){
	intFunc[command-FunctionBufferOffset] = userfunction;
}
void Bessan::unregisterFunction(uint8_t command){
	intFunc[command-FunctionBufferOffset] = errorFunc;
}

bool Bessan::receive(){
	uint8_t lastByte;
	boolean timeout = false;
	while(!timeout)
	{
		while(mySerial.available() > 0)
		{
			lastByte = mySerial.read();
			
			if(lastByte == abord){
				flush();
			}
			else if(lastByte == ack){
				processCommand();
				flush();
			}
			else if(bufferCount < ByteBufferLength){
				buffer[bufferCount] = lastByte;
				bufferCount++;
			}
			else return false;
		}
        // not sure if we should ever stall - but for now
        // only stall if we're waiting for command
		if(mySerial.available() <= 0 && !timeout && bufferCount > 0){
			if(waitTime > 0) delayMicroseconds(waitTime);
			if(mySerial.available() <= 0) timeout = true;
		}
	}
	return timeout;
}




void Bessan::getBuffer(uint8_t buf[]){

	for(int a = 0;a < bufferCount;a++){
		buf[a] = buffer[a];
	}
}

void Bessan::getString(char string[]){

	for(int a = 1;a < bufferCount;a++){
		string[a-1] = buffer[a];
	}
	string[bufferCount-1] = '\0';
}

int Bessan::getInt()
{
	uint8_t b[bufferCount];
	for(int a = 1;a < bufferCount;a++){
		b[a-1] = buffer[a];
	}

	b[bufferCount-1] = '\0';
	return atoi((char*)b);
}

long Bessan::getLong()
{
	uint8_t b[bufferCount];
	for(int a = 1;a < bufferCount;a++){
		b[a-1] = buffer[a];
	}

	b[bufferCount-1] = '\0';
	return atol((char*)b);
}

float Bessan::getFloat()
{
	return (float)getDouble();
}

int Bessan::getArrayLength()
{
	if (bufferCount == 1) return 0; // only a flag and ack was sent, not data attached
	numberOfValues = 1;
	// find the amount of values we got
	for (int a=1; a<bufferCount;a++){
		if (buffer[a]==delimiter) numberOfValues++;
	}
	return numberOfValues;
}

void Bessan::getFloatValues(float values[])
{
	int t = 0; // counter for each char based array
	int pos = 0;

	int start = 1; // start of first value
	for (int end=1; end<bufferCount;end++){
		// find end of value
		if (buffer[end]==delimiter) {
			// now we know start and end of a value
			char b[(end-start)+1]; // create container for one value plus '\0'
			t = 0;
			for(int i = start;i < end;i++){
				b[t++] = (char)buffer[i];
			}
			b[t] = '\0';
			values[pos++] = atof(b);
			start = end+1;
		}
	}
	// get the last value
	char b[(bufferCount-start)+1]; // create container for one value plus '\0'
	t = 0;
	for(int i = start;i < bufferCount;i++){
		b[t++] = (char)buffer[i];
	}
	b[t] = '\0';
	values[pos] = atof(b);
}

// not tested yet
void Bessan::getDoubleValues(float values[])
{
	getFloatValues(values);
}

// not tested yet
void Bessan::getIntValues(int values[])
{
	int t = 0; // counter for each char based array
	int pos = 0;

	int start = 1; // start of first value
	for (int end=1; end<bufferCount;end++){
		// find end of value
		if (buffer[end]==delimiter) {
			// now we know start and end of a value
			char b[(end-start)+1]; // create container for one value plus '\0'
			t = 0;
			for(int i = start;i < end;i++){
				b[t++] = (char)buffer[i];
			}
			b[t] = '\0';
			values[pos++] = atoi(b);
			start = end+1;
		}
	}
	// get the last value
	char b[(bufferCount-start)+1]; // create container for one value plus '\0'
	t = 0;
	for(int i = start;i < bufferCount;i++){
		b[t++] = (char)buffer[i];
	}
	b[t] = '\0';
	values[pos] = atoi(b);
}


double Bessan::getDouble()
{
	char b[bufferCount];
	for(int a = 1;a < bufferCount;a++){
		b[a-1] = (char)buffer[a];
	}

	b[bufferCount-1] = '\0';
	return atof(b);
	
}


void Bessan::write(uint8_t b){
	uint8_t numBytes = mySerial.print(b);
}

void Bessan::send(char c ){
	mySerial.print(startFlag);
	mySerial.print(c);
	mySerial.print(ack);
}

void Bessan::send(const char str[]){
	mySerial.print(startFlag);
	mySerial.print(str);
	mySerial.print(ack);
}
void Bessan::send(uint8_t n){
	mySerial.print(startFlag);
	mySerial.print(n);
	mySerial.print(ack);
}
void Bessan::send(int n){
	mySerial.print(startFlag);
	mySerial.print(n);
	mySerial.print(ack);
}
void Bessan::send(unsigned int n){
	mySerial.print(startFlag);
	mySerial.print(n);
	mySerial.print(ack);
}
void Bessan::send(long n){
	mySerial.print(startFlag);
	mySerial.print(n);
	mySerial.print(ack);
}
void Bessan::send(unsigned long n){
	mySerial.print(startFlag);
	mySerial.print(n);
	mySerial.print(ack);
}
void Bessan::send(long n, int base){
	mySerial.print(startFlag);
	mySerial.print(n, base);
	mySerial.print(ack);
}
void Bessan::send(double n){
	mySerial.print(startFlag);
	mySerial.print(n);
	mySerial.print(ack);
}
void Bessan::sendln(void){
	mySerial.print(startFlag);
	mySerial.println();
	mySerial.print(ack);
}

void Bessan::flush(){
	for(uint8_t a=0; a < ByteBufferLength; a++){
		buffer[a] = 0;
	}
	bufferCount = 0;
	numberOfValues = 0;
}