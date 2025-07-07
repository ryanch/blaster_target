#include <Adafruit_NeoPixel.h>


#define MAX_COUNT 8

#define LED_PIN  2
#define LED_COUNT  MAX_COUNT
#define BRIGHTNESS 250 

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);

#define BUTTON_PIN 25


// the mode that the target is in
#define SETUP_MODE 0
#define SINGLE_TARGET_MODE 1
#define GROUP_TARGET_MODE 2
int targetMode = SETUP_MODE;

// var for tracking proposed mode while in setup
int proposedModeForSetup = SETUP_MODE;
#define SETUP_TIME_MS 4000
unsigned long setupStartTime = 0;
int setupAnimationFrameCount=0;
unsigned long lastAnimationTime = 0;

// var for tracking how many times in a row the target has been hit
int ledCount = 0; 
unsigned long hitFlashAnimationStartTime = 0;
int flashAnimationFrame=0;


// vars for group target mode
int groupTargetAnimationFrame = 0;
unsigned long groupAnimationFrameTime = 0;
bool activeGroupTarget = true;
unsigned long groupHitFlashAnimationStartTime = 0;
int groupHitAnimationFrame=100;
unsigned long groupModeWakeUpTime = 0;

// vars for tracking button - debounce ect. 
int lastSteadyState = LOW;
int lastFlickerableState = LOW;
int currentState;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
unsigned long lastHoldDownTime = 0;

void setup() {

  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);


  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS);

  // setup the random number generator
  randomSeed(analogRead(0));

  // call this once to set the light to show current mode
  prepForSetup();


  Serial.println("Target ready.");
}



// the main loop
void loop() {



  // see if we should be flashing the lights for setup
  if (targetMode == SETUP_MODE) {
    handleFlashingLightsForSetup();
    // check if we should exit setup
    handleCheckExitSetup();
  }

  // see if we should be flashing the lights for target
  else if (targetMode == SINGLE_TARGET_MODE) {
    animateForSingleTarget();
  }

  // see if we should be doing aninmation for group mode
  else if (targetMode == GROUP_TARGET_MODE) {
    animateForGroupTarget();
  }


  // read the button
  currentState = digitalRead(BUTTON_PIN);

  // de-bounce
  if (currentState != lastFlickerableState) {
    lastDebounceTime = millis();
    lastFlickerableState = currentState;
  }

  // determine if we actually got a press
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (lastSteadyState != currentState) {
      lastSteadyState = currentState;
      if (currentState == LOW) {
        onButton();
        lastHoldDownTime = millis();
      }
      else if ( currentState == HIGH) {
        if (millis() - lastHoldDownTime > 5000) {
          Serial.println("Long press detected");
          onLongButtonPress();
        }
      }
    }
  }


  delay(10); // this speeds up the simulation
}


// this is called when button is held down for long time and released
void onLongButtonPress() {
  prepForSetup();
}

// this is called when button is detected as being hit
void onButton() {
  if (targetMode == SINGLE_TARGET_MODE) {
    handleButtonWhileInSingleTargetMode();
  }

  else if (targetMode == GROUP_TARGET_MODE) {
    handleButtonWhileInGroupTargetMode();
  }

  else if (targetMode == SETUP_MODE) {
    handleButtonWhileInSetupTargetMode();
  }
}





/**
--------------------------------
Setup Mode
--------------------------------
**/

void prepForSetup() {
  setupStartTime = millis();
  targetMode = SETUP_MODE;
  proposedModeForSetup = SETUP_MODE;
  handleButtonWhileInSetupTargetMode();
}

// this is called when button is pressed while in setup mode
void handleButtonWhileInSetupTargetMode() {

  Serial.println("setup click");

  if ( proposedModeForSetup == SETUP_MODE) {
    proposedModeForSetup = SINGLE_TARGET_MODE;
  }
  else if ( proposedModeForSetup == SINGLE_TARGET_MODE) {
    proposedModeForSetup = GROUP_TARGET_MODE;
  }
  else if ( proposedModeForSetup == GROUP_TARGET_MODE) {
    proposedModeForSetup = SINGLE_TARGET_MODE;
  }


}

// change the light state based on mode and time
void handleFlashingLightsForSetup() {

  if (lastAnimationTime == 0) {
    setupAnimationFrameCount = 0;
    lastAnimationTime = millis();
  }
  else {
    if (millis() - lastAnimationTime >= 100) {
      setupAnimationFrameCount++;
      if (setupAnimationFrameCount >= LED_COUNT) {
        setupAnimationFrameCount = 0;
      }
      lastAnimationTime = millis();
    }
  }


  uint32_t blackColor = strip.Color(0, 0, 0);

  uint32_t colorToUse;
  if ( proposedModeForSetup == SINGLE_TARGET_MODE) { 
    colorToUse = strip.Color(0, 0, 255);
  }
  else if ( proposedModeForSetup == GROUP_TARGET_MODE) {
    colorToUse = strip.Color(165, 255, 0);
  }
  else {
    colorToUse = blackColor;
  }

  for (int i = 0; i < LED_COUNT; i++) {
    if (setupAnimationFrameCount == i) {
      strip.setPixelColor(i, colorToUse);
    }
    else {
      strip.setPixelColor(i, blackColor);
    }
  }
  strip.show();

}

// void this is called to determine if we can exit setup mode
void handleCheckExitSetup() {
  if ( targetMode == SETUP_MODE) {
    if (millis() - setupStartTime >= SETUP_TIME_MS) {
      
      // it is time to exit
      // clear the led
      for (int i = 0; i < LED_COUNT; i++) {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
      }
      strip.show();

      // save the new mode
      targetMode = proposedModeForSetup;

      if ( targetMode == GROUP_TARGET_MODE) {
        setupForGroupMode();
      }

      Serial.println("exiting setup");

    }
  }
}


/**
--------------------------------
Single Target Mode
--------------------------------
**/

// this is called when button is pressed while in single target mode
void handleButtonWhileInSingleTargetMode() {
  ledCount++;
  if (ledCount > MAX_COUNT) ledCount = 0; 
  Serial.println("The button is pressed");
  Serial.println(ledCount);

  // setup to flash
  hitFlashAnimationStartTime = millis();
  flashAnimationFrame = 0;

}

// update the led while in single target mode
void animateForSingleTarget() {

  if (millis() - hitFlashAnimationStartTime >= 500) {
    flashAnimationFrame++;
    hitFlashAnimationStartTime = millis();
  }

  // for the first and 3 frames of the animation
  // make it all yellow. Then for all other frames show hit count
  if (flashAnimationFrame == 0 || flashAnimationFrame == 2) {
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, strip.Color(255, 255, 0));
    }
    strip.show();
  }
  else {
    syncLedWithHitCount();
  }


}

// set the lights to show how many hits
void syncLedWithHitCount() {

  for (int i = 0; i < LED_COUNT; i++) {
    if ( ledCount >= i+1 ) {
      strip.setPixelColor(i, strip.Color(0, 255, 0));
    }
    else {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
  }

  strip.show();
}



/**
--------------------------------
Group Target Mode
--------------------------------
**/

// this is called when button is pressed while in group mode
void handleButtonWhileInGroupTargetMode() {
  Serial.println("group click");

  // start the hit animation frame if we had a local hit
  if (activeGroupTarget) {
    groupHitAnimationFrame = 0;
    groupHitFlashAnimationStartTime = millis();  
    setupForGroupMode();
  }


}

// setup for group mode
void setupForGroupMode() {
  activeGroupTarget=false;
  // pick a random wakeup time
  groupModeWakeUpTime = millis() + random(5000, 15000);
}

// there 3 animations
// OFF - when not a target
// ON - when an active target
// flashing - when recently hit while being a target
void animateForGroupTarget() {

  // check if we should wake up
  if ( !activeGroupTarget && millis() > groupModeWakeUpTime ) {
    activeGroupTarget = true;
  }  

  // check if we are on the hit animation
  if (groupHitAnimationFrame  < 5 ) {
    if (millis() - groupHitFlashAnimationStartTime >= 500) {
      groupHitAnimationFrame++;
      groupHitFlashAnimationStartTime = millis();
    }

    // for the first and 3 frames of the animation
    // make it all yellow. Then for all other frames show hit count
    if (groupHitAnimationFrame == 0 || groupHitAnimationFrame == 2) {
      for (int i = 0; i < LED_COUNT; i++) {
        strip.setPixelColor(i, strip.Color(255, 255, 0));
      }
      strip.show();
    }
    else {
      for (int i = 0; i < LED_COUNT; i++) {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
      }
      strip.show();
    }
    return;
  }



  // check if we should just be off because we are not a target
  // check if my target should be on
  // should only be on if the network is targeting me
  if (!activeGroupTarget) {
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }

    strip.show();
    return;
  }

  // if we make it here, we are suppose to be doing the
  // active target animation
  if ( groupAnimationFrameTime == 0 ) {
      groupAnimationFrameTime = millis();
  }

  if (millis()-groupAnimationFrameTime >= 250) {
    groupTargetAnimationFrame++;
    if (groupTargetAnimationFrame > 4) {
      groupTargetAnimationFrame = 0;
    }
    groupAnimationFrameTime = millis();
  }

  uint32_t RED = strip.Color(0, 255, 0);
  uint32_t WHITE = strip.Color(255, 255, 255);


  if (groupTargetAnimationFrame == 0) {
    strip.setPixelColor(0, RED);
    strip.setPixelColor(1, WHITE);
    strip.setPixelColor(2, WHITE);
    strip.setPixelColor(3, WHITE);
    strip.setPixelColor(4, WHITE);
    strip.setPixelColor(5, WHITE);
    strip.setPixelColor(6, WHITE);
    strip.setPixelColor(7, RED);
  }
  else if (groupTargetAnimationFrame == 1) {
    strip.setPixelColor(0, WHITE);
    strip.setPixelColor(1, RED);
    strip.setPixelColor(2, WHITE);
    strip.setPixelColor(3, WHITE);
    strip.setPixelColor(4, WHITE);
    strip.setPixelColor(5, WHITE);
    strip.setPixelColor(6, RED);
    strip.setPixelColor(7, WHITE);
  }
  else if (groupTargetAnimationFrame == 2) {
    strip.setPixelColor(0, WHITE);
    strip.setPixelColor(1, WHITE);
    strip.setPixelColor(2, RED);
    strip.setPixelColor(3, WHITE);
    strip.setPixelColor(4, WHITE);
    strip.setPixelColor(5, RED);
    strip.setPixelColor(6, WHITE);
    strip.setPixelColor(7, WHITE);
  }
  else if (groupTargetAnimationFrame == 3) {
    strip.setPixelColor(0, WHITE);
    strip.setPixelColor(1, WHITE);
    strip.setPixelColor(2, WHITE);
    strip.setPixelColor(3, RED);
    strip.setPixelColor(4, RED);
    strip.setPixelColor(5, WHITE);
    strip.setPixelColor(6, WHITE);
    strip.setPixelColor(7, WHITE);
  }
  else if (groupTargetAnimationFrame == 4) {
    strip.setPixelColor(0, WHITE);
    strip.setPixelColor(1, WHITE);
    strip.setPixelColor(2, WHITE);
    strip.setPixelColor(3, WHITE);
    strip.setPixelColor(4, WHITE);
    strip.setPixelColor(5, WHITE);
    strip.setPixelColor(6, WHITE);
    strip.setPixelColor(7, WHITE);
  }


  strip.show();

}