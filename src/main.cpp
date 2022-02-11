#include <Arduino.h>
#include <cppQueue.h>

#include <radio.h>
#include <debugging.h>


/* static parameters */
#define DEBUGGING 1
#define CYCLETIME 5        // aim for X ms per cycle
#define PACKET_RESERVE 300 // MAVLink 2 packet is supposedly max 279 bytes over-the-wire
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

#if DEBUGGING   
    #define DEBUGGER Serial
#endif

/* loop working variables */
uint32_t start;
String sReceivedPacket;
String sForwardPacket;

cppQueue radio_queue(PACKET_RESERVE, 10, FIFO, true);
cppQueue visor_queue(PACKET_RESERVE, 10, FIFO, true);
String sWrappedMessage;
int visor_task = 0;

void setup()
{
    #if DEBUGGING   
    DEBUGGER.begin(9600);
    #define DEBUG Serial.println
    #endif
    

    pinMode(LED_BUILTIN, OUTPUT);

    /* SD card begin */
    if (!SD.begin(chipSelect))
    { /*  ==Fast blink 10 times if the SD card fails to initialise==   */

        #if DEBUGGING
        DEBUG("! SD card failed to initialise");
        #endif

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
        #if (DEBUGGING)
        DEBUG("> SD card initialised");
        #endif

        digitalWrite(LED_BUILTIN, HIGH); // LED stays on until file is opened successfully
        const char *filename = "Log 1.txt";
        for (size_t index = 0; SD.exists(filename); index++) // Until file does not already exist
        {
            // filename = "Log " + index + ".txt";          << Doesn't work, but something like it
        }

        SD.open(filename, FILE_WRITE);
        if (logger)
        {
            // Add something to check the file was opened properly? 
        } else {
            #if DEBUGGING
            DEBUG("! File failed to open.");
            #endif
        }
        
        digitalWrite(LED_BUILTIN, LOW);
    }

    /* Serial ports begin */
    RADIO.begin(57600); // initialise Serial1 : Connection to radio
    PILOT.begin(57600); // initialise Serial2 : Connection to Ardupilot / Mission Planner
    VISOR.begin(57600); // initialise Serial3 : Connection to Autovisor / Ground station

    while (!RADIO || !PILOT || !VISOR) // Long blink until Serial ports open
    {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(1000);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
    }

    // reserving memory for strings supposedly increases speed
    sReceivedPacket.reserve(PACKET_RESERVE);
    sForwardPacket.reserve(PACKET_RESERVE);
    sWrappedMessage.reserve(PACKET_RESERVE);

}


void loop()
{
    start = millis();

    if (RADIO.available())
    {
        sReceivedPacket = RADIO.readStringUntil('\n');

        if (isPilotPacket(sReceivedPacket))
        {
            // forward to pilot serial
            for (unsigned int i = 0; i < sReceivedPacket.length(); i++)
            {
                PILOT.write(sReceivedPacket[i]);
            }
        }
        else
        {
            radio_queue.push(&sReceivedPacket);
        }
    }

    // immediately forward anything from the autopilot to the radio
    if (PILOT.available())
    {
        sReceivedPacket = PILOT.readStringUntil('\n');

        if (!sendLetter(sReceivedPacket, RADIO)) {
            #if DEBUGGING 
            DEBUG("! Incomplete packet sent.");
            #endif
            logger.println(Timestamp("Incomplete packet sent."));
        }
        
        return; // restart the loop
    }

    // read from the RADIO if something is there
    if (RADIO.available())
    {
        return; // restart the loop
    }

    if (VISOR.available())
    {
        sReceivedPacket = VISOR.readStringUntil('\n');
        visor_queue.push(&sReceivedPacket);
    }

    // this could maybe be "if(!RADIO.availible())"
    if (start - millis() < CYCLETIME)
    {
        visor_task = intRotate(visor_task, 2);
        switch (visor_task)
        {
            case 0:
                // wrap VISOR packet
                visor_queue.pop(&sForwardPacket);
                sWrappedMessage = addressToVISOR(sForwardPacket);
            case 1:
                // forward a wrapped packet to the radio
                RADIO.println(sWrappedMessage);
            case 2:
                // forward a stored radio packet to visor
                radio_queue.pop(&sForwardPacket);
                VISOR.println(sForwardPacket);
            default:
                // log error
                break;
        }
    }

    return;
}