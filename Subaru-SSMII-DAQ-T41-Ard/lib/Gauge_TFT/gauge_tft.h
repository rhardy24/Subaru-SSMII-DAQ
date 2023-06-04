#include <Arduino.h>
#include <config.h>

#ifndef GAUGE_TFT__H_
#define GAUGE_TFT__H_

    /*************************************************************************
     * EXTERNAL INCLUDES
    **************************************************************************/
    #include <SPI.h>
    #include <Adafruit_ILI9341.h>
    #include <subaru_ssm.h>
    #include <Adafruit_FT6206.h>
    #include <Wire.h>

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

        void gauge_status_set(bool status){
            ect = rpm = map = speedkm = timing = iat = tps = batvolt = eft = true;
    }
    } gauge_status_t;

    /*************************************************************************
     * PIN ASSIGNMENTS
    **************************************************************************/
    #define TFT_DC 9
    #define TFT_CS 10
    #define TFT_MOSI 11
    #define TFT_CLK 13
    #define TFT_RST 14
    #define TFT_MISO 12
    #define ADJ_PIN A0


    /*************************************************************************
     * INSTANTIATION DECLARATIONS
    **************************************************************************/
    extern Adafruit_ILI9341 tft;
    extern Adafruit_FT6206 ctp;

    extern gauge_status_t gauge_status;

    extern uint8_t prevScreen;
    extern uint8_t curScreen;

    extern TS_Point touchPoint;

    /*************************************************************************
     * FUNCTION PROTOTYPES
    **************************************************************************/
    //initializes the tft gauge display
    bool gaugesBegin();

    // checks touchscreen for new touches, returns the new screen number
    // if touch was detected
    uint8_t checkTouch();

    // takes values from the ECU_Data struct to update the tft gauges
    bool updateGauges(ECU_Data* interpretted_data, uint8_t screenNum);

    /*
    This method will draw a vertical bar graph for single input
  
    &d = display object name
    x = position of bar graph (lower left of bar)
    y = position of bar (lower left of bar
    w = width of bar graph
    h =  height of bar graph (does not need to be the same as the max scale)
    loval = lower value of the scale (can be negative)
    hival = upper value of the scale
    inc = scale division between loval and hival
    curval = date to graph (must be between loval and hival)
    dig = format control to set number of digits to display (not includeing the decimal)
    dec = format control to set number of decimals to display (not includeing the decimal)
    barcolor = color of bar graph
    voidcolor = color of bar graph background
    bordercolor = color of the border of the graph
    textcolor = color of the text
    backcolor = color of the bar graph's background
    label = bottom lable text for the graph
    redraw = flag to redraw display only on first pass (to reduce flickering)
  
    */
    void DrawBarChartV(Adafruit_ILI9341 & d, double x , double y , double w, double h , double loval , double hival , double inc , double curval ,  int dig , int dec, unsigned int barcolor, unsigned int voidcolor, unsigned int bordercolor, unsigned int textcolor, unsigned int backcolor, String label, boolean & redraw);
  
    /*
    This method will draw a dial-type graph for single input
  
    &d = display object name
    cx = center position of dial
    cy = center position of dial
    r = radius of the dial
    loval = lower value of the scale (can be negative)
    hival = upper value of the scale
    inc = scale division between loval and hival
    sa = sweep angle for the dials scale
    curval = date to graph (must be between loval and hival)
    dig = format control to set number of digits to display (not includeing the decimal)
    dec = format control to set number of decimals to display (not includeing the decimal)
    needlecolor = color of the needle
    dialcolor = color of the dial
    textcolor = color of all text (background is dialcolor)
    label = bottom lable text for the graph
    redraw = flag to redraw display only on first pass (to reduce flickering)
    */
    void DrawDial(Adafruit_ILI9341 & d, int cx, int cy, int r, double loval , double hival , double inc, double sa, double curval,  int dig , int dec, unsigned int needlecolor, unsigned int dialcolor, unsigned int  textcolor, String label, boolean & redraw) ;

    /*
    This method will draw a horizontal bar graph for single input
  
    &d = display object name
    x = position of bar graph (upper left of bar)
    y = position of bar (upper left of bar (add some vale to leave room for label)
    w = width of bar graph (does not need to be the same as the max scale)
    h =  height of bar graph
    loval = lower value of the scale (can be negative)
    hival = upper value of the scale
    inc = scale division between loval and hival
    curval = date to graph (must be between loval and hival)
    dig = format control to set number of digits to display (not includeing the decimal)
    dec = format control to set number of decimals to display (not includeing the decimal)
    barcolor = color of bar graph
    voidcolor = color of bar graph background
    bordercolor = color of the border of the graph
    textcolor = color of the text
    back color = color of the bar graph's background
    label = bottom lable text for the graph
    redraw = flag to redraw display only on first pass (to reduce flickering)
    */
    void DrawBarChartH(Adafruit_ILI9341 & d, double x , double y , double w, double h , double loval , double hival , double inc , double curval ,  int dig , int dec, unsigned int barcolor, unsigned int voidcolor, unsigned int bordercolor, unsigned int textcolor, unsigned int backcolor, String label, boolean & redraw);
    
    // function to format a numerical value into a string with the specified number of digits and decimal points
    String Format(double val, int dec, int dig ) ;
    
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