#include <Arduino.h>

// SD card libraries
#include <SPI.h>
#include <SD.h>

/*  Serial aliases  */
#define RADIO Serial1
#define PILOT Serial2
#define VISOR Serial3

unsigned long id;

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    /* SD card begin */
    if (!SD.begin())
    { /*  ==Fast blink 10 times if the SD card fails to initialise==   */
        for (int i = 0; i <= 10; i++)
        {
            digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
            delay(200);
            digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
            delay(100);
        }
    }

    /* Serial ports begin */
    RADIO.begin(57600); // initialise Serial1 : Connection to radio
    PILOT.begin(57600); // initialise Serial2 : Connection to Ardupilot / Mission Planner
    VISOR.begin(57600); // initialise Serial3 : Connection to Autovisor / Co-computer

    while (!RADIO || !PILOT || !VISOR)
    {
        /* Slow flash while waiting for serial ports to initialise */
        digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
        delay(600);
        digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
        delay(600);
    }

    id = 0; // Resets packet id counter
}

/* Sends an addressed packet via the RADIO, returning whether the correct number of bytes were written (WIP) */
bool sendLetter(String message)
{
    RADIO.println(message);
}

/*  Takes a packet, addresses it to the PILOT or the VISOR, formats it for radio transmission, then returns the formatted message  */
String addressTo(String packet, String recipient, unsigned long messageId)
{
    // return format {id:1,recipient:VISOR,message:Crash immediately} (should in theory automatically be appended with null character '\0')
    return String("{id:" + String(messageId) + ",recipient:" + recipient + ",message:" + packet + "}");
}

/*  Opens a message from the RADIO, returns the intended recipient  */
String readAddress(String packet)
{
    
}

/*  Opens a message from the RADIO, returns the id of the message  */
unsigned long readID(String packet)
{
}

/*  Opens a message from the RADIO, returns the packet contained inside  */
String openLetter(String packet)
{
}

void radioIn()
{
}

void radioOut()
{

    RADIO.flush();
}

void pilotIn()
{
}

void pilotOut()
{

    PILOT.flush();
}

void visorIn()
{
}

void visorOut()
{

    VISOR.flush();
}

// loop() goes at the end to avoid compile errors
void loop()
{
    String sReceivedPacket;
    String sForwardPacket;

    sReceivedPacket.reserve(300); // reserving memory for strings increases speed
    sForwardPacket.reserve(300);

    /*  Core forwarding loop: 

    Do While "packet from PILOT" OR "packet from RADIO marked as PILOT" 
        If packet from PILOT 
            Forward to RADIO 
        If packet from RADIO marked as PILOT
            Forward to PILOT
    Loop

    If packet from VISOR
        Forward to RADIO
    If packet from RADIO marked as VISOR
        Forward to VISOR

    */

    char sPayloadExample[] = "This is a test message which is just longer than 64 bytes, for science";

    sReceivedPacket = sPayloadExample;

    sForwardPacket = addressTo(sReceivedPacket, "VISOR", id);
}