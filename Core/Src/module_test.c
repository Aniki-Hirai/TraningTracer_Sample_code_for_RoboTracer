
#include "index.h"
#include "imu_bmx055.h"
#include <stdio.h>

/* ---------------------------------------------------------------
	各機能の動作確認用関数
--------------------------------------------------------------- */
void Module_Test( void )
{
	uint16_t	line 	  = 0;
	uint8_t		key;
	int16_t		duty_l	  = 0;
	int16_t		duty_r	  = 0;

	// DMAを一時的に停止
	HAL_DMA_Abort(huart2.hdmarx);

	// エンコーダのカウントをリセット
	Encoder_Reset_Left();
	Encoder_Reset_Right();

	while(1) {

		// ブートからの時間を表示
		printf("<Boot Time> %8.3f[s]\r\n", Interrupt_GetBootTime()); line++;

		// モータを指定のDutyを表示
		printf("<PWM Duty> L: %4.1f[%%],  R: %4.1f[%%]\r\n",
				(float)duty_l/10.f, (float)duty_r/10.f); line++;

		// エンコーダの角度表示
		printf("<Encoder> L: %6.3f[m],  R: %6.3f[m]\r\n", Encoder_GetDist_Left(), Encoder_GetDist_Right()); line++;

		// バッテリー電圧の表示
		printf("<Battery> %3.2f[V]\r\n", Battery_GetVoltage()); line++;

		// ラインセンサのAD値表示
		printf("<Line Sensor> ML: %4d, L2: %4d L1: %4d, R1: %4d R2: %4d MR: %4d\r\n",
				Sensor_GetValue_ML(),
				Sensor_GetValue_L2(),
				Sensor_GetValue_L1(),
				Sensor_GetValue_R1(),
				Sensor_GetValue_R2(),
				Sensor_GetValue_MR()
				);
		line++;

		//IMU（加速度計とジャイロ）の計測値表示
		/*I2C1_Read_NBytes(BMX055_Addr_Gyro,2);
		HAL_Delay(10);
		I2C1_BMX055_zGyro();
		HAL_Delay(10);
		printf("<IMU> Gyro_Z: %d [rad/s]\r\n", I2C1_BMX055_GetGyro16()); line++;
*/

		//モータのDuty入力
		key = Communicate_TerminalRecv();
		switch( key ) {
			case '1': duty_l += 1;		break;
			case 'q': duty_l -= 1;		break;
			case '2': duty_l += 10;		break;
			case 'w': duty_l -= 10;		break;
			case '3': duty_l += 100;	break;
			case 'e': duty_l -= 100;	break;
			case '7': duty_r += 1;		break;
			case 'u': duty_r -= 1;		break;
			case '8': duty_r += 10;		break;
			case 'i': duty_r -= 10;		break;
			case '9': duty_r += 100;	break;
			case 'o': duty_r -= 100;	break;
			case 'r': // reset
				duty_l = duty_r = 0;
				break;
			case 0x1b: goto END; // [esc] exit
		}
		duty_l = SIGN(duty_l) * MIN( 950, ABS(duty_l) );
		duty_r = SIGN(duty_r) * MIN( 950, ABS(duty_r) );

		// モータを回転
		Motor_SetDuty_Left(duty_l);
		Motor_SetDuty_Right(duty_r);

		fflush(stdout);
		HAL_Delay(200);
		// 画面のクリア
		printf("%c[0J", 0x1b);
		printf("%c[%dA", 0x1b, line);
	}
	END:;
	MOT_STOP_PWM();
}

