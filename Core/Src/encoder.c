
#include "index.h"
#include "param.h"

static uint16_t	cnt_now_L = 0;
static uint16_t	cnt_last_L = 0;
static int16_t	diff_cnt_L = 0;

static uint16_t	cnt_now_R = 0;
static uint16_t	cnt_last_R = 0;
static int16_t	diff_cnt_R = 0;

static float	angle_L = 0.0f;
static float	angle_R = 0.0f;

/* ---------------------------------------------------------------
	左タイヤの走行距離を取得する関数[rad]
--------------------------------------------------------------- */
float Encoder_GetDist_Left( void )
{
	cnt_now_L = ENC_CNT_L;
	diff_cnt_L = (int16_t)(cnt_now_L - cnt_last_L);
	cnt_last_L = cnt_now_L;
	angle_L += (2 * PI * (float)(diff_cnt_L) / (float)ENC_RESOLUTION);

	return(TIRE_DIAMETER * PI* ( RAD2DEG(angle_L) / 360.0f));
}

/* ---------------------------------------------------------------
	右タイヤの走行距離を取得する関数[m]
--------------------------------------------------------------- */
float Encoder_GetDist_Right( void )
{
	cnt_now_R = ENC_CNT_R;
	diff_cnt_R = (int16_t)(cnt_now_R - cnt_last_R);
	cnt_last_R = cnt_now_R;
	angle_R += (2 * PI * (float)(diff_cnt_R) / (float)ENC_RESOLUTION);

	return(TIRE_DIAMETER * PI* ( RAD2DEG(angle_R) / 360.0f));
}

/* ---------------------------------------------------------------
	左エンコーダをリセットするする関数
--------------------------------------------------------------- */
void Encoder_Reset_Left( void )
{
	__HAL_TIM_SET_COUNTER(&htim1, 0x0000);
	cnt_now_L = 0;
	cnt_last_L = 0;
	diff_cnt_L = 0;
	angle_L = 0.0f;
}

/* ---------------------------------------------------------------
	右エンコーダをリセットするする関数
--------------------------------------------------------------- */
void Encoder_Reset_Right( void )
{
	__HAL_TIM_SET_COUNTER(&htim2, 0x0000);
	cnt_now_R = 0;
	cnt_last_R = 0;
	diff_cnt_R = 0;
	angle_R = 0.0f;
}
