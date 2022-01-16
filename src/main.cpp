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
// Only needed if working with Chars

void led(int b)
{
    digitalWrite(LED_BUILTIN, b);
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);

    /* SD card begin */
    if (!SD.begin())
    { /*  ==Fast blink 10 times if the SD card fails to initialise==   */
        for (int i = 0; i <= 10; i++)
        {
            led(HIGH); // turn the LED on (HIGH is the voltage level)
            delay(200);
            led(LOW); // turn the LED off by making the voltage LOW
            delay(100);
        }
    }

    /* Serial ports begin */
    RADIO.begin(57600); // initialise Serial1 : Connection to radio
    PILOT.begin(57600); // initialise Serial2 : Connection to Ardupilot / Mission Planner
    VISOR.begin(57600); // initialise Serial3 : Connection to Autovisor / Co-computer
}

/* Sends an addressed packet via the RADIO, returning whether the correct number of bytes were written */
bool sendLetter(String message)
{
    byte bytesWritten = RADIO.println(message);
    if (bytesWritten == message.length())
    {
        return true;
    }
    else
    {
        return false;
    }
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
    String search = "recipient:";                          // tag to search for
    byte index = packet.indexOf(search) + search.length(); // start index of recipient
    return packet.substring(index, index + 5);             // + 5 because that is the length of the 3 possible recipients: RADIO, PILOT, VISOR
}

/*  Opens a message from the RADIO, returns the id of the message  */
unsigned long readID(String packet)
{
    String search = "id:";                                      // tag to search for
    byte startIndex = packet.indexOf(search) + search.length(); // start index of tag
    byte endIndex = packet.indexOf(",");                        // index of first "," delimiter
    return packet.substring(startIndex, endIndex).toInt();      // the internet told me that .toInt() should work even though it's unsigned long
}

/*  Opens a message from the RADIO, returns the packet contained inside  */
String readMessage(String packet)
{
    String search = "message:";                                 // tag to search for
    byte startIndex = packet.indexOf(search) + search.length(); // start index of tag
    byte endIndex = packet.lastIndexOf("}");                    // index of last "}" tag. Should work even if message contains "}" and if the packet is appended with null characters
    return packet.substring(startIndex, endIndex);
}

void radioSort()
{
}
void pilotSort()
{
}
void visorSort()
{
}

// loop() goes at the end to avoid compile errors
void loop()
{

    while (!RADIO || !PILOT || !VISOR)
    {
        /* Slow flash while waiting for serial ports to initialise */
        led(HIGH); // turn the LED on (HIGH is the voltage level)
        delay(600);
        led(LOW); // turn the LED off by making the voltage LOW
        delay(600);
    }

    /* The idea of the ID is that if either end receives a mesage with an ID which doesn't match their counter,
    they can deduce that message(s) have been missed, and they can then resynchronise. 
    Static should allow it to retain its value between loops */
    static unsigned long int id = 0; // Message ID counter

    bool priority = false; // Does the Autopilot take priority over the co-computer for this loop?

    String sReceivedPacket;
    String sForwardPacket;

    sReceivedPacket.reserve(300); // reserving memory for strings supposedly increases speed
    sForwardPacket.reserve(300);

    // Turn on LED when there are packets in the buffer
    // Turn off LED when the packet buffer has been emptied

    // Forwards from Autopilot / Mission Planner
    if (PILOT.available() > 0)
    {
        led(HIGH);
        priority = true;

        pilotSort();
    }

    if (RADIO.available() > 0)
    {
        led(HIGH);
        priority = true;

        radioSort();
    }

    // Forwards to/from the Autovisor, but only if there were no packets to/from the Autopilot
    if (priority == false && VISOR.available() > 0)
    {
        led(HIGH);

        visorSort();
    }
    if (priority == false && VISOR.available() == 0)
    {
        led(LOW);
    }

    /*  Core forwarding loop ultimate goal (not quite there yet): 

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
}