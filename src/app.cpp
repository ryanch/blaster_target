#include "app.h"
#include "bl_target.h"
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

void App::setup() {

    Serial.println("App Setup");
    targetOne.setup(25,2);
    targetTwo.setup(33, 4);
    targetThree.setup(32, 26);

    randomSeed(analogRead(0));

    //selectAndSyncTargets();
    setupForGameMode(GAME_MODE_SHOOTING_GALLERY);
}


void App::setupForGameMode(int mode) {

    Serial.println("set game mode");

    targetsHit = 0;
    roundStartTime = millis();
    showEndOfRoundAnimation = false;

    if ( mode == GAME_MODE_SHOOTING_GALLERY) {
        targetOne.startBlankAnimation();
        targetTwo.startBlankAnimation();
        targetThree.startBlankAnimation();
        gameMode = GAME_MODE_SHOOTING_GALLERY;
        selectAndSyncTargets();
    }
    else if ( mode == GAME_MODE_ALL_TARGETS_ON) {
        targetOne.startBlankAnimation();
        targetTwo.startBlankAnimation();
        targetThree.startBlankAnimation();
        gameMode = GAME_MODE_ALL_TARGETS_ON;
    }
    else if ( mode == GAME_MODE_SETUP ) {
        proposedGameMode = 0;
        targetTwo.startBlankAnimation();
        targetThree.startBlankAnimation();
        gameMode = GAME_MODE_SETUP;
        syncGameModeSelection();
    }

}


void App::syncGameModeSelection() {
        // set the game mode display
        if ( proposedGameMode == GAME_MODE_ALL_TARGETS_ON ) {
            targetOne.startLedCountAnimation( YELLOW_COLOR, 8 );
        }
        else if ( proposedGameMode == GAME_MODE_SHOOTING_GALLERY ) {
            targetOne.startSeekAnimation();
        }
        else if ( proposedGameMode == GAME_MODE_COLOR_HUNT ) {
            targetOne.startColorHuntAnimation();
        }
}

void App::loop() {


    // update all targets
    targetOne.loop();
    targetTwo.loop();
    targetThree.loop();

    // check for game mode change
    if ( targetOne.checkForLongPressSinceLastLoop() || targetTwo.checkForLongPressSinceLastLoop() || targetThree.checkForLongPressSinceLastLoop() ) {
        setupForGameMode(GAME_MODE_SETUP);
    }


    if ( gameMode == GAME_MODE_SETUP ) {
        if ( targetThree.checkForPressSinceLastLoop() ) {
            // exit setup mode
            setupForGameMode(proposedGameMode);
        }
        else if ( targetOne.checkForPressSinceLastLoop() ) {
            // goto next game mode
            proposedGameMode++;
            if ( proposedGameMode >= MAX_MODES ) {
                proposedGameMode = 0;
            }
            syncGameModeSelection();
        }


    }


    // see if we are at end of game
    if (gameMode != GAME_MODE_SETUP && !showEndOfRoundAnimation && millis() - roundStartTime > SHOOTING_GALLERY_ROUND_TIME) {

        Serial.println("End of Round");

        showEndOfRoundAnimation = true;
        endOfRoundScore = targetsHit;


        int colorCode = 0;
        if ( targetsHit <= 24 ) {
            colorCode = 0;
        }
        else if ( targetsHit <= 24*2 ) {
            colorCode = 1;
        }
        else if ( targetsHit <= 24*3 ) {
            colorCode = 2;
        }
        else if ( targetsHit <= 24*4 ) {
            colorCode = 3;
        }
        else if ( targetsHit <= 24*5 ) {
            colorCode = 4;
        }
        else if ( targetsHit <= 24*6 ) {
            colorCode = 5;
        }

        // now turn the score into a score that is based on a number from 0 to 24
        int adjustedScore = endOfRoundScore - (24*colorCode);

        targetOne.startLedCountAnimation( colorCode, adjustedScore );
        targetTwo.startLedCountAnimation( colorCode, adjustedScore-8 );
        targetThree.startLedCountAnimation( colorCode, adjustedScore-16 );


    }
    else if (gameMode != GAME_MODE_SETUP && millis() - roundStartTime > SHOOTING_GALLERY_ROUND_TIME+END_GAME_SCORE_SHOW_TIME) {
        Serial.println("End of showing scoring");
        showEndOfRoundAnimation = false;
        // reset the game
        setupForGameMode(gameMode);
    }




    /// shooting gallery mode
    else if ( gameMode == GAME_MODE_SHOOTING_GALLERY ) {


       
        
        // check if we have targets hit
        if ( activeTarget == 0 && targetOne.checkForPressSinceLastLoop() ) {
            targetsHit++;
            targetOne.startHitAnimation();
            selectAndSyncTargets();
        }
        else if ( activeTarget == 1 && targetTwo.checkForPressSinceLastLoop() ) {
            targetsHit++;
            targetTwo.startHitAnimation();
            selectAndSyncTargets();
        }
        else if ( activeTarget == 2 && targetThree.checkForPressSinceLastLoop() ) {
            targetsHit++;
            targetThree.startHitAnimation();
            selectAndSyncTargets();
        }
        

    }

    /// all targets on mode
    else if (gameMode == GAME_MODE_ALL_TARGETS_ON )  {
        if ( targetOne.checkForPressSinceLastLoop() ) {
            targetsHit++;
            targetOne.startHitAnimation();
        }
        else if (  targetTwo.checkForPressSinceLastLoop() ) {
            targetsHit++;
            targetTwo.startHitAnimation();
        }
        else if (  targetThree.checkForPressSinceLastLoop() ) {
            targetsHit++;
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
