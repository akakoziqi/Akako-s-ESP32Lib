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

// Personal Defines
static const char *TAG = "KEYTEST";
#define KEY_GPIO GPIO_NUM_0
#define LED_GPIO GPIO_NUM_2

#define GPIO_LOW_LEVEL 0
#define GPIO_HIGH_LEVEL 1

#define LED_ON 1
#define LED_OFF 0

//按键引脚初始化函数
void KeyInit(uint32_t GPIO)
{
   gpio_pad_select_gpio(GPIO);
   gpio_set_direction(GPIO, GPIO_MODE_INPUT);  //输入模式
   gpio_set_pull_mode(GPIO, GPIO_PULLUP_ONLY); //上拉
}
//LED引脚初始化函数
void LEDInit(uint32_t GPIO)
{
   gpio_pad_select_gpio(GPIO);
   gpio_set_direction(GPIO, GPIO_MODE_INPUT_OUTPUT); //输入输出模式
   gpio_set_pull_mode(GPIO, GPIO_FLOATING);          //不上拉不下拉，浮空
   gpio_set_level(GPIO, GPIO_LOW_LEVEL);             //初始化为低电平
}
//读按键引脚电平
uint32_t ReadKeyLevel(uint32_t GPIO)
{
   return gpio_get_level(GPIO);
}
//延时函数
void DelayMS(int t)
{
   vTaskDelay(t / portTICK_PERIOD_MS);
}
//LED电平翻转函数
void LEDtoggle(int LED_GPIO)
{
   gpio_set_level(LED_GPIO, !gpio_get_level(LED_GPIO));
}
// 扫描按键函数
void ScanKey(void *pvParameters)
{
   while (true)
   {
      if (ReadKeyLevel(KEY_GPIO)){
         if (gpio_get_level(LED_GPIO))//如果检测到LED亮，则熄灭
         {
            gpio_set_level(LED_GPIO, GPIO_LOW_LEVEL);
            ESP_LOGI(TAG, "LED off");
         }
      }
      else{
         if (!gpio_get_level(LED_GPIO))//如果检测到LED灭，则点亮
         {
            gpio_set_level(LED_GPIO, GPIO_HIGH_LEVEL);
            ESP_LOGI(TAG, "LED on");
         }
      }
      DelayMS(10);
   }
}

void app_main(void)
{
   // 初始化
   KeyInit(KEY_GPIO);
   LEDInit(LED_GPIO);
   // 创建任务，任务必须是一个死循环，否则会报错
   xTaskCreatePinnedToCore(ScanKey,      //调用的函数指针
                           "ScanKey",    //别名
                           1024,         //堆栈大小（深度）
                           NULL,         //通过指针传递参数
                           1,            //任务优先级
                           NULL,         //任务句柄
                           APP_CPU_NUM   //指定运行的核心，如果不指定可用xTaskCreate()
                           );
}
