/*
  * Inter Task data transfer method using queue
  * Sending Data as a packet using struct
*/
typedef struct X_STRUCT {
  char id;
  char data;
} xStruct;

void sendTask1(void *pvParam) {
  QueueHandle_t QHandle = (QueueHandle_t)pvParam;
  // BaseType_t xStatus;
  // int i = 1;

  xStruct xUSB = { 1, 5 };

  for (;;) {
    BaseType_t xStatus = xQueueSendToBack(QHandle, &xUSB, 0);
    if (xStatus != pdPASS) {
      printf("Task 1 Can't send..!\n");
    } else {
      printf("Task 1 sending done.\n");
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    xUSB.id++;
    if (xUSB.id == 6) {
      xUSB.id = 1;
    }
  }
}
/*
void sendTask2(void *pvParam) {
  QueueHandle_t QHandle = (QueueHandle_t)pvParam;
  BaseType_t xStatus;
  int i = 2;

  for (;;) {
    xStatus = xQueueSendToBack(QHandle, &i, 0);
    if (xStatus != pdPASS) {
      printf("Task 2 Can't send..!\n");
    } else {
      printf("Task 2 sending done.\n");
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}*/

void recvTask(void *pvParam) {
  QueueHandle_t QHandle = (QueueHandle_t)pvParam;
  // BaseType_t xStatus;
  xStruct xUSB = { 0, 0 };

  for (;;) {
    BaseType_t xStatus = xQueueReceive(QHandle, &xUSB, portMAX_DELAY);
    if (xStatus != pdPASS) {
      printf("Receive failed\n");
    } else {
      printf("Received ID : %d, Data : %d!\n", xUSB.id, xUSB.data);
    }
  }
}


void setup() {
  // put your setup code here, to run once:
  QueueHandle_t QHandle = xQueueCreate(5, sizeof(xStruct));

  if (QHandle != NULL) {
    printf("Queue created successfully.!\n");
    xTaskCreate(sendTask1, "send task 1", 1024 * 5, (void *)QHandle, 1, NULL);
    // xTaskCreate(sendTask2, "send task 2", 1024 * 5, (void *)QHandle, 1, NULL);
    xTaskCreate(recvTask, "recv task", 1024 * 5, (void *)QHandle, 1, NULL);
  } else {
    printf("Queue can't be created\n");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}
