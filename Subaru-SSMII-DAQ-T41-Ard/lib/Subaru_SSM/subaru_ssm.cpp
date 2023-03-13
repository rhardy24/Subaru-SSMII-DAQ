#include <Arduino.h>
#include <config.h>
#include <declarations.h>

#include <subaru_ssm.h>

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