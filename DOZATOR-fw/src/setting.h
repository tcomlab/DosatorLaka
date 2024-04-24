// Main project definition file
#ifndef setting_h
#define setting_h

#include <Arduino.h>
#include <ModbusMaster.h>
//#include <arduino-timer.h>
#include <HX711.h>
#include <GyverTimers.h>
//#include <GyverFilters.h>
#include <avr/eeprom.h>

#define DIR1 A1
#define DIR2 A3


struct Setting{
    int calibration1;
    int calibration2;
    int flacon1_w; // время обратного отсоса
    int flacon2_w;  // время обратного отсоса
    int pump1_f_st1;
    int pump1_f_st2;
    int pump1_f_st3;
    int pump2_f_st1;
    int pump2_f_st2;
    int pump2_f_st3;
    int pump1_speed;
    int pump2_speed;
    int delay_auto1;
    int delay_auto2;
    int button_func;
    int correct1;
    int correct2;
};

Setting setting;

#include <Dosator.h>

#include <FastLED.h>

#define KEY1 A4
#define KEY2 A5
#define RESET 7 // 9

#endif
