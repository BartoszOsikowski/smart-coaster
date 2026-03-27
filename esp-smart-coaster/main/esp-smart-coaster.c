#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "hx711.h"
#include "pn532.h"
#include "pn532_driver_i2c.h"

#define HX711_DT_PIN 21
#define HX711_SCK_PIN 20
#define SCL_PIN 9
#define SDA_PIN 8


void app_main(void)
{
    float SCALE_FACTOR = 644.0;
    
    hx711_t dev = 
    {
        .dout = HX711_DT_PIN,
        .pd_sck = HX711_SCK_PIN,
        .gain = HX711_GAIN_A_128
    };

    pn532_io_t pn532_io;

    pn532_new_driver_i2c(SDA_PIN, SCL_PIN, -1, -1, 0, &pn532_io);

    while(pn532_init(&pn532_io) != ESP_OK)
    {
        printf("Error\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

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

        uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; 
        uint8_t uid_length;     
        esp_err_t err = hx711_wait(&dev, 1000);
        
        if(err == ESP_OK) 
        {
            hx711_read_average(&dev, 10, &readings);
            int32_t weights;
            weights = (readings - tare) / SCALE_FACTOR;
            printf("Grams: %ld\n", weights);
        }
        else
        {
            printf("Error from readings!\n");
        }
        if(pn532_read_passive_target_id(&pn532_io, PN532_BRTY_ISO14443A_106KBPS, uid, &uid_length, 1000) == ESP_OK) 
        {
            for(int i=0; i<uid_length; i++)
            {
                printf("%02X ", uid[i]);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}