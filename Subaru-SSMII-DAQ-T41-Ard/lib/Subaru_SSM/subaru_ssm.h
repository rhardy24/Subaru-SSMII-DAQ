#include <Arduino.h>
#include <config.h>
#include <declarations.h>

#ifndef SUBARU_SSM_H
#define SUBARU_SSM_H

    /*************************************************************************
     * USER SETTINGS
    **************************************************************************/


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
     * VARIABLES
    **************************************************************************/
    //stores time from millis() when previous message was read
    uint32_t prevReadTime;

    // data request message
    byte ssmReqMessage[REQUEST_MESSAGE_SIZE] = {  0x80, 0x10, 0xf0, 0x20, 0xa8, 0x01, 
                                                0x00, 0x00, 0x08, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x10, 
                                                0x00, 0x00, 0x11, 0x00, 0x00, 0x12, 0x00, 0x00, 0x15, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x2a, 
                                                0x09    };
    
    //array to store the raw hex bytes received from the ECU
    uint8_t ECUbytes[RECEIVE_MESSAGE_SIZE];

    //true if the resposne from the ECU was good (correct format, sent to this device, good checksum)
    bool responseGood;

    /*************************************************************************
     * INSTATIATIONS
    **************************************************************************/
    //struct holding the interpretted ECU data
    ECU_Data interpretted_data = {};

    /*************************************************************************
     * FUNCTION PROTOTYPES
    **************************************************************************/

    //Initializes SSM
    bool ssmBegin();

#endif