#include <Arduino.h>
#include <config.h>
#include <declarations.h>

#include <subaru_ssm.h>
#include <gauge_tft.h>
#include <sd_logger.h>

void setup() {
  Serial.begin(SERIAL_MONITOR_BAUD);

  loggerBegin();
  gaugesBegin();

  ssmBegin();
}

void loop() {
  

}