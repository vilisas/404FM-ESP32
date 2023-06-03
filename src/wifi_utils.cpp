#include "Arduino.h"
#include "config.h"
#include <esp_task_wdt.h>
#include "WiFi.h"
#include <Adafruit_Si4713.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

extern Adafruit_Si4713 radio;

IPAddress ip;
AsyncWebServer webserver(80);
DynamicJsonDocument status(1024);
extern RTC_DATA_ATTR int reboot_counter;

extern String station_rds_name;
extern String station_rds_buffer;
extern uint16_t station_rds_id;


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script>
    function submitMessage() {
      alert("Saved value to ESP SPIFFS");
      setTimeout(function(){ document.location.reload(false); }, 500);
    }
  </script></head><body>
  <form action="/set" target="hidden-form">
    RDS Name: <input type="text" name="rdsName" value="%rdsName%">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form><br>
  <form action="/set" target="hidden-form">
    RDS Text: <input type="text" name="rdsText" value="%rdsText%">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form><br>
  <form action="/set" target="hidden-form">
    RDS ID: <input type="number" name="rdsID" value="%rdsID%">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form>
  <iframe style="display:none" name="hidden-form"></iframe>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

String processor(const String& var){
  //Serial.println(var);
  if(var == PARAM_NAME){
   return(station_rds_name);
  }
  else if(var == PARAM_TEXT){
    return(station_rds_buffer);
  }
  else if(var == PARAM_ID){
    return(String(station_rds_id));
  }
  return String();
}

void setup_webserver(){
  Serial.println("Initializing web server");
    // Send web page with input fields to client
  webserver.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  webserver.on("/audiolevel", HTTP_GET, [](AsyncWebServerRequest *request)
  {
      request->send(200, "text/text", String(radio.currInLevel));
  });
  webserver.on("/status", HTTP_GET, [](AsyncWebServerRequest *request)
  {
      String s;
      serializeJson(status, s);
      request->send(200, "text/text", s);
  });
  webserver.on("/set", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET inputString value on <ESP_IP>/get?inputString=<inputMessage>
    if (request->hasParam(PARAM_NAME)) {
      inputMessage = request->getParam(PARAM_NAME)->value();
      station_rds_name = inputMessage;
      radio.setRDSstation(station_rds_name.c_str());
      // set and store...
      // writeFile(SPIFFS, "/inputString.txt", inputMessage.c_str());
    }
    else if (request->hasParam(PARAM_TEXT)) {
      inputMessage = request->getParam(PARAM_TEXT)->value();
      station_rds_buffer = inputMessage;
      radio.setRDSbuffer(station_rds_buffer.c_str());
      // set and store...
      // writeFile(SPIFFS, "/inputString.txt", inputMessage.c_str());
    }
    else if (request->hasParam(PARAM_ID)) {
      inputMessage = request->getParam(PARAM_ID)->value();
      station_rds_id = atoi(inputMessage.c_str());
      // set and store...
      // writeFile(SPIFFS, "/inputString.txt", inputMessage.c_str());
    } else {
            inputMessage = "No message sent";
    }
    request->send(200, "text/text", inputMessage);
  } );


  webserver.onNotFound(notFound);
  webserver.begin();
}

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

  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  setup_webserver();
  // webServer.begin();
#endif
}

void web_client_loop(){
  status["radio"]["freq"]   = radio.currFreq;
  status["radio"]["dbuv"]   = radio.currdBuV;
  status["radio"]["antcap"] = radio.currAntCap;
  status["radio"]["currASQ"] = radio.currASQ;
  status["radio"]["inLevel"] = radio.currInLevel;
  status["rds"]["id"] = station_rds_id;
  status["rds"]["name"] = station_rds_name;
  status["rds"]["buffer"] = station_rds_buffer;
  status["wifi"]["rssi"]=WiFi.RSSI();
  status["reboots"] = reboot_counter;
};
