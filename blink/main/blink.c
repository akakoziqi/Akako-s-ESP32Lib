/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "driver/gpio.h"

//Personal Defines
static const char *TAG = "LED";
//打印log时的tag
#define LED_IO GPIO_NUM_2
#define GPIO_HIGH_LEVEL 1
#define GPIO_LOW_LEVEL 0
//自己写的一个延时函数，调用了vTaskDelay()
void DelayMS(int t)
{
    vTaskDelay(t / portTICK_PERIOD_MS);
}
//LED电平翻转函数
void LEDtoggle(int LED_GPIO)
{
    gpio_set_level(LED_GPIO, !gpio_get_level(LED_GPIO));
}

void app_main(void)
{
    gpio_pad_select_gpio(LED_IO);//选择GPIO，作用不大
    gpio_set_direction(LED_IO, GPIO_MODE_INPUT_OUTPUT);//设置GPIO模式
    gpio_set_level(LED_IO, GPIO_HIGH_LEVEL);//设置GPIO电平

    while (true)
    {
        LEDtoggle(LED_IO);
        ESP_LOGI(TAG, "LED toggle, current:%d",gpio_get_level(LED_IO));
        //ESP打印log的函数，需要定义一个tag
        DelayMS(1000);
    }
}
