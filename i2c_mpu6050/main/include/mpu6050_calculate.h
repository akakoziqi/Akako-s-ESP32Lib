#ifndef __MPU6050_CALCULATE_H__
#define __MPU6050_CALCULATE_H__

#include <stdio.h>
#include "mpu6050.h"

#define g 9.7936

typedef struct _mpu6050_acceleration_cal_t
{
    float accel_x;
    float accel_y;
    float accel_z;
} mpu6050_acceleration_cal_t;

typedef struct _mpu6050_rotation_cal_t
{
    float gyro_x;
    float gyro_y;
    float gyro_z;
} mpu6050_rotation_cal_t;

float calc_temperature(int16_t raw);
void calc_acceleration(mpu6050_acceleration_t *raw, mpu6050_acceleration_cal_t *cal);


#endif


