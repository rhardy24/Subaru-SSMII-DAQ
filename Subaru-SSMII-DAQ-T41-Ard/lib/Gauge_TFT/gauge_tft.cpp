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

bool updateGauges(ECU_Data* interpretted_data, uint8_t screenNum){
    if(screenNum == 1){
        DrawBarChartV(tft, 10,  210, 30, 180, 0, 100 , 10, interpretted_data->TPS , 3 , 1, BLUE, DKBLUE, BLUE, WHITE, BLACK, "TPS", graph_1);
        DrawBarChartH(tft, 100, 180, 150, 30, 0, 20, 2, interpretted_data->MAP, 2, 2, GREEN, DKGREEN,  GREEN, WHITE, BLACK, "MAP", graph_2);
        DrawDial(tft, 230, 90, 80, 0, 7000 , 500, 240, interpretted_data->RPM,  4 , 0, RED, WHITE, BLACK, "RPM", graph_3);
    }
    return true;
}