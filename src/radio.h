#include <Arduino.h>

#define ARDUPILOT 0
#define COCOMPUTER 1

String addressTo(String packet, String recipient, unsigned long messageId);

unsigned short readAddress(String packet);