#ifndef SENSOR_RUN
#define SENSOR_RUN


#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LSM9DS0.h>
#include <Adafruit_Sensor.h>  // not used in this demo but required!

// 9 is plugged in to CSXM
// 10 is plugged into CSG
// https://www.pjrc.com/teensy/td_libs_SPI.html
const int CSXM_Pin = 9;
const int CSG_Pin = 10;


/*
 * https://learn.sparkfun.com/tutorials/lsm9ds0-hookup-guide
 * If you're using SPI, here is an example hardware setup:
    LSM9DS0 --------- Arduino
          CSG -------------- 9
          CSXM ------------- 10
          SDOG ------------- 12
          SDOXM ------------ 12 (tied to SDOG)
          SCL -------------- 13
          SDA -------------- 11
          VDD -------------- 3.3V
          GND -------------- GND

 */
void startSensor() {   
  pinMode(6, OUTPUT);      // LED to show if a program is using serial port
  digitalWrite(6, HIGH);   //   (active low signal, HIGH = LED off)
  while (!Serial.dtr()) ;  // wait for user to start the serial monitor
  digitalWrite(6, LOW);
  delay(250);
  Serial.println("Hi there, new serial monitor session");

  // Set the CS Pins to active low (only one is enabled at once for SPI communication)
  pinMode(CSG_Pin, OUTPUT);
  digitalWrite(CSG_Pin, HIGH);
  pinMode(CSXM_Pin, OUTPUT);
  digitalWrite(CSXM_Pin, LOW);
  
  // Or hardware SPI! In this case, only CS pins are passed in
  Adafruit_LSM9DS0 lsm = Adafruit_LSM9DS0(CSG_Pin, CSXM_Pin);
  
  if (!lsm.begin())
  {
  
    Serial.println("Oops ... unable to initialize the LSM9DS0. Check your wiring!");
    delay(1000);
  } else {
    Serial.println("Begin worked....");
  }
}
#endif
