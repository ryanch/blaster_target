
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

    int activeTarget;

    int targetsHit = 0;
    unsigned long roundStartTime = 0;

    int gameMode = GAME_MODE_SHOOTING_GALLERY;

    bool showEndOfRoundAnimation = false;
    int endOfRoundScore = 0;

    void selectAndSyncTargets();

    void setupForGameMode(int mode);

    int proposedGameMode = 0;

};

#endif // APP_H