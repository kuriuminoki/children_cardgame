#include"text.h"
#include"control.h"
#include"DxLib.h"

Text::Text(int x[10], int te_x) {
	play_flag = true;
	finish_flag = false;
	event_cnt = 0;
	max_size = 0;
	teacher_handle = LoadGraph("dot/先生.png");
	dot_handle[0] = LoadGraph("dot/アカツキ.png");
	dot_handle[1] = LoadGraph("dot/ハンダ.png");
	dot_handle[2] = LoadGraph("dot/オオタ.png");
	dot_handle[3] = LoadGraph("dot/イシ.png");
	dot_handle[4] = LoadGraph("dot/タイラ.png");
	dot_handle[5] = LoadGraph("dot/オワダ.png");
	dot_handle[6] = LoadGraph("dot/ミヤトネ.png");
	dot_handle[7] = LoadGraph("dot/ウチモト.png");
	dot_handle[8] = LoadGraph("dot/イバラカワ.png");
	dot_handle[9] = LoadGraph("dot/トモチ.png");
	hukidasi_handle[0] = LoadGraph("dot/フキダシ.png");
	hukidasi_handle[1] = LoadGraph("dot/妄想.png");
	haikei = LoadGraph("event/背景.png");
	//haikei = LoadGraph("event/アカツキ起床.png");
	kettei_sound = LoadSoundMem("sound/決定.wav");
	for (int i = 0; i < 10; i++) {
		xy[i].first = x[i];
		xy[i].second = STAND_Y;
	}
	te_xy.first = te_x;
	te_xy.second = STAND_Y;
}

Text::~Text() {
	for (int i = 0; i < 10; i++) {
		DeleteGraph(dot_handle[i]);
	}
	DeleteGraph(hukidasi_handle[0]);
	DeleteGraph(hukidasi_handle[1]);
	DeleteGraph(haikei);
	if(event_handle != -1){ DeleteGraph(haikei); }
	DeleteSoundMem(kettei_sound);
}

void Text::text_insert(const char* name_str, const char* text_str, int h_kind, int s_kind) {
	std::string str;
	str = text_str;
	texts.push(str);
	str = name_str;
	names.push(str);
	hukidasi_set.push(std::make_pair(h_kind, s_kind));
	max_size++;
}

bool Text::get_flag() {
	return play_flag;
}

//今何番目のテキストか？
int Text::get_num() {
	return max_size - texts.size();
}

//イベント挿絵の画像ハンドルを引数で受け取る
void Text::set_event(int handle) {
	event_cnt = 0;
	event_handle = handle;
}

void Text::event_cancel() {
	event_handle = -1;
}

//次のテキストへ移る
void Text::next_text() {
	SetDrawBright(255, 255, 255);
	display_first.erase(); display_second.erase();
	const int LINE_MAX = 60;
	if (texts.empty()) { finish_flag = true; return; }
	display = texts.front(); texts.pop();
	name = names.front();  names.pop();
	hukidasi_kind = hukidasi_set.front().first;
	speaker = hukidasi_set.front().second;
	hukidasi_set.pop();
	for (unsigned int i = 0; i < display.size(); i++) {
		if (i < LINE_MAX) { display_first += display[i]; }
		else{ display_second += display[i]; }
	}
}

const int WHITE = GetColor(200, 200, 200);
const int BLACK = GetColor(0, 0, 0);
const int BLUE = GetColor(0, 0, 200);
const int EVENT_TIME = 120;
const int EVENT_X = 320, EVENT_Y = 310;

int Text::play() {
	if (right_click() == 60) { SetDrawBright(255, 255, 255); return 1; }
	if (play_flag && finish_flag) { SetDrawBright(255, 255, 255); return 1; }
	if (play_flag && texts.empty() && left_click() == 1) { 
		SetDrawBright(255, 255, 255);
		return 1;
	}//終了
	//クリックされたとき
	if (play_flag && (display.empty() || left_click() == 1)) {
		if (!display.empty()) {
			PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//音
		}
		next_text();
	}
	//描画フェーズ
	//背景
	DrawRotaGraph(320, 240, 1, 0, haikei, TRUE);
	//キャラ
	for (int i = 0; i < 10; i++) {
		DrawRotaGraph2(xy[i].first, xy[i].second, 0, 0, 1, 0, dot_handle[i], TRUE, FALSE);
	}
	DrawRotaGraph2(te_xy.first, te_xy.second, 0, 0, 1, 0, teacher_handle, TRUE, FALSE);
	//フキダシ
	if (play_flag) {
		if (speaker == 10) {
			DrawRotaGraph2(te_xy.first + 38, te_xy.second - 60, 0, 0, 1, 0, hukidasi_handle[hukidasi_kind], TRUE, FALSE);
		}
		else {
			DrawRotaGraph2(xy[speaker].first + 38, xy[speaker].second - 60, 0, 0, 1, 0, hukidasi_handle[hukidasi_kind], TRUE, FALSE);
		}
	}
	//イベントの挿絵
	if (event_cnt > 0 && event_cnt < 40) {
		DrawRotaGraph(EVENT_X, EVENT_Y + (160 - event_cnt * 4), 0.6, 0, event_handle, TRUE);
	}
	else if (event_cnt > 0) {
		DrawRotaGraph(EVENT_X, EVENT_Y, 0.6, 0, event_handle, TRUE);
	}
	//テキスト
	if (play_flag) {
		DrawBox(48, 48, 592, 142, BLUE, TRUE);
		DrawBox(50, 50, 590, 140, WHITE, TRUE);
		DrawBox(48, 48, 300, 90, BLUE, FALSE);
		DrawFormatString(60, 60, BLACK, name.data());
		DrawFormatString(60, 100, BLACK, display_first.data());
		DrawFormatString(60, 120, BLACK, display_second.data());
	}
	if (get_num() == 1) {
		DrawFormatString(0, 0, GetColor(255, 255, 0), "左クリック長押しでスキップする");
	}
	//DrawFormatString(0, 20, GetColor(255, 255, 255), "play_flag = %d", play_flag);
	return 0;
}

int Text::move(int kind, int gx, int speed) {
	if (play_flag && get_num() != 0) {
		PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//音
	}
	play_flag = false;
	if (kind == 10 && te_xy.first < gx) {
		te_xy.first += speed;
		if (te_xy.first > gx) {
			te_xy.first = gx;
		}
	}
	else if (kind == 10 && te_xy.first > gx) {
		te_xy.first -= speed;
		if (te_xy.first < gx) {
			te_xy.first = gx;
		}
	}
	else if (xy[kind].first < gx) {
		xy[kind].first += speed;
		if (xy[kind].first > gx) {
			xy[kind].first = gx;
		}
	}
	else if (xy[kind].first > gx) {
		xy[kind].first -= speed;
		if (xy[kind].first < gx) {
			xy[kind].first = gx;
		}
	}
	else {
		event_cnt = 0;
		play_flag = true;
		next_text();
		return 1;
	}
	return 0;
}

int Text::event() {
	if(play_flag && get_num() != 0){
		PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//音
	}
	play_flag = false;
	event_cnt++;
	if (event_cnt == EVENT_TIME) {
		play_flag = true;
		next_text();
		return 1;
	}
	return 0;
}

int Text::finish() {
	if (play_flag && get_num() != 0) {
		PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//音
	}
	play_flag = false;
	event_cnt+=2;
	if (event_cnt >= 255) {
		play_flag = true;
		event_cnt = 0;
		event_cancel();
		SetDrawBright(255, 255, 255);
		next_text();
		return 1;
	}
	SetDrawBright(255 - event_cnt, 255 - event_cnt, 255 - event_cnt);
	return 0;
}