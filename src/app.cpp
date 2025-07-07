#include "app.h"
#include "bl_target.h"
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

void App::setup() {

    Serial.println("App Setup");
    targetOne.setup(25,2);
    targetTwo.setup(33, 4);
    targetThree.setup(32, 16);

    randomSeed(analogRead(0));

    selectAndSyncTargets();
}

void App::loop() {

    targetOne.loop();
    targetTwo.loop();
    targetThree.loop();

    // check for game mode change
    if ( targetOne.checkForLongPressSinceLastLoop() || targetTwo.checkForLongPressSinceLastLoop() || targetThree.checkForLongPressSinceLastLoop() ) {
        if (gameMode == GAME_MODE_SHOOTING_GALLERY ) {
            gameMode = GAME_MODE_ALL_TARGETS_ON;
        }
        else {
            gameMode = GAME_MODE_SHOOTING_GALLERY;
            selectAndSyncTargets();
        }
    }

    if ( gameMode == GAME_MODE_SHOOTING_GALLERY ) {

        if ( activeTarget == 0 && targetOne.checkForPressSinceLastLoop() ) {
            targetOne.startHitAnimation();
            selectAndSyncTargets();
        }
        else if ( activeTarget == 1 && targetTwo.checkForPressSinceLastLoop() ) {
            targetTwo.startHitAnimation();
            selectAndSyncTargets();
        }
        else if ( activeTarget == 2 && targetThree.checkForPressSinceLastLoop() ) {
            targetThree.startHitAnimation();
            selectAndSyncTargets();
        }

    }
    else if (gameMode == GAME_MODE_ALL_TARGETS_ON )  {
        if ( targetOne.checkForPressSinceLastLoop() ) {
            targetOne.startHitAnimation();
        }
        else if (  targetTwo.checkForPressSinceLastLoop() ) {
            targetTwo.startHitAnimation();
        }
        else if (  targetThree.checkForPressSinceLastLoop() ) {
            targetThree.startHitAnimation();
        }
    }

    

}

void App::selectAndSyncTargets() {
    int targetAdvance = random(1, 3);

    while ( targetAdvance > 0 ) {

        activeTarget++;

        if ( activeTarget > 2 ) {
            activeTarget = 0;
        }

        targetAdvance--;
    }

    if ( activeTarget == 0 ) {
        targetOne.startSeekAnimation();
    }
    else if ( activeTarget == 1  ) {
        targetTwo.startSeekAnimation();
    }
    else if ( activeTarget == 2  ) {
        targetThree.startSeekAnimation();
    }

}
