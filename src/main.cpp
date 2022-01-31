#include <Arduino.h>
#include "radio.h"

/* static parameters */
#define DEBUG 1
#define CYCLETIME 5 // aim for X ms per cycle
#define PACKET_RESERVE 200
#define QUEUE_LENGTH 5


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
unsigned short id;
uint32_t start;
uint32_t time_left;
String sReceivedPacket;
String sForwardPacket;
String radio_queue[QUEUE_LENGTH];
String visor_queue[QUEUE_LENGTH];
String wrapped_message;
int visor_task = 0;

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

            digitalWrite(LED_BUILTIN, HIGH);
            delay(200);
            digitalWrite(LED_BUILTIN, LOW);
            delay(100);
        }
    }
    else
    {
        Serial.println("SD card initialised");
    }

    /* open the file */

    /* Serial ports begin */
    RADIO.begin(57600); // initialise Serial1 : Connection to radio
    PILOT.begin(57600); // initialise Serial2 : Connection to Ardupilot / Mission Planner
    VISOR.begin(57600); // initialise Serial3 : Connection to Autovisor / ground station

    while (!RADIO || !PILOT || !VISOR)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(1000);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
    }

    sReceivedPacket.reserve(PACKET_RESERVE); // reserving memory for strings supposedly increases speed
    sForwardPacket.reserve(PACKET_RESERVE);
    wrapped_message.reserve(PACKET_RESERVE);
    for(int i=0; i < QUEUE_LENGTH; i++){
        radio_queue[i] = String().reserve(PACKET_RESERVE);
    }
    for(int i=0; i < QUEUE_LENGTH; i++){
        visor_queue[i] = String().reserve(PACKET_RESERVE);
    }
}

// loop() goes at the end to avoid compile errors
void loop()
{
    start = millis();

    /* The idea of the ID is that if either end receives a mesage with an ID which doesn't match their counter,
    they can deduce that message(s) have been missed, and they can then resynchronise. 
    Static should allow it to retain its value between loops */
    static unsigned long int id = 0;       // Message ID counter

    if (RADIO.available())
    {
        sReceivedPacket = RADIO.readStringUntil('\n');

        if(isPilotPacket(sReceivedPacket)){
            // forward to pilot serial
            for (unsigned int i = 0; i < sReceivedPacket.length(); i++){
                PILOT.write(sReceivedPacket[i]);
            }
        }
        else{
            replaceQueueOldest(radio_queue, sReceivedPacket);
        }
    }

    // immediately forward anything from the autopilot to the radio
    if (PILOT.available()){
        sReceivedPacket = PILOT.readStringUntil('\n');
        RADIO.println(sReceivedPacket);
        return; // restart the loop
    }

        // read from the RADIO if something is there
    if (RADIO.available()){
        return; // restart the loop
    }

    if (VISOR.available()){
        sReceivedPacket = VISOR.readStringUntil('\n');
        replaceQueueOldest(visor_queue, sReceivedPacket);
    }


    // this could maybe be "if(!RADIO.availible())"
    if (start - millis() < CYCLETIME) {
        visor_task = intRotate(visor_task, 2);
        switch(visor_task){
            case 0:
                // wrap VISOR packet
                wrapped_message = addressToVISOR(visor_queue[0], id);
            case 1:
                // forward a wrapped packet to the radio
                RADIO.println(wrapped_message);
            case 2:
                // forward a stored radio packet to visor
                VISOR.println(radio_queue[0]);
            default:
                // log error
                break;
        }
    }

    return;
}