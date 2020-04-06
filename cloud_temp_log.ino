#include <Arduino.h>
#include <Hash.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>

#define ONE_WIRE_BUS 2

const char *ssid = "CellSpot_2.4GHz_DF28";
const char *password = "Wrobeltoziomal";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
    <style>
      html {
        font-family: Arial;
        display: inline-block;
        margin: 0px auto;
        text-align: center;
      }
      h2 { font-size: 3.0rem; }
      p { font-size: 3.0rem; }
      .units { font-size: 1.2rem; }
      .ds-labels {
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
      }
    </style>
  </head>
  <body>
    <h2>ESP8266 DS18B20 Server</h2>
    <p>
      <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
      <span class="ds-labels">Temperature</span> 
      <span id="temperature">%TEMPERATURE%</span>
      <sup class="units">&deg;C</sup>
    </p>
  </body>
  <script>
    setInterval(function ( ) {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("temperature").innerHTML = this.responseText;
        }
      };
      xhttp.open("GET", "/temperature", true);
      xhttp.send();
    }, 5000) ;
  </script>
</html>
)rawliteral";

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);

AsyncWebServer server(80);

String readDSTemperature()
{
  sensor.requestTemperatures();
  float tempC = sensor.getTempCByIndex(0);

  if (tempC == -127.00)
  {
    Serial.println("Failed to read from DS18B20 sensor!");
    return "--";
  }
  else
  {
    Serial.print("Temperature: ");
    Serial.println(tempC);
  }
  return String(tempC);
}

String processor(const String &var)
{
  if (var == "TEMPERATURE")
  {
    return readDSTemperature();
  }
  return String();
}

void setup()
{
  Serial.begin(115200);
  sensor.begin();
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", readDSTemperature().c_str());
  });
  server.begin();  
}

void loop()
{
  //sensor.requestTemperatures();
  //float temp = sensor.getTempCByIndex(0);
  //Serial.println(temp);
  //delay(5000);
}
