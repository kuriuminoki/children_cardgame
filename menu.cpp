#include "menu.h"
#include"control.h"
#include"create.h"
#include"DxLib.h"

static bool window_flag = FALSE;

CardGame::CardGame() {
	cnt = 0;
	deck = NULL;
	store = NULL;
	item = NULL;
	task = NULL;
	stand_handle = NULL;
	kettei_sound = LoadSoundMem("sound/決定.wav");
	modoru_sound = LoadSoundMem("sound/キャンセル.wav");;
	bgm = LoadSoundMem("music/town14.ogg");
	ChangeVolumeSoundMem(255 * 60 / 100, bgm);
	font_handle = CreateFontToHandle(NULL, 35, 4);//フォント
	mouse_handle = LoadGraph("picture/hand.png");
	task_detail = NULL;
	for (int i = 0; i < DAYLY_MISSION; i++) { daily_mission[i] = NULL; }
	//セーブデータからロードする
	money = 0; day = 1;
	for (int i = 0; i < M_TOTAL; i++) { clear[i] = false; }
	for (int i = 0; i < TODAY_MAX; i++) { today_mission[i] = NO_MISSION; }
	//save_other(&day, clear, today_mission);//テスト
	//save_money(&money);//テスト
	load_money(&money);//完成版
	load_other(&day, clear, today_mission);//完成版
	//ここまではセーブデータからロードする
}

CardGame::~CardGame() {
	save_money(&money);
	save_other(&day, clear, today_mission);
	if (deck != NULL)delete[] deck;
	if (store != NULL)delete[] store;
	if (item != NULL)delete item;
	if (task != NULL)delete task;
	if (stand_handle != NULL)delete[] stand_handle;
}

//CardGame以外で使う画像とフォントをすべて削除する
void CardGame::picture_reflesh() {
	InitGraph();
	InitFontToHandle();
	font_handle = CreateFontToHandle(NULL, 35, 4);//フォント
	mouse_handle = LoadGraph("picture/hand.png");
}
//CardGame以外で使うサウンドをすべて削除する
void CardGame::sound_reflesh() {
	InitSoundMem();
	kettei_sound = LoadSoundMem("sound/決定.wav");
	modoru_sound = LoadSoundMem("sound/キャンセル.wav");
	bgm = LoadSoundMem("music/town14.ogg");
	ChangeVolumeSoundMem(255 * 60 / 100, bgm);
}

//マウスカーソルを表示
void CardGame::picture_mouse() {
	GetMousePoint(&mouse_x, &mouse_y);
	//マウスカーソル描画
	DrawGraph(mouse_x, mouse_y, mouse_handle, TRUE);
}

//メニュー画面のアイコン ミッション:1 デッキ:2 ショップ:3
void CardGame::menu_key(int hand_x, int hand_y, int& indicate) {
	if (hand_x >= 10 && hand_x <= 210 && hand_y >= 350 && hand_y <= 450) {
		DrawBox(8, 348, 212, 452, GetColor(200, 200, 0), TRUE);
		indicate = 1;
	}
	DrawBox(10, 350, 210, 450, GetColor(100, 150, 100), TRUE);
	DrawStringToHandle(20, 390, "ミッション", GetColor(255, 0, 0), font_handle);
	if (hand_x >= 220 && hand_x <= 420 && hand_y >= 350 && hand_y <= 450) {
		DrawBox(218, 348, 422, 452, GetColor(200, 200, 0), TRUE);
		indicate = 2;
	}
	DrawBox(220, 350, 420, 450, GetColor(100, 150, 100), TRUE);
	DrawStringToHandle(260, 390, "デッキ", GetColor(255, 0, 255), font_handle);//260, 370
	//DrawStringToHandle(270, 405, "衣装", GetColor(255, 0, 255), font_handle);
	if (hand_x >= 430 && hand_x <= 630 && hand_y >= 350 && hand_y <= 450) {
		DrawBox(428, 348, 632, 452, GetColor(200, 200, 0), TRUE);
		indicate = 3;
	}
	DrawBox(430, 350, 630, 450, GetColor(100, 150, 100), TRUE);
	DrawStringToHandle(450, 390, "ショップ", GetColor(0, 0, 255), font_handle);
}

//今日のミッション表示
void CardGame::mission_key(int hand_x, int hand_y, int& indicate) {
	static bool daily_flag = false;
	static int daily_difficult = 1;
	if (hand_x >= 10 && hand_x <= 210 && hand_y >= 310 && hand_y <= 340) {
		DrawBox(8, 308, 212, 342, GetColor(200, 200, 0), TRUE);
		if (left_click() == 1) { 
			daily_flag = !daily_flag;
			PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//音
		}
	}
	DrawBox(10, 310, 210, 340, GetColor(100, 150, 100), TRUE);
	DrawFormatString(20, 320, GetColor(255, 0, 0), "ミッション切り替え");
	if (!daily_flag) {
		for (int i = 0; i < 3; i++) {
			if (today_mission[i] == NO_MISSION) { continue; }
			if (task_detail[i].picture_mission(10 + (i * 210), 50, hand_x, hand_y, clear[today_mission[i]])) {
				indicate = i + 4;
			}
		}
	}
	else {
		if (daily_difficult == 1) {
			DrawBox(100, 80, 150, 280, GetColor(50, 50, 50), TRUE);
		}
		else {
			if (hand_x >= 100 && hand_x <= 150 && hand_y >= 80 && hand_y <= 280) {
				DrawBox(100 - 2, 80 - 2, 150 + 2, 280 + 2, GetColor(0, 255, 0), TRUE);
				if (left_click() == 1) { 
					PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//音
					daily_difficult--;
				}
			}
			DrawBox(100, 80, 150, 280, GetColor(150, 150, 150), TRUE);
		}
		DrawFormatString(110, 180, GetColor(0, 0, 0), "←");
		if (daily_difficult == 10) {
			DrawBox(490, 80, 540, 280, GetColor(50, 50, 50), TRUE);
		}
		else {
			if (hand_x >= 490 && hand_x <= 540 && hand_y >= 80 && hand_y <= 280) {
				DrawBox(490 - 2, 80 - 2, 540 + 2, 280 + 2, GetColor(0, 255, 0), TRUE);
				if (left_click() == 1) {
					PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//音
					daily_difficult++;
				}
			}
			DrawBox(490, 80, 540, 280, GetColor(150, 150, 150), TRUE);
		}
		DrawFormatString(500, 180, GetColor(0, 0, 0), "→");
		if (daily_mission[daily_difficult - 1]->picture_mission(10 + (1 * 210), 50, hand_x, hand_y, false)) {
			indicate = daily_difficult + 6;
		}
	}
}

//今日のミッションを決める
void CardGame::mission_decide() {
	if (today_mission[0] != NO_MISSION && daily_mission[0] != NULL)return;//既に決まっている
	if (today_mission[0] == NO_MISSION) {
		mission_rule(today_mission, clear, day);//ルールに沿ってミッションを決める
	}
	//today_mission[0] = MM1;//テスト
	//today_mission[1] = EM18;//テスト
	//today_mission[2] = EM19;//テスト
	task_detail = new MissionText[3]{ today_mission[0], today_mission[1], today_mission[2] };
	for (int i = 0; i < DAYLY_MISSION; i++) {
		daily_mission[i] = new MissionText(DM1 + i);
	}
	save_other(&day, clear, today_mission);
}

//立ち絵と全てのデッキと倉庫をロードする
void CardGame::init_cards() {
	load_cards_stand(store, deck, item, stand_handle);
}
//カードをセーブ、メモリを返す
void CardGame::end_cards() {
	save_store(store);//セーブ
	save_deck(deck);//セーブ
	save_item(*item);
	save_money(&money);
	save_other(&day, clear, today_mission);
	delete[] store;
	delete[] deck;
	delete item;
	delete[] stand_handle;
	deck = NULL;
	store = NULL;
	item = NULL;
	stand_handle = NULL;
}

//クリックされたときの処理
void CardGame::menu_click(int indicate, int& now) {
	if (left_click() != 1)return;//左クリックされていない
	if (indicate == 1) {
		if (now == M_SELECT) {
			now = FALSE;//ミッション表示キャンセル
			PlaySoundMem(modoru_sound, DX_PLAYTYPE_BACK);//音
		}
		else {
			now = M_SELECT;//ミッション表示
			PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//音
		}
	}
	else if (indicate == 2) {
		init_cards();
		now = CUS;//デッキ編集へ
		PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//音
	}
	else if (indicate == 3) {
		shop = new Shop;
		now = SHOP;//ショップへ
		PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//音
	}
	else if (indicate >= 4 && (indicate <= 6 || indicate <= 6+10)) {
		//3つのミッションの1つをプレイ
		sound_reflesh();
		now = M_PLAY;
		m_kind = indicate - 4;
		PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//音
		init_mission();
	}
}

//キャラクター選択 focus変更,もしくはnowを0にして終了
void CardGame::chara_focus(int& focus, int& now) {
	int result = 0;
	result = character_custom(stand_handle, kettei_sound, modoru_sound, focus);
	if (result == 1) {//戻るボタンで終了
		now = 0;
		picture_reflesh();
		end_cards();
	}
}

//メニュー画面 ミッションやアイコンは除く
void CardGame::picture_menu() {
	DrawBox(0, 0, 640, 480, GetColor(50, 50, 100), TRUE);//背景
	DrawFormatString(10, 10, GetColor(0, 0, 0), "メニュー画面  %d日目", day);
	DrawFormatString(300, 10, GetColor(255, 255, 0), "所持金: %d", money);
}

//メイン関数
void CardGame::CMain() {
	static int now = FALSE;//1:ミッション選択中 2:ミッションプレイ中 3:デッキ編集中 4:ショップ内
	int hand_x, hand_y, indicate = 0;
	if (now == DATA && (db->get_now() == DATA_OP || db->get_now() == DATA_ED)) {//OPやEDを閲覧中
		StopSoundMem(bgm);
	}
	if (now != M_PLAY && CheckSoundMem(bgm) == 0) {
		PlaySoundMem(bgm, DX_PLAYTYPE_BACK);//メニュー画面のBGM
	}
	//全画面・ウィンドウ切り替え
	if ((now == FALSE || now == M_SELECT) && control_space() == SPACE_KEY) {
		window_flag = !window_flag;
		ChangeWindowMode(window_flag);
		picture_reflesh(); sound_reflesh();
		SetMouseDispFlag(FALSE);//マウス非表示
		SetMousePoint(320, 240);//マウスカーソルの初期位置
		SetDrawScreen(DX_SCREEN_BACK);
		SetDrawMode(DX_DRAWMODE_BILINEAR);
	}
	if (cnt > 0 && now == FALSE) {
		cnt--;
		int color = 480 - cnt * 2;
		if (color > 255) { color = 255;  }
		if (color < 0) { color = 0; }
		DrawFormatStringToHandle(260, 200, GetColor(color, color, color), font_handle, "%d日目", day);
		return;
	}
	GetMousePoint(&hand_x, &hand_y);
	if (now == M_PLAY) {//ミッションプレイ中
		if (play_mission() == true) {
			now = FALSE;//ミッション終了
			picture_reflesh();
		}
	}
	else if (now == CUS) {//カスタマイズ画面
		static int focus = 0;//編集するキャラ1～10
		if (focus == 0) {//編集するキャラを決める
			chara_focus(focus, now);
		}
		else {//編集するキャラが決まっているとき
			int result = deck_create(deck[focus - 1], store[focus - 1], *item, kettei_sound);
			if (result == 1) { focus = 0; }//戻るボタンでキャラ選択へ戻る
		}
	}
	else if (now == SHOP) {//ショップ画面
		if (shop->shop_main(money) == true) {
			PlaySoundMem(modoru_sound, DX_PLAYTYPE_BACK);//音
			now = FALSE;
			picture_reflesh();
			delete shop;
		}
	}
	else if (now == DATA) {//データ確認画面
		if (db->play()) {
			now = FALSE;
			delete db;
			picture_reflesh();
			sound_reflesh();
			PlaySoundMem(modoru_sound, DX_PLAYTYPE_BACK);//音
		}
	}
	else {//メニュー画面
		picture_menu();
		mission_decide();
		menu_key(hand_x, hand_y, indicate);//アイコン
		if (hand_x >= 550 && hand_x <= 630 && hand_y >= 5 && hand_y <= 35) {
			DrawBox(547, 2, 633, 38, GetColor(0, 255, 0), TRUE);
			if (left_click() == 1) { 
				now = DATA;
				db = new DataBase(clear);
				PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//音
			}//データ確認画面へ
		}
		DrawBox(550, 5, 630, 35, GetColor(100, 100, 255), TRUE);
		DrawFormatString(565, 15, GetColor(0, 0, 0), "データ");
		//今日のミッション表示
		if (now == M_SELECT) { 
			mission_key(hand_x, hand_y, indicate);
		}
		menu_click(indicate, now);//クリックによってnow変化
		DrawFormatString(20, 460, GetColor(0, 0, 0), "エスケープキー長押しで終了、スペースキーで画面モード切り替え");
	}
}

/////ミッションプレイ関連三つ////////////////////////////////////////////////
//準備 プレイの前に一度呼ぶこと
void CardGame::init_mission() {
	if (m_kind <= 2) {
		get_mission(task, today_mission[m_kind], task_detail[m_kind]);//やりたいミッションのオブジェクトを確保して
	}
	else {
		get_mission(task, DM1 + m_kind - 3, *daily_mission[m_kind - 3]);//やりたいミッションのオブジェクトを確保して
	}
	task->init();//ロードする
}
//プレイ
bool CardGame::play_mission() {
	static Card c;
	static int m = 0;
	static bool f_flag = false;
	if (f_flag) {
		picture_result(m, c);
		if (left_click() == 1) {//報酬画面終了
			f_flag = false;
			m = 0; c.set_kind(0);
			end_mission();//終了処理
			PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//音
			return true;//決着
		}
		return false;
	}
	int result;
	result = task->play();//WIN or LOSE or 0
	if (result == WIN && m_kind < 3) {//クリア
		clear[today_mission[m_kind]] = true;
	}
	if (result == WIN || result == LOSE) {
		f_flag = true;
		task->reward(result, m, c);//報酬を受け取る
		day++;//日付変更
		if (day == 21) { day = 1; }
		cnt = DAY_COUNT;
		for (int i = 0; i < 3; i++) { today_mission[i] = NO_MISSION; }//リセット
		money += m;
		save_money(&money);
		save_other(&day, clear, today_mission);
	}
	else if (result == CANCEL) {
		if (task != NULL)delete task;//ミッション破棄
		task = NULL;
		//end_mission();//終了処理
		PlaySoundMem(modoru_sound, DX_PLAYTYPE_BACK);//音
		return true;
	}
	return false;
}
//終了処理
void CardGame::end_mission() {
	if (task != NULL)delete task;//ミッション破棄
	task = NULL;
	delete[] task_detail;
	task_detail = NULL;
	for (int i = 0; i < DAYLY_MISSION; i++) { delete daily_mission[i]; }
	for (int i = 0; i < DAYLY_MISSION; i++) { daily_mission[i] = NULL; }
}
//////////////////////////////////////////////////////////////////////////////

//報酬獲得画面
void CardGame::picture_result(int money, Card& c) {
	DrawBox(0, 0, 640, 480, GetColor(100, 100, 100), TRUE);
	DrawFormatString(20, 100, GetColor(255, 0, 0), "報酬獲得");
	DrawFormatString(20, 150, GetColor(255, 255, 0), "所持金: +%d", money);
	DrawFormatString(10, 10, GetColor(0,0,0), "左クリックで終了");
	if (c.get_kind() != 0) {
		picture_card_state(c.get_hp(), c.get_power(), c.get_speed(), c.get_rigid(), false);
		DrawFormatString(10, 440, GetColor(0, 0, 0), c.get_ability()->get_text());
		DrawFormatString(300, 50, GetColor(0, 0, 255), "獲得したカード");
		SetDrawMode(DX_DRAWMODE_NEAREST);
		DrawRotaGraph2(300, 70, 0, 0, 2.0, 0, c.get_handle(), TRUE, FALSE);
		SetDrawMode(DX_DRAWMODE_BILINEAR);
	}
}