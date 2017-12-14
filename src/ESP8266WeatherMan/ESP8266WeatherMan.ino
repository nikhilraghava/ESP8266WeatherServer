#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// WiFi parameters
const char* ssid = "Orange";
const char* password = "navnik18";

void setup() {
  Serial.begin(115200);
  // Enhance view in monitor
  Serial.println();
  Serial.println();
  Serial.println();
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
}

void loop(){
  // Call weather API
  const char* condition_now = getWeatherCondition();
  Serial.println(condition_now);
  delay(120000);  
}

const char* getWeatherCondition(){
  // Initialise condition
  const char* condition;
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;  //Declare an object of class HTTPClient
    http.begin("http://api.openweathermap.org/data/2.5/weather?lat=1.3366329&lon=103.7350981&appid=d03e75882bdd6d2e61acf2d6df4daf22");
    int httpCode = http.GET();
 
    if (httpCode > 0) {
      // Get payload
      String payload = http.getString();
      // JSON buffer 
      const size_t bufferSize = JSON_ARRAY_SIZE(3) + 2*JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + 3*JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(12) + 480;
      DynamicJsonBuffer jsonBuffer(bufferSize);
      // Parse JSON data
      const char* json = payload.c_str();
      JsonObject& root = jsonBuffer.parseObject(json);
      // Root
      JsonArray& weather = root["weather"];
      JsonObject& weather0 = weather[0];
      // Get main report
      condition = weather0["main"];
    }
    http.end();   //Close connection
  }
  return condition;
}


