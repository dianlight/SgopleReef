/**
 * @file pinconfig.h
 * @author Lucio Tarantino 
 * @brief  C macro definition for PIN connection
 * @version 0.1
 * @date 2020-03-17
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifdef ESP32
    // Buttons
    #define BUTTON1     18
    #define BUTTON2     19
    #define BUTTON3     5

    // Flussometro
    #define FLUX        4

    // Relay Pump
    #define  PUMP       15

    // Level Sensor
    #define LEVEL       27


    //PT100
    #define THERM1      A6 // 34
    #define THERM2      A7 // 35
    #define THERM3      A4 // 33
    #define VREF        GPIO_NUM_25 // Valid are 25/26/27

    // Relay Peltier
    #define PELTIER     12
#elif ESP8266
    // Buttons
    #define BUTTON1     D0 // no work
    #define BUTTON2     D5
    #define BUTTON3     D6

    // Flussometro
    #define FLUX        D7

    // Relay Pump
    #define  PUMP       D3

    // Level Sensor
    #define LEVEL      D8 // No program


    //PT100
//    #define THERM1      35
//   #define THERM2      32
    #define THERM3      A0
//    #define VREF        GPIO_NUM_25

    // Relay Peltier
    #define PELTIER     D4 // No boot
#endif


