#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "include/test_camera.h"
#include "include/test_decode.h"
#include "include/test_deep_learning.h"

uint8_t *pixels;
uint64_t counter = 1;

void app_main(void) {
    while(1){
        task1(&counter);

        test_decode_task(&pixels);
        if(test_deep_learning(&pixels)){
            counter++;        
        }
        else{
            free(pixels);
            pixels = NULL;
        }
        printf("finished!\n");

        vTaskDelay(8000 / portTICK_PERIOD_MS);
    }
}