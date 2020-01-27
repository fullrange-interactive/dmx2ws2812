/*
*/

#include "FRILCD.h"
#include "Conceptinetics.h"

#define SCREEN_TX		3
#define SCREEN_RX		4

FRILCD lcd(SCREEN_RX, SCREEN_TX);
DMX_Slave dmxIn ( 1 );

void setup() {

	pinMode(13, OUTPUT);

	Serial.begin(9600);

	lcd.start();

	// lcd.setBacklight(true);

	// lcd.setSplashScreen("FRI DMX-WS2812", "v1.0");

	lcd.clear();
	lcd.writeLine1("DMX Value:");
	lcd.writeLine2("0");

	dmxIn.enable();
	dmxIn.setStartAddress(1);

	delay(1000);
}

int val = 0;

void loop() {

	// return;

	val = dmxIn.getChannelValue(1);

	char buffer[16];
	itoa(val, buffer, 10);

	lcd.writeLine1("DMX Value:");
	lcd.writeLine2(buffer);

	if (val > 128) {
		digitalWrite(13, HIGH);
	} else {
		digitalWrite(13, LOW);
	}

	delay(500);
	
}
