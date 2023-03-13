#include <Arduino.h>
#include <config.h>
#include <declarations.h>

#include <sd_logger.h>


/*************************************************************************
 * FUNCTIONS
**************************************************************************/

// initializes the sd logger
bool loggerBegin(){
    if (!SD.begin(chipSelect)) {
      Serial.println("SD initialization failed!");
    }
    else
      Serial.println("SD Initialized");

    // CORRECT THE FILE NAME
    logFile = SD.open("newlog.CSV", FILE_WRITE);
   
    if(logFile)
      Serial.println("File opened");
    else
      Serial.println("File open failed");
  
    outputHeaders(logFile);
    logFile.flush();

    return true;
}