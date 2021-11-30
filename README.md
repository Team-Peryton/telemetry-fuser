# telemetry-fuser

## Aim

Combine datastreams from two serial devices into one, using MAVLINK packets. Specifically enable ardupilot and the co-computer communication to a ground station over one radio link.

# Notes
- should not degrade radio link performance (much)
    - need to be able to prioritise ardupilot over the co-computer
    - can probably send normal MAVLINK packets as is and encode the co-computer packets and then pick these out at each end
- runs on a Teensy 4.1
- should log errors / stream to an SD card
