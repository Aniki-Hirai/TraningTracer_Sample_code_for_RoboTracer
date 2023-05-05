
#include "index.h"

/* ---------------------------------------------------------------
	バッテリの電圧を取得する関数
--------------------------------------------------------------- */
float Battery_GetVoltage( void )
{
	return ((BATTERY_REFERENCE * (30.f / 10.f) * (float)Sensor_GetValue_Battery()) / 4096.f) + BATTERY_OFFSET;
}

/* ---------------------------------------------------------------
	バッテリの電圧制限関数
--------------------------------------------------------------- */
void Battery_LimiterVoltage( void )
{
	volatile int	i;
	volatile float	battery_voltage_average;

	for( i = 0; i < 10; i++) {
		HAL_Delay(10);
		battery_voltage_average += Battery_GetVoltage();
	}
	battery_voltage_average /= 10.0;

	if( battery_voltage_average < BATTERY_LIMIT ) {
		Vel_Set(0.0f);
		HAL_Delay(1000);
		Servo_Disable();
		while( 1 ) {
			LED_TOGGLE();
			HAL_Delay(100);
		}
	} else;
}
