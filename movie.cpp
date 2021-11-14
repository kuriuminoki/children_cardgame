#include"DxLib.h"
#include"movie.h"
#include"control.h"

extern bool mouse_flag;

Op::Op() {
	mouse_flag = false;
	SetMouseDispFlag(TRUE);//マウス表示
	//handle1 = LoadGraph("event/放課後の生徒戦線人.png");
	//handle1 = LoadGraph("event/放課後の生徒戦線人.jpg");
	handle1 = LoadGraph("event/放課後の生徒戦線人.bmp");
	handle2 = LoadGraph("event/放課後の生徒戦線.png");
	music = LoadSoundMem("music/12345.mp3");
	ChangeVolumeSoundMem(255 * 60 / 100, music);
	font = CreateFontToHandle(NULL, 20, 5);//フォント
}

Op::~Op() {
	mouse_flag = true;
	SetMouseDispFlag(FALSE);//マウス非表示
	DeleteGraph(handle1);
	DeleteGraph(handle2);
	DeleteSoundMem(music);
	SetDrawBright(255, 255, 255);
}

const int black = GetColor(0,0,0);
bool Op::play() {
	if (left_click() == 1) {
		return true;
	}
	DrawBox(0, 0, 640, 480, black, TRUE);
	if (cnt >= 0 && cnt <= 255) {
		SetDrawBright(cnt, cnt, cnt);
	}
	if (cnt >= MUSIC_START && CheckSoundMem(music) == 0) {
		if (cnt > MUSIC_START && cnt < MUSIC_RESTART) {

		}
		else if (cnt > MUSIC_START) {
			finish_flag = false;
			x = INIT_X;
			x2 = INIT_X2;
			cnt = 0;
			SetDrawBright(0, 0, 0);
			return true;
		}
		else {
			PlaySoundMem(music, DX_PLAYTYPE_BACK);
		}
	}
	if (finish_flag && cnt > MUSIC_START && CheckSoundMem(music) == 0) {
		DrawRotaGraph2(-100, 50, 0, 0, 0.15, 0, handle2, FALSE, FALSE);
		DrawStringToHandle(200, 250, "Click for starting battle", GetColor(255,255,255), font);
	}
	else {
		DrawRotaGraph2(x2, 10, 0, 0, 1, 0, handle1, TRUE, FALSE);
		DrawRotaGraph2(x, 10, 0, 0, 1, 0, handle2, TRUE, FALSE);
	}
	cnt++;
	if (cnt % 3 == 0 || cnt % 3 == 1) { x -= 1; }
	if (cnt % 2 == 0) { x2 -= 1; }

	if (cnt <= MUSIC_START + 250) {
		DrawStringToHandle(200, 250, "制作：KM.Mono", GetColor(255, 255, 255), font);
	}
	if (cnt >= 5400 && CheckSoundMem(music) != 0) {
		DrawStringToHandle(380, 250, "produced by KM.Mono-Games", GetColor(255, 255, 255), font);
	}
	//DrawFormatString(2,2,GetColor(255,255,255),"cnt = %d", cnt);
	if (cnt > MUSIC_START && CheckSoundMem(music) == 0) {//タイトル表示中はtrueを返す
		finish_flag = true;
		return true;
	}
	return false;
}

Ed::Ed() {
	mouse_flag = false;
	SetMouseDispFlag(TRUE);//マウス表示
	akatsuki_handle = LoadGraph("event/ed_akatsuki.png");
	all_handle[0] = LoadGraph("event/ed_all1.png");
	all_handle[1] = LoadGraph("event/ed_all2.png");
	class_room_handle[0] = LoadGraph("event/ed_class_room1.png");
	class_room_handle[1] = LoadGraph("event/ed_class_room2.png");
	class_room_handle[2] = LoadGraph("event/ed_class_room3.png");
	cry_handle[0] = LoadGraph("event/ed_cry1.png");
	cry_handle[1] = LoadGraph("event/ed_cry2.png");
	cry_handle[2] = LoadGraph("event/ed_cry3.png");
	dark_handle[0] = LoadGraph("event/ed_dark1.png");
	dark_handle[1] = LoadGraph("event/ed_dark2.png");
	dark_handle[2] = LoadGraph("event/ed_dark3.png");
	dark_handle[3] = LoadGraph("event/ed_dark4.png");
	dark_handle[4] = LoadGraph("event/ed_dark5.png");
	dark_handle[5] = LoadGraph("event/ed_dark6.png");
	eye_handle[0] = LoadGraph("event/ed_eye1.png");
	eye_handle[1] = LoadGraph("event/ed_eye2.png");
	eye_handle[2] = LoadGraph("event/ed_eye3.png");
	final_handle[0] = LoadGraph("event/ed_final1.png");
	final_handle[1] = LoadGraph("event/ed_final2.png");
	final_handle[2] = LoadGraph("event/ed_final3.png");
	miyatone_handle[0] = LoadGraph("event/ed_miyatone1.png");
	miyatone_handle[1] = LoadGraph("event/ed_miyatone2.png");
	miyatone_handle[2] = LoadGraph("event/ed_miyatone3.png");
	miyatone_n_handle[0] = LoadGraph("event/ed_miyatone1_n.png");
	miyatone_n_handle[1] = LoadGraph("event/ed_miyatone2_n.png");
	miyatone_n_handle[2] = LoadGraph("event/ed_miyatone3_n.png");
	miyatone_46_handle[0] = LoadGraph("event/ed_miyatone4.png");
	miyatone_46_handle[1] = LoadGraph("event/ed_miyatone5.png");
	miyatone_46_handle[2] = LoadGraph("event/ed_miyatone6.png");
	note_handle[0] = LoadGraph("event/ed_note1.png");
	note_handle[1] = LoadGraph("event/ed_note2.png");
	music = LoadSoundMem("music/kobune_ed.mp3");
	ChangeVolumeSoundMem(255 * 60 / 100, music);
	font = CreateFontToHandle(NULL, 30, 5);//フォント
}

Ed::~Ed() {
	mouse_flag = true;
	SetMouseDispFlag(FALSE);//マウス非表示
	DeleteGraph(akatsuki_handle);
	for (int i = 0; i < 2; i++) { DeleteGraph(all_handle[i]); }
	for (int i = 0; i < 3; i++) { DeleteGraph(class_room_handle[i]); }
	for (int i = 0; i < 3; i++) { DeleteGraph(cry_handle[i]); }
	for (int i = 0; i < 6; i++) { DeleteGraph(dark_handle[i]); }
	for (int i = 0; i < 3; i++) { DeleteGraph(eye_handle[i]); }
	for (int i = 0; i < 3; i++) { DeleteGraph(final_handle[i]); }
	for (int i = 0; i < 3; i++) { DeleteGraph(miyatone_handle[i]); }
	for (int i = 0; i < 3; i++) { DeleteGraph(miyatone_n_handle[i]); }
	for (int i = 0; i < 3; i++) { DeleteGraph(miyatone_46_handle[i]); }
	for (int i = 0; i < 2; i++) { DeleteGraph(note_handle[i]); }
	DeleteSoundMem(music);
	SetDrawBright(255, 255, 255);
}

void Ed::black_out(const int TIME) {
	if (cnt < TIME - 255 || cnt > TIME + 255) { return; }
	else if(cnt < TIME){
		int c = TIME - cnt;
		SetDrawBright(c, c, c);
		bright = c;//テスト
	}
	else {
		int c = cnt - TIME;
		SetDrawBright(c, c, c);
		bright = c;//テスト
	}
}

int Ed::get_num(const int START, const int FINISH, const int sum) {
	int i = (FINISH - 60 - START) / sum;
	i = (cnt - START) / i;
	if (i >= sum) {
		i = sum - 1;
	}
	return i;
}

bool Ed::play() {
	if (left_click() == 1) {//スキップ終了
		return true;
	}
	if (cnt >= 0 && cnt <= 255) {
		SetDrawBright(cnt, cnt, cnt);
	}
	if (cnt >= MUSIC_START && CheckSoundMem(music) == 0) {
		if (cnt > MUSIC_START) {//初めからもう一度
			x = INIT_X;
			cnt = 0;
			SetDrawBright(0, 0, 0);
			return true;
		}
		else {//音楽開始
			PlaySoundMem(music, DX_PLAYTYPE_BACK);
		}
	}
	//ここからが動画本体
	cnt++;
	if (cnt < MIYATONE_FINISH) {
		DrawRotaGraph2(0, 0, 0, 0, 1, 0, miyatone_handle[(cnt / MIYATONE_FLAME) % 3], TRUE, FALSE);
	}
	else if(cnt >= MIYATONE_FINISH && cnt < CLASS_ROOM_FINISH){
		int i;
		i = get_num(MIYATONE_FINISH - 40, CLASS_ROOM_FINISH, 3);
		DrawRotaGraph2(0, 0, 0, 0, 1, 0, class_room_handle[i], TRUE, FALSE);
	}
	else if (cnt >= CLASS_ROOM_FINISH && cnt < NOTE_FINISH) {
		int i;
		i = get_num(CLASS_ROOM_FINISH-50, NOTE_FINISH, 2);
		if (cnt % 3 == 0) {
			if (i == 0) { x--; }
			else { x++; }
		}
		DrawRotaGraph2(x, 0, 0, 0, 1.3, 0, note_handle[i], TRUE, FALSE);
	}
	else if (cnt >= NOTE_FINISH && cnt < DARK_CHANGE) {
		DrawRotaGraph2(0, 0, 0, 0, 1, 0, dark_handle[(cnt / MIYATONE_FLAME) % 5], TRUE, FALSE);
	}
	else if (cnt >= DARK_CHANGE && cnt < DARK_FINISH) {
		DrawRotaGraph2(0, 0, 0, 0, 1, 0, dark_handle[5], TRUE, FALSE);
	}
	else if (cnt >= DARK_FINISH && cnt < CRY_FINISH) {
		int i = get_num(DARK_FINISH, CRY_FINISH, 3);
		DrawRotaGraph2(0, 0, 0, 0, 1, 0, cry_handle[i], TRUE, FALSE);
	}
	else if (cnt >= CRY_FINISH && cnt < MIYATONE_N_FINISH) {
		DrawRotaGraph2(0, 0, 0, 0, 1, 0, miyatone_n_handle[(cnt / MIYATONE_FLAME) % 3], TRUE, FALSE);
	}
	else if (cnt >= MIYATONE_N_FINISH && cnt < EYE_FINISH) {
		int i = get_num(MIYATONE_N_FINISH, EYE_FINISH, 3);
		DrawRotaGraph2(0, 0, 0, 0, 1, 0, eye_handle[i], TRUE, FALSE);
		x = 0;
	}
	else if (cnt >= EYE_FINISH && cnt < ALL_1_FINISH) {
		x-=2;
		DrawRotaGraph2(x, 0, 0, 0, 1, 0, all_handle[0], TRUE, FALSE);
	}
	else if (cnt >= ALL_1_FINISH && cnt < ALL_2_FINISH) {
		x+=2;
		DrawRotaGraph2(x, 0, 0, 0, 1, 0, all_handle[1], TRUE, FALSE);
	}
	else if (cnt >= ALL_2_FINISH && cnt < AKATSUKI_FINISH) {
		if (cnt == ALL_2_FINISH) { x = -2; y = -2; }
		x += GetRand(2) - 1; y += GetRand(2) - 1;
		if (x < -5) { x = -5; }
		if (y < -5) { y = -5; }
		if (x > 0) { x = 0; }
		if (y > 0) { y = 0; }
		DrawRotaGraph2(x, y, 0, 0, 1.1, 0, akatsuki_handle, TRUE, FALSE);
	}
	else if (cnt >= AKATSUKI_FINISH && cnt < FINAL_FINISH) {
		DrawRotaGraph2(0, 0, 0, 0, 1, 0, final_handle[0], TRUE, FALSE);
	}
	else{
		DrawRotaGraph2(0, 0, 0, 0, 1, 0, miyatone_46_handle[(cnt / MIYATONE_FLAME) % 3], TRUE, FALSE);
		if (cnt < FINAL_FINISH + 60) {
			DrawRotaGraph2(0, 0, 0, 0, 1, 0, final_handle[1], TRUE, FALSE);
		}
		else if (cnt < FINAL_FINISH + 120) {
			DrawRotaGraph2(0, 0, 0, 0, 1, 0, final_handle[2], TRUE, FALSE);
		}
	}
	black_out(MIYATONE_FINISH);
	black_out(CLASS_ROOM_FINISH);
	black_out(NOTE_FINISH);

	//文字
	if (cnt <= MUSIC_START + 600) {
		DrawStringToHandle(230, 250, "制作：KM.Mono", GetColor(255, 255, 255), font);
	}
	if (cnt > MUSIC_START + 600 && cnt <= MUSIC_START + 1200) {
		DrawStringToHandle(185, 250, "プログラム：KM.Mono", GetColor(255, 255, 255), font);
	}
	if (cnt > MUSIC_START + 1200 && cnt <= MIYATONE_FINISH -  400) {
		DrawStringToHandle(200, 250, "シナリオ：KM.Mono", GetColor(255, 255, 255), font);
	}
	if (cnt >= NOTE_FINISH + 300 && cnt < DARK_CHANGE - 120) {
		DrawStringToHandle(120, 250, "キャラクターデザイン：KM.Mono", GetColor(255, 255, 255), font);
	}
	if (cnt >= DARK_FINISH + 200 && cnt < DARK_FINISH + 600) {
		DrawStringToHandle(230, 230, "ED：銀の小舟", GetColor(255, 255, 255), font);
		DrawStringToHandle(230, 270, "唄：灯津木 結", GetColor(255, 255, 255), font);
	}
	if (cnt >= DARK_FINISH + 600 && cnt < DARK_FINISH + 1000) {
		DrawStringToHandle(250, 230, "OP：12345", GetColor(255, 255, 255), font);
		DrawStringToHandle(250, 270, "唄：Karin", GetColor(255, 255, 255), font);
	}
	if (cnt >= DARK_FINISH + 1000 && cnt < MIYATONE_N_FINISH - 100) {
		DrawStringToHandle(210, 250, "素材提供：魔王魂", GetColor(255, 255, 255), font);
	}
	if (cnt >= FINAL_FINISH+180) {
		if (cnt < FINAL_FINISH + 600) {
			DrawStringToHandle(150, 250, "produced by KM.Mono-Games", GetColor(100,100,250), font);
		}
		else {
			DrawStringToHandle(160, 250, "Thank you for Playing !", GetColor(100,100, 250), font);
		}
	}
	//DrawFormatString(2,2,GetColor(255,255,255),"cnt = %d", cnt);
	//DrawFormatString(2, 20, GetColor(255, 255, 255), "bright = %d", bright);
	return false;
}