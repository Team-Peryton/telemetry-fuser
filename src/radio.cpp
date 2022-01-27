#include <Arduino.h>


/*  Takes a packet, addresses it to the PILOT or the VISOR, formats it for radio transmission, then returns the formatted message  */
String addressTo(String packet, unsigned short recipient, unsigned short messageId)
{
    // return format {id:1,recipient:VISOR,message:Crash immediately} (should in theory automatically be appended with null character '\0')
    return String("{sender:" + String(messageId) + ",recipient:" + String(recipient) + ",message:" + packet + "}");
}

/*  Opens a message, returns the intended recipient  */
unsigned short readAddress(String packet)
{
    // Distinguish between an Ardupilot message and a custom one

}

/*  Opens a message, returns the packet contained inside, only works for non ardupilot messages  */
String openLetter(String packet)
{

}