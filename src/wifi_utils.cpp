#include "Arduino.h"
#include "config.h"
#include <esp_task_wdt.h>
#include "WiFi.h"

IPAddress ip;
WiFiServer webServer(80);


void setup_wifi(){

#ifdef USE_WIFI
  esp_task_wdt_reset();
# ifdef USE_WIFI_AP
  Serial.print("Starting AP with SSID: " WIFI_SSID);
  WiFi.softAP(WIFI_SSID, WIFI_KEY);
  ip = WiFi.softAPIP();
# else
  Serial.print("Connecting to: " WIFI_SSID " ");
  WiFi.begin(WIFI_SSID, WIFI_KEY);
  while (WiFi.status() != WL_CONNECTED) {
    esp_task_wdt_reset();
    delay(500);
    Serial.print(".");
  }
  ip = WiFi.localIP();
# endif
  Serial.println("DONE");

  Serial.println("Local IP: " + WiFi.localIP());
  webServer.begin();
#endif
}

void web_client_loop(){

    
}
