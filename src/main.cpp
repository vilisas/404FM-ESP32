#include <Arduino.h>
#include "config.h"
#include "utils.h"
#include <esp_task_wdt.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Si4713.h>

#ifdef USE_WIFI
#include "wifi_utils.h"
#endif

// #define _BV(n) (1 << n)

#define RESETPIN PIN_SI4712_RESET

// TwoWire I2C_0 = TwoWire(0);
Adafruit_Si4713 radio = Adafruit_Si4713(RESETPIN);
int counter = 1;

RTC_DATA_ATTR int reboot_counter = 0;

void print_tune_status()
{
  // This will tell you the status in case you want to read it from the chip
  radio.readTuneStatus();
  Serial.print("\tCurr freq: ");
  Serial.println(radio.currFreq);
  Serial.print("\tCurr dBuV:");
  Serial.println(radio.currdBuV);
  Serial.print("\tCurr ANTcap:");
  Serial.println(radio.currAntCap);
}

void setup()
{
  esp_task_wdt_init(WDT_TIMEOUT, true); // enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);               // add current thread to WDT watch
  pinMode(RESETPIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(RESETPIN, 0);

  Serial.begin(115200);
  Serial.print("Reboots: ");
  Serial.println(reboot_counter);
  reboot_counter++;

  Wire.begin(PIN_SI4712_SDA, PIN_SI4712_SCL, 100000);
  digitalWrite(RESETPIN, 1);
  delay(50);

  Serial.println("Adafruit Radio - Si4713 Test");
  if (!radio.begin())
  { // begin with address 0x63 (CS high default)
    Serial.println("Couldn't find radio?");
    digitalWrite(RESETPIN, 0);
    delay(1000);
    ESP.restart();
    while (1)
      ;
  }

  // radio.reset();
  // radio.powerUp();

  // Power Up command takes 110 ms (SiLabs Programming guide AN332.pdf, p. 245).
  // Setting 300 to be on safe side.
  // Without this si4713 responds to commands, but transmitter is OFF
  delay(300);

  radio.setProperty(SI4713_PROP_TX_PREEMPHASIS, 1);  // 0=74uS pre-emph (USA std), 1=50 uS pre-emph (EU)
  Serial.print("\nSet TX power");
  radio.setTXpower(TXPOWER); // dBuV, 88-115 max

  Serial.print("\nTuning into ");
  Serial.print(FMSTATION / 100);
  Serial.print('.');
  Serial.println(FMSTATION % 100);
  radio.tuneFM(FMSTATION); // 91.7 mhz

  print_tune_status();

  // begin the RDS/RDBS transmission
  radio.beginRDS(RDS_ID);
  radio.setRDSstation(RDS_STATION);
  radio.setRDSbuffer(RDS_BUFFER_DEFAULT);

  Serial.println("RDS on!");

  radio.setGPIOctrl(_BV(1) | _BV(2));  // set GP1 and GP2 to output
  radio.setGPIO(0); // drive GPIO pins LOW

  delay(100);
#ifdef USE_WIFI
  setup_wifi();
#endif
}

void loop()
{
  esp_task_wdt_reset();
  counter++;

  switch (counter % 16)
  {
  case 1:
    radio.setRDSbuffer(RDS_BUFFER_DEFAULT);
    break;
  case 10:
    radio.setRDSbuffer(" All your base  ");
    break;
  case 13:
    radio.setRDSbuffer("   belong us    ");
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

#ifdef USE_WIFI
  web_client_loop();
#endif

  // toggle GPO1 and GPO2
  // radio.setGPIO(_BV(1));
  // delay(500);
  // radio.setGPIO(_BV(2));
  // delay(500);
  blink();
  delay(980);
}