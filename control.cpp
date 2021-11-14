#include "DxLib.h"
#include "control.h"


//////////キーボードの入力状態を格納する////////////////

int Key[256]; // キーが押されているフレーム数を格納する

			  // キーの入力状態を更新する
int UpdateKey() {
	char tmpKey[256]; // 現在のキーの入力状態を格納する
	GetHitKeyStateAll(tmpKey); // 全てのキーの入力状態を得る
	for (int i = 0; i<256; i++) {
		if (tmpKey[i] != 0) { // i番のキーコードに対応するキーが押されていたら
			Key[i]++;     // 加算
		}
		else {              // 押されていなければ
			Key[i] = 0;   // 0にする
		}
	}
	return 0;
}

void mouse_limit(int& hand_x, const int hand_y) {
	if (hand_x < 0 && hand_y > 0 && hand_y < 480) {
		hand_x = 0;
		SetMousePoint(0, hand_y);//マウスカーソルの位置
	}
	else if (hand_x > 640 && hand_y > 0 && hand_y < 480) {
		hand_x = 640;
		SetMousePoint(640, hand_y);//マウスカーソルの位置
	}
}

///////////マウスの入力状態を返す//////////////
static int left_cnt = 0;
static int right_cnt = 0;
void mouse_click() {
	if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0) { left_cnt++; }
	else { left_cnt = 0; }
	if (GetMouseInput() == MOUSE_INPUT_RIGHT) { right_cnt++; }
	else { right_cnt = 0; }
}
int left_click() {
	return left_cnt;
}
int right_click() {
	return right_cnt;
}


//左右の移動
int control_stick()
{
	if (Key[KEY_INPUT_RIGHT] >= 1) { // 右キーが押されていたら
		return RIGHT_KEY;
	}
	if (Key[KEY_INPUT_LEFT] >= 1) { // 左キーが押されていたら
		return LEFT_KEY;
	}
	if (Key[KEY_INPUT_DOWN] >= 1) { // 下キーが押されていたら
		return DOWN_KEY;
	}
	if (Key[KEY_INPUT_UP] >= 1) { // 下キーが押されていたら
		return UP_KEY;
	}
	else {
		return FALSE;
	}
}

//ジャンプ
int control_space()
{
	if (Key[KEY_INPUT_SPACE] == 1) { // スペースキーが押されたらジャンプ
		return SPACE_KEY;
	}
	if (Key[KEY_INPUT_SPACE] == 60) {//スペースキー長押し
		return SPACE_KEY_LONG;
	}
	else {
		return FALSE;
	}
}

//攻撃
int control_attack()
{
	// スペースキーもしくはAキーが押されたら斬撃
	if (Key[KEY_INPUT_A] == 1 || Key[KEY_INPUT_SPACE] == 1) {
		return ZANGEKI_KEY;
	}
	else {
		return FALSE;
	}
}

//選択画面に使う
int control_focus()
{
	if (Key[KEY_INPUT_RIGHT] == 1) { // 右キーが押されたら
		return RIGHT_KEY;
	}
	if (Key[KEY_INPUT_LEFT] == 1) { // 左キーが押されたら
		return LEFT_KEY;
	}
	if (Key[KEY_INPUT_UP] == 1) { // 上キーが押されたら
		return UP_KEY;
	}
	if (Key[KEY_INPUT_DOWN] == 1) { // 下キーが押されたら
		return DOWN_KEY;
	}

	else {
		return FALSE;
	}
}

//デバッグモード起動用
int control_debug() {
	if (Key[KEY_INPUT_F] == 1) { // Fキーが押されていたら
		return TRUE;
	}
	return FALSE;
}

//ゲーム終了用
int control_esc() {
	if (Key[KEY_INPUT_ESCAPE] == 60) { //ESCキーが60カウント押されていたら
		return TRUE;
	}
	return FALSE;
}