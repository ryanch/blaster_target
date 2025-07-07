
#include <Adafruit_NeoPixel.h>
#include "bl_target.h"

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

    void selectAndSyncTargets();

};

#endif // APP_H