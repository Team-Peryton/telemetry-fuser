#include <Arduino.h>

String Timestamp(String message) {
    // message = floor(millis()/60000) + ":" + millis()/1000 + "     " + message; << also doesn't work, but the idea's there
    return String(message);
}