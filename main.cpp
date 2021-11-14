#include "DxLib.h"
#include "control.h"
#include"movie.h"
#include"menu.h"
#include <math.h>

static int WINDOW = FALSE;
void game_main();
bool mouse_flag = true;
bool load_handle_flag = false;
int loading_handle1;
int loading_handle2;

///////fpsの調整///////////////
static int mStartTime;
static int mCount;
static int debug = FALSE;
static float mFps;
static const int N = 60;
static const int FPS = 60;

bool Update() {
	if (mCount == 0) {
		mStartTime = GetNowCount();
	}
	if (mCount == N) {
		int t = GetNowCount();
		mFps = 1000.f / ((t - mStartTime) / (float)N);
		mCount = 0;
		mStartTime = t;
	}
	mCount++;
	return true;
}

void Draw() {
	DrawFormatString(0, 0, GetColor(255, 255, 255), "%.1f", mFps);
}

void Wait() {
	int tookTime = GetNowCount() - mStartTime;
	int waitTime = mCount * 1000 / FPS - tookTime;
	if (waitTime > 0) {
		Sleep(waitTime);
	}
}

void load(int handle[10]) {
	handle[0] = LoadGraph("event/test.png");
	//handle[1] = LoadGraph("event/放課後の生徒戦線人.png");
	//handle[2] = LoadGraph("dot/オオタ.png");
	//handle[3] = LoadGraph("dot/イシ.png");
	//handle[4] = LoadGraph("dot/タイラ.png");
	//handle[5] = LoadGraph("dot/オワダ.png");
	//handle[6] = LoadGraph("dot/ミヤトネ.png");
	//handle[7] = LoadGraph("dot/ウチモト.png");
	//handle[8] = LoadGraph("dot/イバラカワ.png");
	//handle[9] = LoadGraph("dot/トモチ.png");
}
void test(int handle[10], double ex, int music) {
	DrawRotaGraph2(0, 0, 0, 0, ex, 0, handle[0], TRUE, FALSE);
}

//////////メイン関数///////////////////////
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	SetWindowSizeChangeEnableFlag(TRUE);//windowサイズ変更可能
	SetUseDirectInputFlag(TRUE);
	ChangeWindowMode(WINDOW), DxLib_Init(), SetDrawScreen(DX_SCREEN_BACK);
	SetMainWindowText("放課後の生徒戦線");
	//SetGraphMode(1280, 1024, 16);
	SetDrawMode(DX_DRAWMODE_BILINEAR);
	//SetDrawMode(DX_DRAWMODE_NEAREST);
	////マウス関連////
	SetMouseDispFlag(FALSE);//マウス非表示
	SetMousePoint(320,240);//マウスカーソルの初期位置
	//////////////////
	loading_handle1 = LoadGraph("event/loading1.png");
	loading_handle2 = LoadGraph("event/loading2.png");
	CardGame game;
	int handle[10]; load(handle);
	int music = LoadSoundMem("music/12345.mp3");
	while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0)
	{
		mouse_click();
		//mouse_limit(hand_x, hand_y);
		UpdateKey();
		game_main();
		//game.CMain();//ゲームをプレイ
		//test(handle, 1.0, music);
		//game.picture_mouse();//マウスカーソル表示
		//DrawFormatString(20, 0, GetColor(255, 255, 255), "(%d, %d) %d", hand_x, hand_y, GetMouseInput());
		//FPS操作
		if (control_debug() == TRUE) {
			if (debug == FALSE) { debug = TRUE; }
			else { debug = FALSE; }
		}
		Update();
		if (debug == TRUE) { Draw(); }
		Wait();
		if (control_esc() == TRUE) { DxLib_End(); }
		//FPS操作ここまで
	}

	DxLib_End(); // DXライブラリ終了処理
	return 0;
}

//ゲーム本体
int num;
void game_main() {
	static bool op = false;
	static Op* op_movie = new Op;
	static CardGame game;
	num = GetASyncLoadNum();
	if (num > 0) {
		SetUseASyncLoadFlag(FALSE);//非同期処理終了
		loading_handle1 = LoadGraph("event/loading1.png");
		//loading_handle2 = LoadGraph("event/loading2.png");
		DrawRotaGraph2(0, 0, 0, 0, 1.0, 0, loading_handle1, TRUE, FALSE);
	}
	else {
		if (!op) {
			op_movie->play();
			if (left_click() == 1) {//終了
				SetMousePoint(320, 240);//マウスカーソルの初期位置
				delete op_movie;
				op = true;
				SetAlwaysRunFlag(TRUE);//常に処理するかどうか
			}
		}
		else {
			game.CMain();//ゲームをプレイ
			//if (mouse_flag) {
			//	game.picture_mouse();//マウスカーソル表示
			//}
		}
	}
	if (mouse_flag) {
		game.picture_mouse();//マウスカーソル表示
	}
}