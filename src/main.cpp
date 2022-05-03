#include <Arduino.h>
#include "config.h"
#include "utils.h"
#include <esp_task_wdt.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Si4713.h>


#define _BV(n) (1 << n)
#define RESETPIN PIN_SI4712_RESET



// TwoWire I2C_0 = TwoWire(0);
Adafruit_Si4713 radio = Adafruit_Si4713(RESETPIN);
int counter = 1;

void setup() {

  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch

  pinMode(LED_PIN, OUTPUT);
  pinMode(RESETPIN, OUTPUT);
  // reset si4713 module
  
  digitalWrite(RESETPIN, 0);
  delay(100);
  digitalWrite(RESETPIN, 1);
  delay(50);

  Wire.begin(PIN_SI4712_SDA, PIN_SI4712_SCL, 100000);
  Serial.begin(115200);


    Serial.println("Adafruit Radio - Si4713 Test");
  if (! radio.begin()) {  // begin with address 0x63 (CS high default)
    Serial.println("Couldn't find radio?");
    digitalWrite(RESETPIN, 0);
    // delay(1000);
    // ESP.restart();
    while (1);
  }
  Serial.print("\nSet TX power");
  radio.setTXpower(TXPOWER);  // dBuV, 88-115 max

  Serial.print("\nTuning into "); 
  Serial.print(FMSTATION/100); 
  Serial.print('.'); 
  Serial.println(FMSTATION % 100);
  radio.tuneFM(FMSTATION); // 102.3 mhz

  // This will tell you the status in case you want to read it from the chip
  radio.readTuneStatus();
  Serial.print("\tCurr freq: "); 
  Serial.println(radio.currFreq);
  Serial.print("\tCurr freqdBuV:"); 
  Serial.println(radio.currdBuV);
  Serial.print("\tCurr ANTcap:"); 
  Serial.println(radio.currAntCap);

  // begin the RDS/RDBS transmission
  radio.beginRDS(0x420);
  radio.setRDSstation("404 FM");
  radio.setRDSbuffer( "Radio not found!");

  Serial.println("RDS on!");  

  // radio.setGPIOctrl(_BV(1) | _BV(2));  // set GP1 and GP2 to output
  pinMode(LED_PIN, OUTPUT);
  Serial.println("Setup finished!");

}


void loop() {
  esp_task_wdt_reset();
  counter++;

  switch (counter % 11)
  {
  case 1:
    radio.setRDSbuffer("Radio not found!");
    break;
  case 5:
    radio.setRDSbuffer("Send            ");
    break;    
  case 7:
    radio.setRDSbuffer("nudes!          ");
    break;

  default:
    break;
  }
 
  // put your main code here, to run repeatedly:
  radio.readASQ();
  Serial.print("\tASQ: 0x"); 
  Serial.print(radio.currASQ, HEX);
  Serial.print("\tInLevel:"); 
  Serial.println(radio.currInLevel);
  
  // toggle GPO1 and GPO2
  // radio.setGPIO(_BV(1));
  // delay(500);
  // radio.setGPIO(_BV(2));
  // delay(500);
  blink();
  delay(980);
}