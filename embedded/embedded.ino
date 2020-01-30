/*
*/

#include "FRILCD.h"
#include "Conceptinetics.h"

#define SCREEN_TX		3
#define SCREEN_RX		4

#include <FastLED.h>
// RGB leds specs
#define LED_PIN       5
#define RING          0
#define STRIP         150
#define NUM_LEDS      RING + STRIP

#define LED_TYPE      WS2812
#define COLOR_ORDER   GRB

CRGB leds[NUM_LEDS];

FRILCD lcd(SCREEN_RX, SCREEN_TX);
DMX_Slave dmxIn ( NUM_LEDS * 3 );

void setup() {

	pinMode(13, OUTPUT);

	pinMode(LED_PIN, OUTPUT);

	Serial.begin(9600);

	lcd.start();

	  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
	  .setCorrection(TypicalLEDStrip);

	  FastLED.setBrightness(255);
	  FastLED.show();

	// lcd.setBacklight(true);

	// lcd.setSplashScreen("FRI DMX-WS2812", "v1.0");

	lcd.clear();
	lcd.writeLine1("DMX Value:");
	lcd.writeLine2("0");

	dmxIn.enable();
	dmxIn.setStartAddress(1);

	delay(1000);
}

void updateStrip(DMX_Slave& slave) {
  for (uint16_t i = RING; i < NUM_LEDS; i++) {

	int rIn = dmxIn.getChannelValue(i*3 + 1);
	int gIn = dmxIn.getChannelValue(i*3 + 2);
	int bIn = dmxIn.getChannelValue(i*3 + 3);
    leds[i] = CRGB(rIn,
                   gIn,
                   bIn);

  }

  FastLED.setBrightness(255);
  FastLED.show();

}

int wait = 0;

void loop() {

	// return;

	delay(50);
	int dimIn = 255; //dmxIn.getChannelValue(1);
	// int rIn = dmxIn.getChannelValue(1);
	// int gIn = dmxIn.getChannelValue(2);
	// int bIn = dmxIn.getChannelValue(3);
	// int shutterIn = dmxIn.getChannelValue(5);

	int val = dmxIn.getChannelValue(1);
	char buffer[16];
	itoa(val, buffer, 10);	// if (wait == 0) {

	// 	lcd.writeLine1("DMX Value:");
	// 	lcd.writeLine2(buffer);
	// }
	// wait++;
	// if (wait == 20) {
	// 	wait = 0;
	// }



	// if (val > 128) {
	// 	digitalWrite(13, HIGH);
	// } else {
	// 	digitalWrite(13, LOW);
	// }

	updateStrip(dmxIn);

	// delay(500);
	
}
