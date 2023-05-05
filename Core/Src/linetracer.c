/*
 * linetracer.c
 *
 *  Created on: 2020/06/02
 *      Author: masataka
 */

#include "index.h"
#include "param.h"
#include <stdio.h>

/* ---------------------------------------------------------------
	ライントレースロボットとしての関数軍
--------------------------------------------------------------- */
void Line_Tracer( void )
{

	uint8_t vel_sellct = 0;

	while (1) {

		//モードインクリメント処理
		if(SW1_ONOFF() == SW_ON){
			HAL_Delay(50);					//チャタリング回避
			while(SW1_ONOFF() == SW_ON);	//スイッチが離されるまで待つ
			Buzz_Set(1,50);
			HAL_Delay(50);					//チャタリング回避
			++vel_sellct;
		}
		if(SW2_ONOFF() == SW_ON ){
			HAL_Delay(50);					//チャタリング回避
			while(SW2_ONOFF() == SW_ON);	//スイッチが離されるまで待つ
			Buzz_Set(2,50);
			HAL_Delay(50);					//チャタリング回避
			break;
		}
		//モードオーバーフロー処理と表示
		if(vel_sellct > 10){vel_sellct = 0;}

		printf("<vel_sellct> vel_sellct: 0.%d[m/s] \r\n", vel_sellct);
		printf("%c[0J", 0x1b);
		printf("%c[%dA", 0x1b, 1);

	}

	//加速度・速度設定
	switch( vel_sellct ) {
		case 0: Vel_Set(0.0f);	Acc_Set( ACCEL );	break;
		case 1: Vel_Set(0.1f);	Acc_Set( ACCEL );	break;
		case 2: Vel_Set(0.2f);	Acc_Set( ACCEL );	break;
		case 3: Vel_Set(0.3f);	Acc_Set( ACCEL );	break;
		case 4: Vel_Set(0.4f);	Acc_Set( ACCEL );	break;
		case 5: Vel_Set(0.5f);	Acc_Set( ACCEL );	break;
		case 6: Vel_Set(0.6f);	Acc_Set( ACCEL );	break;
		case 7: Vel_Set(0.7f);	Acc_Set( ACCEL );	break;
		case 8: Vel_Set(0.8f);	Acc_Set( ACCEL );	break;
		case 9: Vel_Set(0.9f);	Acc_Set( ACCEL );	break;
		case 10: Vel_Set(1.0f);	Acc_Set( ACCEL );	break;
	}


	//加速度・速度設定
/*	switch( vel_sellct ) {
		case 0: Vel_Set(0.0f);	Acc_Set( ACCEL );	break;
		case 1: Vel_Set(0.2f);	Acc_Set( ACCEL );	break;
		case 2: Vel_Set(0.4f);	Acc_Set( ACCEL );	break;
		case 3: Vel_Set(0.6f);	Acc_Set( ACCEL );	break;
		case 4: Vel_Set(0.8f);	Acc_Set( ACCEL );	break;
		case 5: Vel_Set(1.0f);	Acc_Set( ACCEL );	break;
		case 6: Vel_Set(1.2f);	Acc_Set( ACCEL );	break;
		case 7: Vel_Set(1.4f);	Acc_Set( ACCEL );	break;
		case 8: Vel_Set(1.6f);	Acc_Set( ACCEL );	break;
	}
*/

	//ライントレース開始
	Servo_Enable();
	while(1){

		//電源監視
		Battery_LimiterVoltage();

		//ラインアウト監視
		LineOut_Cheker();

	}
}

/* ---------------------------------------------------------------
	ラインアウトの監視関数
--------------------------------------------------------------- */
void LineOut_Cheker( void )
{
	volatile uint8_t 	i;
	volatile uint8_t	line_out_cnt = 0;

	for( i = 0; i < 10; i++) {
		HAL_Delay(10);
		line_out_cnt += Get_LineOut_Flag();
	}

	if( line_out_cnt >= 10 ) {
		Vel_Set(0.0f);
		HAL_Delay(1000);
		Servo_Disable();
		while( 1 ) {
			LED_TOGGLE();
			HAL_Delay(300);
		}
	}
	else{ line_out_cnt = 0; }
}
