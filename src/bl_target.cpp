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
    if (millis() - lastFrameTime >= animationStepTime) {
        animationFrame++;
        lastFrameTime = millis();
        needRepaint = true;
    }

    // exit early if the last frame is the same as the one we just drew
    if ( !needRepaint ) {
        return;
    }
    needRepaint = false;

    // do animations

    if ( animationType == ANIMATION_COUNT ){

        int countToShow = animationFrame;
        if ( animationFrame > ledCountAnimationCount ) {
            countToShow = ledCountAnimationCount;
        }
        setStripToShowCountWithColor( countToShow, ledCountAnimationColor );

    }

    //////
    ////// HIT ANIMATION
    else if ( animationType == ANIMATION_HIT ) {

        if (animationFrame == 0 || animationFrame == 2) {
            for (int i = 0; i < G_LED_COUNT; i++) {
                strip->setPixelColor(i, strip->Color(255, 255, 0));
            }
            strip->show();
        }
        else {
            //syncStripToHitCount();
            for (int i = 0; i < G_LED_COUNT; i++) {
                strip->setPixelColor(i, strip->Color(0, 0, 0));
            }
            strip->show();

            
        }

    }
    //////
    //////

    //////
    ////// BLANK ANIMATION
    else if ( animationType == ANIMATION_BLANK ) {

        for (int i = 0; i < G_LED_COUNT; i++) {
            strip->setPixelColor(i, strip->Color(0, 0, 0));
        }
        strip->show();

    }
    //////
    //////

    //////
    ////// Hunt Animation
    else if ( animationType == ANIMATION_HUNT ) { 

        if (animationFrame > 2) {
            animationFrame = 0;
        }

        uint32_t RED = strip->Color(0, 255, 0);
        uint32_t ORANGE = strip->Color(165, 255, 0);
        uint32_t BLUE = strip->Color(0, 0, 255);
        uint32_t BLACK = strip->Color(0, 0, 0);


        if (animationFrame == 0) {
            strip->setPixelColor(0, RED);
            strip->setPixelColor(1, RED);
            strip->setPixelColor(2, RED);
            strip->setPixelColor(3, BLUE);
            strip->setPixelColor(4, BLUE);
            strip->setPixelColor(5, ORANGE);
            strip->setPixelColor(6, ORANGE);
            strip->setPixelColor(7, ORANGE);
        }
        else if (animationFrame == 1) {
            strip->setPixelColor(0, ORANGE);
            strip->setPixelColor(1, ORANGE);
            strip->setPixelColor(2, ORANGE);
            strip->setPixelColor(3, RED);
            strip->setPixelColor(4, RED);
            strip->setPixelColor(5, BLUE);
            strip->setPixelColor(6, BLUE);
            strip->setPixelColor(7, BLUE);
        }
        else if (animationFrame == 2) {
            strip->setPixelColor(0, BLUE);
            strip->setPixelColor(1, BLUE);
            strip->setPixelColor(2, BLUE);
            strip->setPixelColor(3, ORANGE);
            strip->setPixelColor(4, ORANGE);
            strip->setPixelColor(5, RED);
            strip->setPixelColor(6, RED);
            strip->setPixelColor(7, RED);
        }
        strip->show();


    }
    //////
    //////

    //////
    ////// SEEK ANIMATION
    else if ( animationType == ANIMATION_SEEK ) {

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


void BLTarget::startLedCountAnimation( int color, int count) {

    needRepaint = true;
    
    if ( count < 0 ) {
        count = 0;
    }
    else if ( count >= G_LED_COUNT) {
        count = G_LED_COUNT;
    }

    uint32_t colorCode;
    if ( color == RED_COLOR ) {
        colorCode = strip->Color(0, 255, 0);
    }
    else if ( color == BLUE_COLOR ) {
        colorCode = strip->Color(0, 0, 255);
    }
    else if ( color == YELLOW_COLOR ) {
        colorCode = strip->Color(255, 255, 0);
    }
    else if ( color == GREEN_COLOR ) {
        colorCode = strip->Color(255, 0, 0);
    }
    else if ( color == ORANGE_COLOR ) {
        colorCode = strip->Color(165, 255, 0);
    }
    else if ( color == PURPLE_COLOR ) {
        colorCode = strip->Color(0, 128, 128);
    }
    else {
        // unknown color
        colorCode = strip->Color(50, 50, 50);
    }


    animationFrame = 0;
    lastFrameTime = millis();
    animationStepTime = 100;
    animationType = ANIMATION_COUNT;
    ledCountAnimationColor = colorCode;
    ledCountAnimationCount = count;

}


void BLTarget::startHitAnimation() {

    needRepaint = true;

    animationFrame = 0;
    lastFrameTime = millis();
    animationStepTime = 200;
    animationType = ANIMATION_HIT;

    hitCount++;
    if ( hitCount >= G_LED_COUNT ) {
        hitCount = 0;
    }

}


void BLTarget::startSeekAnimation() {

    needRepaint = true;


    animationFrame = 0;
    lastFrameTime = millis();
    animationStepTime = 200;
    animationType = ANIMATION_SEEK;

}    
 
void BLTarget::startBlankAnimation() {

    needRepaint = true;


    animationFrame = 0;
    lastFrameTime = millis();
    animationStepTime = 500;
    animationType = ANIMATION_BLANK;
}


void BLTarget::syncStripToHitCount() {
    needRepaint = true;
    setStripToShowCountWithColor(hitCount, strip->Color(0,255,0));
}

void BLTarget::startColorHuntAnimation() {
    needRepaint = true;
    animationFrame = 0;
    lastFrameTime = millis();
    animationStepTime = 300;
    animationType = ANIMATION_HUNT;
}


void BLTarget::setStripToShowCountWithColor(int count, uint32_t color) {
      needRepaint = true;
        for (int i = 0; i < G_LED_COUNT; i++) {
            if ( count >= i+1 ) {
            strip->setPixelColor(i, color);
            }
            else {
            strip->setPixelColor(i, strip->Color(0, 0, 0));
            }
        }
        strip->show();
}
