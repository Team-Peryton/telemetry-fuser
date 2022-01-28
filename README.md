# telemetry-fuser

## Aim

Combine datastreams from two serial devices into one, using MAVLINK packets. Specifically enable ardupilot and the co-computer communication to a ground station over one radio link.

# Notes
- should not degrade radio link performance (much)
    - need to be able to prioritise ardupilot over the co-computer
    - can probably send normal MAVLINK packets as is and encode the co-computer packets and then pick these out at each end
- runs on a Teensy 4.1
- should log errors / stream to an SD card



```
RADIO recieve

    determine packet source

    if PILOT
        forward to pilot serial
    if VISOR
        store
        if store too large, cut oldest packet

PILOT receive
    forward to radio

check if any RADIO data availible

VISOR recieve
    store

IF TIME
    wrap VISOR packet and store
    if store too large, cut oldest packet
IF TIME
    forward a new VISOR packet to radio
IF TIME
    forward a stored VISOR packet to co-computer serial
```

