#include "FRILCD.h"

#define WAIT_START 700

#define EMPTY_LINE "                "

#define CONTROL_1 0xfe

#define CLEAR_DISPLAY 0x01
#define MOVE_RIGHT_1 0x14
#define MOVE_LEFT_1 0x10

#define LINE_1 0x80
#define LINE_2 0xc0


#define CONTROL_2 0x7c

#define SET_SPLASH 0x0a
#define BACKLIGHT_OFF 0x80
#define BACKLIGHT_ON 0x9d

FRILCD::FRILCD(int rxPin, int txPin) : SoftwareSerial(rxPin, txPin)
{

}

void FRILCD::start()
{
	begin(9600);
	delay(WAIT_START);
}

void FRILCD::clear() 
{

	write(CONTROL_1);
	write(CLEAR_DISPLAY);

}

void FRILCD::setSplashScreen(char* line1, char* line2)
{

	clear();
	write(line1);

	write(CONTROL_1);
	write(LINE_2);
	write(line2);

	write(CONTROL_2);
	write(SET_SPLASH);

}

void FRILCD::setBacklight(bool on)
{

	write(CONTROL_2);
	if (on)
		write(BACKLIGHT_ON);
	else
		write(BACKLIGHT_OFF);

}

void FRILCD::writeLine1(char* text) 
{

	clearLine1();

	write(CONTROL_1);
	write(LINE_1);

	write(text);

}

void FRILCD::writeLine2(char* text)
{

	clearLine2();

	write(CONTROL_1);
	write(LINE_2);

	write(text);

}

void FRILCD::clearLine1()
{

	write(CONTROL_1);
	write(LINE_1);
	write(EMPTY_LINE);

}

void FRILCD::clearLine2()
{

	write(CONTROL_1);
	write(LINE_2);
	write(EMPTY_LINE);

}