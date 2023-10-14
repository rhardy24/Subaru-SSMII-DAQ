#include <Arduino.h>
#include <config.h>

#include <gauge_tft.h>

/*************************************************************************
 * INSTANTIATION DEFINITIONS   
**************************************************************************/
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
gauge_status_t gauge_status;

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

// takes values from the ECU_Data struct to update the tft gauges
bool updateGauges(ECU_Data* interpretted_data, uint8_t screenNum){
    if(screenNum == 1){
      DrawBarChartV(tft, 10,  210, 30, 180, 0, 100 , 10, interpretted_data->TPS , 3 , 1, BLUE, DKBLUE, BLUE, WHITE, BLACK, "TPS", gauge_status.tps);
      DrawBarChartH(tft, 100, 180, 150, 30, 0, 20, 2, interpretted_data->MAP, 2, 2, GREEN, DKGREEN,  GREEN, WHITE, BLACK, "MAP", gauge_status.map);
      DrawDial(tft, 230, 90, 80, 0, 7000 , 500, 240, interpretted_data->RPM,  4 , 0, RED, WHITE, BLACK, "RPM", gauge_status.rpm);
    }
    //seems theres a lag issue with having too many gauges at once. Having 4 causes it to crash, stable with 3
    if(screenNum == 2){
      DrawBarChartV(tft, 10,  210, 30, 180, -20, 120, 20, interpretted_data->ECT , 3 , 0, BLUE, DKBLUE, BLUE, WHITE, BLACK, "ECT", gauge_status.ect);
      DrawBarChartV(tft, 90,  210, 30, 180, -20, 70, 15, interpretted_data->IAT , 2 , 0, WHITE, GREY, WHITE, WHITE, BLACK, "IAT", gauge_status.iat);
      DrawBarChartV(tft, 165,  210, 30, 180, 10, 16, 1, interpretted_data->BATVOLT, 2 , 1, YELLOW, DKYELLOW, YELLOW, WHITE, BLACK, "VOLT", gauge_status.batvolt);
      DrawBarChartV(tft, 250,  210, 30, 180, -20, 55 , 15, interpretted_data->EFT, 2 , 0, ORANGE, DKORANGE, ORANGE, WHITE, BLACK, "EFT", gauge_status.eft);    
    }
    if(screenNum == 3){
      DrawBarChartV(tft, 10,  210, 50, 180, -20, 120, 20, interpretted_data->ECT , 3 , 1, BLUE, DKBLUE, BLUE, WHITE, BLACK, "ECT", gauge_status.ect);
      DrawBarChartV(tft, 120,  210, 50, 180, -20, 70, 15, interpretted_data->IAT , 2 , 1, WHITE, GREY, WHITE, WHITE, BLACK, "IAT", gauge_status.iat);
      DrawBarChartV(tft, 230,  210, 50, 180, 10, 16, 1, interpretted_data->BATVOLT, 2 , 1, YELLOW, DKYELLOW, YELLOW, WHITE, BLACK, "VOLT", gauge_status.batvolt);    }
    
    return true;
}

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
void DrawBarChartV(Adafruit_ILI9341 & d, double x , double y , double w, double h , double loval , double hival , double inc , double curval ,  int dig , int dec, unsigned int barcolor, unsigned int voidcolor, unsigned int bordercolor, unsigned int textcolor, unsigned int backcolor, String label, boolean & redraw){
  
    double stepval;
    double my, level;
    double i, data;
    // draw the border, scale, and label once
    // avoid doing this on every update to minimize flicker
    if (redraw == true) {
      redraw = false;
  
      d.drawRect(x - 1, y - h - 1, w + 2, h + 2, bordercolor);
      d.setTextColor(textcolor, backcolor);
      d.setTextSize(2);
      d.setCursor(x , y + 10);
      d.println(label);
      // step val basically scales the hival and low val to the height
      // deducting a small value to eliminate round off errors
      // this val may need to be adjusted
      stepval = ( inc) * (double (h) / (double (hival - loval))) - .001;
      for (i = 0; i <= h; i += stepval) {
        my =  y - h + i;
        d.drawFastHLine(x + w + 1, my,  5, textcolor);
        // draw lables
        d.setTextSize(1);
        d.setTextColor(textcolor, backcolor);
        d.setCursor(x + w + 12, my - 3 );
        data = hival - ( i * (inc / stepval));
        d.println(Format(data, dig, dec));
      }
    }
    // compute level of bar graph that is scaled to the  height and the hi and low vals
    // this is needed to accompdate for +/- range
    level = (h * (((curval - loval) / (hival - loval))));
    // draw the bar graph
    // write a upper and lower bar to minimize flicker cause by blanking out bar and redraw on update
    d.fillRect(x, y - h, w, h - level,  voidcolor);
    d.fillRect(x, y - level, w,  level, barcolor);
    // write the current value
    d.setTextColor(textcolor, backcolor);
    d.setTextSize(2);
    d.setCursor(x , y - h - 23);
    d.println(Format(curval, dig, dec));
  
  }
  
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
  
void DrawDial(Adafruit_ILI9341 & d, int cx, int cy, int r, double loval , double hival , double inc, double sa, double curval,  int dig , int dec, unsigned int needlecolor, unsigned int dialcolor, unsigned int  textcolor, String label, boolean & redraw) {
  
    double ix, iy, ox, oy, tx, ty, lx, rx, ly, ry, i, offset, stepval, data, angle;
    double degtorad = .0174532778;
    static double pix = cx, piy = cy, plx = cx, ply = cy, prx = cx, pry = cy;
  
    // draw the dial only one time--this will minimize flicker
    if ( redraw == true) {
      redraw = false;
      d.fillCircle(cx, cy, r - 2, dialcolor);
      d.drawCircle(cx, cy, r, needlecolor);
      d.drawCircle(cx, cy, r - 1, needlecolor);
      d.setTextColor(textcolor, dialcolor);
      d.setTextSize(2);
      d.setCursor(cx - 25, cy + 40);
      d.println(label);
  
    }
    // draw the current value
    d.setTextSize(2);
    d.setTextColor(textcolor, dialcolor);
    d.setCursor(cx - 25, cy + 20 );
    d.println(Format(curval, dig, dec));
    // center the scale about the vertical axis--and use this to offset the needle, and scale text
    offset = (270 +  sa / 2) * degtorad;
    // find hte scale step value based on the hival low val and the scale sweep angle
    // deducting a small value to eliminate round off errors
    // this val may need to be adjusted
    stepval = ( inc) * (double (sa) / (double (hival - loval))) + .00;
    // draw the scale and numbers
    // note draw this each time to repaint where the needle was
    for (i = 0; i <= sa; i += stepval) {
      angle = ( i  * degtorad);
      angle = offset - angle ;
      ox =  (r - 2) * cos(angle) + cx;
      oy =  (r - 2) * sin(angle) + cy;
      ix =  (r - 10) * cos(angle) + cx;
      iy =  (r - 10) * sin(angle) + cy;
      tx =  (r - 30) * cos(angle) + cx;
      ty =  (r - 30) * sin(angle) + cy;
      d.drawLine(ox, oy, ix, iy, textcolor);
      d.setTextSize(1.5);
      d.setTextColor(textcolor, dialcolor);
      d.setCursor(tx - 10, ty );
      data = hival - ( i * (inc / stepval)) ;
      d.println(Format(data, dig, dec));
    }
    // compute and draw the needle
    angle = (sa * (1 - (((curval - loval) / (hival - loval)))));
    angle = angle * degtorad;
    angle = offset - angle  ;
    ix =  (r - 10) * cos(angle) + cx;
    iy =  (r - 10) * sin(angle) + cy;
    // draw a triangle for the needle (compute and store 3 vertiticies)
    lx =  5 * cos(angle - 90 * degtorad) + cx;
    ly =  5 * sin(angle - 90 * degtorad) + cy;
    rx =  5 * cos(angle + 90 * degtorad) + cx;
    ry =  5 * sin(angle + 90 * degtorad) + cy;
    // first draw the previous needle in dial color to hide it
    // note draw performance for triangle is pretty bad...
  
    //d.fillTriangle (pix, piy, plx, ply, prx, pry, dialcolor);
    //d.fillTriangle (pix, piy, plx, ply, prx, pry, dialcolor);
  
    //d.fillTriangle (pix, piy, plx, ply, prx - 20, pry - 20, dialcolor);
    //d.fillTriangle (pix, piy, prx, pry, prx + 20, pry + 20, dialcolor);
  
    d.fillTriangle (pix, piy, plx, ply, prx, pry, dialcolor);
    d.drawTriangle (pix, piy, plx, ply, prx, pry, dialcolor);
  
    // then draw the old needle in need color to display it
    d.fillTriangle (ix, iy, lx, ly, rx, ry, needlecolor);
    d.drawTriangle (ix, iy, lx, ly, rx, ry, textcolor);
  
    // draw a cute little dial center
    d.fillCircle(cx, cy, 8, textcolor);
    //save all current to old so the previous dial can be hidden
    pix = ix;
    piy = iy;
    plx = lx;
    ply = ly;
    prx = rx;
    pry = ry;
  
}
  
/*
    This method will draw a horizontal bar graph for single input
    it has a rather large arguement list and is as follows
  
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
  
void DrawBarChartH(Adafruit_ILI9341 & d, double x , double y , double w, double h , double loval , double hival , double inc , double curval ,  int dig , int dec, unsigned int barcolor, unsigned int voidcolor, unsigned int bordercolor, unsigned int textcolor, unsigned int backcolor, String label, boolean & redraw){
    double stepval;
    double level;
    double i, data;
  
    // draw the border, scale, and label once
    // avoid doing this on every update to minimize flicker
    // draw the border and scale
    if (redraw == true) {
      redraw = false;
      d.drawRect(x , y , w, h, bordercolor);
      d.setTextColor(textcolor, backcolor);
      d.setTextSize(2);
      d.setCursor(x , y - 20);
      d.println(label);
      // step val basically scales the hival and low val to the width
      stepval =  inc * (double (w) / (double (hival - loval))) - .00001;
      // draw the text
      for (i = 0; i <= w; i += stepval) {
        d.drawFastVLine(i + x , y + h + 1,  5, textcolor);
        // draw lables
        d.setTextSize(1);
        d.setTextColor(textcolor, backcolor);
        d.setCursor(i + x , y + h + 10);
        // addling a small value to eliminate round off errors
        // this val may need to be adjusted
        data =  ( i * (inc / stepval)) + loval + 0.00001;
        d.println(Format(data, dig, dec));
      }
    }
    // compute level of bar graph that is scaled to the width and the hi and low vals
    // this is needed to accompdate for +/- range capability
    // draw the bar graph
    // write a upper and lower bar to minimize flicker cause by blanking out bar and redraw on update
    level = (w * (((curval - loval) / (hival - loval))));
    d.fillRect(x + level + 1, y + 1, w - level - 2, h - 2,  voidcolor);
    d.fillRect(x + 1, y + 1 , level - 1,  h - 2, barcolor);
    // write the current value
    d.setTextColor(textcolor, backcolor);
    d.setTextSize(2);
    d.setCursor(x + w + 10 , y + 5);
    d.println(Format(curval, dig, dec));
}
  
// function to format a numerical value into a string with the specified number of digits and decimal points
String Format(double val, int dec, int dig ) {
    int addpad = 0;
    char sbuf[20];
    String condata = (dtostrf(val, dec, dig, sbuf));
  
  
    int slen = condata.length();
    for ( addpad = 1; addpad <= dec + dig - slen; addpad++) {
      condata = " " + condata;
    }
    return (condata);
  
}