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
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "esp_event.h"
#include "esp_log.h"

//Personal Defines
static const char *TAG = "LEDC";
//打印log时的tag
#define LED_IO GPIO_NUM_2
#define LED_FADE_TIME 2000 //ms
#define MAX_DUTY 4095

uint32_t current_duty = 0;
uint32_t target_duty = 4095;

//定时器配置结构体
ledc_timer_config_t ledc_timer_conf;
//ledc通道配置结构体
ledc_channel_config_t ledc_conf;

void ledc_init(void)
{

   //定时器配置->timer0
   ledc_timer_conf.duty_resolution = LEDC_TIMER_12_BIT; //PWM分辨率
   ledc_timer_conf.freq_hz = 10000;                     //频率
   ledc_timer_conf.speed_mode = LEDC_HIGH_SPEED_MODE;   //速度
   ledc_timer_conf.timer_num = LEDC_TIMER_0;            //选择定时器0-3
   //设置定时器PWM模式
   ledc_timer_config(&ledc_timer);

   //通道映射GPIO2->channel0
   ledc_conf.gpio_num = LED_IO;                 //GPIO映射
   ledc_conf.channel = LEDC_CHANNEL_0;          //LEDC通道
   ledc_conf.duty = 0;                          //占空比
   ledc_conf.speed_mode = LEDC_HIGH_SPEED_MODE; //速度
   ledc_conf.intr_type = LEDC_INTR_DISABLE;     //中断类型
   ledc_channel_config(&ledc_struct);
   //使能渐变函数
   ledc_fade_func_install(0);
}
//延时函数
void DelayMS(int ms)
{
   vTaskDelay(ms / portTICK_PERIOD_MS);
}
//设置亮度函数
void SetBrightness(void *pvParameters)
{
   while (true)
   {
      if (current_duty != target_duty)
      {
         ledc_set_fade_with_time(ledc_conf.speed_mode, ledc_conf.channel, target_duty, LED_FADE_TIME);
         ESP_LOGI(TAG, "Set brightness %d to %d", current_duty, target_duty);
         ledc_fade_start(ledc_conf.speed_mode, ledc_conf.channel, LEDC_FADE_NO_WAIT);
         current_duty = target_duty;
      }
      DelayMS(50);
   }
}

void app_main(void)
{
   //ledc初始化函数
   ledc_init();
   //轮询检测是否需要调整亮度
   xTaskCreatePinnedToCore(SetBrightness, "SetBrightness", 2048, NULL, 1, NULL, APP_CPU_NUM);
   while (true)
   {
      target_duty = 0;
      DelayMS(LED_FADE_TIME);
      target_duty = MAX_DUTY;
      DelayMS(LED_FADE_TIME);
   }
}
