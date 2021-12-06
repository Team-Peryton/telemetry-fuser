#include <Arduino.h>

/*  Serial aliases  */
#define RADIO   Serial1 
#define PILOT   Serial2
#define VISOR   Serial3


void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    
    RADIO.begin(57600);       // initialise Serial1 : Connection to radio
    PILOT.begin(57600);       // initialise Serial2 : Connection to Ardupilot / Mission Planner 
    VISOR.begin(57600);       // initialise Serial3 : Connection to Autovisor / Co-computer

    /*  Setup SD card logging here */

}

void loop() {
    // digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    // digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW


    /*  Core forwarding loop: 
    Loop

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

    While True
    */
}

void radioIn() {

}

void radioOut() {

    RADIO.flush();
}

void pilotIn() {

}

void pilotOut() {

    PILOT.flush();
}

void visorIn() {

}

void visorOut() {

    VISOR.flush();
}