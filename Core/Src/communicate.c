
#include "index.h"
#include <stdio.h>

#define _BS				0x08	// バックスペース
#define _ESC			0x1b	// エスケープシーケンス
#define TRX_BUFFER_SIZE 128		// 送受信バッファサイズ

struct {
    volatile uint16_t 	head;
    volatile uint16_t	tail;
    volatile uint16_t	remain;
    uint8_t	data[TRX_BUFFER_SIZE];
} tx_buffer, rx_buffer; // FIFOバッファ

uint8_t	tx_data;
uint8_t	rx_data;

/* ---------------------------------------------------------------
	UART1で1文字受信する関数
--------------------------------------------------------------- */
uint8_t Communicate_TerminalRecv( void )
{
	uint8_t data[1];
	HAL_UART_Receive( &huart2, (uint8_t*)data, sizeof(data), 1 );
	return (*data);
}

void Communicate_RxPushData( void )
{
// head（DMACが受信データを書き込む位置）に新しく受信データが蓄積される．
// tailから読み出せばいい
// headがtailに追いつくとバッファオーバーフローとなり正しく読み出せない

	// これ以上格納できない場合はそのまま戻る
	if(rx_buffer.remain >= TRX_BUFFER_SIZE){
		return;
	}

	rx_buffer.data[rx_buffer.head++] = rx_data;	// 書き込みポインタにデータを格納
	rx_buffer.remain++;
	// 終端に来たら、先頭に戻る
	if( rx_buffer.head >= TRX_BUFFER_SIZE ) {
		rx_buffer.head = 0;
	} else;
}

uint8_t Communicate_RxPopData( void )
{
	uint8_t ch;

	// この関数は多重に実行されるとまずいので割り込みを禁止する
	__disable_irq();

	// データがない場合
	if( rx_buffer.remain == 0 ) {
		// 割り込み許可
		__enable_irq();

		// データを受信するまで待機
		while( rx_buffer.remain == 0 ) {
			HAL_UART_Receive_DMA( &huart2, &rx_data, 1 );
		}
	} else;

	// 読み出しデータの取り出し
	ch = rx_buffer.data[rx_buffer.tail++];
	rx_buffer.remain--;

	// 終端に来たら先頭に戻る
	if( rx_buffer.tail >= TRX_BUFFER_SIZE ) {
		rx_buffer.tail = 0;
	} else;

	// 割り込み許可
	__enable_irq();

	return ch;
}

/* ---------------------------------------------------------------
	UART1で1文字送信する関数
--------------------------------------------------------------- */
void Communicate_TxPushData( int8_t data )
{
// headに新しく追加する
// tailは次に送信されるデータを指す
// バッファに空きが無い（headがtailに追いついた）場合は待機する

	// バッファ内データ数をカウントし，空きがない場合待機する
	// バッファフルで待機しているときには割り込みを許可するためにwhileループになっている
	while(1) {
		// この関数は多重に実行されるとまずいので割り込みを禁止する
		__disable_irq();

		// DMAを一時的に停止
		HAL_DMA_Abort(huart2.hdmatx);

		// バッファに空きがあればループから抜ける
		if( tx_buffer.remain < TRX_BUFFER_SIZE ) {
			break;
		} else;

		// DMA動作再開
		HAL_UART_Transmit_DMA(&huart2, &tx_data, 1);

		// 割り込み許可
		__enable_irq();

		// バッファに空きができるまで待機（この間割り込みが発生してもよい）
		while(tx_buffer.remain == TRX_BUFFER_SIZE);
	}
	// ここの時点でDMACは停止，割り込みは禁止されている

	// 書き込みポインタにデータを格納
	tx_buffer.data[tx_buffer.head++] = data;
	tx_buffer.remain++;
	// 終端に来たら、先頭に戻る
	if(tx_buffer.head >= TRX_BUFFER_SIZE){
		tx_buffer.head = 0;
	} else;

	// DMA動作再開
	HAL_UART_Transmit_DMA(&huart2, &tx_data, 1);

	// 割り込み許可
	__enable_irq();
}

void Communicate_TxPopData( void )
{
	// データがない場合
	if( tx_buffer.remain == 0 ) {
		// DMAを停止
		HAL_UART_DMAStop(&huart2);
	} else {
		// 読み出しデータの取り出し
		tx_data = tx_buffer.data[tx_buffer.tail++];
		tx_buffer.remain--;

		// 終端に来たら先頭に戻る
		if(tx_buffer.tail >= TRX_BUFFER_SIZE){
			tx_buffer.tail = 0;
		} else;

		// DMA動作再開
		HAL_UART_Transmit_DMA(&huart2, &tx_data, 1);
	}
}

/* ---------------------------------------------------------------
	受信・送信完了時のコールバック関数
--------------------------------------------------------------- */
void HAL_UART_RxCpltCallback( UART_HandleTypeDef *huart )
{
    if( huart->Instance == USART2 ) {
        Communicate_RxPushData();
    } else;
}

void HAL_UART_TxCpltCallback( UART_HandleTypeDef *huart )
{
    if( huart->Instance == USART2 ) {
        Communicate_TxPopData();
    } else;
}

/* ---------------------------------------------------------------
	printfとscanfを使用するための設定
--------------------------------------------------------------- */
void Communicate_Initialize( void )
{
	setbuf(stdout, NULL);
}

/* ---------------------------------------------------------------
	printfを使用するための設定
--------------------------------------------------------------- */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
PUTCHAR_PROTOTYPE
{
	Communicate_TxPushData(ch);
	return 1;
}

/* ---------------------------------------------------------------
	scanfを使用するための設定
--------------------------------------------------------------- */
#ifdef __GNUC__
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif /* __GNUC__ */
GETCHAR_PROTOTYPE
{
	//return Communicate_TerminalRecv();
	return Communicate_RxPopData();
}
