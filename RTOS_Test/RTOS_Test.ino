#include "DHT.h"
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <time.h>

#define DHTPIN 23
#define DHTTYPE DHT11

#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)

// char dhtData[50];

const char* ssid = "Rifat";         //ssid of your wifi
const char* password = "rifat674";  //wifi password
const char* PING_TOPIC = "TestPing";

String newHostname = "RTOS_ESP32";  //To identify the connedted devices in hotspot network
String clientId = "RTOS_ESP32";

volatile int tick_modem = 0;  // counter for error in modem connectivty
volatile int tick_sntp = 0;   // counter for sntp error (indicates no internet data)
volatile int tick_ping = 0;
String dispenseTopic = "res";
const char* mqtt_server = "prohorii.vertical-innovations.com";

// BearSSL::WiFiClientSecure net;
WiFiClientSecure net;
PubSubClient client(net);
DHT dht(DHTPIN, DHTTYPE);

//Vertical innovations website certificate
const char* x509CA PROGMEM = R"EOF("
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----

")EOF";

typedef struct DHT_DATA {
  String id;
  char data[60];
} xdhtStruct;

/*Task handeler to control task*/
EventGroupHandle_t EventGroupHandle = NULL;
QueueHandle_t QHandle = NULL;
TaskHandle_t TaskWirelessConnectivity_Handle;
TaskHandle_t TaskMQTTConnectivity_Handle;
TaskHandle_t DHTSensor_Handle;

/*Task prototype*/
void TaskWirelessConnectivity(void* pvParameters);
void TaskMQTTConnectivity(void* pvParameters);
void TaskDHTSensor(void* pvParameters);

/* Perifiral functions */
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

void reconnect() {
  // Loop until reconnected (Basic & OTA connection)
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    // Attempt to connect
    if (client.connect(clientId.c_str(), "vilmqtt", "mvqitlt")) {

      Serial.println("connected");
      client.subscribe(dispenseTopic.c_str());
      // client.subscribe(pingTranTopic.c_str());
      // client.subscribe(OtaVerCheckTopic.c_str());
      //  WDT_count = 0;
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // INDICATOR_PIN_off();

      // Wait 5 seconds before retrying
      tick_ping = tick_ping + 1;
      // Serial.printf("Tick ping count %d of 5\n", tick_ping);

      if (tick_ping == 5)  //wait for reconnect to MQTT Server
      {
        ESP.restart();
      }
      vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  String msg = "";
  if (strcmp(topic, dispenseTopic.c_str()) == 0) {
    Serial.println("Message: ");
    for (int i = 0; i < length; i++) {
      msg += (String)(char)payload[i];
    }
    Serial.println(msg);
    if (msg = "Hi") {
      Serial.println("Hello Publisher");
      client.publish("TestRes", "Hello Publisher");
    }
  }
}

void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  Serial.print("setup: Executing on core ");
  Serial.println(xPortGetCoreID());

  EventGroupHandle = xEventGroupCreate();
  QHandle = xQueueCreate(5, sizeof(xdhtStruct));  // Queue handeler to take controll over queue

  if (QHandle && EventGroupHandle != NULL) {
    printf("Queue & EventGroup created successfully.!\n");
    // Now set up two tasks to run independently.
    xTaskCreatePinnedToCore(
      TaskWirelessConnectivity,
      "WiFinMQTT",  // A name just for humans
      1800,         // This stack size can be checked & adjusted by reading the Stack Highwater
      NULL,
      1,  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
      &TaskWirelessConnectivity_Handle,
      0);

    xTaskCreatePinnedToCore(
      TaskMQTTConnectivity,
      "MQTT",  // A name just for humans
      3500,    // This stack size can be checked & adjusted by reading the Stack Highwater
      (void*)QHandle,
      1,  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
      &TaskMQTTConnectivity_Handle,
      0);

    xTaskCreatePinnedToCore(
      TaskDHTSensor,
      "DHT11",
      1400,  // This stack size can be checked & adjusted by reading the Stack Highwater
      (void*)QHandle,
      1,  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
      &DHTSensor_Handle,
      1);
  } else {
    printf("Queue can't be created\n");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  // Serial.print("loop: Executing on core ");
  // Serial.println(xPortGetCoreID());
  // vTaskDelay(3000 / portTICK_PERIOD_MS);
}

/* Task Functions*/
void TaskWirelessConnectivity(void* pvParam) {
  (void)pvParam;
  UBaseType_t uxHighWaterMark;

  setup_wifi();
  setup_time();

  uxHighWaterMark = uxTaskGetStackHighWaterMark(TaskWirelessConnectivity_Handle);  // Returns the unused stack of total given stack to task
  Serial.println(uxHighWaterMark);

  for (;;)  // A Task shall never return or exit.
  {
    Serial.print("WiFi : Executing on core ");
    Serial.println(xPortGetCoreID());

    if (WiFi.status() != WL_CONNECTED) {
      setup_wifi();
      setup_time();
    }
    xEventGroupSetBits(EventGroupHandle, BIT_0);
    uxHighWaterMark = uxTaskGetStackHighWaterMark(TaskWirelessConnectivity_Handle);
    Serial.println(uxHighWaterMark);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void TaskMQTTConnectivity(void* pvParam) {
  UBaseType_t uxHighWaterMark;

  xEventGroupWaitBits(EventGroupHandle, BIT_0, pdFALSE, pdTRUE, portMAX_DELAY);

  net.setCACert(x509CA);
  client.setServer(mqtt_server, 8883);
  client.setCallback(callback);
  reconnect();

  QHandle = (QueueHandle_t)pvParam;  // Queue handle passed as parameter to the task.
  xdhtStruct dhtDataPack = { "", 0 };

  uxHighWaterMark = uxTaskGetStackHighWaterMark(TaskMQTTConnectivity_Handle);  // Returns the unused stack of total given stack to task
  Serial.println(uxHighWaterMark);
  for (;;) {
    Serial.print("MQTT : Executing on core ");
    Serial.println(xPortGetCoreID());
    xEventGroupSetBits(EventGroupHandle, BIT_1);
    if (!client.connected()) {
      reconnect();
    }
    client.loop();  // one tick delay (15ms) in between reads for stability
    BaseType_t xStatus = xQueueReceive(QHandle, &dhtDataPack, portMAX_DELAY);
    if (xStatus != pdPASS) {
      printf("Receive failed\n");
    } else {
      printf("Queue received\n");
      // printf("Received ID : %s, Data : %s\n", dhtDataPack.id, dhtDataPack.data);
      // getchar();
    }
    if (client.publish(PING_TOPIC, dhtDataPack.data)) {
      Serial.println("Data published..!");
    } else {
      Serial.println("Error..!");
    }
    uxHighWaterMark = uxTaskGetStackHighWaterMark(TaskMQTTConnectivity_Handle);
    Serial.println(uxHighWaterMark);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void TaskDHTSensor(void* pvParam) {
  // (void)pvParameters; // If parameter is void then
  UBaseType_t uxHighWaterMark;
  QHandle = (QueueHandle_t)pvParam;  // Queue handle passed as parameter to the task.
  xEventGroupWaitBits(EventGroupHandle, BIT_0 | BIT_1, pdFALSE, pdTRUE, portMAX_DELAY);

  xdhtStruct dhtDataPack = { "", 0 };

  Serial.println("--------Initializing DHT11--------");
  dht.begin();

  StaticJsonDocument<32> doc;

  uxHighWaterMark = uxTaskGetStackHighWaterMark(DHTSensor_Handle);  // Returns the unused stack of total given stack to task
  Serial.println(uxHighWaterMark);

  for (;;)  // A Task shall never return or exit.
  {
    Serial.print("DHT : Executing on core ");
    Serial.println(xPortGetCoreID());

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    float hum = dht.readHumidity();
    float temp = dht.readTemperature();
    dhtDataPack.id = "DHT";

    if (isnan(hum) || isnan(temp)) {  // Check if any reads failed and exit early (to try again).
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    doc["Humidity"] = hum;
    doc["Temperature"] = temp;

    serializeJsonPretty(doc, dhtDataPack.data);  // Serialize with indentation

    BaseType_t xStatus = xQueueSendToBack(QHandle, &dhtDataPack, 0);
    if (xStatus != pdPASS) {
      printf("Can't send data..!\n");
    } else {
      // printf("Sending ID : %s, Data : %s\n", dhtDataPack.id, dhtDataPack.data);
      printf("Queue Sent\n");
    }

    uxHighWaterMark = uxTaskGetStackHighWaterMark(DHTSensor_Handle);
    Serial.println(uxHighWaterMark);
  }
}














