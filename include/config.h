#ifndef _CONFIG_H__


#define RDS_ID 0x420
#define RDS_STATION "404 FM"
#define RDS_BUFFER_DEFAULT "Radio not found!"


#define FMSTATION 9170      // 9170 == 91.70 MHz  8800 - 10800 = 88 - 108 MHz 
#define TXPOWER 115         // 88 - 115 dbuV


#define LED_PIN GPIO_NUM_2
#define PIN_SI4712_RESET GPIO_NUM_23
#define PIN_SI4712_SCL   GPIO_NUM_22
#define PIN_SI4712_SDA   GPIO_NUM_21

#define WDT_TIMEOUT 4


#endif