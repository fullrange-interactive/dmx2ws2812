/*
*/

#include "FRILCD.h"
#include "Conceptinetics.h"

#define SCREEN_TX       7
#define SCREEN_RX       6

#include <FastLED.h>
#include <Rotary.h>

// RGB leds specs
#define LED_PIN         5
#define NUM_LEDS        20
#define LED_CHANNELS    3

// Rotary encoder specs
#define ENC_A           13
#define ENC_B           11
#define ENC_PB          12

#define LED_TYPE      WS2812
#define COLOR_ORDER   GRB

// Timing constants
#define WAIT_DMX        30
#define WAIT_LEDS       30
#define WAIT_LCD        500
#define WAIT_ENCODER    0

#define DEBOUNCE_ENC_PB 50

#define LCD_WAKE_DURATION 5000

CRGB leds[NUM_LEDS];

FRILCD lcd(SCREEN_RX, SCREEN_TX);

DMX_Slave dmxIn ( NUM_LEDS * LED_CHANNELS );

Rotary rotary = Rotary(ENC_A, ENC_B);

unsigned char DMXValues[NUM_LEDS * LED_CHANNELS];

bool lcdNeedsUpdate = false;
int testGlobal = 0;

struct EncoderAction {
    char direction;
    bool pressed;
};

struct Config {
    int DMXAddress;
};

Config currentConfig;

void setup() {

    currentConfig.DMXAddress = 1;

    // Inputs
    pinMode(ENC_PB, INPUT_PULLUP);
    pinMode(ENC_A, INPUT_PULLUP);
    pinMode(ENC_B, INPUT_PULLUP);

    Serial.begin(9600);

    // Outputs
    pinMode(LED_PIN, OUTPUT);

    // LCD
    lcd.start();
    lcd.clear();

    // LED Strip
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(255);

    // DMX
    dmxIn.enable();
    dmxIn.setStartAddress(currentConfig.DMXAddress);

    delay(1000);

    lcd.setBacklight(false);
}

void updateDMX(unsigned long dt) {

    static unsigned long totalDT = 0;
    static unsigned char lastDMXAddress = currentConfig.DMXAddress;

    totalDT += dt;

    if (lastDMXAddress != currentConfig.DMXAddress) {
        dmxIn.setStartAddress(currentConfig.DMXAddress);
        lastDMXAddress = currentConfig.DMXAddress;
    }

    if (totalDT > WAIT_DMX) {
        for (int led = 0; led < NUM_LEDS; led++) {
            for (int channel = 0; channel < LED_CHANNELS; channel++ ) {
                DMXValues[led * LED_CHANNELS + channel] = dmxIn.getChannelValue(led * LED_CHANNELS + channel + 1);
            }
        }

        totalDT = 0;
    }

}

void updateLEDs(unsigned long dt) {

    static unsigned long totalDT = 0;
    totalDT += dt;

    if (totalDT > WAIT_LEDS) {
        for (int led = 0; led < NUM_LEDS; led++) {
            unsigned char r = DMXValues[led * LED_CHANNELS];
            unsigned char g = DMXValues[led * LED_CHANNELS + 1];
            unsigned char b = DMXValues[led * LED_CHANNELS + 2];
            leds[led] = CRGB(r, g, b);
        }

        FastLED.setBrightness(255);
        FastLED.show();

        totalDT = 0;
    }

}

EncoderAction updateEncoder(unsigned long dt) {

    static unsigned long totalDT = 0;
    totalDT += dt;

    static bool lastPressed = false;
    static unsigned long lastPressedTime = 0;

    EncoderAction result;

    if (totalDT > WAIT_ENCODER) {
        
        result.direction = rotary.process();

        bool pinActive = digitalRead(ENC_PB);
        // if (pressed != lastPressed) {
        //     lastPressedTime = millis();
        // }
        // if ((millis - lastPressedTime) > DEBOUNCE_ENC_PB) {
        //     result.pressed = true;
        // }
        // lastPressed = pressed;

        result.pressed = !pinActive;

        totalDT = 0;
    }

    return result;

}

void updateLCD(unsigned long dt, EncoderAction action) {

    static unsigned long screenWakeDt = 0;
    static bool isWoke = false;
    screenWakeDt += dt;


    // Keep scren woke
    if (isWoke && (action.direction != DIR_NONE || action.pressed)) {
        screenWakeDt = 0;
    }

    if (isWoke && action.direction != DIR_NONE) {

        if (action.direction == DIR_CW) {
            currentConfig.DMXAddress++;
            if (action.pressed) {
                currentConfig.DMXAddress += 9;
            }
        } else {
            currentConfig.DMXAddress--;
            if (action.pressed) {
                currentConfig.DMXAddress -= 9;
            }
        }
        if (currentConfig.DMXAddress > 512) {
            currentConfig.DMXAddress = 1;
        }
        if (currentConfig.DMXAddress < 1) {
            currentConfig.DMXAddress = 512;
        } 

        lcd.writeLine1("DMX Address:");
        char buff[16];
        sprintf(buff, "%d", currentConfig.DMXAddress);
        lcd.writeLine2(buff);

    }

    if ((action.pressed || action.direction != DIR_NONE) && !isWoke) {

        screenWakeDt = 0;
        lcd.setBacklight(true);
        isWoke = true;

        delay(50);

        lcd.writeLine1("DMX Address:");
        char buff[16];
        sprintf(buff, "%d", currentConfig.DMXAddress);
        lcd.writeLine2(buff);

        // lcd.writeLine1("DMX Address:");

        // char buff[16];
        // sprintf(buff, "%d", currentConfig.DMXAddress);
        // lcd.writeLine2(buff);

    }

    if (isWoke && screenWakeDt > LCD_WAKE_DURATION) {
        isWoke = false;
        lcd.setBacklight(false);
    }
    //     lc
    //     char buff[16];
    //     itoa(testGlobal, buff, 10);
    //     lcd.writeLine2(buff);

    //     totalDT = 0;
    //     lcdNeedsUpdate = false;
    // }

}

unsigned long lastT = 0;

void loop() {

    unsigned long now = millis();
    unsigned long dt = now - lastT;
    lastT = now;

    updateDMX(dt);
    updateLEDs(dt);
    EncoderAction action = updateEncoder(dt);
    updateLCD(dt, action);
    
}
