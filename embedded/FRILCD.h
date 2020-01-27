#ifndef FRILCD_h
#define FRILCD_h

#include "Arduino.h"
#include <SoftwareSerial.h>

class FRILCD : public SoftwareSerial
{
	public:
		FRILCD(int rxPin, int txPin);

		void start();

		void clear();
		void setSplashScreen(char* line1, char* line2);

		void setBacklight(bool on);

		void writeLine1(char* text);
		void writeLine2(char* text);

		void clearLine1();
		void clearLine2();
};

#endif