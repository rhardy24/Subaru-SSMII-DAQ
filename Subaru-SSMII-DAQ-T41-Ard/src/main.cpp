#include <Arduino.h>

#include <gauge_tft.h>
#include <sd_logger.h>
#include <subaru_ssm.h>

#include <config.h>

void setup() {
  #ifdef DEBUB
    Serial.begin(SERIAL_MONITOR_BAUD);
    while(!Serial)
      delay(10);
  #endif

  loggerBegin();
  gaugesBegin();

  delay(250);

  ssmBegin();
}

void loop() {
  receiveECUdata();
  logCurrentValues(logFile, &interpretted_data);
  //uint8_t newScreenNum = checkTouch();
  //updateGauges(&interpretted_data, newScreenNum);
  updateGauges(&interpretted_data, 0);

}