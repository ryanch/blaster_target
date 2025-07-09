
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#ifndef BL_TARGET_H
#define BL_TARGET_H

#include <string> // Example: if std::string is used in the class

class BLTarget {
public:

    void setup(int pin, int ledPin);
    void loop();
    bool checkForPressSinceLastLoop();
    bool checkForLongPressSinceLastLoop();

    void startHitAnimation();
    void startSeekAnimation();
    void startBlankAnimation();

    void startLedCountAnimation( int color, int count);

private:

    // the pin for this button
    int buttonPin = 0;

    // tracking when the button was hit
    int hitCount;
    int frameCount;
    unsigned long lastAnimationTime;

    // used to debounce button hits
    int lastSteadyState = LOW;
    int lastFlickerableState = LOW;
    int currentState;
    unsigned long lastDebounceTime = 0;
    unsigned long lastHoldDownTime = 0;


    // private functions for when buttons are pressed
    void onButton();
    void onLongButtonPress();

    // tracking button events
    bool pressSinceLastLoop = false;
    bool longPressSinceLastLoop = false;


    // the strip for this button
    Adafruit_NeoPixel *strip;

    // animation 
    int animationFrame = 0;
    unsigned long lastFrameTime = 0;
    int animationType = 0;
    long animationStepTime = 500;

    // count color animation
    uint32_t ledCountAnimationColor = 0;
    int ledCountAnimationCount = 0;

    void syncStripToHitCount();
    void setStripToShowCountWithColor(int count, uint32_t color);

    
};

#endif // BL_TARGET_H