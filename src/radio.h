#include <Arduino.h>

bool sendLetter(String message, HardwareSerial& com);

String addressToVISOR(String packet, unsigned long messageId);

bool isPilotPacket(String packet);

unsigned long readID(String packet);

String readMessage(String packet);

void replaceQueueOldest(String arr[5], String newMessage);

int intRotate(int current, int limit);

void radioSort();

void pilotSort();

void visorSort();