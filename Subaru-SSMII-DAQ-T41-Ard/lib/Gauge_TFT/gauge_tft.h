#include <Arduino.h>
#include <config.h>
#include <declarations.h>

#include <subaru_ssm.h>

#ifndef GAUGE_TFT_H
#define GAUGE_TFT_H

    /*************************************************************************
     * EXTERNAL INCLUDES
    **************************************************************************/
    #include <SPI.h>
    #include <Adafruit_ILI9341.h>

    /*************************************************************************
     * DATA STRUCTURES
    **************************************************************************/
    //struct holding the statuses of all the gauges being displayed
    typedef struct{
        bool ect = true;
        bool rpm = true;
        bool map = true;
        bool speedkm = true;
        bool timing = true;
        bool iat = true;
        bool tps = true;
        bool batvolt = true;
        bool eft = true;
    } gauge_status;


    /*************************************************************************
     * PIN ASSIGNMENTS
    **************************************************************************/
    #define TFT_DC 9
    #define TFT_CS 10
    #define TFT_MOSI 11
    #define TFT_CLK 13
    #define TFT_RST 14
    #define TFT_MISO 12


    /*************************************************************************
     * INSTANTIATIONS
    **************************************************************************/
    Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

    /*************************************************************************
     * FUNCTION PROTOTYPES
    **************************************************************************/
    //initializes the tft gauge display
    bool gaugesBegin();

    bool updateGauges(ECU_Data* interpretted_data, uint8_t screenNum);

    /*************************************************************************
     * OTHER DEFINITIONS
    **************************************************************************/
    #define LTBLUE    0xB6DF
    #define LTTEAL    0xBF5F
    #define LTGREEN         0xBFF7
    #define LTCYAN    0xC7FF
    #define LTRED           0xFD34
    #define LTMAGENTA       0xFD5F
    #define LTYELLOW        0xFFF8
    #define LTORANGE        0xFE73
    #define LTPINK          0xFDDF
    #define LTPURPLE  0xCCFF
    #define LTGREY          0xE71C
    
    #define BLUE            0x001F
    #define TEAL    0x0438
    #define GREEN           0x07E0
    #define CYAN          0x07FF
    #define RED           0xF800
    #define MAGENTA       0xF81F
    #define YELLOW        0xFFE0
    #define ORANGE        0xFD20
    #define PINK          0xF81F
    #define PURPLE    0x801F
    #define GREY        0xC618
    #define WHITE         0xFFFF
    #define BLACK         0x0000
    
    #define DKBLUE        0x000D
    #define DKTEAL    0x020C
    #define DKGREEN       0x03E0
    #define DKCYAN        0x03EF
    #define DKRED         0x6000
    #define DKMAGENTA       0x8008
    #define DKYELLOW        0x8400
    #define DKORANGE        0x8200
    #define DKPINK          0x9009
    #define DKPURPLE      0x4010
    #define DKGREY        0x4A49
  
#endif