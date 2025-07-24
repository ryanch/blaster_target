#include "app.h"
#include "bl_target.h"
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

void App::setup() {
    Serial.begin(9600);
    Serial.println("App Setup");
    // SWITCH / LED
    targetOne.setup(25,2); // Left and Blue  
    targetTwo.setup(33, 4); // Middel and Red
    targetThree.setup(32, 26); // Right and Orange

    randomSeed(analogRead(0));

    //selectAndSyncTargets();
    setupForGameMode(GAME_MODE_SETUP);
}


void App::setupForGameMode(int mode) {

    Serial.print("set game mode ");
    Serial.println( mode );

    if ( mode == GAME_MODE_SCORE || mode == GAME_MODE_SETUP ) {
        setupForGameModeHelper(mode);
        return;
    }

    actualGameMode = mode;
    gameMode = GAME_MODE_PRE_SCORE;
    roundStartTime = millis();

    targetOne.startPreGameAnimation();
    targetTwo.startPreGameAnimation();
    targetThree.startPreGameAnimation();

}

void App::setupForGameModeHelper(int mode) {

    Serial.print("real set game mode");
    Serial.println( mode );



    if ( mode == GAME_MODE_SHOOTING_GALLERY) {
        targetsHit = 0;
        roundStartTime = millis();
        targetOne.startBlankAnimation();
        targetTwo.startBlankAnimation();
        targetThree.startBlankAnimation();
        gameMode = GAME_MODE_SHOOTING_GALLERY;
        selectAndSyncTargets();
    }
    else if ( mode == GAME_MODE_ALL_TARGETS_ON) {
        targetsHit = 0;
        roundStartTime = millis();
        targetOne.startBlankAnimation();
        targetTwo.startBlankAnimation();
        targetThree.startBlankAnimation();
        gameMode = GAME_MODE_ALL_TARGETS_ON;
    }
    else if ( mode == GAME_MODE_SETUP ) {
        targetsHit = 0;
        roundStartTime = millis();
        proposedGameMode = 0;
        targetTwo.startBlankAnimation();
        targetThree.startBlankAnimation();
        gameMode = GAME_MODE_SETUP;
        syncGameModeSelection();
    }
    else if ( mode == GAME_MODE_COLOR_HUNT ) {
        targetsHit = 0;
        roundStartTime = millis();
        gameMode = GAME_MODE_COLOR_HUNT;
        activeHuntTarget = random(0, 3);
        activeHuntHighlight = random(0, 3);
        syncColorHuntAnimations();
    }
    else if ( mode == GAME_MODE_SCORE ) {
        proposedGameMode = gameMode;
        gameMode = GAME_MODE_SCORE;
        hasShownPreScore = false;
        hasShownScore = false;
        targetOne.startBlankAnimation();
        targetTwo.startBlankAnimation();
        targetThree.startBlankAnimation();
    }
}

void App::syncColorHuntAnimations() {

    targetOne.startBlankAnimation();
    targetTwo.startBlankAnimation();
    targetThree.startBlankAnimation();

    // adjust the target
    int targetAdvance = random(1, 3);

    while ( targetAdvance > 0 ) {

        activeHuntTarget++;

        if ( activeHuntTarget > 2 ) {
            activeHuntTarget = 0;
        }

        targetAdvance--;
    }

    // adjust the highlight
    activeHuntHighlight = random(0, 3);

    Serial.print( "setup hunt for target" );
    Serial.print( activeHuntTarget );
    Serial.print( " with color on target" );
    Serial.println( activeHuntHighlight );

    BLTarget *highlightTarget;
    if ( activeHuntHighlight == 0 ) {
        highlightTarget = &targetOne;
    }
    else if ( activeHuntHighlight == 1 ) {
        highlightTarget = &targetTwo;
    }
    else {
        highlightTarget = &targetThree;
    }

    // set the color based on the active target
    if ( activeHuntTarget  == 0 ) {
        highlightTarget->startLedCountAnimation( BLUE_COLOR, 8 );
    }
    else if ( activeHuntTarget  == 1 ) {
        highlightTarget->startLedCountAnimation( RED_COLOR, 8 );
    }
    else {
        highlightTarget->startLedCountAnimation( ORANGE_COLOR, 8 );
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

        targetTwo.startLedCountAnimation(RED_COLOR, roundLengthMins);
}

void App::loop() {

    delay(10); 

    // update all targets
    targetOne.loop();
    targetTwo.loop();
    targetThree.loop();

    // check for game mode change
    if ( targetOne.checkForLongPressSinceLastLoop() || targetTwo.checkForLongPressSinceLastLoop() || targetThree.checkForLongPressSinceLastLoop() ) {
        setupForGameMode(GAME_MODE_SETUP);
    }


    /// check if we are in setup mode
    if ( gameMode == GAME_MODE_SETUP ) {
        if ( targetThree.checkForPressSinceLastLoop() ) {
            // exit setup mode
            setupForGameMode(proposedGameMode);
        }
        else if ( targetOne.checkForPressSinceLastLoop() ) {
            // goto next game mode
            proposedGameMode++;
            if ( proposedGameMode > MAX_MODES ) {
                proposedGameMode = 0;
            }
            syncGameModeSelection();
        }
        else if ( targetTwo.checkForPressSinceLastLoop() ) {
            roundLengthMins++;
            if ( roundLengthMins > G_LED_COUNT ) {
                roundLengthMins = 1;
            }
            syncGameModeSelection();
        }
        return;
    }


    // see if we are in a game mode and the game has ended
    if ( gameMode != GAME_MODE_SETUP && gameMode != GAME_MODE_SCORE && gameMode != GAME_MODE_PRE_SCORE ) {
        if ( millis() - roundStartTime > (roundLengthMins*ONE_MINUTE) ) {
            setupForGameMode( GAME_MODE_SCORE );
        }
    }



    if (gameMode == GAME_MODE_PRE_SCORE ) {

        // see if we can exit pre-game mode
        if ( millis() - roundStartTime > PRE_GAME_ANIMATION_TIME ) {
            setupForGameModeHelper( actualGameMode );
        }


    }
    else if ( gameMode == GAME_MODE_SCORE ) {
        // if we have not shown pre score yet, show that
        if ( !hasShownPreScore ) {
            targetOne.startPreScoreAnimation();
            targetTwo.startPreScoreAnimation();
            targetThree.startPreScoreAnimation();
            hasShownPreScore = true;
        }
        // if we have been showing pre score long enough, now show the score
        else if ( !hasShownScore && millis() - roundStartTime > (roundLengthMins*ONE_MINUTE) + END_GAME_PRE_SCORE_SHOW_TIME ) {
            
            hasShownScore = true;
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

            // check if they are in the first tier
            if ( targetsHit <= 24 ) {
                targetOne.startLedCountAnimation( colorCode, adjustedScore );
                targetTwo.startLedCountAnimation( colorCode, adjustedScore-8 );
                targetThree.startLedCountAnimation( colorCode, adjustedScore-16 );
            }
            // check if they made it into next tier, but just barely
            // if so show, the last tier colors as a reward
            else if ( adjustedScore <= 8 ) {
                targetOne.startLedCountAnimation( colorCode-1, 8 );
                targetTwo.startLedCountAnimation( colorCode-1, 8 );
                targetThree.startLedCountAnimation( colorCode, adjustedScore );
            }
            // check if they made it to the middle of the next tier
            else if ( adjustedScore <= 16 ) {
                targetOne.startLedCountAnimation( colorCode-1, 8 );
                targetTwo.startLedCountAnimation( colorCode, 8 );
                targetThree.startLedCountAnimation( colorCode, adjustedScore-8 );
            }
            else {
                targetOne.startLedCountAnimation( colorCode, 8 );
                targetTwo.startLedCountAnimation( colorCode, 8 );
                targetThree.startLedCountAnimation( colorCode, adjustedScore-16 );
            }


        }
        // if we have shown the end game score long enough, then exit back to previous mode
        else if ( millis() - roundStartTime > (roundLengthMins*ONE_MINUTE) + END_GAME_PRE_SCORE_SHOW_TIME + END_GAME_SCORE_SHOW_TIME ) {
            setupForGameMode(proposedGameMode);
        }
    }

    // color hunt mode
    else if ( gameMode == GAME_MODE_COLOR_HUNT ) {

        // check if we have targets hit
        if ( activeHuntTarget == 0 && targetOne.checkForPressSinceLastLoop() ) {
            targetsHit++;
            syncColorHuntAnimations();
        }
        else if ( activeHuntTarget == 1 && targetTwo.checkForPressSinceLastLoop() ) {
            targetsHit++;
            syncColorHuntAnimations();
        }
        else if ( activeHuntTarget == 2 && targetThree.checkForPressSinceLastLoop() ) {
            targetsHit++;
            syncColorHuntAnimations();
        }

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
