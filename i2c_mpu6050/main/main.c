#include <stdio.h>
#include "driver/i2c.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "mpu6050.h"
#include "mpu6050_calculate.h"

#define I2C_SCL_IO 19
#define I2C_SDA_IO 21
#define MPU6050_I2C_NUM I2C_NUM_0
const static char *TAG = "MPU6050";

void i2c_init()
{
    //i2c配置结构体
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;              //I2C模式
    conf.sda_io_num = I2C_SDA_IO;             //SDA IO映射
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;  //SDA IO模式
    conf.scl_io_num = I2C_SCL_IO;             //SCL IO映射
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;  //SCL IO模式
    conf.master.clk_speed = 100000;           //I2C CLK频率
    i2c_param_config(MPU6050_I2C_NUM, &conf); //设置I2C
    //注册I2C服务即使能
    i2c_driver_install(MPU6050_I2C_NUM, conf.mode, 0, 0, 0);
}

mpu6050_acceleration_t acceleration_raw;
mpu6050_acceleration_cal_t acceleration;
mpu6050_rotation_t rotation_raw;
mpu6050_rotation_cal_t rotation;

float temp;
float destination;

void app_main()
{
    i2c_init();
    mpu6050_init();
    mpu6050_set_temp_sensor_enabled(1);
    ESP_LOGI(TAG, "connection:%d", mpu6050_test_connection());
    ESP_LOGI(TAG, "tag:%c", *mpu6050_get_tag());
    ESP_LOGI(TAG, "aux_vddio_level:%d", mpu6050_get_aux_vddio_level());
    ESP_LOGI(TAG, "rate divider:%d", mpu6050_get_rate());
    ESP_LOGI(TAG, "external_frame_sync:%d", mpu6050_get_external_frame_sync());

    ESP_LOGI(TAG, "dlpf_mode:%d", mpu6050_get_dlpf_mode());
    ESP_LOGI(TAG, "full_scale_gyro_range:%d", mpu6050_get_full_scale_gyro_range());

    // mpu6050_self_test(&destination);
    // ESP_LOGI(TAG, "destination:%f", destination);

    while (true)
    {
        temp = calc_temperature(mpu6050_get_temperature());
        // mpu6050_get_motion(&acceleration_raw, &rotation_raw);
        // calc_acceleration(&acceleration_raw, &acceleration);
        // ESP_LOGI(TAG, "accel:x=%.2f,y=%.2f,z=%.2f", acceleration.accel_x, acceleration.accel_y, acceleration.accel_z);
        ESP_LOGI(TAG, "temp:%f",temp);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}