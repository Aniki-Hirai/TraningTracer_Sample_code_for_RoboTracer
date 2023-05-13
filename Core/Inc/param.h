/*
 * index.h
 *
 *  Created on: 2020/06/02
 *      Author: masataka
 */

#ifndef INC_PARAM_H_
#define INC_PARAM_H_

#include "main.h"
#include "index.h"

/* 明示的な定数 */
#define 	SW_ON			0	// スイッチオン
#define 	SW_OFF			1	// スイッチオフ

#define		MK_NULL			0	// マーカー無し
#define		MK_LEFT			1	// マーカー左読み
#define		MK_RIGHT		2	// マーカー右読み
#define		MK_CROSS		3	// 十字交差読み

/* 機体固有パラメータ*/
#define		TIRE_DIAMETER	(20.8 * 0.001)	//	タイヤ直径[m]
#define		GEAR_RATIO		20				//	ギヤ比
#define		WHEEL_BASE		(75.0 * 0.001)	//	仮想ホイールベース　車軸からラインセンサまでの距離
#define		TRED			(82	* 0.001)	//	車体トレッド[m]

#define		AD_RESOLUTION	4096
#define		AD_MAX			4096
#define		AD_MIM			0

#define		PWM_MOT_MAX		950
#define		PWM_MOT_MIN		0

#define 	ACCEL			1.0 	//m/ss

#define 	VEL_MAX			1.5		//m/s
#define 	VEL_MIN			-1.0	//m/s

#define 	OMEGA_MAX		DEG2RAD(360)
#define 	OMEGA_MIN		-DEG2RAD(360)

#define 	V_KP			1000.0
#define 	V_KI			10.0
#define 	V_KD			0.0		//未使用

#define 	W_KP			350.0
#define 	W_KI			0.0		//未使用
#define 	W_KD			35.0

/*
#define 	V_KP			150.0
#define 	V_KI			50.0
#define 	V_KD			0.0		//未使用

#define 	W_KP			250.0
#define 	W_KI			0.0		//未使用
#define 	W_KD			10.0		//未使用
*/

/* ラインセンサ用パラメータ */
// 自宅
/*#define 	LS_L1_MAX		1950
#define 	LS_L1_MIN 		80
#define 	LS_R1_MAX		1980
#define 	LS_R1_MIN		80

#define 	LS_L2_MAX		1840
#define 	LS_L2_MIN		70
#define 	LS_R2_MAX		1780
#define 	LS_R2_MIN		70

#define 	MK_L_MAX		380
#define 	MK_L_MIN		0
#define 	MK_R_MAX		380
#define 	MK_R_MIN		0
*/

/*#define 	LS_L1_MAX		2050
#define 	LS_L1_MIN 		40
#define 	LS_R1_MAX		2016
#define 	LS_R1_MIN		40

#define 	LS_L2_MAX		2200
#define 	LS_L2_MIN		40
#define 	LS_R2_MAX		1990
#define 	LS_R2_MIN		40

#define 	MK_L_MAX		420
#define 	MK_L_MIN		0
#define 	MK_R_MAX		420
#define 	MK_R_MIN		0
*/

/* 中部 */
/*#define 	LS_L1_MAX		1850
#define 	LS_L1_MIN 		50
#define 	LS_R1_MAX		2100
#define 	LS_R1_MIN		50

#define 	LS_L2_MAX		2200
#define 	LS_L2_MIN		50
#define 	LS_R2_MAX		1800
#define 	LS_R2_MIN		50

#define 	MK_L_MAX		420
#define 	MK_L_MIN		0
#define 	MK_R_MAX		420
#define 	MK_R_MIN		0
*/

/* 関西 */
#define 	LS_L1_MAX		2250
#define 	LS_L1_MIN 		0
#define 	LS_R1_MAX		2350
#define 	LS_R1_MIN		0

#define 	LS_L2_MAX		2200
#define 	LS_L2_MIN		0
#define 	LS_R2_MAX		2050
#define 	LS_R2_MIN		0

#define 	MK_L_MAX		420
#define 	MK_L_MIN		0
#define 	MK_R_MAX		420
#define 	MK_R_MIN		0

#define		LINE_OUT_AD		1000

#define		LS_L1_THRESHOLD	(LS_L1_MAX - LS_L1_MIN)/2 + LS_L1_MIN	//	左1ラインセンサ閾値
#define		LS_L2_THRESHOLD	(LS_L2_MAX - LS_L2_MIN)/2 + LS_L2_MIN	//	左2ラインセンサ閾値
#define		LS_R1_THRESHOLD	(LS_R1_MAX - LS_R1_MIN)/2 + LS_R1_MIN	//	右１ラインセンサ閾値
#define		LS_R2_THRESHOLD	(LS_R2_MAX - LS_R2_MIN)/2 + LS_R2_MIN	//	右２ラインセンサ閾値
#define		MK_L_THRESHOLD	(MK_L_MAX - MK_L_MIN)/2 + MK_L_MIN		//	左マーカーセンサ閾値
#define		MK_R_THRESHOLD	(MK_R_MAX - MK_R_MIN)/2 + MK_R_MIN		//	右マーカーセンサ閾値

/* ラインセンサ距離計算パラメータ 注：別途計測結果から算出 */
#define		LINE_DIFF(x)		(0.0000033449 * x + 0.0008896966)

/* モーターFFパラメータ 注：別途計測結果から算出 */
#define 	MOT_FF_DUTY_L(x)	(1192.7 * x + 4.4869)
#define 	MOT_FF_DUTY_R(x)	(1218.8 * x + 0.6384)

#endif /* INC_PARAM_H_ */
