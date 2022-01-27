#include <Arduino.h>
#include "radio.h"

/*  Serial */
#define RADIO Serial1
#define PILOT Serial2
#define VISOR Serial3

/* SD card relevant */
#include <SD.h>
#include <SPI.h>
File logger;
const int chipSelect = BUILTIN_SDCARD; 

/* loop working variables */
#define CYCLETIME 10 // aim for X ms per cycle
unsigned short id;
uint32_t start;
uint32_t time_left;
String sReceivedPacket;
String sForwardPacket;




void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    /* SD card begin */
    if (!SD.begin(chipSelect))
    { /*  ==Fast blink 10 times if the SD card fails to initialise==   */
        for (int i = 0; i <= 10; i++)
        {
            digitalWrite(LED_BUILTIN, HIGH); delay(200);
            digitalWrite(LED_BUILTIN, LOW); delay(100);
        }
    }
    else {
        Serial.println("SD card initialised");
    }


    /* Serial ports begin */
    RADIO.begin(57600); // initialise Serial1 : Connection to radio
    PILOT.begin(57600); // initialise Serial2 : Connection to Ardupilot / Mission Planner
    VISOR.begin(57600); // initialise Serial3 : Connection to Autovisor / ground station

    while (!RADIO || !PILOT || !VISOR)
    {
        digitalWrite(LED_BUILTIN, HIGH); delay(1000);
        digitalWrite(LED_BUILTIN, LOW); delay(500);
    }

    id = 0; // Resets packet id counter
    sReceivedPacket.reserve(300); // reserving memory for strings increases speed
    sForwardPacket.reserve(300);
}




void loop()
{
    start = millis();

    sReceivedPacket = RADIO.readStringUntil('\n');

    id = readAddress(sReceivedPacket); // determine packet source

    switch (id)
    {
        case ARDUPILOT:
            // forward to pilot serial
            for(unsigned int i=0; i < sReceivedPacket.length(); i++){
                RADIO.write(sReceivedPacket[i]);
            }
            break;
        
        case COCOMPUTER:
            // store, cut oldest packet if store too large
            break;
        
        default:
            break;
    }

    // must be a better way of doing this lol
    sReceivedPacket = PILOT.readStringUntil('\n');
    for(unsigned int i=0; i < sReceivedPacket.length(); i++){
        RADIO.write(sReceivedPacket[i]);
    }


    sReceivedPacket = VISOR.readStringUntil('\n');
    // Store


    time_left = start - millis();
    // need a way to evenly distribute tasks
    /*
        IF TIME
            wrap VISOR packet and store
            if store too large, cut oldest packet
        IF TIME
            forward a new VISOR packet to radio
        IF TIME
            forward a stored VISOR packet to co-computer serial

    */

}