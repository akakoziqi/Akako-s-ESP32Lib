#include "oled.h"

void i2c_init(void)

{
    i2c_config_t conf;

    conf.mode = I2C_MODE_MASTER;

    conf.sda_io_num = OLED_SDA_IO;

    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;

    conf.scl_io_num = OLED_SCL_IO;

    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;

    conf.master.clk_speed = 400000;

    i2c_param_config(OLED_I2C_NUM, &conf);

    i2c_driver_install(OLED_I2C_NUM, conf.mode,0, 0, 0);

}