/*
    To understand the SSM protocol, packet formatting, and requests, see:
    https://www.romraider.com/RomRaider/SsmProtocol
*/

#include <Arduino.h>
#include <config.h>

#include <subaru_ssm.h>

/*************************************************************************
 * VARIABLE DEFINITIONS
**************************************************************************/
//stores time from millis() when previous message was read
double prevReadTime = 0;

// data request message
byte ssmReqMessage[REQUEST_MESSAGE_SIZE] = {  0x80, 0x10, 0xf0, 0x20, 0xa8, 0x01, 
                                            0x00, 0x00, 0x08, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x10, 
                                            0x00, 0x00, 0x11, 0x00, 0x00, 0x12, 0x00, 0x00, 0x15, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x2a, 
                                            0x09    };

//array to store the raw hex bytes received from the ECU
int ECUbytes[RECEIVE_MESSAGE_SIZE]; //try changing back to uint8

//true if the resposne from the ECU was good (correct format, sent to this device, good checksum)
bool responseGood;

/*************************************************************************
 * INSTATIATION DEFINITIONS
**************************************************************************/
//struct holding the interpretted ECU data
ECU_Data interpretted_data = {};

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
    delay(10);
    Serial.println();
    if(Serial2.available())
        responseGood = readECU(ECUbytes);
    else{
        responseGood = false;
        Serial.println("Serial2 unavailable");
        return false;
    }

    Serial.print("Response Good: ");
    Serial.println(responseGood);
    
    printECUbytes(Serial);
    
    if(responseGood){
        interpretECUdata(&interpretted_data, ECUbytes);
        Serial.println();
        outputHeaders(Serial);
        outputValues(Serial, &interpretted_data);
        prevReadTime = millis();
    }
    else {
        //Clearing the serial receive buffer to get rid of the bad packet
        Serial2.end();
        Serial2.begin(4800);
        Serial.println("Flush");
    }

    if((millis() - prevReadTime) > 50){
        writeSSM(ssmReqMessage);
        prevReadTime = millis();
    }
    Serial2.flush();
    return true;
}

// prints the raw hex bytes received from the ECU to the stream of choice
void printECUbytes(Stream &outStream){
    Serial.print("ECU bytes: ");
    for (uint8_t i = 0; i < RECEIVE_MESSAGE_SIZE; i++){
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
    Serial.print("Sending packet... ");
    for (byte x = 0; x < REQUEST_MESSAGE_SIZE; x++) {
        Serial2.write(requestMessage[x]);
        Serial.print(String(requestMessage[x],HEX));
        Serial.print(",");
    }
    Serial2.flush();
    Serial2.end();
    Serial2.begin(4800);
}


// reads the raw data from the ECU off of the serial line
bool readECU(int* rawDataArray){
    byte currentByte = 0;
    bool isNewPacket = 0;
    byte sumBytes = 0;
    uint8_t dataSize = 0;
    uint8_t bytePlace = 0;
    uint8_t dataArrayLength = RECEIVE_MESSAGE_SIZE;
    uint8_t loopLength = 20;

    byte zeroesLoopSpot = 0;
    bool nonZeroes = false;

    for (uint8_t i = 0; i < dataArrayLength; i++)
        rawDataArray[i] = 0;
    Serial.println();
    for (byte j = 0; j < loopLength; j++){  //try as uint8_t
        currentByte = Serial2.read();
        Serial.print(String(currentByte, HEX));
        Serial.print(", ");
        delay(READ_DELAY);
        rawDataArray[bytePlace] = currentByte;

        if (currentByte == 0x80 && dataSize == 0){
            isNewPacket = true;
            j = 0;                              // CAN I REMOVE THIS
        }

        if(j == (loopLength-1) && isNewPacket != true) 
            return false;

        if(isNewPacket){   //REMOVE -1 PART
            // checking that the beginning of the packet is what we expect
            if((bytePlace == 0 && currentByte != 0x80) || (bytePlace == 1 && currentByte != 0xf0) || (bytePlace == 2 && currentByte != 0x10)){
                Serial.println("Bad start");
                return false;

            }
                
            if (bytePlace == 3){
                dataSize = currentByte;
                loopLength = currentByte + 6;
                Serial.print("Loop length: ");
                Serial.println(loopLength);

                // arbitrary value of 20, another check to make sure the data we are receiving is good
                if(loopLength > 20){
                    Serial.println("Loop length too long");
                    return false;
                }
            }
            
            bytePlace += 1;

            if (bytePlace == (dataSize + 5)){
                if(currentByte != calculateChecksum(sumBytes)){
                   Serial.println("Bad checksum");
                   return false; 
                }
                Serial.println("Good checksum");   
                return true;
            }
            else
                sumBytes += currentByte;
        }
    }
}

/*
boolean readECU(uint32_t* dataArray)
{
  bool nonZeroes = false;
  byte data = 0;
  boolean isPacket = false;
  byte sumBytes = 0;
  byte checkSumByte = 0;
  byte dataSize = 0;
  byte bytePlace = 0;
  byte zeroesLoopSpot = 0;
  byte dataArrayLength = RECEIVE_MESSAGE_SIZE;
  byte loopLength = 20;
  for (int i = 0; i < RECEIVE_MESSAGE_SIZE; i++)
    dataArray[i] = 9999;
  for (byte j = 0; j < loopLength; j++)
  //for (byte j = 0; j < sizeof(dataArray)/sizeof(int); j++)
  {
    data = Serial2.read();
    Serial.print(String(data,HEX));
    Serial.print(", ");
    delay(READ_DELAY);
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
        checkSumByte = calculateChecksum(sumBytes);  //the 8 least significant bits of sumBytes

        if (data != checkSumByte) {
          Serial.println(F("checksum error"));
          Serial.print("Loop Length: ");
          Serial.println(loopLength);
          return false;
        }
//        Serial.println("Checksum is good");

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

*/
// takes in the rawhex data received from the ECU, applies the interpretation calculations to get a readable value
bool interpretECUdata(ECU_Data* interpData, int* rawArray){
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

// outputs interpretted ECU data to the specified stream (serial, file, etc.)
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

// logs values currently in the ECU_Data struct to the log file (SD card)
void logCurrentValues(File log, ECU_Data* data){
  outputValues(log, data);
  log.flush();
}