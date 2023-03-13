#include <Arduino.h>
#include <config.h>
#include <declarations.h>

#include <gauge_tft.h>

/*************************************************************************
 * FUNCTIONS
**************************************************************************/

// initializes the tft gauge display
bool gaugesBegin(){
    tft.begin();
    tft.fillScreen(BLACK);
    tft.setRotation(1);
    return true;
}