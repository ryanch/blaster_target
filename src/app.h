
#include <Adafruit_NeoPixel.h>
#include "bl_target.h"
#include "const.h"

#ifndef APP_H
#define APP_H

//#include <string> // Example: if std::string is used in the class

class App {
public:
    void setup();
    void loop();

private:
    BLTarget targetOne;
    BLTarget targetTwo;
    BLTarget targetThree;

    int roundLengthMins = 1;

    int proposedGameMode = 0;

    int activeTarget;

    int targetsHit = 0;
    unsigned long roundStartTime = 0;

    int gameMode = GAME_MODE_SHOOTING_GALLERY;
    int endOfRoundScore = 0;

    // hunt target codes
    int activeHuntTarget = 0;
    int activeHuntHighlight = 0;

    // scoring display
    bool hasShownPreScore = false;
    bool hasShownScore = false;

    void selectAndSyncTargets();

    void setupForGameMode(int mode);



    void syncGameModeSelection();

    void syncColorHuntAnimations();

};

#endif // APP_H