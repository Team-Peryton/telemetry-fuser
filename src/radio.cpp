#include <Arduino.h>

void led(int b)
{
    digitalWrite(LED_BUILTIN, b);
}

/* Sends an addressed packet via the RADIO, returning whether the correct number of bytes were written */
bool sendLetter(String message, HardwareSerial& com)     // !!! this might not work
{
    byte bytesWritten = com.println(message);
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
unsigned short readAddress(String packet)
{
    String search = "recipient:";                          // tag to search for
    byte index = packet.indexOf(search) + search.length(); // start index of recipient
    return packet.substring(index, index + 1).toInt();     // + 1 addresses from 0 - 9
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