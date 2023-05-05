
#include "index.h"
#include "param.h"

#define SERVO_CYCLE_TIME	(10)

//　システム系処理
static float boot_time = 0.0f;

// 制御系変数
static float dist_L_now = 0.0f;
static float dist_L_last = 0.0f;
static float dist_R_now = 0.0f;
static float dist_R_last = 0.0f;
static float dist_C_now = 0.0f;
static float dist_C_last = 0.0f;

static float accel = 0.0f;
static float vel_L = 0.0f;
static float vel_R = 0.0f;
static float vel_C = 0.0f;
static float omega = 0.0f;
static float curvature = 0.0f;

static float vel_set = 0.0f;
static float vel_target = 0.0f;
static float vel_error_p = 0.0f;
static float vel_error_p_last = 0.0f;
static float vel_error_i = 0.0f;
static float vel_error_d = 0.0f;
static float vel_control = 0.0f;

static float omega_set = 0.0f;
static float omega_target = 0.0f;
static float omega_error_p = 0.0f;
static float omega_error_p_last = 0.0f;
static float omega_error_i = 0.0f;
static float omega_error_d = 0.0f;
static float omega_control = 0.0f;

static float vel_L_target = 0.0f;
static float vel_R_target = 0.0f;

static float line_diff_dist = 0.0f;
static int16_t line_diff = 0;

static uint16_t ls_L2 = 0;
static uint16_t ls_L1 = 0;
static uint16_t ls_R1 = 0;
static uint16_t ls_R2 = 0;
static uint16_t mk_L = 0;
static uint16_t mk_R = 0;

static int16_t duty_L;
static int16_t duty_R;

static uint16_t buzz_cnt = 0;
static uint16_t buzz_times = 0;
static uint16_t buzz_set_ms = 0;

static uint8_t servo_cycle_cnt = 0;
static uint8_t servo_enable_flag = 0;

static uint8_t marker_state_L = 0;
static uint8_t marker_state_R = 0;

static uint8_t line_out_flag = 0;

/* ---------------------------------------------------------------
	SysTickによる1ms周期で割り込む処理関数
--------------------------------------------------------------- */
void Interrupt_SysTick( void )
{

	//LED_ON();

	// 	以下にメインの割り込み処理を記述する
	boot_time += 0.001f;		//呼び出されるたびに1ms追加

	//速度・即速度計算は10ms周期で行う
	if(servo_cycle_cnt == SERVO_CYCLE_TIME){

		//エンコーダ読み込み 走行距離計算
		dist_L_now = Encoder_GetDist_Left();
		dist_R_now = Encoder_GetDist_Right();
		dist_C_now = (dist_L_now + dist_R_now) / 2.0f;

		//現在速度・角速度計算
		vel_L = (dist_L_now - dist_L_last) / 0.01f;
		vel_R = (dist_R_now - dist_R_last) / 0.01f;
		vel_C = (dist_C_now - dist_C_last) / 0.01f;
		omega = (vel_R - vel_L)/ ((float)TRED * 2.0f);

		//曲率計算　1/r 旋回半径の逆数を使って無限大演算を防ぐ
		curvature = (vel_R - vel_L) / ((vel_R + vel_L) * (float)TRED );

		//距離値再代入
		dist_L_last = dist_L_now;
		dist_R_last = dist_R_now;
		dist_C_last = dist_C_now;

		servo_cycle_cnt = 0;	//計算周期カウンタクリア

	}
	else{ ++servo_cycle_cnt; }

	//ラインセンサを読み込んで正規化処理
	ls_L1 = (Sensor_GetValue_L1() - LS_L1_MIN) * AD_RESOLUTION / (LS_L1_MAX - LS_L1_MIN);
	ls_R1 = (Sensor_GetValue_R1() - LS_R1_MIN) * AD_RESOLUTION / (LS_R1_MAX - LS_R1_MIN);
	ls_L2 = (Sensor_GetValue_L2() - LS_L2_MIN) * AD_RESOLUTION / (LS_L2_MAX - LS_L2_MIN);
	ls_R2 = (Sensor_GetValue_R2() - LS_R2_MIN) * AD_RESOLUTION / (LS_R2_MAX - LS_R2_MIN);
	mk_L  = (Sensor_GetValue_ML() - MK_L_MIN) * AD_RESOLUTION / (MK_L_MAX - MK_L_MIN);
	mk_R  = (Sensor_GetValue_MR() - MK_R_MIN) * AD_RESOLUTION / (MK_R_MAX - MK_R_MIN);

	//ラインセンサ取得値の最小側のガード
	if(ls_L1 < AD_MIM){ ls_L1 = AD_MIM; }
	if(ls_R1 < AD_MIM){ ls_R1 = AD_MIM; }
	if(ls_L2 < AD_MIM){ ls_L2 = AD_MIM; }
	if(ls_R2 < AD_MIM){ ls_R2 = AD_MIM; }
	if(mk_L < AD_MIM){ mk_L = AD_MIM; }
	if(mk_R < AD_MIM){ mk_R = AD_MIM; }

	//ラインセンサ取得値の最大側のガード
	if(ls_L1 >= AD_MAX){ ls_L1 = AD_MAX; }
	if(ls_R1 >= AD_MAX){ ls_R1 = AD_MAX; }
	if(ls_L2 >= AD_MAX){ ls_L2 = AD_MAX; }
	if(ls_R2 >= AD_MAX){ ls_R2 = AD_MAX; }
	if(mk_L >= AD_MAX){ mk_L = AD_MAX; }
	if(mk_R >= AD_MAX){ mk_R = AD_MAX; }

	//ラインアウトフラグ処理
	if((ls_L2 < LINE_OUT_AD) && (ls_L1< LINE_OUT_AD) && (ls_R1 < LINE_OUT_AD) && (ls_R2 < LINE_OUT_AD)){
		line_out_flag = 1;
	}
	else{ line_out_flag = 0; }

	//左右マーカーセンサの状態処理
	if( mk_L >= MK_L_THRESHOLD ){ marker_state_L = MK_LEFT; }
	else{ marker_state_L = 0; }
	if( mk_R >= MK_R_THRESHOLD ){ marker_state_R = MK_RIGHT; }
	else{ marker_state_R = 0; }

	//ライン差分制御量計算
	line_diff = ( ls_L1 - ls_R1 ) + 2 * ( ls_L2 - ls_R2 );	//ライン差分生値
	line_diff_dist = (float)LINE_DIFF( line_diff );			//ライン差分距離換算

	//機体サーボ系記述
	if(servo_enable_flag == 1){

		//目標速度の上下ガード
		if(vel_set >  VEL_MAX){ vel_set = VEL_MAX; }	//入力速度の最大ガード
		if(vel_set <= VEL_MIN){ vel_set = VEL_MIN; }	//入力速度の最小ガード

		//目標速度の加減速処理
		if( vel_target == vel_set ){
			vel_target = vel_set;
		}
		else if( vel_target < vel_set ){
			vel_target = vel_target + accel * 0.001f;	// V =  V + att
		}
		else if( vel_target > vel_set ){
			vel_target = vel_target - accel * 0.001f;	// V =  V - att
		}

		//目標角速度計算
		omega_set = ((2.0f * line_diff_dist)/(WHEEL_BASE * WHEEL_BASE) - (line_diff_dist * line_diff_dist)) * vel_C;
		omega_target = 2.0f * omega_set; //SSMレート比の倍という隠し味

		//左右輪目標速度再計算
		vel_L_target = vel_target - (TRED/2.0f) * omega_target;
		vel_R_target = vel_target + (TRED/2.0f) * omega_target;

		//速度制御量計算
		vel_error_p = vel_target - vel_C;
		vel_error_i += vel_error_p;
		vel_error_d = vel_error_p - vel_error_p_last;
		vel_error_p_last = vel_error_p;

		//速度制御 積算制御量，上下ガード
		if( vel_error_i >= PWM_MOT_MAX){ vel_error_i = PWM_MOT_MAX; }
		else if( vel_error_i <= -PWM_MOT_MAX){ vel_error_i = -PWM_MOT_MAX; }

		//速度制御量計算の上下ガード
		if( vel_control >= PWM_MOT_MAX){ vel_control = PWM_MOT_MAX; }
		else if( vel_control <= -PWM_MOT_MAX){ vel_control = -PWM_MOT_MAX; }

		//角速度制御量計算
		omega_error_p = omega_target - omega;
		omega_error_i += omega_error_p;
		omega_error_d = omega_error_p - omega_error_p_last;
		omega_error_p_last = omega_error_p;

		//角速度制御 積算制御量，上下ガード
		if( omega_error_i >= PWM_MOT_MAX){ omega_error_i = PWM_MOT_MAX; }
		else if( omega_error_i <= -PWM_MOT_MAX){ omega_error_i = -PWM_MOT_MAX; }

		//角速度制御量計算の上下ガード
		if( omega_control >= PWM_MOT_MAX){ omega_control = PWM_MOT_MAX; }
		else if( omega_control <= -PWM_MOT_MAX){ omega_control = -PWM_MOT_MAX; }

		//速度制御量・角速度制御量　ゲイン演算
		vel_control = vel_error_p * V_KP + vel_error_i * V_KI + vel_error_d * V_KD;
		omega_control = omega_error_p * W_KP + omega_error_i * W_KI + omega_error_d * W_KD;

		//左右輪に制御量の代入
		duty_L = MOT_FF_DUTY_L( vel_L_target ) + (int16_t)vel_control - (int16_t)omega_control;
		duty_L = SIGN(duty_L) * MIN( PWM_MOT_MAX, ABS(duty_L) );
		Motor_SetDuty_Left(duty_L);

		duty_R = MOT_FF_DUTY_R( vel_R_target ) + (int16_t)vel_control + (int16_t)omega_control;
		duty_R = SIGN(duty_R) * MIN( PWM_MOT_MAX, ABS(duty_R) );
		Motor_SetDuty_Right(duty_R);

	}

	//ブザー制御
	if(buzz_times > 0){					//	ブザー使用指示回数が0以上のとき
		BUZZ_ON();						//	ブザー鳴らす
		if(buzz_cnt >= buzz_set_ms){	//	指定時間待ち
			BUZZ_OFF();					//	ブザー落とす
		}
		if(buzz_cnt >= buzz_set_ms*2 ){	//	指定時間待ち
			buzz_cnt = 0;				//	割り込みブザーカウントインクリメントクリア
			--buzz_times;				//	ブザー使用回数デクリメント
		}
		else{++buzz_cnt;}				//	ブザーのカウントインクリメント
	}

	//LED_OFF();
}

/* ---------------------------------------------------------------
	マイコン起動時からの経過時間を取得する関数
--------------------------------------------------------------- */
float Interrupt_GetBootTime( void )
{
	return boot_time;
}

/* ---------------------------------------------------------------
	制御系変数のクリア関数
--------------------------------------------------------------- */
void Control_VariableReset( void ){

	dist_L_now = 0.0f;
	dist_L_last = 0.0f;
	dist_R_now = 0.0f;
	dist_R_last = 0.0f;
	dist_C_now = 0.0f;
	dist_C_last = 0.0f;

	//accel = ACCEL;
	vel_L = 0.0f;
	vel_R = 0.0f;
	vel_C = 0.0f;
	omega = 0.0f;
	curvature = 0.0f;

	//vel_set = 0.0f;
	vel_target = 0.0f;
	vel_error_p = 0.0f;
	vel_error_p_last = 0.0f;
	vel_error_i = 0.0f;
	vel_error_d = 0.0f;
	vel_control = 0.0f;

	omega_set = 0.0f;
	omega_target = 0.0f;
	omega_error_p = 0.0f;
	omega_error_p_last = 0.0f;
	omega_error_i = 0.0f;
	omega_error_d = 0.0f;
	omega_control = 0.0f;

	vel_L_target = 0.0f;
	vel_R_target = 0.0f;

}

/* ---------------------------------------------------------------
	速度制御の設定関数
--------------------------------------------------------------- */
void Servo_Enable( void ) {

	Encoder_Reset_Left();
	Encoder_Reset_Right();
	Control_VariableReset();
	servo_enable_flag = 1;

}
void Servo_Disable( void ){
	servo_enable_flag = 0;
	Control_VariableReset();
	MOT_STOP_PWM();
}

/* ---------------------------------------------------------------
	加速度・速度設定の入力関数
--------------------------------------------------------------- */
void Acc_Set( float a ){ accel = a; }
void Vel_Set( float v ){ vel_set = v; }

/* ---------------------------------------------------------------
	現在距離のリターン関数
--------------------------------------------------------------- */
float Get_Dist(void){ return dist_C_now; }

/* ---------------------------------------------------------------
	現在速度のリターン関数
--------------------------------------------------------------- */
float Get_Vel(void){ return vel_C; }

/* ---------------------------------------------------------------
	ラインアウトフラグのリターン関数
--------------------------------------------------------------- */
uint8_t	Get_LineOut_Flag(void){ return line_out_flag; }

/* ---------------------------------------------------------------
	マーカセンサ状態のリターン関数
--------------------------------------------------------------- */
uint8_t	Get_MarkerState_L(void){ return marker_state_L; }
uint8_t	Get_MarkerState_R(void){ return marker_state_R; }

/* ---------------------------------------------------------------
	ブザーの駆動指令関数
--------------------------------------------------------------- */
void Buzz_Set( uint16_t times, uint16_t ms){

	buzz_times = times;
	buzz_set_ms = ms;

}
