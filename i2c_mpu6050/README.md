# ESP32 I2C MPU6050 driver for esp-idf
It uses hardware I2C port. So you can at most connect two sensors on the same chip with different I2C port number. The clock speed is 400khz. In my practice, the maximum data refresh rate is about 250Hz.

使用i2c读取MPU6050的加速度和角度温度数据，算法待更新