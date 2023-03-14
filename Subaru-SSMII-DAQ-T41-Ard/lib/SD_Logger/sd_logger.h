#include <Arduino.h>
#include <config.h>
#include <declarations.h>

#ifndef SD_LOGGER_H
#define SD_LOGGER_H

    /*************************************************************************
     * EXTERNAL INCLUDES
    **************************************************************************/
    #include <SD.h>

    /*************************************************************************
     * DEFINITIONS
    **************************************************************************/
    #define chipSelect BUILTIN_SDCARD

    /*************************************************************************
     * INSTATIATIONS
    **************************************************************************/
    File logFile;

    /*************************************************************************
     * FUNCTION PROTOTYPES
    **************************************************************************/
    // initializes the sd logger
    bool loggerBegin();

    // logs values currently in the ECU_Data struct to the log file (SD card)
    void logCurrentValues(File log, ECU_Data* data);
#endif