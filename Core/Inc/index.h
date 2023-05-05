
#ifndef INC_INDEX_H_
#define INC_INDEX_H_

#include "main.h"
#include "stm32f3xx_hal.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* 便利な定数群 */
#define 	G				(9.80665f)					//	重量加速度[m/s^2]
#define 	PI				(3.1415926f)				//	円周率
#define 	SQRT2			(1.41421356237f)			//	ルート2
#define 	SQRT3			(1.73205080757f)			//	ルート3
#define 	SQRT5			(2.2360679775f)				//	ルート5
#define 	SQRT7			(2.64575131106f)			//	ルート7

/* 便利なマクロ関数群 */
#define 	DEG2RAD(x)		(((x)/180.0f)*PI)			//	度数法からラジアンに変換
#define 	RAD2DEG(x)		(180.0f*((x)/PI))			//	ラジアンから度数法に変換
#define	 	SWAP(a, b) 		((a != b) && (a += b, b = a - b, a -= b))
#define 	ABS(x) 			((x) < 0 ? -(x) : (x))		//	絶対値
#define 	SIGN(x)			((x) < 0 ? -1 : 1)			//	符号
#define 	MAX(a, b) 		((a) > (b) ? (a) : (b))		//	2つのうち大きい方を返します
#define 	MIN(a, b) 		((a) < (b) ? (a) : (b))		//	2つのうち小さい方を返します
#define 	MAX3(a, b, c) 	((a) > (MAX(b, c)) ? (a) : (MAX(b, c)))
#define 	MIN3(a, b, c) 	((a) < (MIN(b, c)) ? (a) : (MIN(b, c)))

/* LED関数群 */
#define		LED_ON()		HAL_GPIO_WritePin( LED_GPIO_Port, LED_Pin, GPIO_PIN_SET)		// LEDを点灯する
#define 	LED_OFF()		HAL_GPIO_WritePin( LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET)		// LEDを消灯する
#define 	LED_TOGGLE()	HAL_GPIO_TogglePin(GPIOB, LED_Pin)								// LEDの点灯と消灯を切り替える

/* スイッチ関数群 */
#define 	SW1_ONOFF()		HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin)						// 	スイッチが押されるとローが返ってくる
#define 	SW2_ONOFF()		HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin)						// 	スイッチが押されるとローが返ってくる

/* ブザー関数群 */
#define 	BUZZ_ON()		HAL_GPIO_WritePin( BUZZ_GPIO_Port, BUZZ_Pin, GPIO_PIN_SET)		// 	ブザー鳴らす
#define 	BUZZ_OFF()		HAL_GPIO_WritePin( BUZZ_GPIO_Port, BUZZ_Pin, GPIO_PIN_RESET)	// 	ブザーを止める

/* UART通信関数群(communicate.c) */
/* 自動生成されたsyscalls.cをSrcファイルに移し、stdio.hをインクルードすることでprintfやscanfも使用可能 */
void 		Communicate_TerminalSend( uint8_t );	// 1文字送信
uint8_t 	Communicate_TerminalRecv( void );		// 1文字受信
void 		Communicate_Initialize( void );			// printfとscanfを使用するための設定
void 		Communicate_ClearScreen( void );		// 画面クリア&カーソル初期化

/* モータ関数群(motor.c) */
// モータの回転方向とPWMのデューティ設定
#define		MOT_SET_FORWARD_L()		HAL_GPIO_WritePin( MT_L_DIR_GPIO_Port, MT_L_DIR_Pin, GPIO_PIN_SET)
#define		MOT_SET_REVERSE_L()		HAL_GPIO_WritePin( MT_L_DIR_GPIO_Port, MT_L_DIR_Pin, GPIO_PIN_RESET)
#define		MOT_SET_FORWARD_R()		HAL_GPIO_WritePin( MT_R_DIR_GPIO_Port, MT_R_DIR_Pin, GPIO_PIN_RESET)
#define		MOT_SET_REVERSE_R()		HAL_GPIO_WritePin( MT_R_DIR_GPIO_Port, MT_R_DIR_Pin, GPIO_PIN_SET)

#define		MOT_INIT()				HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1); HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1) // モータ駆動用タイマーの開始
#define		MOT_SET_COMPARE_L(x)	__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, x)				// 左モーターPWMセット
#define		MOT_SET_COMPARE_R(x)	__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, x)				// 右モーターPWMセット
#define		MOT_STOP_PWM()			MOT_SET_COMPARE_L(0x0000); MOT_SET_COMPARE_R(0x0000)			// 左右モータを停止
void 		Motor_SetDuty_Left( int16_t );			// 左モータを指定したDutyで回転させる[-1000~1000]
void 		Motor_SetDuty_Right( int16_t );			// 右モータを指定したDutyで回転させる[-1000~1000]

/* エンコーダ関数群(encoder.c) */
#define		ENC_CNT_L				(TIM1 -> CNT)	// 左エンコーダのタイマカウンタ
#define 	ENC_CNT_R				(TIM2 -> CNT)	// 右エンコーダのタイマカウンタ
#define		ENC_RESOLUTION 			(560)			// エンコーダの分解能
#define		ENC_INIT()				HAL_TIM_Encoder_Start( &htim1, TIM_CHANNEL_ALL ); HAL_TIM_Encoder_Start( &htim2, TIM_CHANNEL_ALL )
void 		Encoder_Reset_Left( void );				// 左エンコーダのリセット
void		Encoder_Reset_Right( void );			// 右エンコーダのリセット
float		Encoder_GetDist_Left( void );			// 左タイヤの走行距離を取得する[rad]
float		Encoder_GetDist_Right( void );			// 右タイヤの走行距離を取得する[rad]

/* センサ・バッテリAD関数群(sensor_adc.c) */
void 		Sensor_Initialize( void );				// AD変換の初期設定
uint16_t	Sensor_GetValue_R2( void );				// ラインセンサR2のAD値を取得する
uint16_t	Sensor_GetValue_R1( void );				// ラインセンサR1のAD値を取得する
uint16_t	Sensor_GetValue_L2( void );				// ラインセンサL2のAD値を取得する
uint16_t	Sensor_GetValue_L1( void );				// ラインセンサL1のAD値を取得する
uint16_t	Sensor_GetValue_ML( void );				// マーカーセンサ左のAD値を取得する
uint16_t	Sensor_GetValue_MR( void );				// マーカーセンサ右のAD値を取得する
uint16_t	Sensor_GetValue_Battery( void );		// 電源電圧のAD値を取得する

/* バッテリー関数群(battery.c) */
#define		BATTERY_REFERENCE	(3.3f)				// AD変換の基準電圧
#define 	BATTERY_LIMIT		(4.0f)				// 下限電圧設定
#define		BATTERY_OFFSET		(0.17f)				// 計測電圧の誤差補正
float		Battery_GetVoltage( void );				// バッテリの電圧を取得する[V]
void		Battery_LimiterVoltage( void );			// バッテリの電圧が4V以下になると起動しないように制限する

/* 割り込み制御系関数群(control.c) */
void		Interrupt_SysTick( void );				// SysTickによる1ms周期で割り込む処理関数
float		Interrupt_GetBootTime( void );			// マイコンが起動してから経過した時間を取得する[s]
void		Servo_Enable( void );
void		Servo_Disable( void );
void		Control_VariableReset( void );
void		Acc_Set( float a );
void		Vel_Set( float v );
void		Buzz_Set( uint16_t, uint16_t);
float 		Get_Dist( void );
float		Get_Vel( void );
uint8_t		Get_LineOut_Flag( void );
uint8_t		Get_MarkerState_L( void );
uint8_t		Get_MarkerState_R( void );

/* モジュールテスト関数群(module_test.c) */
void 		Module_Test( void );					// 全モジュールの動作確認用テスト関数

/* ライントレースロボット関数軍(linetracer.c) */
void 		Line_Tracer( void );
void 		LineOut_Cheker( void );

/* ロボトレーサー関数軍(robotracer.c) */
void 		Robo_Tracer( void );
void		Search_Run( void );
uint8_t 	Read_Marker( uint8_t );
void 		Marker_Write( float, float, float );
void 		Make_DriveCom( uint8_t );
void		Command_Run( uint8_t goal_num );

#endif /* INC_INDEX_H_ */
