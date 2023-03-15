/*
    To understand the SSM protocol, packet formatting, and requests, see:
    https://www.romraider.com/RomRaider/SsmProtocol
*/

#include <Arduino.h>
#include <config.h>

#ifndef SUBARU_SSM__H_
#define SUBARU_SSM__H_

    /*************************************************************************
     * EXTERNAL INCLUDES
    **************************************************************************/
    #include <SD.h>
    #include <sd_logger.h>

    /*************************************************************************
     * DEFINITIONS
    **************************************************************************/
    //delay in milliseconds between reading in bytes
    #define READ_DELAY 2

    //number of addresses in the request message
    #define NUM_REQUESTED_ADDRESSES 10

    #define REQUEST_MESSAGE_SIZE (6+3*NUM_REQUESTED_ADDRESSES+1)
    #define RECEIVE_MESSAGE_SIZE (5+NUM_REQUESTED_ADDRESSES+1)

    /*************************************************************************
     * DATA STRUCTURES
    **************************************************************************/
    //struct to store the interpretted ECU data
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

    /*************************************************************************
     * VARIABLE DECLARATION
    **************************************************************************/
    //stores time from millis() when previous message was read
    extern double prevReadTime;

    // data request message
    extern byte ssmReqMessage[REQUEST_MESSAGE_SIZE];
    
    //array to store the raw hex bytes received from the ECU
    extern int ECUbytes[RECEIVE_MESSAGE_SIZE];

    //true if the resposne from the ECU was good (correct format, sent to this device, good checksum)
    extern bool responseGood;

    /*************************************************************************
     * INSTATIATION DECLARATION
    **************************************************************************/
    //struct holding the interpretted ECU data
    extern ECU_Data interpretted_data;

    /*************************************************************************
     * FUNCTION PROTOTYPES
    **************************************************************************/

    // Initializes SSM
    bool ssmBegin();

    // Sends re
    bool receiveECUdata();

    // prints the raw hex bytes received from the ECU to the stream of choice
    void printECUbytes(Stream &outStream);

    /*
        Calculates the checksum based on the sum byte inputted into the function.
        Just returns the 8 least significant bits of the input byte
    */
    byte calculateChecksum(byte sum);

    // writes the request packet over serial to go to the ECU
    void writeSSM(byte requestMessage[]);

    // reads the raw data from the ECU off of the serial line
    bool readECU(int* rawDataArray);

    // takes in the rawhex data received from the ECU, applies the interpretation calculations to get a readable value
    bool interpretECUdata(ECU_Data* interpData, int* rawArray);

    // outputs interpretted ECU data to the specified stream (serial, file, etc.)
    void outputValues(Stream &outStream, ECU_Data* data);

    // logs values currently in the ECU_Data struct to the log file (SD card)
    void logCurrentValues(File log, ECU_Data* data);

#endif