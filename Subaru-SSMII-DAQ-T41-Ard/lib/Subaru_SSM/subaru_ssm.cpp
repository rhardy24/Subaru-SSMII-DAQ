/*
    To understand the SSM protocol, packet formatting, and requests, see:
    https://www.romraider.com/RomRaider/SsmProtocol
*/

#include <Arduino.h>
#include <config.h>
#include <declarations.h>

#include <subaru_ssm.h>
#include <sd_logger.h>

/*************************************************************************
 * FUNCTIONS
**************************************************************************/

//Initializes SSM
bool ssmBegin(){
    Serial.println("Establishing SSM Connection");
    Serial2.begin(4800);
    while(!Serial2)
        delay(50);
    Serial.println("SSM Connection Established");

    writeSSM(ssmReqMessage);

    return true;
}

bool receiveECUdata(){
    Serial.println();
    responseGood = readECU(ECUbytes);

    Serial.print("Response Good: ");
    Serial.println(responseGood);
    
    printECUbytes(Serial);
    
    if(responseGood){
        interpretData(&interpretted_data, ECUbytes);
        Serial.println();
        outputHeaders(Serial);
        outputValues(Serial, &interpretted_data);
        prevReadTime = millis();
    }
    else {
        //Clearing the serial receive buffer to get rid of the bad packet
        Serial2.end();
        Serial2.begin(4800);
    }

    if((millis() - prevReadTime) > 50){
        writeSSM(ssmReqMessage);
        prevReadTime = millis();
    }

    return true;
}

// prints the raw hex bytes received from the ECU to the stream of choice
void printECUbytes(Stream &outStream){
    Serial.print("ECU bytes: ");
    for (uint8_t i = 0; i < sizeof(ECUbytes)/sizeof(int); i++){
        outStream.print(String(ECUbytes[i], HEX));
        outStream.print(",");
    }
}

/*
    Calculates the checksum based on the sum byte inputted into the function.
    Just returns the 8 least significant bits of the input byte
*/
byte calculateChecksum(byte sum){
    byte counter = 0;
    byte power = 1;
    for (byte n = 0; n < 8; n++) {
        counter += bitRead(sum, n) * power;
        power = power * 2;
    }
    return counter;
}

// writes the request packet over serial to go to the ECU
void writeSSM(byte requestMessage[]){
    Serial.print(F("Sending packet... "));
    for (byte x = 0; x < sizeof(requestMessage)/sizeof(byte); x++) {
        Serial2.write(requestMessage[x]);
        Serial.print(String(requestMessage[x],HEX));
        Serial.print(",");
    }
    Serial2.flush();
}

// reads the raw data from the ECU off of the serial line
bool readECU(uint8_t* rawDataArray){
    byte currentByte = 0;
    bool isNewPacket = 0;
    byte sumBytes = 0;
    uint8_t dataSize = 0;
    uint8_t bytePlace = 0;
    uint8_t dataArrayLength = sizeof(rawDataArray)/sizeof(int);
    uint8_t loopLength = 20;

    for (uint8_t i = 0; i < dataArrayLength; i++)
        rawDataArray[i] = 0;
    
    for (uint8_t j = 0; j < loopLength; j++){
        currentByte = Serial2.read();
        delay(READ_DELAY);
        rawDataArray[bytePlace] = currentByte;

        if (currentByte == 0x80 && dataSize == 0){
            isNewPacket = true;
            j = 0;                              // CAN I REMOVE THIS
        }

        if(isNewPacket){
            // checking that the beginning of the packet is what we expect
            if((bytePlace == 0 && currentByte != 0x80) || (bytePlace == 1 && currentByte != 0xf0) || (bytePlace == 2 && currentByte != 0x10))
                return false;
            
            if (bytePlace == 3){
                dataSize = currentByte;
                loopLength = currentByte + 6;

                // arbitrary value of 20, another check to make sure the data we are receiving is good
                if(loopLength > 20){
                    Serial.println("Loop length too long");
                    return false;
                }
            }

            bytePlace += 1;

            if (bytePlace == (dataSize + 5)){
                if(currentByte != calculateChecksum(sumBytes))
                    return false;
                return true;
            }
            else
                sumBytes += currentByte;
        }
    }
}