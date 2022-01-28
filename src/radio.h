#include <Arduino.h>

#define ARDUPILOT 0
#define COCOMPUTER 1

void led(int b);

bool sendLetter(String message, HardwareSerial& com);

String addressTo(String packet, String recipient, unsigned long messageId);

unsigned short readAddress(String packet);

unsigned long readID(String packet);

String readMessage(String packet);

void radioSort();

void pilotSort();

void visorSort();