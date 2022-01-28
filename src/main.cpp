#include <Arduino.h>
#include "radio.h"
#define DEBUG 1

/* SD card libraries */
#include <SD.h>
#include <SPI.h>
File logger;
const int chipSelect = BUILTIN_SDCARD; 

/* Serial aliases */
#define RADIO Serial1
#define PILOT Serial2
#define VISOR Serial3

/* loop working variables */
#define CYCLETIME 10 // aim for X ms per cycle
unsigned short id;
uint32_t start;
uint32_t time_left;
String sReceivedPacket;
String sForwardPacket;

void setup()
{
    Serial.begin(9600);

    pinMode(LED_BUILTIN, OUTPUT);

    /* SD card begin */
    if (!SD.begin(chipSelect))
    { /*  ==Fast blink 10 times if the SD card fails to initialise==   */
    
        Serial.println("SD card failed to initialise");
        
        for (int i = 0; i <= 10; i++)
        {

            digitalWrite(LED_BUILTIN, HIGH); delay(200);
            digitalWrite(LED_BUILTIN, LOW); delay(100);
        }
    }
    else {
        Serial.println("SD card initialised");
    }
  
    /* open the file */
    

    /* Serial ports begin */
    RADIO.begin(57600); // initialise Serial1 : Connection to radio
    PILOT.begin(57600); // initialise Serial2 : Connection to Ardupilot / Mission Planner
    VISOR.begin(57600); // initialise Serial3 : Connection to Autovisor / ground station

    while (!RADIO || !PILOT || !VISOR)
    {
        digitalWrite(LED_BUILTIN, HIGH); delay(1000);
        digitalWrite(LED_BUILTIN, LOW); delay(500);
    }

    sReceivedPacket.reserve(300); // reserving memory for strings supposedly increases speed
    sForwardPacket.reserve(300);
}


// loop() goes at the end to avoid compile errors
void loop()
{  
    start = millis();

    /* The idea of the ID is that if either end receives a mesage with an ID which doesn't match their counter,
    they can deduce that message(s) have been missed, and they can then resynchronise. 
    Static should allow it to retain its value between loops */
    static unsigned long int id = 0; // Message ID counter
    static unsigned short int address = 0; // Message ID counter

    if(RADIO.available()){
        sReceivedPacket = RADIO.readStringUntil('\n');
        address = readAddress(sReceivedPacket); // determine packet source

        switch (address)
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
    }

    // immediately forward anything from the autopilot to the radio
    sReceivedPacket = PILOT.readStringUntil('\n');
    RADIO.println(sReceivedPacket);

    if(RADIO.available()){
        return; // restart the loop
    }
    
    sReceivedPacket = VISOR.readStringUntil('\n');
    // Store

    time_left = start - millis();
}