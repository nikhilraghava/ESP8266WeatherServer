#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// WiFi parameters
const char* ssid = "Orange";
const char* password = "navnik18";

ESP8266WebServer server(80);

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
      // Root object
      JsonArray& weather = root["weather"];
      JsonObject& weather0 = weather[0];
      // Get main report
      condition = weather0["main"];
    }
    http.end();   //Close connection
  }
  return condition;
}

void handleRoot() {
  // Call weather API
  const char* condition_now = getWeatherCondition();
  server.send(200, "text/plain", condition_now);
}

void handleNotFound(){
  String message = "Bad request!\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup() {
  Serial.begin(115200);
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Move to next line
  Serial.println();
  // Print WiFi Parameters
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(){
  server.handleClient();
}

