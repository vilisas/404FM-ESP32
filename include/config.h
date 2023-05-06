#ifndef _CONFIG_H__
#define _CONFIG_H__
/**
 * Default configuration.
*/
#include "config_local.h"

#ifndef RDS_ID
# define RDS_ID 0x420
#endif

#ifndef RDS_SATION
# define RDS_STATION "404 FM"
#endif

#ifndef RDS_BUFFER_DEFAULT
# define RDS_BUFFER_DEFAULT "Radio not found!"
#endif

#ifndef FM_STATION
# define FMSTATION 9170      // 9170 == 91.70 MHz  8800 - 10800 = 88 - 108 MHz
#endif

#ifndef TXPOWER
# define TXPOWER 115         // 88 - 115 dbuV
#endif

#define LED_PIN GPIO_NUM_2
#define PIN_SI4712_RESET GPIO_NUM_23
#define PIN_SI4712_SCL   GPIO_NUM_22
#define PIN_SI4712_SDA   GPIO_NUM_21

#ifndef WDT_TIMEOUT
// ESP32 will reset if watchdog is not kicked in this number of seconds.
# define WDT_TIMEOUT 4
#endif

#endif
