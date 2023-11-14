#include <stdio.h>
#include <esp_log.h>
#include <esp_chip_info.h>
#include <esp_system.h>
#include <esp_flash.h>
#include <nvs_flash.h>
#include <esp_netif.h>
#include <esp_event.h>

#include "esp32/components/face-dl/include/detect_face.h"
#include "esp32/components/face-dl/detect_face.cpp"

static const char *TAG = "main";

extern "C" {
    void app_main (void);
}

void app_main (void) {
    ESP_LOGI(TAG, "program started");

    printf("testing\n");
    detect_face();

    while (true) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}