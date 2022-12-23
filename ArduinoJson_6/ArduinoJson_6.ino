#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

StaticJsonDocument<32> doc;
// JsonObject& root = doc.as<JsonObject>();
char dhtData[50];

void setup() {
  // put your setup code here, to run once:
  randomSeed(analogRead(0));
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  doc["humidity"] = random(0, 100);
  doc["temperature"] = random(0, 30);

  // serializeJson(doc, dhtData);
  serializeJsonPretty(doc, dhtData);  // Serialize with indentation
  delay(1000);
  Serial.println(dhtData);

  deserializeJson(doc, dhtData);

  // DeserializationError err = deserializeJson(doc, dhtData);
  // if (err) {
  //   Serial.print("Error : ");
  //   Serial.println(err.c_str());
  //   return;
  // }

  int hum = doc ["humidity"];
  Serial.println(hum);
}
