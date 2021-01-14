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
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "driver/gpio.h"

// #define USING_POLLING   //使用轮询方式检测
#define USING_INTR //使用按键中断

// Personal Defines
static const char *TAG = "KEYTEST";
#define KEY_GPIO GPIO_NUM_0
#define LED_GPIO GPIO_NUM_2
#define KEY_GPIO_SEL (1ULL << GPIO_NUM_0) //位掩码
#define ESP_INTR_FLAG_DEFAULT 0

#define GPIO_LOW_LEVEL 0
#define GPIO_HIGH_LEVEL 1

#define LED_ON 1
#define LED_OFF 0

#ifdef USING_POLLING
//按键引脚初始化函数
void KeyInit(uint32_t GPIO)
{
   gpio_pad_select_gpio(GPIO);
   gpio_set_direction(GPIO, GPIO_MODE_INPUT);  //输入模式
   gpio_set_pull_mode(GPIO, GPIO_PULLUP_ONLY); //上拉
}
#endif

#ifdef USING_INTR
//创建事件队列
static xQueueHandle gpio_evt_queue = NULL;
// 中断句柄
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
   uint32_t gpio_num = (uint32_t)arg;
   xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

//初始化GPIO结构体
void KeyInit(uint32_t GPIO)
{
   gpio_config_t key_conf;
   //interrupt at rising edge
   key_conf.intr_type = GPIO_PIN_INTR_POSEDGE; //上升沿触发中断
   key_conf.mode = GPIO_MODE_INPUT;            //输入模式
   key_conf.pin_bit_mask = KEY_GPIO_SEL;       //位掩码
   key_conf.pull_up_en = 1;                    //上拉使能
   key_conf.pull_down_en = 0;                  //下拉不使能
   gpio_config(&key_conf);
}

//LED引脚初始化函数
void LEDInit(uint32_t GPIO)
{
   gpio_pad_select_gpio(GPIO);
   gpio_set_direction(GPIO, GPIO_MODE_INPUT_OUTPUT); //输入输出模式
   gpio_set_pull_mode(GPIO, GPIO_FLOATING);          //不上拉不下拉，浮空
   gpio_set_level(GPIO, GPIO_LOW_LEVEL);             //初始化为低电平
}

#endif
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

#ifdef USING_POLLING
//读按键引脚电平
uint32_t ReadKeyLevel(uint32_t GPIO)
{
   return gpio_get_level(GPIO);
}

// 扫描按键函数
void ScanKey(void *pvParameters)
{
   while (true)
   {
      if (ReadKeyLevel(KEY_GPIO))
      {
         if (gpio_get_level(LED_GPIO)) //如果检测到LED亮，则熄灭
         {
            gpio_set_level(LED_GPIO, GPIO_LOW_LEVEL);
            ESP_LOGI(TAG, "LED off");
         }
      }
      else
      {
         if (!gpio_get_level(LED_GPIO)) //如果检测到LED灭，则点亮
         {
            gpio_set_level(LED_GPIO, GPIO_HIGH_LEVEL);
            ESP_LOGI(TAG, "LED on");
         }
      }
      DelayMS(10);
   }
}
#endif

#ifdef USING_INTR
// 中断服务函数
static void gpio_task_example(void *arg)
{
   uint32_t io_num;
   for (;;)
   {
      if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
      {
         ESP_LOGI(TAG, "GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
         LEDtoggle(LED_GPIO);
         DelayMS(100);
      }
   }
}

#endif

void app_main(void)
{
   // 初始化
   KeyInit(KEY_GPIO);
   LEDInit(LED_GPIO);

#ifdef USING_POLLING
   // 创建任务，任务必须是一个死循环，否则会报错
   xTaskCreatePinnedToCore(ScanKey,    //调用的函数指针
                           "ScanKey",  //别名
                           1024,       //堆栈大小（深度）
                           NULL,       //通过指针传递参数
                           1,          //任务优先级
                           NULL,       //任务句柄
                           APP_CPU_NUM //指定运行的核心，如果不指定可用xTaskCreate()
   );
#endif
#ifdef USING_INTR
   // 改变GPIO中断触发类型
   gpio_set_intr_type(KEY_GPIO, GPIO_INTR_NEGEDGE);
   // 创建中断服务程序（isr）事件队列
   gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
   // 创建任务
   xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);

   // 安装GPIO中断服务
   gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
   // 将中断句柄与对应IO绑定
   gpio_isr_handler_add(KEY_GPIO, gpio_isr_handler, (void *)KEY_GPIO);
   // 删除绑定
   gpio_isr_handler_remove(KEY_GPIO);
   // 重新绑定
   gpio_isr_handler_add(KEY_GPIO, gpio_isr_handler, (void *)KEY_GPIO);
#endif
}
