#include "DHT.h"
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

#define DHTPIN 23
#define DHTTYPE DHT11

StaticJsonDocument<32> doc;

DHT dht(DHTPIN, DHTTYPE);

TaskHandle_t DHTSensor_Handle;
xQueueHandle_t 

void TaskDHTSensor(void* pvParameters);

void setup() {
  Serial.begin(115200);
  Serial.print("setup: Executing on core ");
  Serial.println(xPortGetCoreID());

  xTaskCreatePinnedToCore(
    TaskDHTSensor,
    "DHT11",
    1200,  // This stack size can be checked & adjusted by reading the Stack Highwater
    NULL,
    1,  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    &DHTSensor_Handle,
    1);
}

void loop() {
}

void TaskDHTSensor(void* pvParameters) {
  (void)pvParameters;
  UBaseType_t uxHighWaterMark;

  Serial.println("--------Initializing DHT11--------");
  dht.begin();

  char dhtData[60];

  uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);  // Returns the unused stack of total given stack to task
  Serial.println(uxHighWaterMark);

  for(;;)  // A Task shall never return or exit.
  {
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    float hum = dht.readHumidity();
    float temp = dht.readTemperature();

    if (isnan(hum) || isnan(temp)) {  // Check if any reads failed and exit early (to try again).
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    doc["Humidity"] = hum;
    doc["Temperature"] = temp;

    serializeJsonPretty(doc, dhtData);  // Serialize with indentation
    Serial.println(dhtData);


    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    Serial.println(uxHighWaterMark);
  }
}
