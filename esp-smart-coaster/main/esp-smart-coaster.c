#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "hx711.h"


#define HX711_DT_PIN 21
#define HX711_SCK_PIN 20


void app_main(void)
{
    float SCALE_FACTOR = 644.0;
    
    hx711_t dev = 
    {
        .dout = HX711_DT_PIN,
        .pd_sck = HX711_SCK_PIN,
        .gain = HX711_GAIN_A_128
    };

     while(hx711_init(&dev) != ESP_OK) 
    {
    printf("Error!!\n");
    vTaskDelay(pdMS_TO_TICKS(1000));
    }

    esp_err_t err = hx711_wait(&dev, 1000);
    int32_t tare;
    if(err == ESP_OK) 
    {
        hx711_read_average(&dev, 30, &tare);
    }
    else
    {
        printf("Error!\n");
    }

   

    int32_t readings;

    while(1) {
        esp_err_t err = hx711_wait(&dev, 1000);
        hx711_read_average(&dev, 10, &readings);
        if(err == ESP_OK) 
        {
            int32_t weights = (readings - tare) / SCALE_FACTOR;
            printf("Grams: %ld\n", weights);
        }
        else
        {
            printf("Error from readings!\n");
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}



