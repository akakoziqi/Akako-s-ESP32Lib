#include "mpu6050_calculate.h"

float calc_temperature(int16_t raw)
{
    float temp = 36.53 + ((double)raw - 21) / 340.0f;
    return temp;
}

void calc_acceleration(mpu6050_acceleration_t *raw, mpu6050_acceleration_cal_t *cal)
{
    cal->accel_x = (double)raw->accel_x * (g * 2 / 32768);
    cal->accel_y = (double)raw->accel_y * (g * 2 / 32768);
    cal->accel_z = (double)raw->accel_z * (g * 2 / 32768);
}