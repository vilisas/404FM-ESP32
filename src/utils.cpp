#include <Arduino.h>
#include "config.h"

void blink(int time){
    digitalWrite(LED_PIN, HIGH);
    delay(time);
    digitalWrite(LED_PIN, LOW);
    delay(time);
}