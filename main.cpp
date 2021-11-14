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

///////fps�̒���///////////////
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
	//handle[1] = LoadGraph("event/���ی�̐��k����l.png");
	//handle[2] = LoadGraph("dot/�I�I�^.png");
	//handle[3] = LoadGraph("dot/�C�V.png");
	//handle[4] = LoadGraph("dot/�^�C��.png");
	//handle[5] = LoadGraph("dot/�I���_.png");
	//handle[6] = LoadGraph("dot/�~���g�l.png");
	//handle[7] = LoadGraph("dot/�E�`���g.png");
	//handle[8] = LoadGraph("dot/�C�o���J��.png");
	//handle[9] = LoadGraph("dot/�g���`.png");
}
void test(int handle[10], double ex, int music) {
	DrawRotaGraph2(0, 0, 0, 0, ex, 0, handle[0], TRUE, FALSE);
}

//////////���C���֐�///////////////////////
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	SetWindowSizeChangeEnableFlag(TRUE);//window�T�C�Y�ύX�\
	SetUseDirectInputFlag(TRUE);
	ChangeWindowMode(WINDOW), DxLib_Init(), SetDrawScreen(DX_SCREEN_BACK);
	SetMainWindowText("���ی�̐��k���");
	//SetGraphMode(1280, 1024, 16);
	SetDrawMode(DX_DRAWMODE_BILINEAR);
	//SetDrawMode(DX_DRAWMODE_NEAREST);
	////�}�E�X�֘A////
	SetMouseDispFlag(FALSE);//�}�E�X��\��
	SetMousePoint(320,240);//�}�E�X�J�[�\���̏����ʒu
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
		//game.CMain();//�Q�[�����v���C
		//test(handle, 1.0, music);
		//game.picture_mouse();//�}�E�X�J�[�\���\��
		//DrawFormatString(20, 0, GetColor(255, 255, 255), "(%d, %d) %d", hand_x, hand_y, GetMouseInput());
		//FPS����
		if (control_debug() == TRUE) {
			if (debug == FALSE) { debug = TRUE; }
			else { debug = FALSE; }
		}
		Update();
		if (debug == TRUE) { Draw(); }
		Wait();
		if (control_esc() == TRUE) { DxLib_End(); }
		//FPS���삱���܂�
	}

	DxLib_End(); // DX���C�u�����I������
	return 0;
}

//�Q�[���{��
int num;
void game_main() {
	static bool op = false;
	static Op* op_movie = new Op;
	static CardGame game;
	num = GetASyncLoadNum();
	if (num > 0) {
		SetUseASyncLoadFlag(FALSE);//�񓯊������I��
		loading_handle1 = LoadGraph("event/loading1.png");
		//loading_handle2 = LoadGraph("event/loading2.png");
		DrawRotaGraph2(0, 0, 0, 0, 1.0, 0, loading_handle1, TRUE, FALSE);
	}
	else {
		if (!op) {
			op_movie->play();
			if (left_click() == 1) {//�I��
				SetMousePoint(320, 240);//�}�E�X�J�[�\���̏����ʒu
				delete op_movie;
				op = true;
				SetAlwaysRunFlag(TRUE);//��ɏ������邩�ǂ���
			}
		}
		else {
			game.CMain();//�Q�[�����v���C
			//if (mouse_flag) {
			//	game.picture_mouse();//�}�E�X�J�[�\���\��
			//}
		}
	}
	if (mouse_flag) {
		game.picture_mouse();//�}�E�X�J�[�\���\��
	}
}