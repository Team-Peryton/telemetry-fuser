#include <Arduino.h>

void led(int b)
{
    digitalWrite(LED_BUILTIN, b);
}

/* Sends an addressed packet via the RADIO, returning whether the correct number of bytes were written */
bool sendLetter(String message, HardwareSerial &com) // !!! this might not work
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
String addressToVISOR(String packet, unsigned long messageId)
{
    // return format "%VISOR,id:1,message:Crash immediately"
    return String("%VISOR,id:" + String(messageId) + ",message:" + packet);
}

/*  Opens a message from the RADIO, returns whether it is intended for the PILOT  */
bool isPilotPacket(String packet)
{
    byte index = packet.indexOf("%VISOR"); // start index of %VISOR tag
    return (index == -1);                  // %VISOR tag not present: PILOT packet
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

/* 0th item is oldest, remove it and add the newMessage as last array item */
void replaceQueueOldest(String arr[5], String newMessage){
    for(int i = 0; i < 3; i++){
        arr[i] = arr[i+1];
    }
    arr[4] = newMessage;
}

int intRotate(int current, int limit){
    if(current++ > limit){
        return 0;
    }
    else{
        return current++;
    }
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

