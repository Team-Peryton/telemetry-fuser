#include <Arduino.h>

#define ARDUPILOT 0
#define COCOMPUTER 1

void led(int b);

bool sendLetter(String message, HardwareSerial& com);

String addressToVISOR(String packet, unsigned long messageId);

bool isPilotPacket(String packet);

unsigned long readID(String packet);

String readMessage(String packet);

void radioSort();

void pilotSort();

void visorSort();