/*
 * robotracer.c
 *
 *  Created on: 2020/06/02
 *      Author: masataka
 */

#include "index.h"
#include "param.h"
#include <stdio.h>

// 区間マップ情報格納用の構造体配列
#define		PARCEL_NUM	  200	// 区画数設定
struct parcel_map{
	float 	mk_dist_C;
	float 	mk_dist_L;
	float 	mk_dist_R;
	float	vel_set;
	float	corner_R;
}Parcel_map[PARCEL_NUM];

#define		MK_ERROR_DIST	0.033	//マーカ重複回避距離　3.3cm
#define		READ			0
#define		SEARCH			1

static float		accel = ACCEL;
static float		search_vel	= 0.5f;
static float		drive_vel_min = 0.5f;	// R10ターン実用速度 0.6m/s
static float		drive_vel_max = 0.6f;	// 実用最大速度 0.8m/s

static uint8_t		marker_cnt = 0;

/* ---------------------------------------------------------------
	ロボトレーサとしての関数
--------------------------------------------------------------- */
void Robo_Tracer( void )
{
	uint8_t run_sellct = 0;
	uint8_t goal_cnt = 0;

	//ロボトレース機能のループ
	while(1) {

		//機能選択のループ
		while (1) {

			//モードインクリメント処理
			if(SW1_ONOFF() == SW_ON){
				HAL_Delay(50);					//チャタリング回避
				while(SW1_ONOFF() == SW_ON);	//スイッチが離されるまで待つ
				Buzz_Set(1,50);
				HAL_Delay(50);					//チャタリング回避
				++run_sellct;
			}
			if(SW2_ONOFF() == SW_ON ){
				HAL_Delay(50);					//チャタリング回避
				while(SW2_ONOFF() == SW_ON);	//スイッチが離されるまで待つ
				Buzz_Set(2,50);
				HAL_Delay(50);					//チャタリング回避
				break;
			}
			//モードオーバーフロー処理と表示
			if(run_sellct > 5){run_sellct = 0;}

			printf("<run_sellct> run_sellct: %d \r\n", run_sellct);
			printf("%c[0J", 0x1b);
			printf("%c[%dA", 0x1b, 1);

		}

		//加速度・速度設定
		switch( run_sellct ) {
			case 0: //探索走行
				Search_Run();
				goal_cnt = marker_cnt;
				++run_sellct;
				break;

			case 1: //二次走行	パラメータ１
				accel = 2.0f;
				drive_vel_min = 0.5f;
				drive_vel_max = 0.6f;
				Command_Run(goal_cnt);
				++run_sellct;
				break;

			case 2: //二次走行	パラメータ２
				accel = 3.0f;
				drive_vel_min = 0.6f;
				drive_vel_max = 0.7f;
				Command_Run(goal_cnt);
				++run_sellct;
				break;

			case 3: //二次走行	パラメータ３
				accel = 4.0f;
				drive_vel_min = 0.6f;
				drive_vel_max = 0.75f;
				Command_Run(goal_cnt);
				break;

			case 4: break;	//経路書き込み
			case 5: break;	//経路読み出し
		}

	}
	while (1) { HAL_Delay(10); }

}

/* ---------------------------------------------------------------
	マーカーセンシング関数
--------------------------------------------------------------- */
uint8_t Read_Marker( uint8_t read_mode){

	static float dist_C = 0.0f;			//現在距離バッファ
	static float dist_L = 0.0f;			//左輪距離バッファ
	static float dist_R = 0.0f;			//右輪距離バッファ
	uint8_t cross_flag = 0;				//両マーカー判定フラグ
	uint8_t marker_state = 0;			//マーカー状態リターン変数

	// 左マーカー処理　左から入って右を見る
	if( (Get_MarkerState_L() == MK_LEFT) && (Get_MarkerState_R() != MK_RIGHT) ){
		dist_C = Get_Dist();      										// 現在距離代入
		while(Get_Dist() <= dist_C + 0.01){								// ＋1cm走行
			HAL_Delay(1);
		}
		if(Get_MarkerState_L() == MK_LEFT ){ cross_flag = 0; }		// 左マーカー有り！
		if(Get_MarkerState_R() == MK_RIGHT){ cross_flag = 1; }		// 両マーカー判定！

		//左マーカー確定 現在マーカー距離を保持
		if( cross_flag == 0 ){
			dist_C = Get_Dist();
			dist_L = Encoder_GetDist_Left();
			dist_R = Encoder_GetDist_Right();
		}

		if( read_mode == SEARCH ){
			//右マーカーがこないか最大4㎝待つ
			while(Get_Dist() <= dist_C + 0.04){
				if(Get_MarkerState_R() == MK_RIGHT){ cross_flag = 1; }		// 両マーカー判定！
			}
		}
		else if(read_mode == READ){
			//右マーカーがこないか最大1㎝待つ
			while(Get_Dist() <= dist_C + 0.01){
				if(Get_MarkerState_R() == MK_RIGHT){ cross_flag = 1; }		// 両マーカー判定！
			}
		}

		//左マーカー判定
		if(cross_flag == 0){
			if( read_mode == SEARCH ){
				Marker_Write(dist_C,dist_L,dist_R);		//マーカー距離を記録
			}
			Buzz_Set(1,50);
			marker_state = MK_LEFT;					//結果は左マーカー
		}
		//両マーカー判定
		else if(cross_flag == 1){
			marker_state = MK_CROSS;				//結果は十字交差
		}

		cross_flag = 0;								//両マーカーフラグクリア
	}

	// 右マーカー処理 右から入って，左を見る
	else if( (Get_MarkerState_L() != MK_LEFT) && (Get_MarkerState_R() == MK_RIGHT) ){
		dist_C = Get_Dist();      										// 現在距離代入
		while(Get_Dist() <= dist_C + 0.01){								// ＋1cm走行
			HAL_Delay(1);
		}
		if(Get_MarkerState_L() == MK_LEFT ){ cross_flag = 1; }		// 両マーカー判定！
		if(Get_MarkerState_R() == MK_RIGHT){ cross_flag = 0; }		// 右マーカー有り！

		//右マーカー確定 現在マーカー距離を保持
		if( cross_flag == 0 ){
			dist_C = Get_Dist();
			dist_L = Encoder_GetDist_Left();
			dist_R = Encoder_GetDist_Right();
		}

		if( read_mode == SEARCH ){
			//左マーカーがこないか最大4㎝待つ
			while(Get_Dist() <= dist_C + 0.04){
				if(Get_MarkerState_L() == MK_LEFT ){ cross_flag = 1; }		// 両マーカー判定！
			}
		}

		else if(read_mode == READ){
			//左マーカーがこないか最大1㎝待つ
			while(Get_Dist() <= dist_C + 0.01){
				if(Get_MarkerState_R() == MK_LEFT){ cross_flag = 1; }		// 両マーカー判定！
			}
		}

		//右マーカー判定
		if(cross_flag == 0){
			if( read_mode == SEARCH ){
				Marker_Write(dist_C,dist_L,dist_R);		//マーカー距離を記録
			}
			Buzz_Set(2,50);
			marker_state = MK_RIGHT;				//結果は左マーカー
		}
		//両マーカー判定
		else if(cross_flag == 1){
			marker_state = MK_CROSS;				//結果は十字交差
		}
		cross_flag = 0;								//両マーカーフラグクリア
	}

	//両マーカー処理
	else if( (Get_MarkerState_L() == MK_LEFT) && (Get_MarkerState_R() == MK_RIGHT) ){
		dist_C = Get_Dist();      										// 現在距離代入
		while(Get_Dist() <= dist_C + 0.01){								// ＋1cm走行
			HAL_Delay(1);
		}
		if(Get_MarkerState_L() == MK_LEFT ){ cross_flag = 1; }		// 両マーカー判定！
		if(Get_MarkerState_R() == MK_RIGHT){ cross_flag = 1; }		// 両マーカー判定！

		//両マーカー確定
		if( cross_flag == 1){
			marker_state = MK_CROSS;				//結果は十字交差
		}

		cross_flag = 0;								//両マーカーフラグクリア
	}

	return marker_state;
}

/* ---------------------------------------------------------------
	マーカー位置情報記録関数
--------------------------------------------------------------- */
void Marker_Write(float dist_c, float dist_l, float dist_r){

	++marker_cnt;									//区画数インクリメント
	Parcel_map[marker_cnt].mk_dist_C = dist_c;		//区画情報・距離代入
	Parcel_map[marker_cnt].mk_dist_L = dist_l;		//区画情報・左輪距離代入
	Parcel_map[marker_cnt].mk_dist_R = dist_r;		//区画情報・右輪距離代入

	// さっきとのマーカー距離判定  距離が短すぎるときはカウントしない
	if(Parcel_map[marker_cnt].mk_dist_C - Parcel_map[marker_cnt - 1].mk_dist_C <= MK_ERROR_DIST){
		--marker_cnt;		//区間処理をおこなわないので区間指標をデクリメント

	}
}

/* ---------------------------------------------------------------
	探索走行の関数
--------------------------------------------------------------- */
void Search_Run(void){

	//探索走行の加速度・速度の指示
	Acc_Set(accel);
	Vel_Set(search_vel);

	//ライントレース開始
	Servo_Enable();

	//スタートマーカ通過
	while(Read_Marker(SEARCH) != MK_RIGHT){
		//Battery_LimiterVoltage();	//電源監視
		//LineOut_Cheker();			//ラインアウト監視
		HAL_Delay(10);
	}

	//ゴールマーカー通過
	while(Read_Marker(SEARCH) != MK_RIGHT){
		//Battery_LimiterVoltage();	//電源監視
		//LineOut_Cheker();			//ラインアウト監視
		HAL_Delay(10);
	}

	HAL_Delay(200);
	Acc_Set(accel);
	Vel_Set(0.0f);
	HAL_Delay(2000);
	Servo_Disable();

}

/* ---------------------------------------------------------------
	コース解析，二次走行コマンド作成関数
--------------------------------------------------------------- */
void Make_DriveCom(uint8_t goal_cnt){

	float parcel_dist_C = 0.0f;
	float parcel_dist_L = 0.0f;
	float parcel_dist_R = 0.0f;
	uint8_t parcel_cnt = 1; 			//スタート区画以降の区画情報を作る

	//区画ログの中で角速度を解析し区画速度を決定する
	while( parcel_cnt < goal_cnt ){		//区画数はマーカの数-1

		//区間の旋回半径計算
		parcel_dist_C = Parcel_map[parcel_cnt + 1].mk_dist_C - Parcel_map[parcel_cnt].mk_dist_C;	//車体中心の区間走行距離
		parcel_dist_L = Parcel_map[parcel_cnt + 1].mk_dist_L - Parcel_map[parcel_cnt].mk_dist_L;	//左輪の区間走行距離
		parcel_dist_R = Parcel_map[parcel_cnt + 1].mk_dist_R - Parcel_map[parcel_cnt].mk_dist_R;	//右輪の区間走行距離

		Parcel_map[parcel_cnt].corner_R = (float)TRED * parcel_dist_C / (parcel_dist_L - parcel_dist_R);
		Parcel_map[parcel_cnt].corner_R = ABS(Parcel_map[parcel_cnt].corner_R);

		//旋回半径より区画速度を決定する
		if(Parcel_map[parcel_cnt].corner_R <= 0.1f ){
			Parcel_map[parcel_cnt].vel_set = drive_vel_min;
		}
		else if(Parcel_map[parcel_cnt].corner_R >= 0.1f && Parcel_map[parcel_cnt].corner_R < 0.15f){
			Parcel_map[parcel_cnt].vel_set = drive_vel_min;
		}
		else if(Parcel_map[parcel_cnt].corner_R >= 0.15f && Parcel_map[parcel_cnt].corner_R < 0.2f){
			Parcel_map[parcel_cnt].vel_set = drive_vel_min + 0.1f;
		}
		else if(Parcel_map[parcel_cnt].corner_R >= 0.2f && Parcel_map[parcel_cnt].corner_R < 0.4f){
			//Parcel_map[parcel_cnt].vel_set = drive_vel_min + 0.2f;
			Parcel_map[parcel_cnt].vel_set = drive_vel_max;
		}
		else if(Parcel_map[parcel_cnt].corner_R >= 0.4f && Parcel_map[parcel_cnt].corner_R < 0.7f){
			//Parcel_map[parcel_cnt].vel_set = drive_vel_min + 0.3f;
			Parcel_map[parcel_cnt].vel_set = drive_vel_max;
		}
		else if(Parcel_map[parcel_cnt].corner_R >= 0.7f && Parcel_map[parcel_cnt].corner_R < 1.0f){
			//Parcel_map[parcel_cnt].vel_set = drive_vel_min + 0.4f;
			Parcel_map[parcel_cnt].vel_set = drive_vel_max;
		}
		else{
			Parcel_map[parcel_cnt].vel_set = drive_vel_max;
		}

		//短い直線フィルタ
		if(parcel_dist_C <= 0.33){
			Parcel_map[parcel_cnt].vel_set = drive_vel_min;
		}
		++parcel_cnt;
	}

	//最終区画の走行速度設定
	Parcel_map[goal_cnt].vel_set = drive_vel_min;

}

/* ---------------------------------------------------------------
	指令走行・・・二次走行関数
--------------------------------------------------------------- */
void Command_Run(uint8_t goal_num){

	//コマンド生成
	Make_DriveCom( goal_num );

	//デバックコード
/*	uint8_t j;
	while(SW1_ONOFF() != SW_ON){

		while(SW2_ONOFF() != SW_ON){
			printf(" READY \r\n");
			HAL_Delay(200);
		}
		for(j=0; j<= goal_num; ++j){
			printf("%d R: %6.3f[m], V: %6.3f[m]\r\n", j, Parcel_map[j].corner_R, Parcel_map[j].vel_set);

			//printf("%d L: %6.3f[m], C: %6.3f[m] R: %6.3f[m]\r\n", i,
				//	Parcel_map[j].mk_dist_L,
					//Parcel_map[j].mk_dist_C,
					//Parcel_map[j].mk_dist_R
			//);
		}
	}
*/

	//スタート待ち
	while(SW2_ONOFF() != SW_ON){
		LED_TOGGLE();
		HAL_Delay(200);
	}

	LED_ON();
	Buzz_Set(2,50);
	HAL_Delay(1000);

	//ライントレース開始
	LED_OFF();
	Acc_Set(accel);
	Vel_Set(drive_vel_min);
	Servo_Enable();

	//スタートマーカ通過
	while(Read_Marker(READ) != MK_RIGHT){HAL_Delay(10);}

	//速度コマンド走行
	uint8_t i = 0;
	float parcel_dist = 0.0f;
	float temp_dist = 0.0f;
	float local_dist = 0.0f;
	float decel_dist = 0.0f;

	for(i = 1; i <= goal_num - 2; ++i){

		//区間距離の長さを計算
		parcel_dist = Parcel_map[i + 1].mk_dist_C - Parcel_map[i].mk_dist_C;
		temp_dist = Get_Dist();
		decel_dist = 0.0f;
		Vel_Set(Parcel_map[i].vel_set);

		//次の区間の方が速いとき
		if(Parcel_map[i].vel_set <= Parcel_map[i+1].vel_set){

			while(1){

				//区間ローカル距離演算
				local_dist = Get_Dist() - temp_dist;

				//区間距離を走行してからループ出る
				if(local_dist >= parcel_dist){break;}
			}
		}

		//次の区間が遅い時
		else if(Parcel_map[i].vel_set > Parcel_map[i+1].vel_set){

			while(1){

				//区間ローカル距離演算
				local_dist = Get_Dist() - temp_dist;

				//減速距離演算
				decel_dist = (Get_Vel() * Get_Vel() - Parcel_map[i+1].vel_set * Parcel_map[i+1].vel_set) / (2.0f * accel);

				//減速開始距離になったら次の区間速度にしてループ出る
				if(local_dist + decel_dist >= parcel_dist - 0.05f){		//減速開始距離待ち
					Vel_Set(Parcel_map[i+1].vel_set);					//減速指令
					break;
				}
			}

			//左区間切れマーカー待ち
			LED_ON();

			while(Read_Marker(READ) != MK_LEFT){

				//区間ローカル距離演算
				local_dist = Get_Dist() - temp_dist;

				//ただし，ローカル走行距離が，区間距離を超えたら強制的に次の区画へ行く
				if(local_dist >= parcel_dist + 0.05f){break;}
			}

			LED_OFF();
		}

	}

	//ゴールマーカー通過
	LED_ON();
	while(Read_Marker(READ) != MK_RIGHT){HAL_Delay(10);}
	LED_OFF();

	HAL_Delay(200);
	Acc_Set(accel);
	Vel_Set(0.0f);
	HAL_Delay(2000);
	Servo_Disable();

}
