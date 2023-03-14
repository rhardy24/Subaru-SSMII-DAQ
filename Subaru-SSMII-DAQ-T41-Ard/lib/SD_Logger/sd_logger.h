#include <Arduino.h>
#include <config.h>

#ifndef SD_LOGGER__H_
#define SD_LOGGER__H_

    /*************************************************************************
     * EXTERNAL INCLUDES
    **************************************************************************/
    #include <SD.h>

    /*************************************************************************
     * DEFINITIONS
    **************************************************************************/
    #define chipSelect BUILTIN_SDCARD

    /*************************************************************************
     * INSTATIATION DECLARATIONS
    **************************************************************************/
    extern File logFile;

    /*************************************************************************
     * FUNCTION PROTOTYPES
    **************************************************************************/
    // initializes the sd logger
    bool loggerBegin();

    // outputs data column headers to the specified stream (serial, file, etc.)
    void outputHeaders(Stream &outStream);
#endif