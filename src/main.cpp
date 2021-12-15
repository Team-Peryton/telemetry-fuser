#include <Arduino.h>

/* SD card libraries */
#include <SPI.h>
#include <SD.h>

/* Serial aliases */
#define RADIO Serial1
#define PILOT Serial2
#define VISOR Serial3

/* Define max message length to avoid using dynamic memory. (The MAVLink max packet size is 280 bytes) */
#define MAX_LENGTH 320

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

/*  Takes a packet, addresses it to the PILOT or the VISOR, formats it for radio transmission, then returns the formatted message  */
void addressTo(char packet[MAX_LENGTH], char recipient[5], unsigned long int messageId)
{
    // return format {id:1,recipient:VISOR,message:Crash immediately}
    // packet = ("{id:" + messageId + ",recipient:" + recipient + ",message:" + packet + "}");
    packet = strcat(packet,recipient);
}

/* Sends a packet via the RADIO, returning whether the correct number of bytes were written (WIP) */
bool sendLetter(char packet[MAX_LENGTH], char recipient[5], unsigned long int messageID)
{
    addressTo(packet, recipient, messageID);
    RADIO.println(packet);
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


// loop() goes at the end to avoid compile errors
void loop()
{

    /* The idea of the ID is that if either end receives a mesage with an ID which doesn't match their counter,
    they can deduce that message(s) have been missed, and they can then resynchronise. 
    Static should allow it to retain its value between loops */
    static unsigned long int id = 0; // Message ID counter

    static char currentmessage[MAX_LENGTH]; // Message variable to be pointed at by functions
    static char recipient[5]; // Recipient: either "PILOT" or "VISOR"


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

    /* on receiving letter: 
    if letter ID < current ID, packets were missed at the other end
    if letter ID = current ID, no packets were missed, or an equal number of packets were missed
    if letter ID > current ID, packets were missed at this end */


    // sendLetter();
    sendLetter(currentmessage, recipient, id);
    id++; // Increments ID after sending letter

}