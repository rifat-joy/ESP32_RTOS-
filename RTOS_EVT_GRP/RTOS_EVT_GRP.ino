#include "DHT.h"
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <WiFiClientSecure.h>

#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)

#define DHTPIN 23
#define DHTTYPE DHT11

const char* ssid = "Rifat";         //ssid of your wifi
const char* password = "rifat674";  //wifi password

volatile int tick_modem = 0;
volatile int tick_sntp = 0;
String newHostname = "RTOS_ESP32";

StaticJsonDocument<32> doc;

WiFiClientSecure net;
DHT dht(DHTPIN, DHTTYPE);

EventGroupHandle_t EventGroupHandle = NULL;
TaskHandle_t TaskWirelessConnectivity_Handle;
TaskHandle_t TaskMQTTConnectivity_Handle;
TaskHandle_t DHTSensor_Handle;

void TaskDHTSensor(void* pvParameters);
void TaskWirelessConnectivity(void* pvParameters);

void wifi_status(int status_code) {  //Check the Wi-Fi Status while connecting to any wireless network
  switch (status_code) {
    case 0:
      {
        Serial.println("WiFi is in process of changing between statuses");
        break;
      }
    case 1:
      {
        Serial.println("SSID cannot be reached");
        break;
      }
    case 2:
      {
        Serial.println("Scan Completed");
        break;
      }
    case 3:
      {
        Serial.println("Successful connection is established");
        break;
      }
    case 4:
      {
        Serial.println("Password is incorrect");
        break;
      }
    case 5:
      {
        Serial.println("Connection Lost");
        break;
      }
    case 6:
      {
        Serial.println("Module is disconnected or not configured in station mode");
        break;
      }
  }
}

void setup_wifi() {
  vTaskDelay(10 / portTICK_PERIOD_MS);

  Serial.println();
  Serial.print("Connecting to \"");  //Prints the Wi-Fi name in serial monitor
  Serial.print(ssid);

  Serial.print("\" and password \"");  //Prints the wifi-password in serial monitor. This practice should be avoided. Anyone can access the wifi very easily
  Serial.print(password);
  Serial.println("\"");

  WiFi.mode(WIFI_STA);
  //WiFi.hostname("ESP-host");
  WiFi.hostname(newHostname.c_str());  //Sets the host name to indentify the divie in network
  WiFi.begin(ssid, password);
  Serial.println();
  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    wifi_status(WiFi.status());
    tick_modem = tick_modem + 1;
    // Serial.printf("Tick modem count %d of 120 \n", tick_modem);

    if (tick_modem == 120)  //wait for 60s to connect to modem
    {
      // Serial.print(WiFi.status()); Serial.print(' ');
      Serial.println("");
      Serial.println("Wait 60s");
      Serial.println("ESP Resets");
      ESP.restart();
    }
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());
}

void setup_time() {
  Serial.println("Setting time using NTP");
  configTime(-5 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  time_t now = time(nullptr);
  while (now < 1510592825) {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    tick_sntp = tick_sntp + 1;
    // Serial.printf("Tick sntp count %d of 120\n", tick_sntp);

    if (tick_sntp == 120)  //wait for 60s to get sntp.
    {
      Serial.println("Wait 60s");
      Serial.println("ESP Resets");
      ESP.restart();
    }
    now = time(nullptr);
  }
  Serial.println("done!");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

void setup() {
  Serial.begin(115200);
  Serial.print("setup: Executing on core ");
  Serial.println(xPortGetCoreID());

  EventGroupHandle = xEventGroupCreate();

  if (EventGroupHandle != NULL) {
    xTaskCreatePinnedToCore(
      TaskWirelessConnectivity,
      "WiFinMQTT",  // A name just for humans
      3600,         // This stack size can be checked & adjusted by reading the Stack Highwater
      NULL,
      1,  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
      &TaskWirelessConnectivity_Handle,
      0);

    xTaskCreatePinnedToCore(
      TaskDHTSensor,
      "DHT11",
      1200,  // This stack size can be checked & adjusted by reading the Stack Highwater
      NULL,
      1,  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
      &DHTSensor_Handle,
      1);
  }
}

void loop() {
}

void TaskWirelessConnectivity(void* pvParameters) {
  (void)pvParameters;
  UBaseType_t uxHighWaterMark;

  setup_wifi();
  setup_time();
  // xEventGroupSetBits(EventGroupHandle, BIT_0);

  uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);  // Returns the unused stack of total given stack to task
  Serial.println(uxHighWaterMark);

  for (;;)  // A Task shall never return or exit.
  {
    // Serial.print("TaskWirelessConnectivity: Executing on core ");
    // Serial.println(xPortGetCoreID());

    if (WiFi.status() != WL_CONNECTED) {
      setup_wifi();
      setup_time();
    }

    xEventGroupSetBits(EventGroupHandle, BIT_0);

    Serial.println("Connected..!");
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    Serial.println(uxHighWaterMark);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void TaskMQTTConnectivity(void* pvParameters) {
  (void)pvParameters;

  for (;;) {
  }
}

void TaskDHTSensor(void* pvParameters) {
  (void)pvParameters;
  UBaseType_t uxHighWaterMark;

  // xEventGroupWaitBits(
  //   EventGroupHandle,
  //   BIT_0,    // Indicates to trigger upon srtting of certain BITS
  //   pdFALSE,  // If in loop then pdTRUE, if in Setup pdFALSE
  //   pdTRUE,
  //   portMAX_DELAY);

  Serial.println("--------Initializing DHT11--------");
  dht.begin();
  char dhtData[60];

  uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);  // Returns the unused stack of total given stack to task
  Serial.println(uxHighWaterMark);

  for (;;)  // A Task shall never return or exit.
  {
    xEventGroupWaitBits(EventGroupHandle, BIT_0, pdTRUE, pdTRUE, portMAX_DELAY);

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
