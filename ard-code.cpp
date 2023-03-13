
// 06 SpecB Rom ID
// 4512783106

// Clutch and Brake Switch Info
// 72  (0x48) [01001000] = Brake On
// 128 (0x80) [10000000] = Clutch On
// 200 (0xC8) [11001000] = Both On

// Can Stuff
#include <Arduino.h>

#define SINGLE_REQ
#define INTERPRET
#define LOG
#define USE_DISPLAY

#ifdef LOG
  #include <SD.h>
  #define chipSelect BUILTIN_SDCARD
  File logFile;
#endif

#ifdef USE_DISPLAY
  #include <SPI.h>
  #include <Adafruit_ILI9341.h>

  boolean graph_1 = true;
  boolean graph_2 = true;
  boolean graph_3 = true;

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
  
  #define TFT_DC 9
  #define TFT_CS 10
  #define TFT_MOSI 11
  #define TFT_CLK 13
  #define TFT_RST 14
  #define TFT_MISO 12

  Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
#endif

//int ECUbytes[11] = {};
int ECUbytes[16] = {};

/*                                      |rpm - 2 bytes| tmp |  spd |  swt | gear   | tps |
byte ReqData[28] = {128,16,240,23,168,0,0,0,15,0,0,14,0,0,8,0,0,16,0,1,33,255,83,5,0,0,21,2}; // add throttle
                  //80,10,f0,17,a8,0,0,0,f,0,0,e,0,0,8,0,0,10,0,1,21,ff,53,5,0,0,15,2
byte ReqDataSize = 28;
*/
byte capBitsReq[6] = {0x80, 0x10, 0xf0, 0x01, 0xBF, 0x40}; 
byte capBitsReqSize = 6;


//subtract 40 from decomal value for deg C
//byte coolTempReq[22] = {0x80, 0x10, 0xf0, 0x11, 0xa8, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x10, 0x7b}; //7 byte response expected
//ECT, MAP, IAT, TPS, VOLT

#ifdef SINGLE_REQ
  byte coolTempReq[37] = { 0x80, 0x10, 0xf0, 0x20, 0xa8, 0x01, 
                        0x00, 0x00, 0x08, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x10, 
                        0x00, 0x00, 0x11, 0x00, 0x00, 0x12, 0x00, 0x00, 0x15, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x2a, 
                        0x09};
                        
  //byte coolTempReq[22] = {0x80, 0x10, 0xf0, 0x11, 0xa8, 0x01, 0x00, 0x00, 0x08, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x12, 0x00, 0x00, 0x15, 0x00, 0x00, 0x1c, 0x92}; //7 byte response expected
#else
  byte coolTempReq[22] = {0x80, 0x10, 0xf0, 0x11, 0xa8, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x12, 0x00, 0x00, 0x15, 0x00, 0x00, 0x1c, 0x91}; //7 byte response expected
#endif


//byte coolTempReqSize = 22;
byte coolTempReqSize = 37;
/*
byte coolTempReq[37] = { 0x80, 0x10, 0xf0, 0x20, 0xa8, 0x00, 
                        0x00, 0x00, 0x08, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x10, 
                        0x00, 0x00, 0x11, 0x00, 0x00, 0x12, 0x00, 0x00, 0x15, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x2a, 
                        0x08}; //7 byte response expected
*/
//22d without address with 5 size
//Expected response: 0x80, 0xf0, 0x10, 0x2, 0xe8, 0x28, 0x(check)   (for 0 deg C)

//multiply decimal value by 0.08
byte batVoltReq[10] = {0x80, 0x10, 0xf0, 0x05 , 0xa8, 0x00, 0x00, 0x00, 0x1c, 0x49};
byte batVoltReqSize = 10;
//Expected response: 0x80, 0xf0, 0x10, 0x2, 0xe8, 0x9c, 0x(check)

typedef struct{
  double ECT;
  double MAP;
  double RPM;
  double speedkm;
  double timing;
  double IAT;
  double TPS;
  double BATVOLT;
  double EFT;
} ECU_Data;
ECU_Data interpretted_data = {};

int SerialStatus=0;
int milli;
int ClrToSnd;


//byte ReqData[13] = {128,16,240,8,168,0,0,0,15,0,0,14,77};
//byte ReqDataSize = 13;
int readDelay = 2;
unsigned long prvTime;
unsigned long prvReadTime;
unsigned long curTime;

void setup()
{
  Serial.begin(115200); //for diagnostics
    while (!Serial) {
      // wait
    }
  Serial.println("Connecting to Serial");
  Serial2.begin(4800); //SSM uses 4800 8N1 baud rate
    while (!Serial2) {
      //wait
      delay(50);
  }
  Serial.println("Serial Line Established");
  Serial.println("Setup Complete");
  delay(50);
  /*
  writeSSM(capBitsReq, capBitsReqSize, Serial2);
  Serial2.flush();
  delay(500);
  */

  #ifdef LOG
    if (!SD.begin(chipSelect)) {
      Serial.println("SD initialization failed!");
    }
    else
      Serial.println("SD Initialized");
    logFile = SD.open("log.txt", FILE_WRITE);
   
    if(logFile)
      Serial.println("File opened");
    else
      Serial.println("File open failed");
  
    outputHeaders(logFile);
    logFile.flush();
  #endif

  #ifdef USE_DISPLAY
    tft.begin();
    tft.fillScreen(BLACK);
    tft.setRotation(1);
  #endif
  
  #ifdef SINGLE_REQ
    delay(500);
    writeSSM(coolTempReq, coolTempReqSize, Serial2);
  #endif
  
  delay (2);
}

bool responseGood = 0;

/*************************************************************************
 * LEFT-OFF HERE IN THE CODE CONVERSION
**************************************************************************/

void loop()
{
curTime = millis();
milli=curTime - prvTime;  
Serial.println("Loop ");
delay(10);

//if (milli > 250) {
if(true){
  Serial2.flush(); //waits for serial out buffer to empty (everything sent)
  //delay(5);
  //writeSSM(ReqData, ReqDataSize, Serial2);
  
  #ifndef SINGLE_REQ
    writeSSM(coolTempReq, coolTempReqSize, Serial2);
  #endif
  
  //Serial.println("Timer Popped");
  prvTime=millis();
  }

  if (Serial2.available()) {  
    Serial.println();
    responseGood = readECU(ECUbytes, false);
    /*if(!responseGood){
      while(Serial2.available()){
        if( (Serial2.peek()) != 0x80){
           Serial2.read();
        }
        Serial.println("flushing");
      }
    }*/

    if(!responseGood){
      //Serial2.flush();
      Serial2.end();
      Serial2.begin(4800);
      Serial.println("Flushed");
    }
    
    Serial.print("Response Good: ");
    Serial.println(responseGood);
    
    prvTime = curTime;
    
    //if(responseGood){
    if(true){
      Serial.print("ECU bytes: ");
      for (int i = 0; i < sizeof(ECUbytes)/sizeof(int); i++){
        Serial.print(String(ECUbytes[i], HEX));
        Serial.print(",");
      }
      #ifdef INTERPRET
        if(responseGood){
          interpretData(&interpretted_data, ECUbytes);
          Serial.println();
          outputHeaders(Serial);
          outputValues(Serial, &interpretted_data);
          #ifdef LOG
            outputValues(logFile, &interpretted_data);
            logFile.flush();
          #endif
          #ifdef USE_DISPLAY
            DrawBarChartV(tft, 10,  210, 30, 180, 0, 100 , 10, interpretted_data.TPS , 3 , 1, BLUE, DKBLUE, BLUE, WHITE, BLACK, "TPS", graph_1);
            DrawBarChartH(tft, 100, 180, 150, 30, 0, 20, 2, interpretted_data.MAP, 2, 2, GREEN, DKGREEN,  GREEN, WHITE, BLACK, "MAP", graph_2);
            DrawDial(tft, 230, 90, 80, 0, 7000 , 500, 240, interpretted_data.RPM,  4 , 0, RED, WHITE, BLACK, "RPM", graph_3);
          #endif
          }
      #endif
      Serial.println();
    }
    
    //delay(20);    
  }
  else{
    delay(100);
    Serial.println("Serial 2 unavailable");
  }
    

  #ifdef SINGLE_REQ
    if ((curTime - prvReadTime) > 50){
      writeSSM(coolTempReq, coolTempReqSize, Serial2);
      prvReadTime = millis();
      //readECU(ECUbytes, false);
    }
      
    if (responseGood)
      prvReadTime = millis();
  #else
    if (ClrToSnd == 0) {
      //writeSSM(ReqData, ReqDataSize, Serial2);
      writeSSM(coolTempReq, coolTempReqSize, Serial2);
      ClrToSnd = 1;
      Serial2.flush();
      Serial2.end();
      Serial2.open(4800);
    }
  #endif

  //delay(250);
}

/* returns the 8 least significant bits of an input byte*/
byte CheckSum(byte sum) {
  byte counter = 0;
  byte power = 1;
  for (byte n = 0; n < 8; n++) {
    counter += bitRead(sum, n) * power;
    power = power * 2;
  }
  return counter;
}

/*writes data over the software serial port
the &digiSerial passes a reference to the external
object so that we can control it outside of the function*/
void writeSSM(byte data[], byte length, Stream &digiSerial) {
  Serial.print(F("Sending packet... "));
  for (byte x = 0; x < length; x++) {
    digiSerial.write(data[x]);
    Serial.print(String(data[x],HEX));
    Serial.print(",");
  }
  digiSerial.flush();
  Serial.println();
}

//this will change the values in dataArray, populating them with values respective of the poll array address calls
boolean readECU(int* dataArray, boolean nonZeroes)
{
  byte data = 0;
  boolean isPacket = false;
  byte sumBytes = 0;
  byte checkSumByte = 0;
  byte dataSize = 0;
  byte bytePlace = 0;
  byte zeroesLoopSpot = 0;
  byte dataArrayLength = sizeof(dataArray)/sizeof(int);
  byte loopLength = 20;
  for (int i = 0; i < sizeof(dataArray)/sizeof(int); i++)
    dataArray[i] = 9999;
  for (byte j = 0; j < loopLength; j++)
  //for (byte j = 0; j < sizeof(dataArray)/sizeof(int); j++)
  {
    data = Serial2.read();
    Serial.print(String(data,HEX));
    Serial.print(", ");
    delay(readDelay);
    dataArray[bytePlace] = data;

    if (data == 128 && dataSize == 0) { //0x80 or 128 marks the beginning of a packet
      isPacket = true;
      j = 0;
      //Serial.println("Begin Packet");
    }

    //terminate function and return false if no response is detected
    if (j == (loopLength - 1) && isPacket != true)
    {
      return false;
    }

    if (isPacket == true && data != -1) {
  //    Serial.print(data); // for debugging: shows in-packet data
  //    Serial.print(" ");

      if ( (bytePlace == 0 && data != 0x80) || (bytePlace == 1 && data != 0xf0) || (bytePlace == 2 && data != 0x10) ){
        Serial.println("Bad start of read");
        return false;
      }
        

      if (bytePlace == 3) { // how much data is coming
        dataSize = data;
        loopLength = data + 6;
        if (loopLength > 20){
          Serial.println("Loop length too long");
          return false;
        }
          
      }

      if (bytePlace > 4 && bytePlace - 5 < dataArrayLength && nonZeroes == false)
      {
        //dataArray[bytePlace - 5] = data;
      }
      else if (bytePlace > 4 && zeroesLoopSpot < dataArrayLength / 2 && nonZeroes == true && data != 0 && bytePlace < dataSize + 4)
      {
        //dataArray[zeroesLoopSpot] = data;
        //dataArray[zeroesLoopSpot + (dataArrayLength / 2)] = bytePlace;
        zeroesLoopSpot++;
      }

      bytePlace += 1; //increment bytePlace

      //once the data is all recieved, checksum and re-set counters
     // Serial.print("byte place: ");
     // Serial.println(bytePlace);
      if (bytePlace == dataSize + 5) {
        checkSumByte = CheckSum(sumBytes);  //the 8 least significant bits of sumBytes

        if (data != checkSumByte) {
          Serial.println(F("checksum error"));
          Serial.print("Loop Length: ");
          Serial.println(loopLength);
          return false;
        }
//        Serial.println("Checksum is good");
/*
        ClrToSnd = 0;
        isPacket = false;
        sumBytes = 0;
        bytePlace = 0;
        checkSumByte = 0;
        dataSize = 0;
*/
        Serial.print("Loop Length: ");
        Serial.println(loopLength);
        return true;
      }
      else {
        sumBytes += data; // this is to compare with the checksum byte
        //Serial.print(F("sum: "));
        //Serial.println(sumBytes);
      }
    }
  }
  
}

bool interpretData(ECU_Data* interpData, int* rawArray){
  if(rawArray[1] != 0xf0) //ignore anything that isn't being sent to the diagnostic tool (us)
    return false;
  
  interpData->ECT = rawArray[5]-40;        //deg C
  interpData->MAP = rawArray[6]*37.0/255;    //absolute PSI
  interpData->RPM = (rawArray[8] | (rawArray[7] << 8))/4.0;    //RPM
  interpData->speedkm = rawArray[9];         //km/h
  interpData->timing = (rawArray[10]-128)/2.0;     //degrees BTDC
  interpData->IAT = rawArray[11]-40;       //deg C
  interpData->TPS = rawArray[12]*100/255.0;  //percent
  interpData->BATVOLT = rawArray[13]*0.08; //volt
  interpData->EFT = rawArray[14]-40;       //deg C
  return true;
}

void outputHeaders(Stream &outStream){
  outStream.println("RPM\tSpeed\tTPS\tMAP\tTiming\tECT\tIAT\tEFT\tVoltage");
}

void outputValues(Stream &outStream, ECU_Data* data){
  outStream.println();
  outStream.print(data->RPM);

  outStream.print("\t");
  outStream.print(data->speedkm);

  outStream.print("\t");
  outStream.print(data->TPS);

  outStream.print("\t");
  outStream.print(data->MAP);

  outStream.print("\t");
  outStream.print(data->timing);

  outStream.print("\t");
  outStream.print(data->ECT);

  outStream.print("\t");
  outStream.print(data->IAT);

  outStream.print("\t");
  outStream.print(data->EFT);

  outStream.print("\t");
  outStream.print(data->BATVOLT);
}

#ifdef USE_DISPLAY
    
  /*
    This method will draw a vertical bar graph for single input
    it has a rather large arguement list and is as follows
  
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
  
  void DrawBarChartV(Adafruit_ILI9341 & d, double x , double y , double w, double h , double loval , double hival , double inc , double curval ,  int dig , int dec, unsigned int barcolor, unsigned int voidcolor, unsigned int bordercolor, unsigned int textcolor, unsigned int backcolor, String label, boolean & redraw)
  {
  
    double stepval, range;
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
    it has a rather large arguement list and is as follows
  
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
    static double px = cx, py = cy, pix = cx, piy = cy, plx = cx, ply = cy, prx = cx, pry = cy;
  
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
  
  void DrawBarChartH(Adafruit_ILI9341 & d, double x , double y , double w, double h , double loval , double hival , double inc , double curval ,  int dig , int dec, unsigned int barcolor, unsigned int voidcolor, unsigned int bordercolor, unsigned int textcolor, unsigned int backcolor, String label, boolean & redraw)
  {
    double stepval, range;
    double mx, level;
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
#endif