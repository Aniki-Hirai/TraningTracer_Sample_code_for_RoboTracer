#include "index.h"

static	uint16_t	adc1_value[5];		// ADC1変換値格納用
static 	uint16_t	adc2_value[5];		// ADC2変換値格納用

/* ---------------------------------------------------------------
	センサADの初期設定と開始の関数
--------------------------------------------------------------- */
void Sensor_Initialize( void )
{
	HAL_ADC_Start_DMA(&hadc1,(uint32_t *)adc1_value,3);
	HAL_ADC_Start_DMA(&hadc2,(uint32_t *)adc2_value,4);
}

/* ---------------------------------------------------------------
	ラインセンサのAD値を取得する関数
--------------------------------------------------------------- */
uint16_t Sensor_GetValue_R2( void ){ return( adc1_value[0] ); }
uint16_t Sensor_GetValue_R1( void ){ return( adc2_value[0] ); }
uint16_t Sensor_GetValue_L2( void ){ return( adc2_value[2] ); }
uint16_t Sensor_GetValue_L1( void ){ return( adc2_value[1] ); }
uint16_t Sensor_GetValue_ML( void ){ return( adc1_value[2] ); }
uint16_t Sensor_GetValue_MR( void ){ return( adc1_value[1] ); }

/* ---------------------------------------------------------------
	バッテリのAD値を取得する関数
--------------------------------------------------------------- */
uint16_t Sensor_GetValue_Battery( void ){return( adc2_value[3] );}

