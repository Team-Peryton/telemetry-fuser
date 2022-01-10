#include <Arduino.h>

/* SD card libraries */
#include <SPI.h>
#include <SD.h>

/* Serial aliases */
#define RADIO Serial1
#define PILOT Serial2
#define VISOR Serial3

/* Define max message length to avoid using dynamic memory. (The MAVLink max packet size is 280 bytes) */
// #define MAX_LENGTH 320
// Only needed if we work with Chars

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
}

/* Sends an addressed packet via the RADIO, returning whether the correct number of bytes were written (WIP) */
bool sendLetter(String message)
{
    RADIO.println(message);
}

/*  Takes a packet, addresses it to the PILOT or the VISOR, formats it for radio transmission, then returns the formatted message  */
String addressTo(String packet, String recipient, unsigned long messageId)
{
    // return format {id:1,recipient:VISOR,message:Crash immediately}
    return String("{id:" + String(messageId) + ",recipient:" + recipient + ",message:" + packet + "}");
}

/*  Opens a message from the RADIO, returns the intended recipient  */
String readAddress(String packet)
{
    String search = "recipient:";  // tag to search for
    byte index = packet.indexOf(search) + search.length();    // start index of recipient
    return packet.substring(index, index + 5);    // + 5 because that is the length of the 3 possible recipients: RADIO, PILOT, VISOR
}

/*  Opens a message from the RADIO, returns the id of the message  */
unsigned long readID(String packet)
{
    String search = "id:";  // tag to search for
    byte startIndex = packet.indexOf(search) + search.length();    // start index of tag
    byte endIndex = packet.indexOf(",");    // index of first "," delimiter
    return packet.substring(startIndex, endIndex).toInt();    // the internet told me that .toInt() should work even though it's unsigned long
}

/*  Opens a message from the RADIO, returns the packet contained inside  */
String openLetter(String packet)
{
    String search = "message:";  // tag to search for
    byte startIndex = packet.indexOf(search) + search.length();    // start index of tag
    byte endIndex = packet.lastIndexOf("}");    // index of last "}" tag. Should work even if message contains "}" and if the packet  is appended with null characters
    return packet.substring(startIndex, endIndex).toInt();    // the internet told me that .toInt() should work even though it's unsigned long
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
    // [Turn on LED when there are packets in the buffer]
    // [Turn off LED when the packet buffer has been emptied]

    
        /* The idea of the ID is that if either end receives a mesage with an ID which doesn't match their counter,
    they can deduce that message(s) have been missed, and they can then resynchronise. 
    Static should allow it to retain its value between loops */
    static unsigned long int id = 0; // Message ID counter


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