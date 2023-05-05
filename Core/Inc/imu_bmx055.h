/*
 * imu_bmx055.h
 *
 *  Created on: May 6, 2020
 *      Author: aokimasatake
 */

#ifndef	SRC_BMX055_H_
#define	SRC_BMX055_H_

// BMX055　加速度センサのI2Cアドレス
#define BMX055_Addr_Accl		0x19  // (JP1,JP2,JP3 = Openの時)

// BMX055　ジャイロセンサのI2Cアドレス
#define BMX055_Addr_Gyro		0x69  // (JP1,JP2,JP3 = Openの時)

// BMX055　磁気センサのI2Cアドレス
#define BMX055_Addr_Mag			0x13   // (JP1,JP2,JP3 = Openの時)

#define BMX055_GYRO_RANGE 		0x0f
#define BMX055_GYRO_BANDWIDTH	0x10
#define BMX055_GYRO_LPM1		0x11

#define BMX055_GYRO_RATE_Z_LSB 	0x06
#define BMX055_GYRO_RATE_Z_MSB 	0x07


void I2C1_BMX055_Init(void);
void I2C1_BMX055_zGyro(void);
void I2C1_Read_NBytes(uint8_t IC_Addr,uint8_t number_Bytes);
short I2C1_BMX055_GetGyro16(void);

#endif /* SRC_BMX055_H_ */
