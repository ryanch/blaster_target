#include "bl_target.h" // Include the corresponding header file
#include "const.h"
#include <Arduino.h>

void BLTarget::setup(int pin, int ledPin) {

    strip = new Adafruit_NeoPixel(G_LED_COUNT , ledPin, NEO_RGB + NEO_KHZ800);
    strip->begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    strip->show();            // Turn OFF all pixels ASAP
    strip->setBrightness(BRIGHTNESS);

    buttonPin = pin;
    pinMode(buttonPin, INPUT_PULLUP);
}

void BLTarget::loop() {


    pressSinceLastLoop = false;
    longPressSinceLastLoop = false;

    // read the button
    currentState = digitalRead(buttonPin);

    // de-bounce
    if (currentState != lastFlickerableState) {
        lastDebounceTime = millis();
        lastFlickerableState = currentState;
    }

    // determine if we actually got a press
    if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
        if (lastSteadyState != currentState) {
        lastSteadyState = currentState;
        if (currentState == LOW) {
            onButton();
            lastHoldDownTime = millis();
        }
        else if ( currentState == HIGH) {
            if (millis() - lastHoldDownTime > LONG_BUTTON_PRESS_MS) {
                Serial.println("Long press detected");
                onLongButtonPress();
            }
        }
        }
    }


    // goto next animation frame
    if (millis() - lastFrameTime >= 500) {
        animationFrame++;
        lastFrameTime = millis();
    }

    // do animations


    //////
    ////// HIT ANIMATION
    if ( animationType == ANIMATION_HIT ) {

        if (animationFrame == 0 || animationFrame == 2) {
            for (int i = 0; i < G_LED_COUNT; i++) {
                strip->setPixelColor(i, strip->Color(255, 255, 0));
            }
            strip->show();
        }
        else {
            syncStripToHitCount();
        }

    }
    //////
    //////

    //////
    ////// BLANK ANIMATION
    if ( animationType == ANIMATION_BLANK ) {

        for (int i = 0; i < G_LED_COUNT; i++) {
            strip->setPixelColor(i, strip->Color(0, 0, 0));
        }
        strip->show();

    }
    //////
    //////


    //////
    ////// SEEK ANIMATION
    if ( animationType == ANIMATION_SEEK ) {

        if (animationFrame > 4) {
            animationFrame = 0;
        }

        uint32_t RED = strip->Color(0, 255, 0);
        uint32_t WHITE = strip->Color(255, 255, 255);


        if (animationFrame == 0) {
            strip->setPixelColor(0, RED);
            strip->setPixelColor(1, WHITE);
            strip->setPixelColor(2, WHITE);
            strip->setPixelColor(3, WHITE);
            strip->setPixelColor(4, WHITE);
            strip->setPixelColor(5, WHITE);
            strip->setPixelColor(6, WHITE);
            strip->setPixelColor(7, RED);
        }
        else if (animationFrame == 1) {
            strip->setPixelColor(0, WHITE);
            strip->setPixelColor(1, RED);
            strip->setPixelColor(2, WHITE);
            strip->setPixelColor(3, WHITE);
            strip->setPixelColor(4, WHITE);
            strip->setPixelColor(5, WHITE);
            strip->setPixelColor(6, RED);
            strip->setPixelColor(7, WHITE);
        }
        else if (animationFrame == 2) {
            strip->setPixelColor(0, WHITE);
            strip->setPixelColor(1, WHITE);
            strip->setPixelColor(2, RED);
            strip->setPixelColor(3, WHITE);
            strip->setPixelColor(4, WHITE);
            strip->setPixelColor(5, RED);
            strip->setPixelColor(6, WHITE);
            strip->setPixelColor(7, WHITE);
        }
        else if (animationFrame == 3) {
            strip->setPixelColor(0, WHITE);
            strip->setPixelColor(1, WHITE);
            strip->setPixelColor(2, WHITE);
            strip->setPixelColor(3, RED);
            strip->setPixelColor(4, RED);
            strip->setPixelColor(5, WHITE);
            strip->setPixelColor(6, WHITE);
            strip->setPixelColor(7, WHITE);
        }
        else if (animationFrame == 4) {
            strip->setPixelColor(0, WHITE);
            strip->setPixelColor(1, WHITE);
            strip->setPixelColor(2, WHITE);
            strip->setPixelColor(3, WHITE);
            strip->setPixelColor(4, WHITE);
            strip->setPixelColor(5, WHITE);
            strip->setPixelColor(6, WHITE);
            strip->setPixelColor(7, WHITE);
        }



        strip->show();

    }
    //////
    //////

}

void BLTarget::onButton() {
    pressSinceLastLoop=true;
}

void BLTarget::onLongButtonPress() {
    longPressSinceLastLoop = true;
}


bool BLTarget::checkForPressSinceLastLoop() {
    return pressSinceLastLoop;
}
bool BLTarget::checkForLongPressSinceLastLoop() {
    return longPressSinceLastLoop;
}



void BLTarget::startHitAnimation() {

    Serial.println("start hit");


    animationFrame = 0;
    lastFrameTime = millis();
    animationStepTime = 500;
    animationType = ANIMATION_HIT;

    hitCount++;
    if ( hitCount >= G_LED_COUNT ) {
        hitCount = 0;
    }

}


void BLTarget::startSeekAnimation() {
    Serial.println("start seek");
    Serial.println(buttonPin);

    animationFrame = 0;
    lastFrameTime = millis();
    animationStepTime = 200;
    animationType = ANIMATION_SEEK;

}    
 
void BLTarget::startBlankAnimation() {

    Serial.println("start blank");


    animationFrame = 0;
    lastFrameTime = millis();
    animationStepTime = 500;
    animationType = ANIMATION_BLANK;
}


void BLTarget::syncStripToHitCount() {


  for (int i = 0; i < G_LED_COUNT; i++) {
    if ( hitCount >= i+1 ) {
      strip->setPixelColor(i, strip->Color(0, 255, 0));
    }
    else {
      strip->setPixelColor(i, strip->Color(0, 0, 0));
    }
  }

  strip->show();


}
