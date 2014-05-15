//handles basic device IO
#include <Arduino.h>
#include <Bounce.h>

#include "Options.h"

IO::IO() {                                                                      //default constructor, sets up all the inputs and outputs                                                
    digitalWrite(RESETPIN, HIGH);                                               //this needs to be set high before anything else so the device doesn't reset
    pinMode(RESETPIN, OUTPUT);
    pinMode(CONLED, OUTPUT);
    pinMode(LCDPOWPIN, OUTPUT);
    pinMode(FN1PIN, INPUT);
    pinMode(FN2PIN, INPUT);
    pinMode(REDLITE, OUTPUT);
    pinMode(GREENLITE, OUTPUT);
    pinMode(BLUELITE, OUTPUT);
    dbFN1 = Bounce();                                                           //set up the function button debouncing
    dbFN2 = Bounce(); 
    dbFN1.attach(FN1PIN);
    dbFN2.attach(FN2PIN);
}

void IO::connectionLED(byte mode) {                                             //controls the connection LED, needs a mode byte
    switch(mode) {
        case 0:                                                                 //turn the LED off
            digitalWrite(CONLED, LOW);
            break;  
        case 1:                                                                 //turn the LED on
            digitalWrite(CONLED, HIGH);
            break;
        case 2:                                                                 //blink the LED (non-blocking, must be continuously called to blink)
            unsigned long currentMillis = millis();
            if(currentMillis - previousMillis > BLINKTIME) {
                previousMillis = currentMillis;
                if (blinkState) {
                    blinkState = 0;
                }
                else {
                    blinkState = 1;
                }
                connectionLED(blinkState);
            }
            break;
        default:
            break;
    }
}

byte IO::checkButtons() {                                                       //checks the debounced buttons for any changes, needs to be called continuously
    if(dbFN1.update()) {                                                        //if fn1's state changed
        if(!dbFN1.read()) {                                                     //if the button was just released
            return 1;
        }                              
    }
    else if(dbFN2.update()) {                                                   //if fn2's state changed
        if(!dbFN2.read()) {                                                     //if the button was just released
            return 2;
        }
    }
    else {
        return 0;                                                               //no button state changes
    }
}

int IO::checkPot() {                                                            //used to check the speed pot position, should be called continuously
    return analogRead(SPEEDPIN) / 2;
}

//==============================================================================

void IO::setBacklight(uint8_t r, uint8_t g, uint8_t b, byte brightness) {       //set the backlight to a specific color and brightness
    r = map(r, 0, 255, 0, brightness);
    g = map(g, 0, 255, 0, brightness);
    b = map(b, 0, 255, 0, brightness);

    //common anode so invert
    r = map(r, 0, 255, 255, 0);
    g = map(g, 0, 255, 255, 0);
    b = map(b, 0, 255, 255, 0);
    
    red = r;
    green = g;
    blue = b;

    analogWrite(REDLITE, red);
    analogWrite(GREENLITE, green);
    analogWrite(BLUELITE, blue);
}

//void IO::setBlinkSpeed(byte in) {                                               //used to get the value from Options
//    blinkSpeed = in;
//}

//void IO::enableTweetBlink(bool in) {                                            //used to get the value from Options
//    blinkEnabled = in;
//}

void IO::tweetBlink() {                                                         //used to trigger a tweet blink, should be called while waiting in the tweet beginning
//    if (blinkEnabled) {
    if (opt.getBlink()) {
        unsigned long currentMillis = millis();
//        if(currentMillis - previousMillis5 > blinkSpeed) {
        if(currentMillis - previousMillis5 > opt.getBlinkSpd()) {
            previousMillis5 = currentMillis;
            if(count == 4) {
                count = 0;
                timeToBlink = false;
            }
            else if(count % 0) {
                opt.updateCol();
            }
            else {
                opt.updateBlinkCol();
            }
            count++;
        }
    }
}

void IO::rainbow() {
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis6 > rainSpeed) {
        previousMillis6 = currentMillis;
        if(rainLevel < 255) {
            switch(currentColor) {
                case 0:
                    opt.setCol(rain, 0, 255-rain);
                    break;
                case 1:
                    opt.setCol(255-rain, rain, 0);
                    break;
                case 2:
                    opt.setCol(0, 255-rain, rain);
                    break;
            }
            rainLevel++;
        }
        else {
            rainLevel = 0;
            currentColor++;
            if(currentColor == 3) {
                currentColor = 0;
            }
        }
    }
}