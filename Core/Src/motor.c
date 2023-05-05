
#include "index.h"
#include "param.h"

#define PCLK			(HAL_RCC_GetPCLK1Freq() * 2)//PCLK1はタイマに送られるクロックの半分なので2倍する
#define PWMFREQ			(1000)						// モータのPWM動作周波数[Hz]
#define MOT_DUTY_MIN	(PWM_MOT_MIN)				// モータのPWM最低Duty
#define MOT_DUTY_MAX	(PWM_MOT_MAX)				// モータのPWM最大Duty

/* ---------------------------------------------------------------
	左モータを指定のDuty（-1000～1000）で回転させる関数
--------------------------------------------------------------- */
void Motor_SetDuty_Left( int16_t duty_l )
{
	uint32_t	pulse_l;

	if( ABS(duty_l) > MOT_DUTY_MAX ) {
		pulse_l = (uint32_t)(PCLK / PWMFREQ * MOT_DUTY_MAX / 1000) - 1;
	}
	else if( ABS(duty_l) < MOT_DUTY_MIN ) {
		pulse_l = (uint32_t)(PCLK / PWMFREQ * MOT_DUTY_MIN / 1000) - 1;
	}
	else {
		pulse_l = (uint32_t)(PCLK / PWMFREQ * ABS(duty_l) / 1000) - 1;
	}

	if( duty_l > 0 ) {
		MOT_SET_FORWARD_L();
		MOT_SET_COMPARE_L( pulse_l );
	}
	else if( duty_l < 0 ) {
		MOT_SET_REVERSE_L()	;
		MOT_SET_COMPARE_L( pulse_l );
	} else {
		MOT_SET_COMPARE_L( 0x0000 );
	}
}

/* ---------------------------------------------------------------
	右モータを指定のDuty（-1000～1000）で回転させる関数
--------------------------------------------------------------- */
void Motor_SetDuty_Right( int16_t duty_r )
{
	uint32_t	pulse_r;

	if( ABS(duty_r) > MOT_DUTY_MAX ) {
		pulse_r = (uint32_t)(PCLK / PWMFREQ * MOT_DUTY_MAX / 1000) - 1;
	}
	else if( ABS(duty_r) < MOT_DUTY_MIN ) {
		pulse_r = (uint32_t)(PCLK / PWMFREQ * MOT_DUTY_MIN / 1000) - 1;
	}
	else {
		pulse_r = (uint32_t)(PCLK / PWMFREQ * ABS(duty_r) / 1000) - 1;
	}

	if( duty_r > 0 ) {
			MOT_SET_FORWARD_R();
			MOT_SET_COMPARE_R( pulse_r );
		}
		else if( duty_r < 0 ) {
			MOT_SET_REVERSE_R();
			MOT_SET_COMPARE_R( pulse_r );
		} else {
			MOT_SET_COMPARE_R( 0x0000 );
		}
}


