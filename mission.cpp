#include"mission.h"
#include"DxLib.h"
#include"control.h"

const char* student_name(int kind) {
	switch (kind) {
	case -1:
		return "不可";
		break;
	case 1:
		return "赤月";
		break;
	case 2:
		return "半田";
		break;
	case 3:
		return "太田";
		break;
	case 4:
		return "伊志";
		break;
	case 5:
		return "台羅";
		break;
	case 6:
		return "小和田";
		break;
	case 7:
		return "宮利根";
		break;
	case 8:
		return "内本";
		break;
	case 9:
		return "茨川";
		break;
	case 10:
		return "砥用";
		break;
	}
	return "自由";
}

const char* teacher_name(int kind) {
	switch (kind) {
	case 1:
		return "O先生";
		break;
	case 2:
		return "P先生";
		break;
	case 3:
		return "Q先生";
		break;
	case 4:
		return "R先生";
		break;
	case 5:
		return "S先生";
		break;
	case 6:
		return "T先生";
		break;
	case 7:
		return "U先生";
		break;
	case 8:
		return "V先生";
		break;
	case 9:
		return "W先生";
		break;
	case 10:
		return "X先生";
		break;
	case 11:
		return "Y先生";
		break;
	case 12:
		return "Z校長";
		break;
	case 13:
		return "A先生";
		break;
	case 14:
		return "B先生";
		break;
	case 15:
		return "雷堂";
		break;
	}
	return "なし";
}

Mission::Mission(MissionText& detail) {
	now = 0;
	result = 0;
	com = NULL;
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		kind[i] = 0;
		fixed[i] = false;
	}
	for (int i = 0; i < 3; i++) {
		if (detail.get_student()[i] != 0) {
			kind[i] = detail.get_student()[i];
			fixed[i] = true;
			if (kind[i] == -1) { kind[i] = 0; }
		}
		teacher[i].kind_set(detail.get_teacher()[i]);
	}
	reward_money = detail.get_reward();
	bgm = NULL;
}

Mission::~Mission() {
	delete bse;
	if (bgm != NULL) {
		delete[] bgm;
	}
}

//playの前に必ず1回呼ぶこと
void Mission::init() {
	for (int i = 0; i < 3; i++) {//デッキの各カードの画像などをロード
		teacher[i].get_cards()->load_cards();
	}
	bse = new Bsequence(teacher, kind, fixed);
}

void Mission::kind_set(int first, int second, int third) {
	teacher[0].kind_set(first);
	teacher[1].kind_set(second);
	teacher[2].kind_set(third);
}

void Mission::size_set(int first, int second, int third) {
	teacher[0].get_cards()->set_size(first);//デッキのカード枚数
	teacher[1].get_cards()->set_size(second);//デッキのカード枚数
	teacher[2].get_cards()->set_size(third);//デッキのカード枚数
}

void Mission::st_set(bool f, bool s, bool t, int first, int second, int third) {
	fixed[0] = f;
	fixed[1] = s;
	fixed[2] = t;
	kind[0] = first;
	kind[1] = second;
	kind[2] = third;
}

//使用したキャラの数1～3 kindを左に寄せる
int student_sum(int kind[]) {
	int sum = 0;
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (kind[i] != 0) {
			kind[sum] = kind[i];
			sum++;
		}
	}
	return sum;
}

//会話パート 終了処理までする
void Mission::text_play() {
	if (com->play() == 1) {
		now++;
		delete com;
		com = NULL;
	}
}

//キャラの移動
void Mission::move(int num, int kind, int gx, int speed) {
	if (com->get_num() == num && (num == 0 || left_click() == 1 || !com->get_flag())) {
		com->move(kind, gx, speed);//移動
	}
}
//イベントの挿絵
void Mission::event(int num, const char* path) {
	if (com->get_num() == num && (left_click() == 1 || !com->get_flag())) {
		if (left_click() == 1 && com->get_flag()) {
			com->set_event(LoadGraph(path));
		}
		com->event();
	}
}
//場面終了
void Mission::finish(int num) {
	if (com->get_num() == num && (left_click() == 1 || !com->get_flag())) {
		com->finish();
	}
}

//num1 - num2 まで指定のBGMを繰り返し流す
void Mission::play_bgm(int handle, int num1, int num2) {
	if (right_click() == 60) {
		StopSoundMem(handle);
	}
	else if (com->get_num() >= num1 && com->get_num() < num2) {
		if (CheckSoundMem(handle) == 0) {
			PlaySoundMem(handle, DX_PLAYTYPE_BACK);
		}
	}
	else if((right_click() == 60 || (left_click() == 1 || !com->get_flag())) && CheckSoundMem(handle) != 0){
		StopSoundMem(handle);
	}
}

void Mission::volume_bgm(int sum) {
	for (int i = 0; i < sum; i++) {
		ChangeVolumeSoundMem(255 * 60 / 100, bgm[i]);
	}
}

//報酬を受け取る 外部からmenuが呼び出す
bool Mission::reward(int result, int& money, Card& c) {
	if (result == WIN) {
		money += reward_money;
		int sum = student_sum(kind);//戦った生徒は誰？
		if (sum == 0)return true;
		int r;
		r = GetRand(sum * 3) % sum;//どのキャラのカードか？r = GetRand(sum - 1)
		reward_card(kind[r], c, rare[0], rare[1], rare[2], rare[3]);//報酬のカードを手に入れて
		c.load_card();
		bse->battle_end(kind[r], c);//バトルシーケンス終了処理 カードを倉庫へ入れてセーブ
	}
	else {
		money += reward_money / 10;
	}

	return true;
}


int* MissionText::get_student() {
	return kind;
}
int* MissionText::get_teacher() {
	return teacher;
}
int MissionText::get_reward() {
	return reward;
}

//ミッションの詳細を表示する
const int BLACK = GetColor(0, 0, 0);
bool MissionText::picture_mission(int x, int y, int hand_x, int hand_y, bool clear) {
	const int WIDE = 200, HEIGHT = 250;
	bool mouse_flag = false;
	if (hand_x >= x && hand_x <= x + WIDE && hand_y >= y && hand_y <= y + HEIGHT) {
		DrawBox(x - 2, y - 2, x + WIDE + 2, y + HEIGHT + 2, GetColor(200, 200, 0), TRUE);
		mouse_flag = true;
	}
	DrawBox(x, y, x + WIDE, y + HEIGHT, GetColor(150, 150, 150), TRUE);
	DrawBox(x+3, y+28, x + WIDE-3, y + HEIGHT-143, GetColor(100, 100, 250), TRUE);
	x += 2;
	DrawFormatString(x, y + 5, BLACK, name.data());
	std::string str[4];
	unsigned int now = 0;
	while (now < text.length()) {
		str[now / 22] += text[now];
		now++;
	}
	for (int i = 0; i < 4; i++) {
		DrawFormatString(x+3, y + 30 + (i * 20), BLACK, str[i].data());
	}
	DrawFormatString(x, y + HEIGHT - 140, BLACK, "難易度:%d", difficult);
	DrawFormatString(x, y + HEIGHT - 120, BLACK, "報酬:%d", reward);
	DrawFormatString(x, y + HEIGHT - 100, BLACK, "固定キャラ:");
	DrawFormatString(x + 10, y + HEIGHT - 80, BLACK, "%s, %s, %s", student_name(kind[0]), student_name(kind[1]), student_name(kind[2]));
	DrawFormatString(x, y + HEIGHT - 60, BLACK, "敵:");
	DrawFormatString(x + 10, y + HEIGHT - 40, BLACK, "%s, %s, %s", teacher_name(teacher[0]), teacher_name(teacher[1]), teacher_name(teacher[2]));
	if(clear){ DrawFormatString(x, y + HEIGHT - 20, GetColor(255, 0, 0), "クリア済み"); }
	return mouse_flag;
}

void Mission::set_rare_rate(int c_rate, int b_rate, int a_rate, int s_rate) {
	rare[0] = c_rate;
	rare[1] = b_rate;
	rare[2] = a_rate;
	rare[3] = s_rate;
}

void MissionText::set_st(int first, int second, int third) {
	kind[0] = first;
	kind[1] = second;
	kind[2] = third;
}
void MissionText::set_te(int first, int second, int third) {
	teacher[0] = first;
	teacher[1] = second;
	teacher[2] = third;
}
void MissionText::set_te_random() {
	int candidate[Z_T];
	for (int i = 0; i < Z_T; i++) {
		candidate[i] = i + 1;
	}
	for (int k = 0; k < 10; k++) {
		for (int i = Z_T; i > 1; i--) {
			int j = GetRand(100) % i;
			i--;
			int t = candidate[i];
			candidate[i] = candidate[j];
			candidate[j] = t;
		}
	}
	for (int i = 0; i < 3; i++) {
		teacher[i] = candidate[i];
	}
}

//メインミッションの章をまたがる途切れ
void chapter_cancel(int* now, const bool clear[]) {
	if (*now != MM1 && *now != MM4 && *now != MM7 && *now != MM10 && *now != MM14 && *now != MM17 && *now != MM20) {
		if (!clear[*now - 1]) {
			*now = NO_MISSION;//章の始め以外は前の日をクリアする必要アリ
		}
	}
	if (*now >= MM10 && *now <= MM12 && !clear[MM9]) {
		*now = NO_MISSION;//3章をクリアしないと4章できない
	}
	if (*now >= MM14 && *now <= MM16 && !clear[MM6]) {
		*now = NO_MISSION;//3章をクリアしないと5章できない
	}
	if (*now >= MM17 && *now <= MM19 && (!clear[MM3] || !clear[MM12])) {
		*now = NO_MISSION;//1,3章をクリアしないと6章できない
	}
	if (*now == MM20 && (!clear[MM3] || !clear[MM16] || !clear[MM19])) {
		*now = NO_MISSION;//1,5,6章をクリアしないと7章できない
	}
}
//今日のミッションを決める（ルールに沿って）
void mission_rule(int today_mission[], bool clear[], int day) {
	if (day == 1 && !clear[MM1]) {//一番最初
		today_mission[0] = MM1;
		today_mission[1] = NO_MISSION;
		today_mission[2] = NO_MISSION;
		return;
	}
	//メインミッション
	today_mission[0] = day-1;
	chapter_cancel(&today_mission[0], clear);
	if (today_mission[0] == NO_MISSION) {//メインミッションが途切れてたらEMをランダム
		today_mission[0] = GetRand(EM20 - EM1) + EM1;
	}
	//サブミッション
	if (day == 1) {
		//today_mission[1] = SM1;
	}
	else if (clear[day - 2]) {
		//today_mission[1] = day + SM1 - 1;
	}
	if(today_mission[1] == NO_MISSION) {//サブミッションが途切れてたらEMをランダム
		do{
			today_mission[1] = GetRand(EM20 - EM1) + EM1;
		} while (today_mission[0] == today_mission[1]);
		//today_mission[1] = EM1;
	}
	//イベントミッション
	do{
		today_mission[2] = GetRand(EM20 - EM1) + EM1;
	} while (today_mission[0] == today_mission[2] || today_mission[1] == today_mission[2]);
}

//戦闘前と後に一回ずつの会話があるパターン
int Mission::normal_text() {
	switch (now) {
	case 0:
		text1();
		break;
	case 1:
		result = bse->battle_loop(kind);//使用したキャラをkindに入れて教えてもらう
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 2:
		text2();
		break;
	case 3:
		return result;
		break;
	}
	return 0;
}

//戦闘後に一回会話があるパターン
int Mission::after_text() {
	switch (now) {
	case 0:
		result = bse->battle_loop(kind);//使用したキャラをkindに入れて教えてもらう
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 1:
		text2();
		break;
	case 2:
		return result;
		break;
	}
	return 0;
}

//kindに応じてミッションを確保
void get_mission(Mission*& m, int kind, MissionText& detail) {
	switch (kind) {
	case MM1:
		m = new MainMission1(detail);
		break;
	case MM2:
		m = new MainMission2(detail);
		break;
	case MM3:
		m = new MainMission3(detail);
		break;
	case MM4:
		m = new MainMission4(detail);
		break;
	case MM5:
		m = new MainMission5(detail);
		break;
	case MM6:
		m = new MainMission6(detail);
		break;
	case MM7:
		m = new MainMission7(detail);
		break;
	case MM8:
		m = new MainMission8(detail);
		break;
	case MM9:
		m = new MainMission9(detail);
		break;
	case MM10:
		m = new MainMission10(detail);
		break;
	case MM11:
		m = new MainMission11(detail);
		break;
	case MM12:
		m = new MainMission12(detail);
		break;
	case MM13:
		m = new MainMission13(detail);
		break;
	case MM14:
		m = new MainMission14(detail);
		break;
	case MM15:
		m = new MainMission15(detail);
		break;
	case MM16:
		m = new MainMission16(detail);
		break;
	case MM17:
		m = new MainMission17(detail);
		break;
	case MM18:
		m = new MainMission18(detail);
		break;
	case MM19:
		m = new MainMission19(detail);
		break;
	case MM20:
		m = new MainMission20(detail);
		break;
	case SM1:

		break;
	case DM1:
		m = new DailyMission1(detail);
		break;
	case DM2:
		m = new DailyMission2(detail);
		break;
	case DM3:
		m = new DailyMission3(detail);
		break;
	case DM4:
		m = new DailyMission4(detail);
		break;
	case DM5:
		m = new DailyMission5(detail);
		break;
	case DM6:
		m = new DailyMission6(detail);
		break;
	case DM7:
		m = new DailyMission7(detail);
		break;
	case DM8:
		m = new DailyMission8(detail);
		break;
	case DM9:
		m = new DailyMission9(detail);
		break;
	case DM10:
		m = new DailyMission10(detail);
		break;
	case EM1:
		m = new EventMission1(detail);
		break;
	case EM2:
		m = new EventMission2(detail);
		break;
	case EM3:
		m = new EventMission3(detail);
		break;
	case EM4:
		m = new EventMission4(detail);
		break;
	case EM5:
		m = new EventMission5(detail);
		break;
	case EM6:
		m = new EventMission6(detail);
		break;
	case EM7:
		m = new EventMission7(detail);
		break;
	case EM8:
		m = new EventMission8(detail);
		break;
	case EM9:
		m = new EventMission9(detail);
		break;
	case EM10:
		m = new EventMission10(detail);
		break;
	case EM11:
		m = new EventMission11(detail);
		break;
	case EM12:
		m = new EventMission12(detail);
		break;
	case EM13:
		m = new EventMission13(detail);
		break;
	case EM14:
		m = new EventMission14(detail);
		break;
	case EM15:
		m = new EventMission15(detail);
		break;
	case EM16:
		m = new EventMission16(detail);
		break;
	case EM17:
		m = new EventMission17(detail);
		break;
	case EM18:
		m = new EventMission18(detail);
		break;
	case EM19:
		m = new EventMission19(detail);
		break;
	case EM20:
		m = new EventMission20(detail);
		break;
	default:
		m = new MainMission1(detail);
		break;
	}
}

//////////各ミッションの追加実装は以下から/////////////////
MissionText::MissionText(int m_kind) {
	const int FREE = 0;
	difficult = -1;
	switch (m_kind) {
	case MM1:
		name = "チュートリアル";
		text = "ハンダ：戦闘の基本を確認しよう！";
		set_st(AKATSUKI, HANDA, -1);
		set_te(0, O_T, 0);
		difficult = 1;
		reward = 30;
		break;
	case MM2:
		name = "武器を持っているか？";
		text = "ハンダ：なに、太田が来ない？……分かった、すぐ連絡する！";
		set_st(OOTA, FREE, FREE);
		set_te(0, Q_T, 0);
		difficult = 1;
		reward = 50;
		break;
	case MM3:
		name = "その先生、凶暴につき";
		text = "アカツキ：……………………";
		set_st(-1, AKATSUKI, -1);
		set_te(0, X_T, 0);
		difficult = 4;
		reward = 200;
		break;
	case MM4:
		name = "外来種";
		text = "ハンダ：自由に、とは言ったけど――まさか茨川さん、一人で戦いに行くなんて…………";
		set_st(IBARAKAWA, OOTA, FREE);
		set_te(S_T, T_T, 0);
		difficult = 1;
		reward = 50;
		break;
	case MM5:
		name = "狼";
		text = "イバラカワ：さっさと終わらせて帰ろ。";
		set_st(IBARAKAWA, OWADA, UCHIMOTO);
		set_te(O_T, P_T, Q_T);
		difficult = 1;
		reward = 50;
		break;
	case MM6:
		name = "遺す思い出、壊す約束";
		text = "オオタ：へっ、やっぱあいつ、ほっとけねえな。";
		set_st(IBARAKAWA, OOTA, -1);
		set_te(U_T, V_T, 0);
		difficult = 2;
		reward = 100;
		break;
	case MM7:
		name = "始まりは噂とともに";
		text = "イシ：今日は砥用さんとかあ。怖いなあ。";
		set_st(ISHI, TOMOCHI, -1);
		set_te(0, W_T, 0);
		difficult = 1;
		reward = 50;
		break;
	case MM8:
		name = "私を見て";
		text = "ウチモト：今日？いいよー、大丈夫だよー。";
		set_st(ISHI, UCHIMOTO, -1);
		set_te(0, O_T, 0);
		difficult = 1;
		reward = 50;
		break;
	case MM9:
		name = "完璧人間トモチスエ";
		text = "イシ：どうしよう…………かなりやばい。";
		set_st(ISHI, TOMOCHI, -1);
		set_te(P_T, R_T, S_T);
		difficult = 2;
		reward = 100;
		break;
	case MM10:
		name = "未だ若き、されど足掻き";
		text = "タイラ：ミッションか。大丈夫、協力するさ。";
		set_st(ISHI, TAIRA, 0);
		set_te(X_T, O_T, Q_T);
		difficult = 2;
		reward = 100;
		break;
	case MM11:
		name = "あの日あの時そして今";
		text = "トモチ：まだハンダ来てないかー。早く終わらせたいんだけどなー。";
		set_st(TOMOCHI, HANDA, 0);
		set_te(S_T, V_T, 0);
		difficult = 2;
		reward = 100;
		break;
	case MM12:
		name = "石";
		text = "タイラ：一度、腹を割って話すべきなんだろうな。";
		set_st(TAIRA, 0, 0);
		set_te(0, W_T, 0);
		difficult = 3;
		reward = 150;
		break;
	case MM13:
		name = "試行錯誤的友情";
		text = "イシ：いいんだ…………これで、いいんだ。";
		set_st(ISHI, 0, 0);
		set_te(P_T, R_T, 0);
		difficult = 3;
		reward = 150;
		break;
	case MM14:
		name = "子犬を手懐ける";
		text = "ハンダ：今日は二手に分かれてミッションだ。杏子、そっちはよろしく。";
		set_st(HANDA, AKATSUKI, MIYATONE);
		set_te(Y_T, T_T, 0);
		difficult = 2;
		reward = 100;
		break;
	case MM15:
		name = "繋がる友情";
		text = "イバラカワ：太田…………";
		set_st(HANDA, AKATSUKI, IBARAKAWA);
		set_te(V_T, X_T, 0);
		difficult = 3;
		reward = 150;
		break;
	case MM16:
		name = "ヴァルキリーに捧ぐ";
		text = "アカツキ：……………………";
		set_st(HANDA, OWADA, UCHIMOTO);
		set_te(0, Q_T, 0);
		difficult = 3;
		reward = 150;
		break;
	case MM17:
		name = "無知こそが恐怖の根源";
		text = "オワダ：今日は桃花ちゃんと一緒かあ。";
		set_st(AKATSUKI, OWADA, MIYATONE);
		set_te(O_T, U_T, P_T);
		difficult = 4;
		reward = 200;
		break;
	case MM18:
		name = "青い肌";
		text = "タイラ：砥用がいるし、サポートにっ徹するのがいいかもな。";
		set_st(OWADA, TOMOCHI, TAIRA);
		set_te(Y_T, R_T, 0);
		difficult = 4;
		reward = 200;
		break;
	case MM19:
		name = "宮利根方程式";
		text = "ミヤトネ：ごめんなさいごめんなさいごめんなさい――";
		set_st(AKATSUKI, MIYATONE, -1);
		set_te(0, T_T, 0);
		difficult = 4;
		reward = 200;
		break;
	case MM20:
		name = "焼ける青春の客";
		text = "ハンダ：……………………";
		set_st(AKATSUKI, HANDA, -1);
		set_te(0, Z_T, 0);
		difficult = 5;
		reward = 250;
		break;
	case SM1:

		break;
	case DM1:
		name = "デイリーミッション";
		text = "{参加者募集中}";
		set_st(0, 0, 0);
		set_te_random();
		difficult = 1;
		reward = 50;
		break;
	case DM2:
		name = "デイリーミッション";
		text = "{参加者募集中}";
		set_st(0, 0, 0);
		set_te_random();
		difficult = 2;
		reward = 100;
		break;
	case DM3:
		name = "デイリーミッション";
		text = "{参加者募集中}";
		set_st(0, 0, 0);
		set_te_random();
		difficult = 3;
		reward = 150;
		break;
	case DM4:
		name = "デイリーミッション";
		text = "{参加者募集中}";
		set_st(0, 0, 0);
		set_te_random();
		difficult = 4;
		reward = 200;
		break;
	case DM5:
		name = "デイリーミッション";
		text = "{参加者募集中}";
		set_st(0, 0, 0);
		set_te_random();
		difficult = 5;
		reward = 250;
		break;
	case DM6:
		name = "デイリーミッション";
		text = "{参加者募集中}";
		set_st(0, 0, 0);
		set_te_random();
		difficult = 6;
		reward = 300;
		break;
	case DM7:
		name = "デイリーミッション";
		text = "{参加者募集中}";
		set_st(0, 0, 0);
		set_te_random();
		difficult = 7;
		reward = 350;
		break;
	case DM8:
		name = "デイリーミッション";
		text = "{参加者募集中}";
		set_st(0, 0, 0);
		set_te_random();
		difficult = 8;
		reward = 400;
		break;
	case DM9:
		name = "デイリーミッション";
		text = "{参加者募集中}";
		set_st(0, 0, 0);
		set_te_random();
		difficult = 9;
		reward = 450;
		break;
	case DM10:
		name = "デイリーミッション";
		text = "{参加者募集中}";
		set_st(0, 0, 0);
		set_te_random();
		difficult = 10;
		reward = 500;
		break;
	}
	if (difficult != -1) { return; }
	switch (m_kind) {
	case EM1:
		name = "イベントミッション１";
		text = "オワダ：覚えてる？懐かしいよね～";
		set_st(1, 2, 6);
		set_te(O_T, P_T, T_T);
		difficult = 3;
		reward = 150;
		break;
	case EM2:
		name = "イベントミッション２";
		text = "トモチ：ねえ、好きな人とかいるの？";
		set_st(3, 6, 10);
		set_te(O_T, Q_T, V_T);
		difficult = 3;
		reward = 150;
		break;
	case EM3:
		name = "イベントミッション３";
		text = "ウチモト：あーまたテスト近いなー。";
		set_st(8, 5, 0);
		set_te(O_T, S_T, U_T);
		difficult = 3;
		reward = 150;
		break;
	case EM4:
		name = "イベントミッション４";
		text = "ミヤトネ：はあ……今回やばい……";
		set_st(4, 5, 7);
		set_te(O_T, T_T, Y_T);
		difficult = 3;
		reward = 150;
		break;
	case EM5:
		name = "イベントミッション５";
		text = "イバラカワ：まったく。どうして私に頼むの。";
		set_st(1, 9, 0);
		set_te(O_T, U_T, X_T);
		difficult = 4;
		reward = 200;
		break;
	case EM6:
		name = "イベントミッション６";
		text = "オオタ：賢介、また今度遊ぼうぜ！";
		set_st(3, 4, 0);
		set_te(P_T, Q_T, W_T);
		difficult = 4;
		reward = 200;
		break;
	case EM7:
		name = "イベントミッション７";
		text = "ハンダ：ごめん、急に協力頼んじゃって。";
		set_st(2, 8, 9);
		set_te(P_T, R_T, U_T);
		difficult = 4;
		reward = 200;
		break;
	case EM8:
		name = "イベントミッション８";
		text = "ミヤトネ：砥用さんって、ちょっと怖いかも……";
		set_st(7, 10, 0);
		set_te(P_T, U_T, Z_T);
		difficult = 4;
		reward = 200;
		break;
	case EM9:
		name = "イベントミッション９";
		text = "イシ：（うわあ、女子二人と……怖い！！）";
		set_st(4, 6, 9);
		set_te(Q_T, S_T, W_T);
		difficult = 5;
		reward = 250;
		break;
	case EM10:
		name = "イベントミッション１０";
		text = "ハンダ：二人とも、ありがとう。またよろしく。";
		set_st(2, 7, 10);
		set_te(Q_T, T_T, Z_T);
		difficult = 5;
		reward = 250;
		break;
	case EM11:
		name = "イベントミッション１１";
		text = "アカツキ：……………………";
		set_st(1, 3, 8);
		set_te(Q_T, W_T, X_T);
		difficult = 5;
		reward = 250;
		break;
	case EM12:
		name = "イベントミッション１２";
		text = "ハンダ：いい作戦が思いついたぞ！";
		set_st(2, 5, 6);
		set_te(R_T, S_T, V_T);
		difficult = 5;
		reward = 250;
		break;
	case EM13:
		name = "イベントミッション１３";
		text = "イシ：砥用さんとか……気を付けないと。";
		set_st(4, 10, 0);
		set_te(R_T, U_T, X_T);
		difficult = 6;
		reward = 300;
		break;
	case EM14:
		name = "イベントミッション１４";
		text = "タイラ：茨川とか……そういえばあまりしゃべる機会なかったな。";
		set_st(5, 9, 7);
		set_te(S_T, T_T, V_T);
		difficult = 6;
		reward = 300;
		break;
	case EM15:
		name = "イベントミッション１５";
		text = "トモチ：赤月ってさー、なんでそんなにしゃべんないのー？";
		set_st(1, 2, 10);
		set_te(S_T, V_T, W_T);
		difficult = 6;
		reward = 300;
		break;
	case EM16:
		name = "イベントミッション１６";
		text = "オオタ：あぁ！！くそ、勘弁してくれ！！";
		set_st(3, 6, 7);
		set_te(T_T, U_T, W_T);
		difficult = 6;
		reward = 300;
		break;
	case EM17:
		name = "イベントミッション１７";
		text = "ウチモト：太田には仲間意識持ってるよ！";
		set_st(3, 5, 8);
		set_te(T_T, V_T, X_T);
		difficult = 7;
		reward = 350;
		break;
	case EM18:
		name = "イベントミッション１８";
		text = "イシ：アインシュタインって凄いよ！";
		set_st(4, 8, 9);
		set_te(T_T, W_T, Y_T);
		difficult = 7;
		reward = 350;
		break;
	case EM19:
		name = "イベントミッション１９";
		text = "太田：あーちくしょう！！ついてねえ！！";
		set_st(1, 2, 3);
		set_te(U_T, V_T, Z_T);
		difficult = 7;
		reward = 350;
		break;
	case EM20:
		name = "イベントミッション２０";
		text = "オワダ：ねー、いっつも思うんだけどさー。";
		set_st(4, 5, 6);
		set_te(V_T, X_T, Y_T);
		difficult = 7;
		reward = 350;
		break;
	}
}

//ミッションごとの先生の情報を以下に
//MM1
//先生の種類とカード
MainMission1::MainMission1(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/piano21.mp3");
	volume_bgm(1);
	set_rare_rate(100, 0, 0, 0);
	size_set(9, 9, 9);
	teacher[1].get_cards()->set_card(Card(22, 0, 300, 30, 120, 110), 0);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 30, 120, 110), 1);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 30, 120, 110), 2);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 30, 120, 110), 3);
}
//会話とバトルをする
int MainMission1::play() {
	return normal_text();
}
void MainMission1::text1() {
	if (com == NULL) {
		int x[10] = { 10,500,-100,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("ハンダ", "これから毎日先生と戦っていくことになるけど、まずは戦闘の基本を確認するよ。", H_NORMAL, 1);
		com->text_insert("アカツキ", "…………", H_NORMAL, 0);
		com->text_insert("ハンダ", "今回は僕と亮也の二人でO先生と戦おう。", H_NORMAL, 1);
		com->text_insert("アカツキ", "…………", H_NORMAL, 0);
		com->text_insert("ハンダ", "戦闘は各生徒が最大５枚のカードで構成したデッキを使っておこなわれる。", H_NORMAL, 1);
		com->text_insert("ハンダ", "カードは一枚目から順番に発動されていくよ。", H_NORMAL, 1);
		com->text_insert("ハンダ", "ただし、カードを発動するのには時間がかかる。", H_NORMAL, 1);
		com->text_insert("ハンダ", "発動するのにかかる時間はカードによって違うよ。", H_NORMAL, 1);
		com->text_insert("ハンダ", "カードを発動しようとしているところに相手からの攻撃を受けてしまうと、カードは発動せずに次のカードに移ってしまう。", H_NORMAL, 1);
		com->text_insert("ハンダ", "だから、カードの発動時間、発動後の硬直時間を考えてデッキを作ることが大事だよ。", H_NORMAL, 1);
		com->text_insert("アカツキ", "…………", H_NORMAL, 0);
		com->text_insert("ハンダ", "生徒と先生の全員が最後のカードまで発動したら、もう一度デッキを組みなおせる。", H_NORMAL, 1);
		com->text_insert("ハンダ", "相手がどれくらいのタイミングでどんな技を使ってきたかを考慮してデッキを作り直すと有効だ。", H_NORMAL, 1);
		com->text_insert("ハンダ", "まあ、習うより慣れろってことで、早速行こうか。", H_NORMAL, 1);
		com->text_insert("アカツキ", "…………", H_NORMAL, 0);
	}
	move(15, 0, -500, 1);//移動
	move(15, 1, -500, 5);//移動
	finish(15);
	play_bgm(bgm[0], 1, 15);
	text_play();
}
void MainMission1::text2() {
	if (com == NULL) {
		int x[10] = { 10,500,-100,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("ハンダ", "いい感じにできたね。", H_NORMAL, 1);
		com->text_insert("アカツキ", "…………", H_NORMAL, 0);
		com->text_insert("ハンダ", "戦闘後は報酬がもらえるよ。", H_NORMAL, 1);
		com->text_insert("ハンダ", "勝った方が報酬が高いうえに、カードを一枚ランダムでもらえる。", H_MOUSOU, 1);
		com->text_insert("ハンダ", "もらえるカードの種類は、戦った生徒のうちの誰かのだよ。", H_MOUSOU, 1);
		com->text_insert("ハンダ", "報酬やカードを売って手に入れたお金はアイテムを買うのに使える。", H_MOUSOU, 1);
		com->text_insert("ハンダ", "アイテムは一度使ったら消えちゃうけど、誰のデッキにも組み込めるし、強力だよ。", H_MOUSOU, 1);
	}
	//move(1, 9, 300, 5);//移動
	//event(3, "event/トモチデート勧誘.png");//挿絵
	text_play();
}

//MM2
//先生の種類とカード
MainMission2::MainMission2(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[2];//bgm確保
	bgm[0] = LoadSoundMem("bgm/piano32.mp3");
	bgm[1] = LoadSoundMem("bgm/piano28.mp3");
	volume_bgm(2);
	set_rare_rate(100, 0, 0, 0);
	size_set(9, 9, 9);
	teacher[1].get_cards()->set_card(Card(24, 0, 1000, 50, 90, 100), 0);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 120, 110), 1);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 120, 110), 2);
	teacher[1].get_cards()->set_card(Card(24, 0, 0, 50, 90, 100), 3);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 120, 110), 4);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 120, 110), 5);
}
//会話とバトルをする
int MainMission2::play() {
	return normal_text();
}
void MainMission2::text1() {
	if (com == NULL) {
		int x[10] = { 10,500,-100,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("ハンダ", "おい、太田！今どこにいるんだ！？", H_NORMAL, 1);
		com->text_insert("オオタ", "おー裕斗、戦い行ってるぜー。", H_NORMAL, 2);
		com->text_insert("オオタ", "ちょっくらQ先生とやりあってくるぜ。", H_NORMAL, 2);
		com->text_insert("ハンダ", "はあ！？待てよ、勝手な、ことを……", H_NORMAL, 1);
		com->text_insert("オオタ", "あーいーだろ別にぃ。", H_NORMAL, 2);
		com->text_insert("オオタ", "ちゃんと倒したらみんなも助かるだろ？", H_NORMAL, 2);
		com->text_insert("ハンダ", "そういう、問題じゃ、ない……！！", H_NORMAL, 1);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
		com->text_insert("ハンダ", "はあ、わかった。せめてちょっと待ってくれ。", H_NORMAL, 1);
		com->text_insert("ハンダ", "今すぐ、そっちに何人か送る。", H_NORMAL, 1);
		com->text_insert("ハンダ", "だから、一人で突っ込まないでくれよ。", H_NORMAL, 1);
		com->text_insert("オオタ", "おいおい、別にいいって。", H_NORMAL, 2);
		com->text_insert("ハンダ", "頼むから、待っててくれ。", H_NORMAL, 1);
		com->text_insert("オオタ", "…………わーったよ。", H_NORMAL, 2);
	}
	play_bgm(bgm[0], 1, 14);
	finish(14);
	text_play();
}
void MainMission2::text2() {
	if (com == NULL) {
		int x[10] = { 10,500,-100,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("ハンダ", "…………はあ、まったく太田は……", H_NORMAL, 1);
		com->text_insert("アカツキ", "心配なのか？", H_NORMAL, 0);
		com->text_insert("ハンダ", "まあ、な……あいつだからってわけじゃない。", H_NORMAL, 1);
		com->text_insert("ハンダ", "これは後でみんなに言おうとしてたんだけど、なるべく一人で戦うのはやめてほしいんだ。", H_NORMAL, 1);
		com->text_insert("アカツキ", "…………", H_NORMAL, 0);
		com->text_insert("ハンダ", "あと、戦う前は絶対に僕に教えてほしいんだ。", H_NORMAL, 1);
		com->text_insert("ハンダ", "特に、Ｘ先生なんかは絶対に一人で戦っちゃだめだ。あの人はやばい。", H_NORMAL, 1);
		com->text_insert("ハンダ", "入念に準備をして戦うべき相手だ。", H_NORMAL, 1);
		com->text_insert("ハンダ", "まあ、亮也とは付き合い長いし……言う必要もないとは思うけど……", H_NORMAL, 1);
		com->text_insert("ハンダ", "これは後でみんなにも言っとくよ。", H_NORMAL, 1);
		com->text_insert("アカツキ", "ああ。", H_NORMAL, 0);
		com->text_insert("アカツキ", "…………", H_NORMAL, 0);
		com->text_insert("アカツキ", "（半田は――）", H_MOUSOU, 0);
		com->text_insert("アカツキ", "（どうして、先生と戦おうなんて言い出したんだろうか。）", H_MOUSOU, 0);
		com->text_insert("アカツキ", "（まあ、オレは深く考える必要ない。）", H_MOUSOU, 0);
		com->text_insert("アカツキ", "（いつも通り、ただ半田の後ろをついていけばいい。）", H_MOUSOU, 0);
	}
	play_bgm(bgm[1], 1, 16);
	finish(16);
	text_play();
}

//MM3
//先生の種類とカード
MainMission3::MainMission3(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[2];//bgm確保
	bgm[0] = LoadSoundMem("bgm/piano29.mp3");
	bgm[1] = LoadSoundMem("bgm/piano22.mp3");
	volume_bgm(2);
	set_rare_rate(70, 21, 8, 1);
	size_set(9, 9, 9);
	teacher[1].get_cards()->set_card(Card(3, 0, 900, 90, 130, 110), 0);//hp500
	teacher[1].get_cards()->set_card(Card(3, 0, 0, 90, 130, 110), 1);
	teacher[1].get_cards()->set_card(Card(2, 0, 0, 50, 90, 100), 2);
	teacher[1].get_cards()->set_card(Card(23, 0, 0, 110, 160, 140), 3);
	teacher[1].get_cards()->set_card(Card(3, 0, 0, 90, 130, 110), 4);
}
//会話とバトルをする
int MainMission3::play() {
	return normal_text();
}
void MainMission3::text1() {
	if (com == NULL) {
		int x[10] = { 10,500,-100,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("ハンダ", "なに……！？わかった、今すぐ行く。", H_NORMAL, 1);
		com->text_insert("アカツキ", "…………", H_NORMAL, 0);
		com->text_insert("ハンダ", "ごめん亮也、ちょっとオワダのとこ行ってくる。", H_NORMAL, 1);
		com->text_insert("アカツキ", "ああ。", H_NORMAL, 0);
		com->text_insert("ハンダ", "…………ほんと、ごめん。今日は先帰ってていいよ。", H_NORMAL, 1);
		com->text_insert("アカツキ", "ああ。", H_NORMAL, 0);
		com->text_insert("アカツキ", "（…………帰るか。）", H_MOUSOU, 0);
		com->text_insert("アカツキ", "…………あ", H_NORMAL, 0);
		com->text_insert("X先生", "おお、お前は…………赤月か。部活は？", H_NORMAL, 10);
		com->text_insert("アカツキ", "帰宅部です", H_NORMAL, 0);
		com->text_insert("X先生", "ああ、そうか。", H_NORMAL, 10);
		com->text_insert("アカツキ", "じゃあ、先生、さようなら。", H_NORMAL, 0);
		com->text_insert("X先生", "部活ぐらい入ったらどうだ？", H_NORMAL, 10);
		com->text_insert("アカツキ", "…………", H_NORMAL, 0);
		com->text_insert("X先生", "お前、ヒョロヒョロじゃねえか。男らしくねえ。", H_NORMAL, 10);
		com->text_insert("アカツキ", "……そんなの、オレの勝手でしょう", H_NORMAL, 0);
		com->text_insert("X先生", "あ？……お前なんだ、その態度は。", H_NORMAL, 10);
		com->text_insert("アカツキ", "（部活？くだらない。）", H_MOUSOU, 0);
		com->text_insert("アカツキ", "（俺は部活に入るとか入らないとか、そんなことよりもずっと違う世界を見てるんだ。）", H_MOUSOU, 0);
		com->text_insert("アカツキ", "（それでいいはずなのに……なんだ、この苛立ちは。）", H_MOUSOU, 0);
		com->text_insert("X先生", "おい、なんだっつってんだろ！！", H_NORMAL, 10);
		com->text_insert("アカツキ", "（ヒョロヒョロだとか、男らしいとか、そんなこともどうだっていいはずだ。）", H_MOUSOU, 0);
		com->text_insert("アカツキ", "（それなのに……なんなんだ。無性に腹が立つ。）", H_MOUSOU, 0);
		com->text_insert("X先生", "おい、なに黙ってんだ！！なめてんのか！！", H_NORMAL, 10);
		com->text_insert("アカツキ", "……先生ってさ、すぐ怒鳴るよな。", H_NORMAL, 0);
	}
	play_bgm(bgm[0], 1, 25);
	move(6, 1, -300, 6);//移動
	move(7, 0, 700, 5);//移動
	move(8, 0, 400, 5);//移動
	move(8, 10, 300, 5);//移動
	move(12, 0, 50, 5);//移動
	finish(7);
	text_play();
}
void MainMission3::text2() {
	if (com == NULL) {
		int x[10] = { 300,-100,-100,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, 500);
		com->text_insert("X先生", "ぐぉ……が、あ…………", H_NORMAL, 10);
		com->text_insert("アカツキ", "…………", H_NORMAL, 0);
		com->text_insert("X先生", "お前っ…………お前……！！", H_NORMAL, 10);
		com->text_insert("アカツキ", "…………", H_NORMAL, 0);
		com->text_insert("X先生", "俺がっ！！……絶対　躾けて……！！", H_NORMAL, 10);
		com->text_insert("アカツキ", "先生はここで死ぬんだ。――ここで、終わりだ。", H_NORMAL, 0);
		com->text_insert("X先生", "…………お前…………", H_NORMAL, 10);
		com->text_insert("X先生", "……勘違いしてんだよ、お前は…………", H_NORMAL, 10);
		com->text_insert("アカツキ", "あんたは……クズだ。邪魔な存在だ。", H_NORMAL, 0);
		com->text_insert("X先生", "拗らせてんだよ、お前は……", H_NORMAL, 10);
		com->text_insert("X先生", "お前は、大人に反抗してるだけの生意気なガキだ。", H_NORMAL, 10);
		com->text_insert("アカツキ", "あんただけじゃない――", H_NORMAL, 0);
		com->text_insert("X先生", "は…………？", H_NORMAL, 10);
		com->text_insert("アカツキ", "先生も、みんなも、半田も――", H_NORMAL, 0);
		com->text_insert("アカツキ", "どいつもこいつもクズばっか…………", H_NORMAL, 0);
		com->text_insert("アカツキ", "みんなみんな、死ねばいい…………", H_NORMAL, 0);
		com->text_insert("X先生", "……………………", H_NORMAL, 10);
		com->text_insert("アカツキ", "…………", H_NORMAL, 0);
		com->text_insert("X先生", "お前…………狂ってる…………", H_NORMAL, 10);
	}
	play_bgm(bgm[1], 14, 19);
	finish(19);
	text_play();
}

//MM4
//先生の種類とカード
MainMission4::MainMission4(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/piano40.mp3");
	volume_bgm(1);
	set_rare_rate(100, 0, 0, 0);
	size_set(9, 9, 9);
	teacher[0].get_cards()->set_card(Card(8, 0, 300, 30, 70, 90), 0);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 60, 110, 110), 1);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 60, 110, 110), 2);
	teacher[0].get_cards()->set_card(Card(8, 0, 0, 30, 70, 90), 3);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 60, 110, 110), 4);

	teacher[1].get_cards()->set_card(Card(22, 0, 300, 55, 120, 100), 0);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 55, 120, 100), 1);
	teacher[1].get_cards()->set_card(Card(5, 0, 0, 40, 80, 70), 2);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 55, 120, 100), 3);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 55, 120, 100), 4);
}
//会話とバトルをする
int MainMission4::play() {
	return normal_text();
}
void MainMission4::text1() {
	if (com == NULL) {
		int x[10] = { -100,-100,400,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("ハンダ", "オオタ、今いいか？", H_NORMAL, 1);
		com->text_insert("オオタ", "ああいいぜ、ミッションか？", H_NORMAL, 2);
		com->text_insert("ハンダ", "イバラカワにS先生とT先生を倒すのを任せたんだが、誰も誘わず一人で行ったんだ。", H_NORMAL, 1);
		com->text_insert("オオタ", "…………そうか、わかった。助けに行けばいいんだな？", H_NORMAL, 2);
		com->text_insert("ハンダ", "ああ。あとできれば一人、そっちに送る。頼んだぞ。", H_NORMAL, 1);
		com->text_insert("オオタ", "任せろ！！すぐ向かうぜ！！", H_NORMAL, 2);
	}
	move(6, 2, -200, 8);//移動
	finish(6);
	text_play();
}
void MainMission4::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,100,-100,-100,-100,-100,-100,300,-100 };
		com = new Text(x, -100);
		com->text_insert("イバラカワ", "どうして", H_NORMAL, 8);
		com->text_insert("オオタ", "それはこっちのセリフだっての。なんで一人で戦おうとしたんだよ。", H_NORMAL, 2);
		com->text_insert("イバラカワ", "別に……いいでしょ。私の勝手。", H_NORMAL, 8);
		com->text_insert("オオタ", "後で裕斗にも言われると思うけどな……一人で戦うなよ。", H_NORMAL, 2);
		com->text_insert("イバラカワ", "あんたもこの前半田に注意されたって聞いたけど。人のこと言える？", H_NORMAL, 8);
		com->text_insert("オオタ", "オレのことは関係ない。とにかく、これからはオレのこと呼べよ。", H_NORMAL, 2);
		com->text_insert("イバラカワ", "なにそれ……ほっといてよ。", H_NORMAL, 8);
		com->text_insert("オオタ", "ほっとけねーよ。遠慮すんなって！", H_NORMAL, 2);
		com->text_insert("イバラカワ", "遠慮してるわけじゃねーよ！バカ！！", H_NORMAL, 8);
		com->text_insert("オオタ", "お、おい！！", H_NORMAL, 2);
		com->text_insert("オオタ", "…………。", H_NORMAL, 2);
	}
	play_bgm(bgm[0], 1, 11);
	move(9, 8, 800, 6);//移動
	move(10, 2, 300, 6);//移動
	finish(11);
	text_play();
}

//MM5
//先生の種類とカード
MainMission5::MainMission5(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/piano40.mp3");
	volume_bgm(1);
	set_rare_rate(90, 10, 0, 0);
	size_set(9, 9, 9);
	teacher[0].get_cards()->set_card(Card(22, 0, 200, 50, 110, 100), 0);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 50, 110, 100), 1);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 50, 110, 100), 2);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 50, 110, 100), 3);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 50, 110, 100), 4);

	teacher[1].get_cards()->set_card(Card(7, 0, 200, 30, 90, 60), 0);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 50, 100, 110), 1);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 50, 100, 110), 2);
	teacher[1].get_cards()->set_card(Card(7, 0, 0, 30, 90, 60), 3);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 50, 100, 110), 4);

	teacher[2].get_cards()->set_card(Card(22, 0, 200, 50, 120, 120), 0);
	teacher[2].get_cards()->set_card(Card(24, 0, 0, 20, 90, 110), 1);
	teacher[2].get_cards()->set_card(Card(3, 0, 0, 60, 140, 140), 2);
	teacher[2].get_cards()->set_card(Card(22, 0, 0, 50, 120, 120), 3);
	teacher[2].get_cards()->set_card(Card(22, 0, 0, 50, 120, 120), 4);
}
//会話とバトルをする
int MainMission5::play() {
	return normal_text();
}
void MainMission5::text1() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,-100,-100,100,-100,200,400,-100 };
		com = new Text(x, -100);
		com->text_insert("オワダ", "さあ、いくよ！二人とも準備はいい？", H_NORMAL, 5);
		com->text_insert("ウチモト", "オッケーだよー！！", H_NORMAL, 7);
		com->text_insert("イバラカワ", "うん。", H_NORMAL, 8);
		com->text_insert("オワダ", "…………よし、じゃあ行こうか。", H_NORMAL, 5);
	}
	move(4, 5, -400, 4);//移動
	move(4, 7, -400, 4);//移動
	move(4, 8, -400, 4);//移動
	finish(4);
	text_play();
}
void MainMission5::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,-100,-100,100,-100,200,300,-100 };
		com = new Text(x, -100);
		com->text_insert("ウチモト", "やったー！ありがとう杏子！", H_NORMAL, 7);
		com->text_insert("オワダ", "うん。うまく連携取れたわね。", H_NORMAL, 5);
		com->text_insert("ウチモト", "天子ちゃんもありがとう！またよろしくね！", H_NORMAL, 7);
		com->text_insert("イバラカワ", "うん。", H_NORMAL, 8);
		com->text_insert("イバラカワ", "じゃあ、また。", H_NORMAL, 8);
		com->text_insert("オワダ", "…………", H_NORMAL, 5);
		com->text_insert("ウチモト", "あれ、もしかして天子ちゃん元気ない？気のせいかな？", H_NORMAL, 7);
		com->text_insert("オワダ", "元気がないのは気のせいじゃないと思うけど……いつもあんな感じね。", H_NORMAL, 5);
		com->text_insert("ウチモト", "そうかも……確かにそうかもしれない。普段から元気…………", H_NORMAL, 7);
		com->text_insert("ウチモト", "あれ？元気がないというよりクールなだけ？", H_NORMAL, 7);
		com->text_insert("オワダ", "わかんない……そうかもしれないけど……", H_NORMAL, 5);
		com->text_insert("ウチモト", "けど？", H_NORMAL, 7);
		com->text_insert("オワダ", "なんだか……天子ちゃんとは距離を感じるわ。", H_NORMAL, 5);
		com->text_insert("イバラカワ", "――はあ…………", H_NORMAL, 8);
		com->text_insert("イバラカワ", "……ダメだって、わかってるのに…………", H_NORMAL, 8);
		com->text_insert("イバラカワ", "――言わなきゃいけないよね……本当は。", H_NORMAL, 8);
		com->text_insert("イバラカワ", "みんなが優しくなければいいのに――", H_NORMAL, 8);
	}
	play_bgm(bgm[0], 1, 13);
	move(5, 8, -200, 6);//移動
	move(13, 5, -300, 300);//移動
	move(13, 7, -300, 300);//移動
	move(14, 8, 400, 6);//移動
	finish(17);
	text_play();
}

//MM6
//先生の種類とカード
MainMission6::MainMission6(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[2];//bgm確保
	bgm[0] = LoadSoundMem("bgm/piano36.mp3");
	bgm[1] = LoadSoundMem("bgm/piano41.mp3");
	volume_bgm(2);
	set_rare_rate(80, 20, 0, 0);
	size_set(9, 9, 9);
	teacher[0].get_cards()->set_card(Card(13, 0, 600, 200, 80, 60), 0);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 45, 110, 100), 1);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 45, 110, 100), 2);
	teacher[0].get_cards()->set_card(Card(13, 0, 0, 200, 80, 60), 3);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 45, 110, 100), 4);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 45, 110, 100), 5);

	teacher[1].get_cards()->set_card(Card(14, 0, 400, 13, 100, 120), 0);
	teacher[1].get_cards()->set_card(Card(10, 0, 0, 40, 100, 110), 1);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 40, 120, 80), 2);
	teacher[1].get_cards()->set_card(Card(10, 0, 0, 40, 90, 60), 3);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 50, 120, 80), 4);
}
//会話とバトルをする
int MainMission6::play() {
	return normal_text();
}
void MainMission6::text1() {
	if (com == NULL) {
		int x[10] = { -100,-100,-300,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("オオタ", "おいっ！――おい待てって！！", H_NORMAL, 2);
		com->text_insert("イバラカワ", "――――――――", H_NORMAL, 8);
		com->text_insert("オオタ", "先生倒しに行くんだろ！？俺も行くぜ！！", H_NORMAL, 2);
		com->text_insert("イバラカワ", "はあ――もう、いいって言ったでしょ。", H_NORMAL, 8);
		com->text_insert("オオタ", "オレを呼べって言っただろ？", H_NORMAL, 2);
		com->text_insert("オオタ", "なんだよー、なんでそんな遠慮するんだよ！", H_NORMAL, 2);
		com->text_insert("イバラカワ", "遠慮してるわけじゃない！！", H_NORMAL, 8);
		com->text_insert("イバラカワ", "あんたほんとバカでしょ！！", H_NORMAL, 8);
		com->text_insert("オオタ", "そうだな……オレは言ってもらわないと分からねえ。", H_NORMAL, 2);
		com->text_insert("イバラカワ", "――――――――", H_NORMAL, 8);
		com->text_insert("オオタ", "教えてくれよ、なんでみんなを頼ろうとしないんだ？", H_NORMAL, 2);
		com->text_insert("イバラカワ", "――――――――", H_NORMAL, 8);
		com->text_insert("オオタ", "オレは――茨川のこと…………大事に想ってるんだよ。", H_NORMAL, 2);
		com->text_insert("イバラカワ", "私は――――――――", H_NORMAL, 8);
		com->text_insert("イバラカワ", "――私はあんたのこと、大事に想ってない。", H_NORMAL, 8);
		com->text_insert("オオタ", "…………そう、かよ……", H_NORMAL, 2);
		com->text_insert("イバラカワ", "勘違い、しないで。嫌いなわけじゃない。", H_NORMAL, 8);
		com->text_insert("イバラカワ", "仲間だとは思ってる。でも――", H_NORMAL, 8);
		com->text_insert("イバラカワ", "これ以上仲良くは、なれない。", H_NORMAL, 8);
		com->text_insert("オオタ", "…………じゃあ――", H_NORMAL, 2);
		com->text_insert("オオタ", "認めてもらえるよう、頑張らねえとな！", H_NORMAL, 2);
		com->text_insert("イバラカワ", "――――――――", H_NORMAL, 8);
		com->text_insert("オオタ", "よし、じゃあ行こうぜ！！", H_NORMAL, 2);
	}
	play_bgm(bgm[0], 1, 23);
	move(1, 8, 400, 7);//移動
	move(1, 2, 200, 7);//移動
	move(2, 2, 300, 6);//移動
	finish(23);
	text_play();
}
void MainMission6::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,250,-100,-100,-100,-100,-100,500,-100 };
		com = new Text(x, -100);
		com->text_insert("オオタ", "ふう…………茨川、おつかれ！！", H_NORMAL, 2);
		com->text_insert("イバラカワ", "…………おつかれ。", H_NORMAL, 8);
		com->text_insert("オオタ", "…………じゃあ、また明日な。", H_NORMAL, 2);
		com->text_insert("イバラカワ", "――どうして", H_NORMAL, 8);
		com->text_insert("オオタ", "え？", H_NORMAL, 2);
		com->text_insert("イバラカワ", "私は、あなたを拒絶したも同然よ？", H_NORMAL, 8);
		com->text_insert("イバラカワ", "それでも、どうして私に構うの？", H_NORMAL, 8);
		com->text_insert("オオタ", "――それは――", H_NORMAL, 2);
		com->text_insert("オオタ", "お前のことが――", H_NORMAL, 2);
		com->text_insert("オオタ", "――いや、オレはみんなのことが、一番大事だからだ。", H_NORMAL, 2);
		com->text_insert("イバラカワ", "――はん、よくそんなクサイこと言えるね。", H_NORMAL, 8);
		com->text_insert("オオタ", "へへ……その……", H_NORMAL, 2);
		com->text_insert("イバラカワ", "…………ほんと、バカみたい", H_NORMAL, 8);
		com->text_insert("オオタ", "――――――――", H_NORMAL, 2);
		com->text_insert("イバラカワ", "――私が転校生だってのは知ってる？", H_NORMAL, 8);
		com->text_insert("オオタ", "え？――そういえば、一年の時はいなかったような…………", H_NORMAL, 2);
		com->text_insert("イバラカワ", "私は去年ここの学校に転校してきた。", H_NORMAL, 8);
		com->text_insert("イバラカワ", "それが、あんたたちと距離を置く理由。", H_NORMAL, 8);
		com->text_insert("オオタ", "えっと、その……", H_NORMAL, 2);
		com->text_insert("イバラカワ", "――ちゃんと詳しく言わないと、わからないよね。", H_NORMAL, 8);
		com->text_insert("オオタ", "…………。", H_NORMAL, 2);
		com->text_insert("イバラカワ", "前の学校に、親友と呼べる人がいたの。", H_NORMAL, 8);
		com->text_insert("イバラカワ", "私は……その友達のことがずっと忘れられない。", H_NORMAL, 8);
		com->text_insert("イバラカワ", "ずっとずっと……忘れられない……", H_NORMAL, 8);
		com->text_insert("イバラカワ", "この学校で友達ができても――１番の友達にはならない。", H_NORMAL, 8);
		com->text_insert("イバラカワ", "その親友と比べてしまう。", H_NORMAL, 8);
		com->text_insert("イバラカワ", "あの人に比べたら仲良くない、気が合わないって…………。", H_NORMAL, 8);
		com->text_insert("オオタ", "茨川…………。", H_NORMAL, 2);
		com->text_insert("イバラカワ", "最低だってわかってる。でも――――――", H_NORMAL, 8);
		com->text_insert("イバラカワ", "私は忘れない。――忘れたく、ないよ…………。", H_NORMAL, 8);
		com->text_insert("オオタ", "だったら！！", H_NORMAL, 2);
		com->text_insert("イバラカワ", "――――――――", H_NORMAL, 8);
		com->text_insert("オオタ", "オレが、変えてやる！！", H_NORMAL, 2);
		com->text_insert("イバラカワ", "――――――――", H_NORMAL, 8);
		com->text_insert("オオタ", "オレは知らねえけど、その友達は大事だし、忘れちゃいけねえ！", H_NORMAL, 2);
		com->text_insert("オオタ", "――でも！！オレもおんなじくらい、仲良くなってみせる！！", H_NORMAL, 2);
		com->text_insert("オオタ", "転校してよかった、なんて思うことは絶対にない！！", H_NORMAL, 2);
		com->text_insert("オオタ", "けど、ここでの生活もよかったって、思ってもらえるくらいの存在になる！！", H_NORMAL, 2);
		com->text_insert("イバラカワ", "――――――――", H_NORMAL, 8);
		com->text_insert("オオタ", "過去は変えることができねえけど……せめて今くらいは変えようぜ。", H_NORMAL, 2);
		com->text_insert("イバラカワ", "――――ぐ――", H_NORMAL, 8);
		com->text_insert("オオタ", "…………", H_NORMAL, 2);
		com->text_insert("イバラカワ", "――――――やって、みろよ――", H_NORMAL, 8);
		com->text_insert("オオタ", "ああ。", H_NORMAL, 2);
		com->text_insert("イバラカワ", "やれるもんなら……やってみろよ！！", H_NORMAL, 8);
		com->text_insert("オオタ", "ああ！！", H_NORMAL, 2);
	}
	play_bgm(bgm[1], 31, 46);
	move(3, 2, 200, 6);//移動
	move(30, 2, 220, 6);//移動
	move(32, 2, 240, 6);//移動
	move(37, 2, 260, 6);//移動
	finish(46);
	text_play();
}

//MM7
//先生の種類とカード
MainMission7::MainMission7(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[2];//bgm確保
	bgm[0] = LoadSoundMem("bgm/piano27.mp3");
	bgm[1] = LoadSoundMem("bgm/piano22.mp3");
	volume_bgm(2);
	set_rare_rate(100, 0, 0, 0);
	size_set(9, 9, 9);
	teacher[1].get_cards()->set_card(Card(19, 0, 800, 200, 70, 60), 0);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 110, 100), 1);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 110, 100), 2);
	teacher[1].get_cards()->set_card(Card(4, 0, 0, 180, 80, 80), 3);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 110, 100), 4);
	teacher[1].get_cards()->set_card(Card(19, 0, 0, 200, 70, 60), 5);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 110, 100), 6);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 110, 100), 7);
}
//会話とバトルをする
int MainMission7::play() {
	return normal_text();
}
void MainMission7::text1() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,200,-100,-100,-100,-100,-100,400 };
		com = new Text(x, -100);
		com->text_insert("トモチ", "伊志君、今日はよろしくねー。", H_NORMAL, 9);
		com->text_insert("イシ", "あ、ああ。じゃあ、行こうか。", H_NORMAL, 3);
		com->text_insert("トモチ", "どうしたの？緊張してるの？", H_NORMAL, 9);
		com->text_insert("イシ", "ま、まあ、ね。", H_NORMAL, 3);
		com->text_insert("トモチ", "…………", H_NORMAL, 9);
		com->text_insert("イシ", "…………", H_NORMAL, 3);
		com->text_insert("トモチ", "今日はW先生一人だったよね？", H_NORMAL, 9);
		com->text_insert("イシ", "そ、そう。北館にいるらしい。", H_NORMAL, 3);
		com->text_insert("トモチ", "準備は完璧？自信ある？", H_NORMAL, 9);
		com->text_insert("イシ", "心配では、あるけど。完璧ってのは難しいから。", H_NORMAL, 3);
		com->text_insert("トモチ", "大丈夫よ。完璧人間の私がついてるんだから。", H_NORMAL, 9);
		com->text_insert("イシ", "は、はは…………それはありがとう。", H_NORMAL, 3);
		com->text_insert("トモチ", "あ、もしかして疑ってる？私のことあんまり知らない？", H_NORMAL, 9);
		com->text_insert("イシ", "疑ってはないよ……でも、あんまり知らないかな…………。", H_NORMAL, 3);
		com->text_insert("トモチ", "はあ、そうよねー。クラス違うもんねー。", H_NORMAL, 9);
		com->text_insert("イシ", "住んでる世界も違うもんね。", H_NORMAL, 3);
		com->text_insert("トモチ", "あ、それは卑屈な考えだねー伊志君。", H_NORMAL, 9);
		com->text_insert("トモチ", "まあ、私と同じレベルまでこれる人なんてほとんどいないのも事実だけどね。", H_NORMAL, 9);
	}
	play_bgm(bgm[0], 1, 18);
	move(8, 9, 380, 6);//移動
	finish(18);
	text_play();
}
void MainMission7::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,400,-100,-100,-100,-100,-100, 100 };
		com = new Text(x, -100);
		com->text_insert("イシ", "はあ、はあ…………。", H_NORMAL, 3);
		com->text_insert("トモチ", "伊志君おつかれー。大丈夫ー？", H_NORMAL, 9);
		com->text_insert("イシ", "はあ、はあ。だいじょう、ぶ……。", H_NORMAL, 3);
		com->text_insert("トモチ", "うーん、伊志君トレーニングとかした方がいいんじゃない？", H_NORMAL, 9);
		com->text_insert("イシ", "まあ確かに……このままじゃやばいよな……", H_NORMAL, 3);
		com->text_insert("イシ", "明日はまだしも、明後日戦う先生は今日より強そうだし……", H_NORMAL, 3);
		com->text_insert("トモチ", "――そう、明後日の、ことなんだけどさ。", H_NORMAL, 9);
		com->text_insert("トモチ", "明後日のミッション、やめとかない？", H_NORMAL, 9);
		com->text_insert("イシ", "え？なんで？", H_NORMAL, 3);
		com->text_insert("トモチ", "多分、学校の北館が爆発するから。", H_NORMAL, 9);
		com->text_insert("イシ", "――――――――は？", H_NORMAL, 3);
		com->text_insert("トモチ", "ミッションなんかやってたら巻き込まれて死んじゃうよ。", H_NORMAL, 9);
		com->text_insert("イシ", "嘘にしても下手すぎるよ……なんか用事でもあるの？", H_NORMAL, 3);
		com->text_insert("トモチ", "私がこんなバカな嘘つくと思う？", H_NORMAL, 9);
		com->text_insert("イシ", "……じゃあ仮に本当だとして、なんでそんなこと……と、砥用さんが知ってるんだよ。", H_NORMAL, 3);
		com->text_insert("トモチ", "ちょっと噂で聞いたの。誰の手によるものかはわかんない。", H_NORMAL, 9);
		com->text_insert("イシ", "ますます怪しい…………そもそも爆発なんて大ごと、学校そのものの危機だ。", H_NORMAL, 3);
		com->text_insert("トモチ", "それより、今初めて苗字呼んでくれたね！", H_NORMAL, 9);
		com->text_insert("イシ", "……………………", H_NORMAL, 3);
		com->text_insert("イシ", "やっぱ信じられない。台羅とか半田がいうならまだしも……悪いけど君の言うことは信用できない。", H_NORMAL, 3);
		com->text_insert("トモチ", "ひどーい。", H_NORMAL, 9);
		com->text_insert("トモチ", "いろいろと、ひどーい。", H_NORMAL, 9);
		com->text_insert("イシ", "明後日のミッション、そんなに嫌なら参加しなくていいよ。", H_NORMAL, 3);
		com->text_insert("イシ", "別の人誘うから。半田にも僕から伝えとく。", H_NORMAL, 3);
		com->text_insert("トモチ", "だからー、爆発するんだって。死んじゃうよ？", H_NORMAL, 9);
		com->text_insert("イシ", "う……信じてないけど、絶対に嘘とは言い切れないのがつらい……", H_NORMAL, 3);
		com->text_insert("イシ", "これが嘘だとしたら君、相当酷いと思うんだけど……", H_NORMAL, 3);
		com->text_insert("トモチ", "嘘じゃないって。信じてよー。", H_NORMAL, 9);
		com->text_insert("トモチ", "明後日は学校から離れたとこでデートしようよ！", H_NORMAL, 9);
		com->text_insert("イシ", "――あのさあ……", H_NORMAL, 3);
		com->text_insert("トモチ", "私からの誘いよ。贅沢だと思わない？", H_NORMAL, 9);
		com->text_insert("イシ", "――やっぱり", H_NORMAL, 3);
		com->text_insert("トモチ", "？", H_NORMAL, 9);
		com->text_insert("イシ", "君は、おかしい。狂ってる。サイコパスだ。", H_NORMAL, 3);
		com->text_insert("トモチ", "――――――――", H_NORMAL, 9);
		com->text_insert("イシ", "じゃあ、今日はありがとう。明後日は別の人とミッション行くから。", H_NORMAL, 3);
		com->text_insert("トモチ", "――――――――", H_NORMAL, 9);
	}
	play_bgm(bgm[1], 11, 31);
	move(17, 9, 150, 6);//移動
	move(22, 3, 450, 6);//移動
	move(28, 9, 200, 6);//移動
	move(33, 3, 500, 6);//移動
	move(36, 3, 800, 6);//移動
	finish(37);
	text_play();
}

//MM8
//先生の種類とカード
MainMission8::MainMission8(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[2];//bgm確保
	bgm[0] = LoadSoundMem("bgm/piano21.mp3");
	bgm[1] = LoadSoundMem("bgm/piano33.mp3");
	volume_bgm(2);
	set_rare_rate(90, 10, 0, 0);
	size_set(9, 9, 9);
	teacher[1].get_cards()->set_card(Card(22, 0, 800, 60, 90, 110), 0);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 90, 110), 1);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 90, 110), 2);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 90, 110), 3);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 90, 110), 4);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 90, 110), 5);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 90, 110), 6);
}
//会話とバトルをする
int MainMission8::play() {
	return normal_text();
}
void MainMission8::text1() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,200,-100,-100,-100,400,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("イシ", "内本さん、今日はよろしく。", H_NORMAL, 3);
		com->text_insert("ウチモト", "うん、よろしく！！", H_NORMAL, 7);
		com->text_insert("イシ", "急でごめん。砥用さんが用事で数日参加できないみたいで。", H_NORMAL, 3);
		com->text_insert("ウチモト", "そうなんだ？私は全然大丈夫だよ、気にしないで！", H_NORMAL, 7);
		com->text_insert("ウチモト", "明日もだっけ？今日と明日、よろしくね！！", H_NORMAL, 7);
		com->text_insert("イシ", "うん。ありがとう。", H_NORMAL, 3);
	}
	move(6, 3, -500, 6);//移動
	move(6, 7, -500, 6);//移動
	finish(6);
	text_play();
}
void MainMission8::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,100,-100,-100,-100,400,-100, -100 };
		com = new Text(x, -100);
		com->text_insert("イシ", "はあ、はあ、はあ…………", H_NORMAL, 3);
		com->text_insert("ウチモト", "おつかれー！援護助かったよ！", H_NORMAL, 7);
		com->text_insert("イシ", "はあ、はあ、ありが、とう……。", H_NORMAL, 3);
		com->text_insert("ウチモト", "大丈夫？なにか飲み物買ってこようか？", H_NORMAL, 7);
		com->text_insert("イシ", "大丈夫……ありがとう。", H_NORMAL, 3);
		com->text_insert("ウチモト", "そう？無理せず今日は早めに寝た方がいいね！", H_NORMAL, 7);
		com->text_insert("イシ", "うん。今日はありがとう。また、明日……", H_NORMAL, 3);
		com->text_insert("ウチモト", "うん、じゃあね！", H_NORMAL, 7);
		com->text_insert("ウチモト", "……あ！そういえばさ！", H_NORMAL, 7);
		com->text_insert("イシ", "ん、どうか、したの？", H_NORMAL, 3);
		com->text_insert("ウチモト", "砥用ちゃんって数日用事があるの？", H_NORMAL, 7);
		com->text_insert("イシ", "う、うん……まあ……", H_NORMAL, 3);
		com->text_insert("ウチモト", "んー、それは困ったなー。", H_NORMAL, 7);
		com->text_insert("ウチモト", "私、砥用ちゃんに頼み事してるんだけどなー。", H_NORMAL, 7);
		com->text_insert("イシ", "え、そ、そうなの？", H_NORMAL, 3);
		com->text_insert("ウチモト", "そーう。結構重要な頼み事なんだー。", H_NORMAL, 7);
		com->text_insert("イシ", "…………", H_NORMAL, 3);
		com->text_insert("ウチモト", "仕方ないよねー。残念だけど延期しよっか。", H_NORMAL, 7);
		com->text_insert("イシ", "――ご、ごめん！", H_NORMAL, 3);
		com->text_insert("ウチモト", "え？", H_NORMAL, 7);
		com->text_insert("イシ", "用事っていうのは、嘘なんだ……", H_NORMAL, 3);
		com->text_insert("ウチモト", "あ、そうなんだ！よかったー。", H_NORMAL, 7);
		com->text_insert("イシ", "…………", H_NORMAL, 3);
		com->text_insert("ウチモト", "……あれ？じゃあ、なんで今日明日、私にミッション頼んだの？", H_NORMAL, 7);
		com->text_insert("イシ", "それは…………", H_NORMAL, 3);
		com->text_insert("ウチモト", "――ま、全然いいけどね！明日も私が行くよ！", H_NORMAL, 7);
		com->text_insert("イシ", "…………", H_NORMAL, 3);
		com->text_insert("ウチモト", "…………伊志くん？", H_NORMAL, 7);
		com->text_insert("イシ", "…………ごめん、実は…………", H_NORMAL, 3);
		com->text_insert("ウチモト", "どうしたの？相談なら何でも聞くよ！", H_NORMAL, 7);
		com->text_insert("イシ", "実はっ……！砥用さんが！", H_NORMAL, 3);
		com->text_insert("イシ", "明日、学校の北館が爆発するって言いだしたんだ！！", H_NORMAL, 3);
		com->text_insert("ウチモト", "え！？", H_NORMAL, 7);
		com->text_insert("イシ", "ほんと滅茶苦茶だよ！！あいつ、狂ってるんだ！！", H_NORMAL, 3);
		com->text_insert("イシ", "正直やばい奴だよ！！一緒にミッション行くのが嫌だったんだ！！", H_NORMAL, 3);
		com->text_insert("ウチモト", "伊志くんも知ってたんだー！！", H_NORMAL, 7);
		com->text_insert("イシ", "――――――――", H_NORMAL, 3);
		com->text_insert("ウチモト", "いいと思わない？学校爆破！！", H_NORMAL, 7);
		com->text_insert("ウチモト", "戦わずに先生を一気にやれちゃうんだよ！", H_NORMAL, 7);
		com->text_insert("ウチモト", "砥用ちゃんには当日、協力を頼んでたんだー！", H_NORMAL, 7);
		com->text_insert("ウチモト", "でも伊志くんも知ったなら、せっかくだし協力してくれない？", H_NORMAL, 7);
		com->text_insert("ウチモト", "当日、北館で先生を引き付けてほしいの！", H_NORMAL, 7);
		com->text_insert("ウチモト", "それで、爆破ギリギリで北館から逃げて、なるべくたくさんの先生を倒すの！！", H_NORMAL, 7);
		com->text_insert("ウチモト", "砥用ちゃんと伊志くんは二手に分かれたとして…………", H_NORMAL, 7);
		com->text_insert("ウチモト", "先生を３人ずつひきつけたらそれだけで６人やれちゃうね！！", H_NORMAL, 7);
		com->text_insert("ウチモト", "あー楽しみだねー！みんな驚いて喜ぶよ！！", H_NORMAL, 7);
	}
	play_bgm(bgm[0], 1, 18);
	play_bgm(bgm[1], 37, 46);
	move(8, 7, 450, 6);//移動
	move(18, 3, 150, 6);//移動
	move(33, 3, 200, 6);//移動
	move(35, 7, 400, 6);//移動
	move(37, 7, 350, 6);//移動
	move(38, 7, 300, 6);//移動
	event(42, "event/ウチモト本性.png");//挿絵
	finish(46);
	text_play();
}

//MM9
//先生の種類とカード
MainMission9::MainMission9(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[2];//bgm確保
	bgm[0] = LoadSoundMem("bgm/piano36.mp3");
	bgm[1] = LoadSoundMem("bgm/piano26.mp3");
	volume_bgm(2);
	set_rare_rate(80, 20, 0, 0);
	size_set(9, 9, 9);
	teacher[0].get_cards()->set_card(Card(22, 0, 250, 60, 100, 120), 0);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 60, 100, 120), 1);
	teacher[0].get_cards()->set_card(Card(7, 0, 0, 35, 80, 90), 2);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 60, 100, 120), 3);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 60, 100, 120), 4);

	teacher[1].get_cards()->set_card(Card(22, 0, 250, 65, 110, 110), 0);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 65, 110, 110), 1);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 65, 110, 110), 2);
	teacher[1].get_cards()->set_card(Card(6, 0, 0, 25, 140, 110), 3);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 65, 110, 110), 4);

	teacher[2].get_cards()->set_card(Card(22, 0, 250, 50, 100, 110), 0);
	teacher[2].get_cards()->set_card(Card(8, 0, 0, 20, 90, 80), 1);
	teacher[2].get_cards()->set_card(Card(22, 0, 0, 50, 100, 110), 2);
	teacher[2].get_cards()->set_card(Card(8, 0, 0, 20, 90, 80), 3);
	teacher[2].get_cards()->set_card(Card(22, 0, 0, 50, 100, 110), 4);
}
//会話とバトルをする
int MainMission9::play() {
	return normal_text();
}
void MainMission9::text1() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,200,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, 500);
		com->text_insert("イシ", "はあ、はあ、はあ…………", H_NORMAL, 3);
		com->text_insert("イシ", "（どうしよう、どうしよう…………！！）", H_MOUSOU, 3);
		com->text_insert("イシ", "（結局誰にも……台羅にすら打ち明けることができなかった……）", H_MOUSOU, 3);
		com->text_insert("イシ", "どうすれば……どうすれば…………", H_NORMAL, 3);
		com->text_insert("イシ", "（そもそも僕一人で先生三人を引き付けることすら無理だ……！）", H_MOUSOU, 3);
		com->text_insert("イシ", "（そのうえ校舎が爆破……だめだ、僕…………！！）", H_MOUSOU, 3);
		com->text_insert("イシ", "結局…………", H_NORMAL, 3);
		com->text_insert("イシ", "（砥用さんの言ったことは嘘じゃなかった……彼女は狂ってるから疑ったのは仕方ないけど……）", H_MOUSOU, 3);
		com->text_insert("イシ", "まさか、内本さんまで狂ってたなんて…………！！", H_NORMAL, 3);
		com->text_insert("イシ", "（やっぱり無理だ……！！今からでも遅くない、逃げ……）", H_MOUSOU, 3);

		com->text_insert("トモチ", "――あぁ……やっぱり、来てたんだ。", H_NORMAL, 9);
		com->text_insert("イシ", "――――――――え", H_NORMAL, 3);
		com->text_insert("トモチ", "やっほぉ……伊志君……", H_NORMAL, 9);
		com->text_insert("イシ", "どう、して…………", H_NORMAL, 3);
		com->text_insert("イシ", "（ボロボロの姿、息切れ、あの砥用さんが……）", H_MOUSOU, 3);
		com->text_insert("イシ", "――僕は…………", H_NORMAL, 3);
		com->text_insert("トモチ", "さ、ぼさっとしない。ピンチなんでしょ？", H_NORMAL, 9);
		com->text_insert("トモチ", "私が来たから大丈夫。さっさと倒そ？", H_NORMAL, 9);
	}
	play_bgm(bgm[0], 1, 18);
	move(10, 9, 100, 3);//移動
	move(17, 9, 300, 6);//移動
	finish(18);
	text_play();
}
void MainMission9::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,100,-100,-100,-100,-100,-100, 200 };
		com = new Text(x, -100);
		com->text_insert("イシ", "はあっ、はあ…………、やった…………", H_NORMAL, 3);
		com->text_insert("トモチ", "ねえ、ちょっと外でない？疲れちゃった。早く！", H_NORMAL, 9);
		com->text_insert("イシ", "もう、そうやって隠さなくていいよ……。", H_NORMAL, 3);
		com->text_insert("トモチ", "――――――――", H_NORMAL, 9);
		com->text_insert("イシ", "内本さんに協力を頼まれてたんだよね…………", H_NORMAL, 3);
		com->text_insert("トモチ", "――そう、やっぱもう知ってたの。", H_NORMAL, 9);
		com->text_insert("イシ", "そろそろ爆発するんだよね？急ごう。", H_NORMAL, 3);
		com->text_insert("トモチ", "そうねー…………", H_NORMAL, 9);
		com->text_insert("イシ", "…………", H_NORMAL, 3);
		com->text_insert("トモチ", "…………", H_NORMAL, 9);
		com->text_insert("イシ", "……………………", H_NORMAL, 3);
		com->text_insert("トモチ", "……………………", H_NORMAL, 9);
		com->text_insert("イシ", "…………爆発、しない…………", H_NORMAL, 3);
		com->text_insert("トモチ", "内本の口ぶりからして、そんなにたくさん爆弾用意してるわけじゃなさそうだったわねー…………", H_NORMAL, 9);
		com->text_insert("イシ", "まさか…………砥用、さん……", H_NORMAL, 3);
		com->text_insert("トモチ", "やっぱり内本にはびっくりしたー？", H_NORMAL, 9);
		com->text_insert("トモチ", "校舎爆発なんてねえ。さすがの私もびっくりしたわ。", H_NORMAL, 9);
		com->text_insert("イシ", "どうして…………", H_NORMAL, 3);
		com->text_insert("イシ", "内本さんを批判しないの…………？", H_NORMAL, 3);
		com->text_insert("イシ", "それに…………どうして半田とかに言わなかったんだよ。", H_NORMAL, 3);
		com->text_insert("トモチ", "伊志君だって誰にも言わなかったでしょ？", H_NORMAL, 9);
		com->text_insert("トモチ", "きっとみんな混乱するし、内本がみんなに嫌われるかもしれないでしょー？", H_NORMAL, 9);
		com->text_insert("イシ", "でも、彼女はそれだけのことをやったわけだし…………", H_NORMAL, 3);
		com->text_insert("トモチ", "伊志君だってわかってるくせにー。", H_NORMAL, 9);
		com->text_insert("トモチ", "多分内本に悪気はないよね。", H_NORMAL, 9);
		com->text_insert("イシ", "…………", H_NORMAL, 3);
		com->text_insert("トモチ", "今回の件は、これがきっと誰も不幸にならない選択だったのよ。", H_NORMAL, 9);
		com->text_insert("イシ", "…………", H_NORMAL, 3);
		com->text_insert("イシ", "……ねえ、砥用さん……", H_NORMAL, 3);
		com->text_insert("トモチ", "ん？", H_NORMAL, 9);
		com->text_insert("イシ", "ごめん……僕勘違いしてたかも……してた。", H_NORMAL, 3);
		com->text_insert("トモチ", "伊志くーん", H_NORMAL, 9);
		com->text_insert("イシ", "砥用さんって、やっぱり…………", H_NORMAL, 3);
		com->text_insert("トモチ", "伊志くーーーーーん！！！", H_NORMAL, 9);
		com->text_insert("イシ", "え、な、なに！？", H_NORMAL, 3);
		com->text_insert("トモチ", "デート、しようよ！", H_NORMAL, 9);
	}
	play_bgm(bgm[1], 16, 36);
	move(8, 3, -1400, 6);//移動
	move(8, 9, -1300, 6);//移動
	move(9, 3, 400, 3000);//移動
	move(9, 9, 500, 3000);//移動
	finish(8);
	finish(36);
	event(35, "event/トモチデート勧誘.png");//挿絵
	text_play();
}

//MM10
//先生の種類とカード
MainMission10::MainMission10(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/piano21.mp3");
	//bgm[1] = LoadSoundMem("bgm/piano26.mp3");
	volume_bgm(1);
	set_rare_rate(80, 20, 0, 0);
	size_set(9, 9, 9);
	teacher[0].get_cards()->set_card(Card(3, 0, 400, 60, 100, 120), 0);
	teacher[0].get_cards()->set_card(Card(3, 0, 0, 60, 100, 120), 1);
	teacher[0].get_cards()->set_card(Card(2, 0, 0, 30, 80, 90), 2);
	teacher[0].get_cards()->set_card(Card(3, 0, 0, 60, 100, 120), 3);
	teacher[0].get_cards()->set_card(Card(3, 0, 0, 60, 100, 120), 4);

	teacher[1].get_cards()->set_card(Card(22, 0, 400, 45, 110, 110), 0);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 45, 110, 110), 1);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 45, 110, 110), 2);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 45, 110, 110), 3);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 45, 110, 110), 4);

	teacher[2].get_cards()->set_card(Card(22, 0, 400, 50, 120, 110), 0);
	teacher[2].get_cards()->set_card(Card(24, 0, 0, 30, 80, 90), 1);
	teacher[2].get_cards()->set_card(Card(22, 0, 0, 50, 120, 110), 2);
	teacher[2].get_cards()->set_card(Card(22, 0, 0, 50, 120, 110), 3);
	teacher[2].get_cards()->set_card(Card(22, 0, 0, 50, 120, 110), 4);
}
//会話とバトルをする
int MainMission10::play() {
	return normal_text();
}
void MainMission10::text1() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,200,300,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("イシ", "ごめん台羅。協力してもらって。", H_NORMAL, 3);
		com->text_insert("タイラ", "全然いいさ。いつでも頼ってくれ。", H_NORMAL, 4);
		com->text_insert("イシ", "でも…………今日塾は？", H_NORMAL, 3);
		com->text_insert("タイラ", "今日は塾じゃない。それに……", H_NORMAL, 4);
		com->text_insert("タイラ", "塾ぐらい休むさ。自分で勉強すればいい。", H_NORMAL, 4);
		com->text_insert("イシ", "けど授業料がもったいないだろ？", H_NORMAL, 3);
		com->text_insert("タイラ", "ふ、そんなの賢介が気にすることじゃない。", H_NORMAL, 4);
		com->text_insert("イシ", "そうは言ってもさ…………", H_NORMAL, 3);
		com->text_insert("タイラ", "親が勝手に塾を強制してるだけさ。", H_NORMAL, 4);
		com->text_insert("タイラ", "遊びや友達の方が勉強よりも大事だ。", H_NORMAL, 4);
		com->text_insert("イシ", "はは……流石、天才は言うことが違うな。", H_NORMAL, 3);
		
	}
	finish(11);
	text_play();
}
void MainMission10::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,400,500,-100,-100,-100,-100, -100 };
		com = new Text(x, -100);
		com->text_insert("イシ", "はあ、はあ……今日はありがとう。", H_NORMAL, 3);
		com->text_insert("タイラ", "予定より早く終わった。まだ門限まで時間あるな。", H_NORMAL, 4);
		com->text_insert("タイラ", "どっか寄って帰らないか？", H_NORMAL, 4);
		com->text_insert("イシ", "いいね。……でも大丈夫？", H_NORMAL, 3);
		com->text_insert("タイラ", "何が？――親に怒られないかってことか？", H_NORMAL, 4);
		com->text_insert("イシ", "ああ。門限前でも遅かったら遊んでるって疑われるんじゃないか？", H_NORMAL, 3);
		com->text_insert("タイラ", "賢介は心配しすぎだ。俺の親はそこまで厳しくない。", H_NORMAL, 4);
		com->text_insert("タイラ", "もし何か言われても学校に残って勉強してたって言えばいい。", H_NORMAL, 4);
		com->text_insert("イシ", "…………", H_NORMAL, 3);
		com->text_insert("タイラ", "――まだあの日のこと気にしてるのか？", H_NORMAL, 4);
		com->text_insert("イシ", "いや――大丈夫。行こうか。", H_NORMAL, 3);
		com->text_insert("タイラ", "ああ。", H_NORMAL, 4);
		com->text_insert("タイラ", "（大丈夫…………か。）", H_MOUSOU, 4);
	
	}
	play_bgm(bgm[0], 1, 13);
	move(13, 3, -1400, 6);//移動
	move(13, 4, -1300, 6);//移動
	finish(13);
	text_play();
}

//MM11
//先生の種類とカード
MainMission11::MainMission11(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/piano37.mp3");
	//bgm[1] = LoadSoundMem("bgm/piano26.mp3");
	volume_bgm(1);
	set_rare_rate(75, 25, 0, 0);
	size_set(9, 9, 9);
	teacher[0].get_cards()->set_card(Card(8, 0, 700, 40, 80, 100), 0);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 60, 120, 120), 1);
	teacher[0].get_cards()->set_card(Card(8, 0, 0, 40, 80, 100), 2);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 60, 120, 120), 3);
	teacher[0].get_cards()->set_card(Card(8, 0, 0, 40, 80, 100), 4);

	teacher[1].get_cards()->set_card(Card(14, 0, 700, 13, 90, 110), 0);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 65, 110, 110), 1);
	teacher[1].get_cards()->set_card(Card(14, 0, 0, 13, 90, 110), 2);
	teacher[1].get_cards()->set_card(Card(10, 0, 0, 20, 80, 100), 3);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 65, 110, 110), 4);
}
//会話とバトルをする
int MainMission11::play() {
	return normal_text();
}
void MainMission11::text1() {
	if (com == NULL) {
		int x[10] = { -100,700,-100,-100,-100,-100,-100,-100,-100,200 };
		com = new Text(x, -100);
		com->text_insert("トモチ", "今日は学校終わったら伊志君からかいに行こうと思ったのに……", H_NORMAL, 9);
		com->text_insert("トモチ", "（半田はあと一人に声かけるって言ってたけど、どうだか……）", H_MOUSOU, 9);
		com->text_insert("トモチ", "早くしてほしいな…………", H_NORMAL, 9);
		com->text_insert("トモチ", "（まあ半田は少なくとも遅刻する人ではないけど、さっさと終わらせて帰りたいなあ。）", H_MOUSOU, 9);
		com->text_insert("トモチ", "なんか今日は…………やる気起きないなあ。", H_NORMAL, 9);
	}
	move(5, 1, 400, 3);
	finish(5);
	text_play();
}
void MainMission11::text2() {
	if (com == NULL) {
		int x[10] = { -100,200,-100,-100,-100,-100,-100,-100,-100, 400 };
		com = new Text(x, -100);
		com->text_insert("ハンダ", "おつかれ。また明日。", H_NORMAL, 1);
		com->text_insert("トモチ", "おつかれー。", H_NORMAL, 9);

		com->text_insert("トモチ", "――あ、伊志君じゃーん！", H_NORMAL, 9);
		com->text_insert("イシ", "え、あ、砥用さん。", H_NORMAL, 3);
		com->text_insert("トモチ", "どうしたの？こんな時間まで残って。", H_NORMAL, 9);
		com->text_insert("イシ", "ああ、委員会の仕事があって。", H_NORMAL, 3);
		com->text_insert("トモチ", "そうなんだー。じゃあ今から帰るの？", H_NORMAL, 9);
		com->text_insert("イシ", "うん。まあ…………", H_NORMAL, 3);
		com->text_insert("トモチ", "じゃあ一緒に――いや、台羅君と帰るのかな？", H_NORMAL, 9);
		com->text_insert("イシ", "うん、そう……ごめん。", H_NORMAL, 3);
		com->text_insert("トモチ", "いやいや、気にしないで。台羅君は？どこにいるの？", H_NORMAL, 9);
		com->text_insert("イシ", "僕が委員会の仕事終わるまで図書室で勉強するって。", H_NORMAL, 3);
		com->text_insert("イシ", "今図書室へ向かうとこなんだ。", H_NORMAL, 3);
		com->text_insert("トモチ", "なるほどね。", H_NORMAL, 9);
		com->text_insert("イシ", "うん、じゃあ、ね。また明日……", H_NORMAL, 3);
		com->text_insert("トモチ", "またねー……", H_NORMAL, 9);
		com->text_insert("トモチ", "……ねえ伊志君、どうかしたの？", H_NORMAL, 9);
		com->text_insert("イシ", "え、何が……？", H_NORMAL, 3);
		com->text_insert("トモチ", "なんか元気なさそうに見えたからさ。相談なら聞いてあげるよ？", H_NORMAL, 9);
		com->text_insert("イシ", "いや、大丈夫。ありがとう。", H_NORMAL, 3);
		com->text_insert("トモチ", "…………", H_NORMAL, 9);
		com->text_insert("イシ", "…………", H_NORMAL, 3);
		com->text_insert("イシ", "……あの、さ。じゃあ一つだけ訊いていい？", H_NORMAL, 3);
		com->text_insert("トモチ", "もちろん。なんでも！", H_NORMAL, 9);
		com->text_insert("イシ", "大人になるってどういうことだと思う？", H_NORMAL, 3);
		com->text_insert("トモチ", "大人になるってどういうことか…………？", H_NORMAL, 9);
		com->text_insert("トモチ", "なに伊志君、大人になるの？いい心がけだね。", H_NORMAL, 9);
		com->text_insert("イシ", "大人になるのっていいことかな？", H_NORMAL, 3);
		com->text_insert("トモチ", "…………", H_NORMAL, 9);
		com->text_insert("トモチ", "……なるほど、ね。", H_NORMAL, 9);
		com->text_insert("トモチ", "私の見解を言わせてもらうと…………当然いいことだと思う。", H_NORMAL, 9);
		com->text_insert("トモチ", "それは誰もが通るべき道であるし、大人になれなかったら社会に出たときに困るから。", H_NORMAL, 9);
		com->text_insert("トモチ", "でも……大人になることが――幸せとは限らない。", H_NORMAL, 9);
		com->text_insert("イシ", "…………", H_NORMAL, 3);
		com->text_insert("トモチ", "私の予想としては……そんなところだよね。伊志君が悩んでるのは。", H_NORMAL, 9);
		com->text_insert("イシ", "…………流石、砥用さんだ。", H_NORMAL, 3);
		com->text_insert("トモチ", "ま、私は完璧人間だからね。――で、元の質問の答えだけど", H_NORMAL, 9);
		com->text_insert("トモチ", "大人になるっていうのは……我慢することだと思うね、私は。", H_NORMAL, 9);
		com->text_insert("イシ", "――我慢、か…………。", H_NORMAL, 3);
		com->text_insert("トモチ", "まあ？私みたいな完璧人間ならまだしも伊志君みたいなダメダメ人間じゃあ悩んで当然ね。", H_NORMAL, 9);
		com->text_insert("イシ", "はは…………", H_NORMAL, 3);
		com->text_insert("トモチ", "……ま、あくまで私の考えだけどね。", H_NORMAL, 9);
		com->text_insert("トモチ", "これは誰しも通る道だと思うよ。存分に悩み苦しみたまえ、伊志賢介君。", H_NORMAL, 9);
		com->text_insert("イシ", "…………我慢することが大人になることだっていうなら", H_NORMAL, 3);
		com->text_insert("イシ", "友達と今まで通りに付き合うことも我慢しないといけないよね？", H_NORMAL, 3);
		com->text_insert("トモチ", "…………", H_NORMAL, 9);
		com->text_insert("トモチ", "……伊志君、それは", H_NORMAL, 9);
		com->text_insert("トモチ", "その「友達」と直接話すべきことよ", H_NORMAL, 9);
	}
	play_bgm(bgm[0], 3, 48);
	move(2, 1, -1400, 4);//移動
	move(2, 9, 1200, 6);//移動
	move(3, 3, 200, 1000);//移動
	move(3, 9, 800, 1000);//移動
	move(4, 9, 350, 6);//移動
	move(16, 3, 100, 3);//移動
	move(26, 9, 300, 6);//移動
	finish(2);
	finish(48);
	text_play();
}

//MM12
//先生の種類とカード
MainMission12::MainMission12(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[2];//bgm確保
	bgm[0] = LoadSoundMem("bgm/piano24.mp3");
	bgm[1] = LoadSoundMem("bgm/piano31.mp3");
	volume_bgm(2);
	set_rare_rate(70, 25, 5, 0);
	size_set(9, 9, 9);
	teacher[1].get_cards()->set_card(Card(19, 0, 1800, 240, 70, 80), 0);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 75, 110, 110), 1);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 75, 110, 110), 2);
	teacher[1].get_cards()->set_card(Card(19, 0, 0, 240, 70, 80), 3);
	teacher[1].get_cards()->set_card(Card(4, 0, 0, 180, 100, 130), 4);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 75, 110, 110), 5);
	teacher[1].get_cards()->set_card(Card(19, 0, 0, 240, 70, 80), 6);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 75, 110, 110), 7);
}
//会話とバトルをする
int MainMission12::play() {
	return normal_text();
}
void MainMission12::text1() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,-100,300,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("タイラ", "（賢介が変わったのはあの日がきっかけだった。）", H_MOUSOU, 4);
		com->text_insert("タイラ", "（本人が俺に直接相談することはなかったが――）", H_MOUSOU, 4);
		com->text_insert("タイラ", "（あの日から、賢介はあからさまに俺と距離を置くようになった。）", H_MOUSOU, 4);
		com->text_insert("タイラ", "（幼い頃から――俺たちはずっと一緒だった。）", H_MOUSOU, 4);
		com->text_insert("タイラ", "（それは今も変わらない。だが全く同じ関係というわけではない。）", H_MOUSOU, 4);
		com->text_insert("タイラ", "（今は……よく言えば、大人になったということだろうか。）", H_MOUSOU, 4);
		com->text_insert("タイラ", "（あの日……俺の親が、遊びに誘う頻度を下げろと賢介に注意したあの日――）", H_MOUSOU, 4);
		com->text_insert("タイラ", "（賢介は時間の流れを実感したんだろう。）", H_MOUSOU, 4);
		com->text_insert("タイラ", "（だが――それは俺も一緒だった。）", H_MOUSOU, 4);
		com->text_insert("タイラ", "（ずっとずっと、大人になってからも同じような日々は続かないと。実感したんだ。）", H_MOUSOU, 4);
		com->text_insert("タイラ", "（気持ちはわかる。賢介、お前の考えは間違っていない。）", H_MOUSOU, 4);
		com->text_insert("タイラ", "（ただな、賢介、俺はまだ子供でいたいんだよ。）", H_MOUSOU, 4);
		com->text_insert("タイラ", "（俺はまだ、お前とバカをやる日々を過ごしたいんだよ。）", H_MOUSOU, 4);
		com->text_insert("タイラ", "（俺の親が反対しようが、先生が怒ろうが、俺達には関係なかったはずだろう？）", H_MOUSOU, 4);
		com->text_insert("タイラ", "（――そして賢介、お前は1つだけ、決定的に間違っている。）", H_MOUSOU, 4);
		com->text_insert("タイラ", "（お前は俺のことを天才だというが、本物の天才はお前のようなやつのことを言うんだよ。）", H_MOUSOU, 4);
		com->text_insert("タイラ", "（確かに俺は勉強量のわりに学年１桁の順位以外をとったことがない。）", H_MOUSOU, 4);
		com->text_insert("タイラ", "（ただな、テストなんてものは理論的に考えて答えを導けば、絶対に正しい答えが得られるんだよ。）", H_MOUSOU, 4);
		com->text_insert("タイラ", "（賢介、お前は天才だ。）", H_MOUSOU, 4);
		com->text_insert("タイラ", "（今まで、お前は何度も新しい遊びで俺の世界を変えてくれたな。）", H_MOUSOU, 4);
		com->text_insert("タイラ", "（なあ、賢介。もう一度、俺に魅せてくれよ。）", H_MOUSOU, 4);
		com->text_insert("タイラ", "（真面目な日々なんて退屈だ。刺激的な日々を俺にくれよ、賢介。）", H_MOUSOU, 4);
	}
	play_bgm(bgm[0], 1, 22);
	finish(22);
	text_play();
}
void MainMission12::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,-100,200,-100,-100,-100,-100,-100 };
		com = new Text(x, 500);
		com->text_insert("W先生", "ぐ、あ…………この、ガキが……！！", H_NORMAL, 10);
		com->text_insert("W先生", "てめえらみたいな不良は……社会不適合者なんだよ！！", H_NORMAL,10);
		com->text_insert("タイラ", "ガキ…………か…………", H_NORMAL, 4);
		com->text_insert("タイラ", "…………", H_NORMAL, 4);
		com->text_insert("タイラ", "なあ、先生…………", H_NORMAL, 4);
		com->text_insert("タイラ", "あんたは…………その辺に落ちてる石ころで何ができる？", H_NORMAL, 4);
		com->text_insert("W先生", "あぁ……！？何言ってんだてめえ……！！", H_NORMAL, 10);
		com->text_insert("タイラ", "驚きだよな……その辺の石ころで丸一日楽しめる人がいるんだから……", H_NORMAL, 4);
		com->text_insert("W先生", "…………", H_NORMAL, 10);
		com->text_insert("タイラ", "お前ら大人には、無理だよな。", H_NORMAL, 4);
		com->text_insert("タイラ", "大人にとって、石ころは、ただの石ころだ。", H_NORMAL, 4);
	}
	play_bgm(bgm[1], 1, 11);
	finish(11);
	text_play();
}

//MM13
//先生の種類とカード
MainMission13::MainMission13(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[2];//bgm確保
	bgm[0] = LoadSoundMem("bgm/piano24.mp3");
	bgm[1] = LoadSoundMem("bgm/pianoMM13.mp3");
	volume_bgm(2);
	set_rare_rate(70, 25, 5, 0);
	size_set(9, 9, 9);
	teacher[0].get_cards()->set_card(Card(7, 0, 900, 30, 80, 90), 0);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 70, 100, 120), 1);
	teacher[0].get_cards()->set_card(Card(7, 0, 0, 30, 80, 90), 2);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 70, 100, 120), 3);
	teacher[0].get_cards()->set_card(Card(7, 0, 0, 30, 80, 90), 4);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 70, 100, 120), 5);

	teacher[1].get_cards()->set_card(Card(6, 0, 900, 25, 140, 110), 0);
	teacher[1].get_cards()->set_card(Card(6, 0, 0, 25, 140, 110), 1);
	teacher[1].get_cards()->set_card(Card(6, 0, 0, 25, 140, 110), 2);
	teacher[1].get_cards()->set_card(Card(6, 0, 0, 25, 140, 110), 3);
	teacher[1].get_cards()->set_card(Card(6, 0, 0, 25, 140, 110), 4);
	teacher[1].get_cards()->set_card(Card(6, 0, 0, 25, 140, 110), 5);
}
//会話とバトルをする
int MainMission13::play() {
	return normal_text();
}
void MainMission13::text1() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,250,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("イシ", "（真面目な奴だなってずっと思ってた。）", H_NORMAL, 3);
		com->text_insert("イシ", "（タイラは勉強自体が好きなわけじゃない。）", H_NORMAL, 3);
		com->text_insert("イシ", "（ただ、あいつが興味を持つものは全部、知的なものだった。）", H_NORMAL, 3);
		com->text_insert("イシ", "（最初のころは躊躇いがあった。）", H_NORMAL, 3);
		com->text_insert("イシ", "（僕なんかと遊んでて、内心では退屈してるんじゃないかって不安だった。）", H_NORMAL, 3);
		com->text_insert("イシ", "（でも、今思えば――あいつはいつも本気で楽しんでたんだろうな。）", H_NORMAL, 3);
		com->text_insert("イシ", "（だけど、タイラの母さんに注意されてから、タイラと遊ぶことをまた躊躇うようになった。）", H_NORMAL, 3);
		com->text_insert("イシ", "（思い出したんだ。――あいつは僕と生きてる世界が違うって。）", H_NORMAL, 3);
		com->text_insert("イシ", "（僕なんかと付き合ってたら――あいつの将来に泥がついちゃうんだ。）", H_NORMAL, 3);
		com->text_insert("イシ", "（あいつは――天才だ。僕は早く大人になって、空気を読まないといけないんだ。）", H_NORMAL, 3);
		com->text_insert("イシ", "（あいつの両親を心配させるのも、あいつが先生に怒られるのも、本来ありえない事なんだ。）", H_NORMAL, 3);
	}
	play_bgm(bgm[0], 1, 11);
	finish(11);
	text_play();
}
void MainMission13::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,300,700,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("タイラ", "賢介、もう帰るだろ？一緒に帰ろうか。", H_NORMAL, 4);
		com->text_insert("イシ", "ああ。ちょっと教室に荷物取りに行くから、ちょっと待ってて。", H_NORMAL, 3);
		com->text_insert("タイラ", "…………なあ賢介", H_NORMAL, 4);
		com->text_insert("イシ", "え？", H_NORMAL, 3);
		com->text_insert("タイラ", "お前は――躊躇ってるよな。俺とつるむことを。", H_NORMAL, 4);
		com->text_insert("イシ", "なんだよ…………急に。", H_NORMAL, 3);
		com->text_insert("タイラ", "なんだろうな…………ずっと話そうと思ってはいたんだが……", H_NORMAL, 4);
		com->text_insert("タイラ", "それなりに、自分の中で考えを整理できたんだ。", H_NORMAL, 4);
		com->text_insert("タイラ", "だから――思い切って今、話そうって思えたんだろうな。", H_NORMAL, 4);
		com->text_insert("イシ", "そうか…………僕も、だよ。", H_NORMAL, 3);
		com->text_insert("タイラ", "それは都合がいいな。", H_NORMAL, 4);
		com->text_insert("イシ", "――台羅、やっぱり僕と今まで通り付き合うのはよくないと思うんだ。", H_NORMAL, 3);
		com->text_insert("タイラ", "俺の、両親を気にしてるのか？", H_NORMAL, 4);
		com->text_insert("イシ", "違うよ。もちろんそれもあるけど……", H_NORMAL, 3);
		com->text_insert("イシ", "一番は、台羅の将来だよ。将来を、心配してるんだ。", H_NORMAL, 3);
		com->text_insert("タイラ", "俺の将来を心配……？それはありがたいけどな、余計なお世話だ。", H_NORMAL, 4);
		com->text_insert("イシ", "は…………？なんだよ、僕の気も知らずに！", H_NORMAL, 3);
		com->text_insert("タイラ", "俺の将来を心配するのは自分と親だけで十分だ。", H_NORMAL, 4);
		com->text_insert("タイラ", "賢介は俺の将来を心配して、代わりに今楽しむことを諦めさせるのか？", H_NORMAL, 4);
		com->text_insert("イシ", "何言ってんだよ！！ふざけてるんじゃないのか！？", H_NORMAL, 3);
		com->text_insert("タイラ", "…………", H_NORMAL, 4);
		com->text_insert("イシ", "勉強が嫌になったのか！？別のことに逃げようってのか！？", H_NORMAL, 3);
		com->text_insert("イシ", "もったいないだろ！！お前は天才なのに！！", H_NORMAL, 3);
		com->text_insert("タイラ", "…………ふ", H_NORMAL, 4);
		com->text_insert("イシ", "何がおかしいんだよ！", H_NORMAL, 3);
		com->text_insert("タイラ", "親も同じ事言ってたなって思ったんだ。", H_NORMAL, 4);
		com->text_insert("イシ", "…………", H_NORMAL, 3);
		com->text_insert("イシ", "…………大人になるっていうのは、我慢すること……", H_NORMAL, 3);
		com->text_insert("タイラ", "…………", H_NORMAL, 4);
		com->text_insert("イシ", "僕も、そう思うよ。", H_NORMAL, 3);
		com->text_insert("タイラ", "そうか……賢介がそうしたいならそれでもいい。", H_NORMAL, 4);
		com->text_insert("イシ", "…………", H_NORMAL, 3);
		com->text_insert("タイラ", "――俺はただ、少なくとも今はまだ賢介と遊んでいたいって思っただけだ。", H_NORMAL, 4);
		com->text_insert("イシ", "台羅…………", H_NORMAL, 3);
		com->text_insert("タイラ", "賢介はどうしたいんだ？それを教えてほしい。", H_NORMAL, 4);
		com->text_insert("タイラ", "賢介は気にしすぎなんだよ。実際、まだ俺は成績悪くなってないだろ？", H_NORMAL, 4);
		com->text_insert("イシ", "僕は…………", H_NORMAL, 3);
		com->text_insert("イシ", "遊び、たいよ…………本当は……！", H_NORMAL, 3);
		com->text_insert("タイラ", "…………そうか", H_NORMAL, 4);
		com->text_insert("タイラ", "――よかった…………その一言が聞けてな。", H_NORMAL, 4);
		com->text_insert("イシ", "…………", H_NORMAL, 3);
		com->text_insert("タイラ", "やっぱ俺達って最高のコンビだよな", H_NORMAL, 4);
	}
	play_bgm(bgm[1], 1, 42);
	move(0, 4, 500, 3);
	move(2, 3, 200, 3);
	move(16, 3, 250, 6);
	move(19, 3, 300, 6);
	move(21, 3, 350, 6);
	event(40, "event/タイラとイシ.png");//挿絵
	finish(42);
	text_play();
}

//MM14
//先生の種類とカード
MainMission14::MainMission14(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[2];//bgm確保
	bgm[0] = LoadSoundMem("bgm/piano32.mp3");
	bgm[1] = LoadSoundMem("bgm/piano36.mp3");
	volume_bgm(2);
	set_rare_rate(75, 25, 0, 0);
	size_set(9, 9, 9);
	teacher[0].get_cards()->set_card(Card(9, 0, 700, 240, 70, 120), 0);
	teacher[0].get_cards()->set_card(Card(18, 0, 0, 55, 40, 70), 1);
	teacher[0].get_cards()->set_card(Card(28, 0, 0, 60, 60, 80), 2);
	teacher[0].get_cards()->set_card(Card(18, 0, 0, 55, 40, 70), 3);
	teacher[0].get_cards()->set_card(Card(28, 0, 0, 60, 60, 80), 4);

	teacher[1].get_cards()->set_card(Card(22, 0, 700, 75, 120, 130), 0);
	teacher[1].get_cards()->set_card(Card(12, 0, 0, 25, 105, 130), 1);
	teacher[1].get_cards()->set_card(Card(5, 0, 0, 37, 70, 90), 2);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 75, 120, 130), 3);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 75, 120, 130), 4);
}
//会話とバトルをする
int MainMission14::play() {
	return normal_text();
}
void MainMission14::text1() {
	if (com == NULL) {
		int x[10] = { 100,200,-100,-100,-100,-100,500,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("ハンダ", "よし、そろったね。行こうか。", H_NORMAL, 1);
		com->text_insert("ミヤトネ", "よ、よろしくお願いします……", H_NORMAL, 6);
		com->text_insert("アカツキ", "…………", H_NORMAL, 0);
		com->text_insert("ハンダ", "よろしく！", H_NORMAL, 1);
		com->text_insert("オワダ", "｛あー、あー、裕斗、聞こえるー？｝", H_NORMAL, 5);
		com->text_insert("ハンダ", "杏子。どうした？", H_NORMAL, 1);
		com->text_insert("オワダ", "太田が来ないんだけど。ちゃんと声かけたんだよね？", H_NORMAL, 5);
		com->text_insert("ハンダ", "え！？……声はちゃんとかけたはずだけど……", H_NORMAL, 1);
		com->text_insert("オワダ", "……もしかしてこれってまた……", H_NORMAL, 5);
		com->text_insert("ハンダ", "っ……！！あいつ、また一人で…………！！", H_NORMAL, 1);
		com->text_insert("オワダ", "はあ……どうする、追いかけようか？", H_NORMAL, 5);
		com->text_insert("ハンダ", "……内本は、来てるか？", H_NORMAL, 1);
		com->text_insert("オワダ", "うん、春香はちゃんと来てるわ。", H_NORMAL, 5);
		com->text_insert("ハンダ", "じゃあ……追いかけてくれ。", H_NORMAL, 1);
		com->text_insert("ハンダ", "見つからなくても、二人で戦うのはやめて今日はあきらめよう。", H_NORMAL, 1);
		com->text_insert("オワダ", "わかった。そっちも気を付けてね。", H_NORMAL, 5);
		com->text_insert("ハンダ", "ああ。よろしくな。", H_NORMAL, 1);
		com->text_insert("ハンダ", "…………", H_NORMAL, 1);
		com->text_insert("ミヤトネ", "…………だ、大丈夫そうですか……", H_NORMAL, 6);
		com->text_insert("ハンダ", "わからない……", H_NORMAL, 1);
		com->text_insert("ハンダ", "あいつはっ……本当に…………！！", H_NORMAL, 1);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
	}
	play_bgm(bgm[0], 7, 22);
	finish(22);
	text_play();
}
void MainMission14::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,100,-100,-100,300,-100,400,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("オオタ", "ふう…………", H_NORMAL, 2);
		com->text_insert("オワダ", "ふう、じゃないわよ！！", H_NORMAL, 5);
		com->text_insert("オワダ", "太田、勝手な行動しすぎよ！！", H_NORMAL, 5);
		com->text_insert("オオタ", "来てくれたのは感謝するけどよ、無理して来なくてよかったんだぜ。", H_NORMAL, 2);
		com->text_insert("オワダ", "あんたねえ、なんで裕斗の言うこと無視するのよ！！", H_NORMAL, 5);
		com->text_insert("オオタ", "逆に聞くけどよ、なんで小和田は裕斗の言うことを聞くんだよ。", H_NORMAL, 2);
		com->text_insert("オワダ", "裕斗はみんなのことを第一に考えてるのよ。", H_NORMAL, 5);
		com->text_insert("オワダ", "あんたは違うでしょ？自分勝手よ。", H_NORMAL, 5);
		com->text_insert("オオタ", "あいつは！！", H_NORMAL, 2);
		com->text_insert("オワダ", "っ！！", H_NORMAL, 5);
		com->text_insert("オオタ", "あいつは……みんなに危険なことをさせてるじゃねえか……", H_NORMAL, 2);
		com->text_insert("オワダ", "…………", H_NORMAL, 5);
		com->text_insert("オオタ", "倒したい先生がいる、だったら自分一人でやるべきじゃねえのか？", H_NORMAL, 2);
		com->text_insert("オオタ", "俺はみんなのために戦うだけだ。裕斗は、俺だけに任せてもいいんだよ。", H_NORMAL, 2);
		com->text_insert("オワダ", "…………", H_NORMAL, 5);
		
	}
	play_bgm(bgm[1], 1, 15);
	move(8, 2, 200, 7);
	finish(15);
	text_play();
}

//MM15
//先生の種類とカード
MainMission15::MainMission15(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[2];//bgm確保
	bgm[0] = LoadSoundMem("bgm/piano24.mp3");
	bgm[1] = LoadSoundMem("bgm/piano22.mp3");
	volume_bgm(2);
	set_rare_rate(70, 25, 5, 0);
	size_set(9, 9, 9);
	teacher[0].get_cards()->set_card(Card(14, 0, 900, 15, 110, 140), 0);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 80, 100, 120), 1);
	teacher[0].get_cards()->set_card(Card(14, 0, 0, 15, 110, 140), 2);
	teacher[0].get_cards()->set_card(Card(10, 0, 0, 40, 70, 90), 3);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 80, 100, 120), 4);

	teacher[1].get_cards()->set_card(Card(2, 0, 900, 40, 80, 100), 0);
	teacher[1].get_cards()->set_card(Card(23, 0, 0, 95, 160, 170), 1);
	teacher[1].get_cards()->set_card(Card(23, 0, 0, 95, 160, 170), 2);
	teacher[1].get_cards()->set_card(Card(2, 0, 0, 40, 80, 100), 3);
	teacher[1].get_cards()->set_card(Card(23, 0, 0, 95, 160, 170), 4);
}
//会話とバトルをする
int MainMission15::play() {
	switch (now) {
	case 0:
		text1();
		break;
	case 1:
		result = bse->battle_loop(kind);//使用したキャラをkindに入れて教えてもらう
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 2:
		text2();
		break;
	case 3:
		text3();
		break;
	case 4:
		text4();
		break;
	case 5:
		text5();
		break;
	case 6:
		return result;
		break;
	}
	return 0;
}
void MainMission15::text1() {
	if (com == NULL) {
		int x[10] = { 100,300,-100,-100,-100,-100,-100,-100,400,-100 };
		com = new Text(x, -100);
		com->text_insert("ハンダ", "よし、じゃあ行こうか。", H_NORMAL, 1);
		com->text_insert("アカツキ", "…………", H_NORMAL, 0);
		com->text_insert("イバラカワ", "ねえ……太田は、大丈夫かな……？", H_NORMAL, 8);
		com->text_insert("ハンダ", "え、ああ……注意はしたけど、どうだろうね……。", H_NORMAL, 1);
		com->text_insert("アカツキ", "（これまでも半田は散々注意してきた。）", H_MOUSOU, 0);
		com->text_insert("アカツキ", "（これだけぶつかっても、半田は太田との関わりを絶たない。）", H_MOUSOU, 0);
		com->text_insert("アカツキ", "（目に見えない友情というものがあるんだろうか。）", H_MOUSOU, 0);
		com->text_insert("イバラカワ", "太田…………", H_NORMAL, 8);
		com->text_insert("ハンダ", "（太田……お前はいっつも、俺の言うことを否定するよな。）", H_MOUSOU, 1);
		com->text_insert("ハンダ", "（今回だってそうだ。昨日のことを注意しても、俺一人に任せればいいって言ったな。）", H_MOUSOU, 1);
		com->text_insert("ハンダ", "（お前はいっつも人に心配をかける。）", H_MOUSOU, 1);
		com->text_insert("ハンダ", "（いつだって一人で突っ走る。）", H_MOUSOU, 1);
		com->text_insert("ハンダ", "（俺とは真逆だ。）", H_MOUSOU, 1);
		com->text_insert("ハンダ", "（本当に理解できない。）", H_MOUSOU, 1);
		com->text_insert("ハンダ", "（――ただ）", H_MOUSOU, 1);
		com->text_insert("ハンダ", "（――――お前のことは、嫌いじゃない。）", H_MOUSOU, 1);
	}
	play_bgm(bgm[0], 8, 16);
	finish(16);
	text_play();
}
void MainMission15::text2() {
	if (com == NULL) {
		int x[10] = { 500,300,-100,-100,-100,-100,-100,-100,400,-100 };
		com = new Text(x, -100);
		com->text_insert("オワダ", "｛裕斗！！裕斗！！｝", H_NORMAL, 5);
		com->text_insert("ハンダ", "杏子！？ど、どうした！？", H_NORMAL, 1);
		com->text_insert("オワダ", "太田が…………！！太田が…………！！", H_NORMAL, 5);
		com->text_insert("ハンダ", "――っ、太田が……", H_NORMAL, 1);
		com->text_insert("イバラカワ", "え…………？", H_NORMAL, 8);
		com->text_insert("ハンダ", "今すぐ、っすぐに、行く！！", H_NORMAL, 1);

		com->text_insert("イバラカワ", "太田！！", H_NORMAL, 8);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
	}
	play_bgm(bgm[1], 1, 8);
	move(6, 1, -100, 7);
	move(7, 8, -500, 7);
	finish(8);
	text_play();
}
void MainMission15::text3() {
	if (com == NULL) {
		int x[10] = { -100,-100,400,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, 300);
		com->text_insert("オオタ", "うぅ…………", H_NORMAL, 2);
		com->text_insert("Q先生", "はあっ、はあっ……喧嘩売ってくるから、そうなんだよ…………！！", H_NORMAL, 10);
		com->text_insert("オオタ", "………………………………", H_NORMAL, 2);

		com->text_insert("オオタ", "…………ぐ", H_NORMAL, 2);
		com->text_insert("オオタ", "だめ、だ…………", H_NORMAL, 2);
		com->text_insert("オオタ", "俺…………", H_NORMAL, 2);
		com->text_insert("オオタ", "（――――――――死ぬ。）", H_MOUSOU, 2);
		com->text_insert("オオタ", "…………裕斗…………", H_NORMAL, 2);
		com->text_insert("オオタ", "ごめ…………", H_NORMAL, 2);
		com->text_insert("オオタ", "（…………ごめんな……）", H_MOUSOU, 2);
	}
	move(2, 10, -100, 3);
	finish(10);
	text_play();
}
void MainMission15::text4() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("オワダ", "太田が来なかったから前みたいに追いかけたんだけど、血まみれのQ先生を見かけたから…………", H_NORMAL, 4);
		com->text_insert("ハンダ", "…………はあ、はあ…………太田…………！！", H_NORMAL, 4);
		com->text_insert("オワダ", "多分太田は――Q先生に――", H_NORMAL, 4);
		com->text_insert("ハンダ", "違う！！", H_NORMAL, 4);
		com->text_insert("オワダ", "裕斗…………", H_NORMAL, 4);
	}
	move(1, 1, 800, 7);
	finish(5);
	text_play();
}
void MainMission15::text5() {
	if (com == NULL) {
		int x[10] = { -100,-100,250,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("オオタ", "（裕斗……お前は……危なっかしいんだよ……）", H_MOUSOU, 4);
		com->text_insert("オオタ", "（臆病なくせによ……リーダーみたいに振舞ってんじゃねえよ……）", H_MOUSOU, 4);
		com->text_insert("オオタ", "（みんなのこと大事に想ってるってのは分かるぜ。でもな……）", H_MOUSOU, 4);
		com->text_insert("オオタ", "（全員を巻き込むことねえんだよ…………）", H_MOUSOU, 4);
		com->text_insert("オオタ", "（俺がいるじゃねえか……俺だけを、頼ってくれよ……）", H_MOUSOU, 4);
		com->text_insert("オオタ", "（まじで……理解できねえ。）", H_MOUSOU, 4);
		com->text_insert("オオタ", "（でもよ、裕斗、俺ぁ……）", H_MOUSOU, 4);
		com->text_insert("オオタ", "（――お前のこと、嫌いじゃないぜ。）", H_MOUSOU, 4);

		com->text_insert("オオタ", "（あの日――あの時――俺は誓ったんだ。）", H_MOUSOU, 4);
		com->text_insert("オオタ", "（教室の隅で一人、下を向いてた俺を…………）", H_MOUSOU, 4);
		com->text_insert("オオタ", "（友達のいなかった俺を……助けてくれた。）", H_MOUSOU, 4);
		com->text_insert("オオタ", "（ありがとう、裕斗――）", H_MOUSOU, 4);
		com->text_insert("オオタ", "（――ごめん――誓い、破っちまった――）", H_MOUSOU, 4);
	}
	play_bgm(bgm[0], 1, 13);
	event(8, "event/オオタ過去.png");//挿絵
	event(11, "event/オオタとハンダ.png");//挿絵
	finish(13);
	text_play();
}

//MM16
//先生の種類とカード
MainMission16::MainMission16(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/piano33.mp3");
	volume_bgm(1);
	set_rare_rate(70, 25, 5, 0);
	size_set(9, 9, 9);
	teacher[1].get_cards()->set_card(Card(24, 0, 1500, 40, 80, 100), 0);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 95, 130, 120), 1);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 95, 130, 120), 2);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 95, 130, 120), 3);
	teacher[1].get_cards()->set_card(Card(24, 0, 0, 40, 80, 100), 4);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 95, 130, 120), 5);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 95, 130, 120), 6);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 90, 130, 120), 7);
}
//会話とバトルをする
int MainMission16::play() {
	return normal_text();
}
void MainMission16::text1() {
	if (com == NULL) {
		int x[10] = { 500,-100,-100,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("イバラカワ", "ねえ、ちょっと！！何してんの！？", H_NORMAL, 8);
		com->text_insert("アカツキ", "…………", H_NORMAL, 0);
		com->text_insert("イバラカワ", "なんでついてこないの！？", H_NORMAL, 8);
		com->text_insert("アカツキ", "…………", H_NORMAL, 0);
		com->text_insert("イバラカワ", "太田が……太田が……心配じゃないの！？", H_NORMAL, 8);
		com->text_insert("アカツキ", "……俺は…………", H_NORMAL, 0);
		com->text_insert("イバラカワ", "…………", H_NORMAL, 8);
		com->text_insert("アカツキ", "半田から、指示されてない……", H_NORMAL, 0);
		com->text_insert("アカツキ", "だから、ここで待機する……", H_NORMAL, 0);
		com->text_insert("イバラカワ", "なに……それ…………", H_NORMAL, 8);
		com->text_insert("イバラカワ", "あんた……太田が死んでもいいの！？", H_NORMAL, 8);
		com->text_insert("アカツキ", "そんなことは思ってない……", H_NORMAL, 0);
		com->text_insert("アカツキ", "（ただ、戦いってのは死と隣り合わせだ。）", H_MOUSOU, 0);
		com->text_insert("アカツキ", "（人の死を好きなように操作するのは無茶なことだ。）", H_MOUSOU, 0);
		com->text_insert("アカツキ", "俺は……ヴァルキリーに逆らう気はない……", H_NORMAL, 0);
		com->text_insert("イバラカワ", "はあ…………もういい！！勝手にしろ！！", H_NORMAL, 8);
	}
	play_bgm(bgm[0], 1, 16);
	move(0, 8, 300, 6);
	move(16, 8, -500, 6);
	finish(16);
	text_play();
}
void MainMission16::text2() {
	if (com == NULL) {
		int x[10] = { -100,200,-100,-100,-100,400,-100,500,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("ハンダ", "太田……仇は、とったぞ…………", H_NORMAL, 1);
		com->text_insert("オワダ", "裕斗…………", H_NORMAL, 5);
		com->text_insert("ハンダ", "…………", H_NORMAL, 1);
		com->text_insert("ハンダ", "（――きっと、失ったのは太田だけじゃない。）", H_MOUSOU, 1);
		com->text_insert("ハンダ", "（俺、かけがえのないものを失ったんだ。）", H_MOUSOU, 1);
		com->text_insert("ハンダ", "（俺にないものをたくさん持っている、大事な友達を、なくしたんだ。）", H_MOUSOU, 1);
	}
	finish(6);
	text_play();
}

//MM17
//先生の種類とカード
MainMission17::MainMission17(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/piano27.mp3");
	volume_bgm(1);
	set_rare_rate(70, 23, 7, 0);
	size_set(9, 9, 9);
	teacher[0].get_cards()->set_card(Card(22, 0, 700, 60, 120, 130), 0);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 60, 120, 130), 1);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 60, 120, 130), 2);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 60, 120, 130), 3);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 60, 120, 130), 4);

	teacher[1].get_cards()->set_card(Card(22, 0, 1000, 70, 150, 170), 0);
	teacher[1].get_cards()->set_card(Card(13, 0, 0, 300, 110, 110), 1);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 70, 150, 170), 2);
	teacher[1].get_cards()->set_card(Card(13, 0, 0, 300, 110, 110), 3);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 70, 150, 170), 4);

	teacher[2].get_cards()->set_card(Card(22, 0, 700, 70, 100, 110), 0);
	teacher[2].get_cards()->set_card(Card(22, 0, 0, 70, 100, 110), 1);
	teacher[2].get_cards()->set_card(Card(7, 0, 0, 40, 100, 110), 2);
	teacher[2].get_cards()->set_card(Card(22, 0, 0, 70, 140, 120), 3);
	teacher[2].get_cards()->set_card(Card(22, 0, 0, 70, 140, 120), 4);
}
//会話とバトルをする
int MainMission17::play() {
	return normal_text();
}
void MainMission17::text1() {
	if (com == NULL) {
		int x[10] = { 800,-100,-100,-100,-100,200,300,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("オワダ", "桃花ちゃん、今日はよろしく！", H_NORMAL, 5);
		com->text_insert("ミヤトネ", "よ、よろしく…………", H_NORMAL, 6);
		com->text_insert("オワダ", "お、赤月も来たわね。じゃあ行こう。", H_NORMAL, 5);
		com->text_insert("アカツキ", "ああ…………", H_NORMAL, 0);
		com->text_insert("ミヤトネ", "――あ、あの！！", H_NORMAL, 6);
		com->text_insert("ミヤトネ", "あの……足引っ張っちゃったら、ごめんなさい……", H_NORMAL, 6);
		com->text_insert("オワダ", "え、いいのいいの、気にしないで！", H_NORMAL, 5);
		com->text_insert("オワダ", "それに大丈夫よ。きっとうまくやれるって。", H_NORMAL, 5);
		com->text_insert("ミヤトネ", "でも……この前は失敗しちゃって……", H_NORMAL, 6);
		com->text_insert("オワダ", "そんなの気にする人いないって。", H_NORMAL, 5);
		com->text_insert("オワダ", "あと、もっと気楽に話してくれていいのよ。", H_NORMAL, 5);
		com->text_insert("オワダ", "桃花ちゃん、ちょっと硬くなりすぎよ。", H_NORMAL, 5);
		com->text_insert("ミヤトネ", "うう…………", H_NORMAL, 6);
		com->text_insert("オワダ", "もっと距離縮めてくれていいわよ。", H_NORMAL, 5);
		com->text_insert("アカツキ", "…………", H_NORMAL, 0);
	}
	play_bgm(bgm[0], 1, 15);
	move(2, 0, 500, 5);
	finish(15);
	text_play();
}
void MainMission17::text2() {
	if (com == NULL) {
		int x[10] = { 200,-100,-100,-100,-100,300,400,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("オワダ", "ふう。今日はありがとう、またよろしく！", H_NORMAL, 5);
		com->text_insert("ミヤトネ", "う、うん……よろしく…………", H_NORMAL, 6);
		com->text_insert("オワダ", "じゃあまた明日！", H_NORMAL, 5);
		com->text_insert("ミヤトネ", "ま、また…………", H_NORMAL, 6);
		com->text_insert("アカツキ", "…………", H_NORMAL, 0);
		com->text_insert("ミヤトネ", "あ…………", H_NORMAL, 6);
		com->text_insert("アカツキ", "ん…………", H_NORMAL, 0);
		com->text_insert("ミヤトネ", "いや…………なんでもない……", H_NORMAL, 6);
		com->text_insert("アカツキ", "…………", H_NORMAL, 0);
	}
	move(3, 5, -200, 5);
	move(5, 0, 100, 3);
	move(9, 0, -300, 3);
	finish(9);
	text_play();
}

//MM18
//先生の種類とカード
MainMission18::MainMission18(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/piano27.mp3");
	volume_bgm(1);
	set_rare_rate(70, 23, 7, 0);
	size_set(9, 9, 9);
	teacher[0].get_cards()->set_card(Card(18, 0, 1200, 70, 30, 60), 0);
	teacher[0].get_cards()->set_card(Card(28, 0, 0, 80, 80, 90), 1);
	teacher[0].get_cards()->set_card(Card(28, 0, 0, 80, 80, 90), 2);
	teacher[0].get_cards()->set_card(Card(9, 0, 0, 300, 70, 120), 3);
	teacher[0].get_cards()->set_card(Card(9, 0, 0, 300, 70, 120), 4);

	teacher[1].get_cards()->set_card(Card(22, 0, 1200, 90, 100, 120), 0);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 90, 100, 120), 1);
	teacher[1].get_cards()->set_card(Card(6, 0, 0, 30, 120, 140), 2);
	teacher[1].get_cards()->set_card(Card(6, 0, 0, 30, 120, 140), 3);
	teacher[1].get_cards()->set_card(Card(6, 0, 0, 30, 120, 140), 4);
}
//会話とバトルをする
int MainMission18::play() {
	return normal_text();
}
void MainMission18::text1() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,-100,500,200,-100,-100,-100,400 };
		com = new Text(x, -100);
		com->text_insert("オワダ", "はあ…………", H_NORMAL, 5);
		com->text_insert("トモチ", "…………", H_NORMAL, 9);
		com->text_insert("タイラ", "…………", H_NORMAL, 4);
		com->text_insert("オワダ", "ちょっと……「どうしたの？」くらい言ってくれないの？", H_NORMAL, 5);
		com->text_insert("タイラ", "砥用が言ってくれるだろう、と思った。", H_NORMAL, 4);
		com->text_insert("トモチ", "はん、言わないってわかってるくせに。", H_NORMAL, 9);
		com->text_insert("タイラ", "いや、これから言うさ。", H_NORMAL, 4);
		com->text_insert("タイラ", "砥用は俺と仲良くしたいはずだからな。", H_NORMAL, 4);
		com->text_insert("トモチ", "どういうこと？全く意味が分からないんだけど。", H_NORMAL, 9);
		com->text_insert("タイラ", "俺と仲良くなるほど、賢介とも仲良くなれるからだ。", H_NORMAL, 4);
		com->text_insert("トモチ", "…………", H_NORMAL, 9);
		com->text_insert("タイラ", "デートしたんだってな。賢介が知ってることは全て俺も知ってると思った方がいい。", H_NORMAL, 4);
		com->text_insert("トモチ", "あんのおしゃべりめ…………", H_NORMAL, 9);
		com->text_insert("タイラ", "安心しろ。あいつ、砥用からの好意には気づいてない。", H_NORMAL, 4);
		com->text_insert("トモチ", "でもデートはただの演技で、裏があるかもよ？", H_NORMAL, 9);
		com->text_insert("トモチ", "…………ま、いいわ。小和田、悩みを打ち明けて？", H_NORMAL, 9);
		com->text_insert("オワダ", "えー！？ともっちゃん、伊志が好きなのー！？", H_NORMAL, 5);
		com->text_insert("オワダ", "ほんと意外。ともっちゃん理想高そうなのに！", H_NORMAL, 5);
		com->text_insert("タイラ", "それは伊志に失礼だろ…………", H_NORMAL, 4);
		com->text_insert("トモチ", "まああいつ、ダメダメだからねー。", H_NORMAL, 9);
		com->text_insert("タイラ", "おい…………", H_NORMAL, 4);
		com->text_insert("トモチ", "で、悩みってのはなに？多分そこまで深刻じゃないよね？", H_NORMAL, 9);
		com->text_insert("オワダ", "ああ、うん――桃花ちゃん、のことなんだけど……", H_NORMAL, 5);
		com->text_insert("タイラ", "宮利根の……", H_NORMAL, 4);
		com->text_insert("オワダ", "なんか距離あるんだよね。天子ちゃんとはまた違う感じで……", H_NORMAL, 5);
		com->text_insert("オワダ", "すごく、心配なの。桃花ちゃんのこと、支えてあげられないかなって…………", H_NORMAL, 5);
		com->text_insert("タイラ", "心の支え？", H_NORMAL, 4);
		com->text_insert("オワダ", "知らない？……比較的有名な話なんだけど――", H_NORMAL, 5);
		com->text_insert("オワダ", "桃花ちゃん、親がすごく厳しいって噂。", H_NORMAL, 5);
		com->text_insert("トモチ", "あー、たまに聞くよねー。", H_NORMAL, 9);
		com->text_insert("トモチ", "彼女自身、成績以外目立ってないし、あんまり話題にならないけど。", H_NORMAL, 9);
		com->text_insert("タイラ", "そうか……けど、本人が何も言わないなら、俺たちが口出しすることじゃないんじゃないか？", H_NORMAL, 4);
		com->text_insert("オワダ", "そうかも、しれないけど……", H_NORMAL, 5);
		com->text_insert("トモチ", "ま、今ここで答えだすのは無理でしょ。とりあえずミッション片付けない？", H_NORMAL, 9);
		com->text_insert("タイラ", "そうだな。", H_NORMAL, 4);
	}
	play_bgm(bgm[0], 1, 35);
	move(0, 4, 450, 3);
	move(0, 5, 150, 3);
	move(0, 9, 350, 3);
	move(1, 4, 400, 3);
	move(1, 5, 100, 3);
	move(1, 9, 300, 3);
	move(2, 4, 350, 3);
	move(2, 5, 50, 3);
	move(2, 9, 250, 3);
	move(16, 5, 100, 3);
	finish(35);
	text_play();
}
void MainMission18::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,-100,350,200,-100,-100,-100,500 };
		com = new Text(x, -100);
		com->text_insert("オワダ", "おつかれ。じゃあ、また明日……。", H_NORMAL, 5);
		com->text_insert("タイラ", "じゃあ。", H_NORMAL, 4);
		com->text_insert("トモチ", "――ねえ、宮利根さんのことだけど", H_NORMAL, 9);
		com->text_insert("オワダ", "――――――――", H_NORMAL, 5);
		com->text_insert("トモチ", "まあ、ちょっとだけ気に留めておくから。", H_NORMAL, 9);
		com->text_insert("オワダ", "――！ありがとう！", H_NORMAL, 5);
		com->text_insert("トモチ", "…………", H_NORMAL, 9);
		com->text_insert("トモチ", "賢介には何も言わなくていいから。別に興味ないし。", H_NORMAL, 9);
		com->text_insert("タイラ", "今から賢介と会って一緒に帰るが、砥用もどうだ？", H_NORMAL, 4);
		com->text_insert("トモチ", "行く！！", H_NORMAL, 9);
	}
	play_bgm(bgm[0], 1, 10);
	move(2, 5, 100, 4);
	move(6, 5, -300, 4);
	finish(10);
	text_play();
}

//MM19
//先生の種類とカード
MainMission19::MainMission19(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[2];//bgm確保
	bgm[0] = LoadSoundMem("bgm/piano22.mp3");
	bgm[1] = LoadSoundMem("bgm/pianoMM19.mp3");
	volume_bgm(2);
	set_rare_rate(70, 23, 7, 0);
	size_set(9, 9, 9);
	teacher[1].get_cards()->set_card(Card(5, 0, 2000, 35, 90, 110), 0);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 90, 120, 100), 1);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 90, 120, 100), 2);
	teacher[1].get_cards()->set_card(Card(5, 0, 0, 35, 90, 110), 3);
	teacher[1].get_cards()->set_card(Card(12, 0, 0, 40, 110, 110), 4);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 90, 120, 100), 5);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 90, 120, 100), 6);
	teacher[1].get_cards()->set_card(Card(12, 0, 0, 40, 110, 110), 7);
}
//会話とバトルをする
int MainMission19::play() {
	return normal_text();
}
void MainMission19::text1() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,-100,-100,-100,250,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("ミヤトネ", "…………", H_NORMAL, 6);
		com->text_insert("アカツキ", "…………", H_NORMAL, 0);
		com->text_insert("ミヤトネ", "ぁ…………", H_NORMAL, 6);
		com->text_insert("アカツキ", "…………", H_NORMAL, 0);
		com->text_insert("ミヤトネ", "…………ごめ、ん……なさい…………", H_NORMAL, 6);
		com->text_insert("アカツキ", "（ミヤトネは、集合場所に来なかった。）", H_MOUSOU, 0);
		com->text_insert("アカツキ", "（連絡してもつながらないから、半田に言われて探しに来たが…………）", H_MOUSOU, 0);
		com->text_insert("ミヤトネ", "ごめんなさい、ごめんなさい…………私…………", H_NORMAL, 6);
		com->text_insert("アカツキ", "（彼女は、きれいな涙をぽろぽろと流していた。）", H_MOUSOU, 0);
		com->text_insert("ミヤトネ", "ごめんなさいごめんなさいごめんなさい…………", H_NORMAL, 6);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
		com->text_insert("ミヤトネ", "ごめん……なさい…………", H_NORMAL, 6);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
		com->text_insert("ミヤトネ", "……………………", H_NORMAL, 6);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
		com->text_insert("ミヤトネ", "どう、して…………？", H_NORMAL, 6);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
		com->text_insert("ミヤトネ", "なにも……言わないの……？", H_NORMAL, 6);
		com->text_insert("アカツキ", "別に…………なにも…………", H_NORMAL, 0);
		com->text_insert("アカツキ", "（興味がないだけだ。宮利根だって、なにもわかってない。）", H_MOUSOU, 0);
		com->text_insert("アカツキ", "（どうせ誰も俺にはついてこれない。）", H_MOUSOU, 0);
		com->text_insert("ミヤトネ", "……変わってる、ね……", H_NORMAL, 6);
		com->text_insert("ミヤトネ", "なんだろう……なんだか少し、安心、する……", H_NORMAL, 6);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
		com->text_insert("アカツキ", "（なんだ……？様子が、変わった……）", H_MOUSOU, 0);
		com->text_insert("ミヤトネ", "あ……ごめん……おかしい、よね。", H_NORMAL, 6);
		com->text_insert("ミヤトネ", "でも何も言わない赤月君に、なんだか安心したの…………", H_NORMAL, 6);
		com->text_insert("アカツキ", "…………", H_NORMAL, 0);
		com->text_insert("ミヤトネ", "なんでだろう…………みんな……小和田さんも、優しいんだけど……", H_NORMAL, 6);
		com->text_insert("ミヤトネ", "心配や同情してもらっても、安心できなかったから…………", H_NORMAL, 6);
		com->text_insert("アカツキ", "…………俺は", H_NORMAL, 0);
		com->text_insert("ミヤトネ", "ありがとう、赤月君…………", H_NORMAL, 6);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 6);
		com->text_insert("アカツキ", "（変な、感じがした。）", H_MOUSOU, 0);
		com->text_insert("ミヤトネ", "ごめんね、すぐ行くよ。ミッション。", H_NORMAL, 6);
		com->text_insert("アカツキ", "…………りで", H_NORMAL, 0);
		com->text_insert("ミヤトネ", "え？", H_NORMAL, 6);
		com->text_insert("アカツキ", "二人で、行こう…………ミッション。", H_NORMAL, 0);
		com->text_insert("ミヤトネ", "ええ！？", H_NORMAL, 6);
		com->text_insert("アカツキ", "大丈夫だ……勝てる。", H_NORMAL, 0);
		com->text_insert("ミヤトネ", "う、うん……", H_NORMAL, 6);
		com->text_insert("アカツキ", "（俺は…………人生で初めてだろうか。）", H_MOUSOU, 0);
		com->text_insert("アカツキ", "（半田の指示を無視する。）", H_MOUSOU, 0);
	}
	play_bgm(bgm[0], 1, 43);
	move(1, 0, 100, 3);
	finish(43);
	text_play();
}
void MainMission19::text2() {
	if (com == NULL) {
		int x[10] = { 200,-100,-100,-100,-100,-100,400,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("ミヤトネ", "ありがとう…………赤月君。", H_NORMAL, 6);
		com->text_insert("アカツキ", "…………ああ。", H_NORMAL, 0);
		com->text_insert("ミヤトネ", "…………", H_NORMAL, 6);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
		com->text_insert("ミヤトネ", "……私の噂…………知ってる？", H_NORMAL, 6);
		com->text_insert("アカツキ", "噂……？いや…………", H_NORMAL, 0);
		com->text_insert("ミヤトネ", "そっか…………", H_NORMAL, 6);
		com->text_insert("アカツキ", "（宮利根は、噂の内容を言わない。）", H_MOUSOU, 0);
		com->text_insert("ミヤトネ", "ねえ…………勉強できたら、幸せになれると思う？", H_NORMAL, 6);
		com->text_insert("アカツキ", "まあ……そうなんじゃないか", H_NORMAL, 0);
		com->text_insert("アカツキ", "（幸せの定義にもよる。いい職に就くことが幸せなら、の話だ。）", H_MOUSOU, 0);
		com->text_insert("アカツキ", "（俺は、幸せなんかに興味ない。）", H_MOUSOU, 0);
		com->text_insert("ミヤトネ", "がんばって勉強して、そしたらいい高校に行って", H_NORMAL, 6);
		com->text_insert("ミヤトネ", "いい高校で勉強頑張ったら、いい大学に行って", H_NORMAL, 6);
		com->text_insert("ミヤトネ", "いい大学で勉強頑張ったら、いい職について", H_NORMAL, 6);
		com->text_insert("ミヤトネ", "いい職についたら毎日仕事頑張ったり周りと競争したり…………", H_NORMAL, 6);
		com->text_insert("ミヤトネ", "時々ね、考えるの…………", H_NORMAL, 6);
		com->text_insert("ミヤトネ", "私…………いつ幸せになれるんだろうって。", H_NORMAL, 6);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
		com->text_insert("アカツキ", "（また、泣きだした…………）", H_MOUSOU, 0);
		com->text_insert("アカツキ", "（くだらない…………俺はなにも、言わなくていい…………）", H_MOUSOU, 0);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
		com->text_insert("ミヤトネ", "……………………", H_NORMAL, 6);
		com->text_insert("アカツキ", "…………や……なら……", H_NORMAL, 0);
		com->text_insert("アカツキ", "嫌なら…………勉強は、やめればいい…………", H_NORMAL, 0);
		com->text_insert("ミヤトネ", "そう、だよね……でも、無理なの…………", H_NORMAL, 6);
		com->text_insert("ミヤトネ", "今まで頑張ってきたから…………もうっ……止まれないの…………", H_NORMAL, 6);
		com->text_insert("アカツキ", "俺は…………勉強できないから…………", H_NORMAL, 0);
		com->text_insert("アカツキ", "宮利根のことはすごいと思うし、俺よりも幸せになるんだと思う。", H_NORMAL, 0);
		com->text_insert("ミヤトネ", "今更辞めるなんて…………もったいないよね…………", H_NORMAL, 6);
		com->text_insert("アカツキ", "ただ…………幸せってのはそんな単純じゃないのかもって今思った。", H_NORMAL, 0);
		com->text_insert("ミヤトネ", "…………", H_NORMAL, 6);
		com->text_insert("アカツキ", "……行こうか。早く帰らないといけないって言ってたよな。", H_NORMAL, 0);

		com->text_insert("ミヤトネ", "……………………", H_NORMAL, 6);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
		com->text_insert("ミヤトネ", "…………お願い", H_NORMAL, 6);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
		com->text_insert("ミヤトネ", "私が…………限界になったとき――", H_NORMAL, 6);
		com->text_insert("ミヤトネ", "――私に――諦めさせてほしいの…………", H_NORMAL, 6);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
		com->text_insert("アカツキ", "（宮利根に引導を渡す日が来ることを、俺は心から願った。）", H_MOUSOU, 0);
	}
	play_bgm(bgm[1], 1, 41);
	move(33, 6, 270, 3);
	event(35, "event/ミヤトネとアカツキ.png");//挿絵
	finish(41);
	text_play();
}

//MM20
//先生の種類とカード
MainMission20::MainMission20(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/pianoMM20.mp3");
	volume_bgm(1);
	ed = NULL;
	set_rare_rate(52, 33, 13, 2);
	size_set(9, 9, 9);
	teacher[1].get_cards()->set_card(Card(28, 0, 1600, 70, 70, 70), 0);//2500
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 80, 100, 100), 1);
	teacher[1].get_cards()->set_card(Card(23, 0, 0, 100, 130, 130), 2);
	teacher[1].get_cards()->set_card(Card(28, 0, 0, 70, 70, 70), 3);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 80, 100, 100), 4);
	teacher[1].get_cards()->set_card(Card(23, 0, 0, 100, 130, 130), 5);
	teacher[1].get_cards()->set_card(Card(28, 0, 0, 70, 70, 70), 6);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 80, 100, 100), 7);
	teacher[1].get_cards()->set_card(Card(23, 0, 0, 100, 130, 130), 8);
}
//会話とバトルをする
extern bool mouse_flag;
int MainMission20::play() {
	switch (now) {
	case 0:
		text1();
		break;
	case 1:
		result = bse->battle_loop(kind);//使用したキャラをkindに入れて教えてもらう
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 2:
		text2();
		break;
	case 3:
		if (ed == NULL) {
			ed = new Ed;
		}
		if (ed->play() == true) {
			delete ed;
			now++;
		}
		break;
	case 4:
		return result;
		break;
	}
	return 0;
}
void MainMission20::text1() {
	if (com == NULL) {
		int x[10] = { -100,300,-100,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("ハンダ", "（約束の時間なのに……亮也…………）", H_MOUSOU, 1);
		com->text_insert("ハンダ", "……………………なんで", H_NORMAL, 1);
		com->text_insert("ハンダ", "（亮也が俺の言ったことを無視するなんてありえない。）", H_MOUSOU, 1);
		com->text_insert("ハンダ", "（でも、今なら少しだけ疑ってしまう。）", H_MOUSOU, 1);
		com->text_insert("ハンダ", "（昨日、亮也は初めて俺の指示を無視した行動をとった。）", H_MOUSOU, 1);
		com->text_insert("ハンダ", "どう、して…………", H_NORMAL, 1);
		com->text_insert("ハンダ", "（亮也のことは世界で一番信用してる。昔からそうだった。）", H_MOUSOU, 1);
		com->text_insert("ハンダ", "今日、二人で校長を倒して終わりにしようって、言ったのに……", H_NORMAL, 1);
		com->text_insert("ハンダ", "（ずっとリーダーとしてみんなを引っ張ってたつもりだったけど）", H_MOUSOU, 1);
		com->text_insert("ハンダ", "なんだろう……孤独を、感じる…………", H_NORMAL, 1);

		com->text_insert("ハンダ", "……………………", H_NORMAL, 1);

		com->text_insert("ハンダ", "……………………！", H_NORMAL, 1);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
		com->text_insert("ハンダ", "りょう、や……", H_NORMAL, 1);
		com->text_insert("アカツキ", "Z校長、倒そう、か。", H_NORMAL, 0);
		com->text_insert("ハンダ", "なんで…………今更来たんだよ。", H_NORMAL, 1);
		com->text_insert("アカツキ", "何のことだ。", H_NORMAL, 0);
		com->text_insert("ハンダ", "何時間待ったと思ってるんだ。どうして……遅くなったんだよ。", H_NORMAL, 1);
		com->text_insert("アカツキ", "何を言ってる。まだ夕方だ。", H_NORMAL, 0);
		com->text_insert("ハンダ", "…………は？", H_NORMAL, 1);
		com->text_insert("アカツキ", "まだ授業が終わったばっかだ。", H_NORMAL, 0);
		com->text_insert("ハンダ", "そ、そんなわけ……なんだ、どうなってるんだ！？", H_NORMAL, 1);
		com->text_insert("アカツキ", "やるんだろ、Z校長。行こうか。", H_NORMAL, 0);
		com->text_insert("ハンダ", "こんなのおかしい！！俺は、何時間も待ったんだ！！", H_NORMAL, 1);
		com->text_insert("アカツキ", "世界なんてずっとおかしいもんだ。", H_NORMAL, 0);
		com->text_insert("ハンダ", "…………ちょっと待て、亮也、お前、口数多くないか？", H_NORMAL, 1);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
		com->text_insert("ハンダ", "お前はいつも、こんなにしゃべらないだろ。何かが、違う。", H_NORMAL, 1);
		com->text_insert("アカツキ", "なんだろうな…………分かる。オレも、そう思う。", H_NORMAL, 0);
		com->text_insert("ハンダ", "……………………", H_NORMAL, 1);
		com->text_insert("アカツキ", "俺はいつも、ただの傍観者だった。", H_NORMAL, 0);
		com->text_insert("ハンダ", "亮也…………？", H_NORMAL, 1);
		com->text_insert("アカツキ", "俺は…………", H_NORMAL, 0);
		com->text_insert("アカツキ", "半田、俺はお前のことを友達とは思ってない。", H_NORMAL, 0);
		com->text_insert("ハンダ", "え…………", H_NORMAL, 1);
		com->text_insert("アカツキ", "当然、太田や小和田も。誰も友達とは思ってない。", H_NORMAL, 0);
		com->text_insert("アカツキ", "どいつもこいつも、何も考えてないんだ。浅はかなんだ。", H_NORMAL, 0);
		com->text_insert("ハンダ", "は…………あ…………？", H_NORMAL, 1);
		com->text_insert("アカツキ", "全部全部、俺にとってはくだらない。", H_NORMAL, 0);
		com->text_insert("アカツキ", "俺はいつだって…………青春の傍観者だった。", H_NORMAL, 0);
		com->text_insert("ハンダ", "……………………", H_NORMAL, 1);
		com->text_insert("ハンダ", "嘘……………………だったのか？", H_NORMAL, 1);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
		com->text_insert("ハンダ", "今まで…………全部、嘘だったのか？", H_NORMAL, 1);
		com->text_insert("ハンダ", "俺が一人で勝手に…………盛り上がってただけだってのか？", H_NORMAL, 1);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
		com->text_insert("アカツキ", "ああ、そうだ。", H_NORMAL, 0);
		com->text_insert("ハンダ", "――――――――嘘だ", H_NORMAL, 1);
		com->text_insert("ハンダ", "うそだあああぁあぁぁぁ！！！", H_NORMAL, 1);

		com->text_insert("アカツキ", "殴るのか、俺を。", H_NORMAL, 0);
		com->text_insert("ハンダ", "うっ、うぅ……………………", H_NORMAL, 1);
		com->text_insert("アカツキ", "……驚いた、半田、そんな顔ができたのか。", H_NORMAL, 0);
		com->text_insert("ハンダ", "亮也！！絶対、絶対！！許さない！！", H_NORMAL, 1);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
		com->text_insert("アカツキ", "（――俺が見てきた青春の景色はいつだって夕方で――）", H_MOUSOU, 0);
		com->text_insert("アカツキ", "（全部、俺にとってくだらない。けど輝いてるんだ。）", H_MOUSOU, 0);
		com->text_insert("アカツキ", "（これだってそうだ。夕陽をバックに半田が無意味に咆哮している。）", H_MOUSOU, 0);
		com->text_insert("ハンダ", "亮也あ！！俺を、見ろよおぉぉ！！", H_NORMAL, 1);
		com->text_insert("アカツキ", "っ！！", H_NORMAL, 0);
		com->text_insert("ハンダ", "無関係装ってんじゃねえよ！！", H_NORMAL, 1);
		com->text_insert("ハンダ", "無視してれば、無関心なら、逃げられると、思ってんのかあ！！", H_NORMAL, 1);
		com->text_insert("アカツキ", "――――――――っ", H_NORMAL, 0);
		com->text_insert("ハンダ", "お前はっ……俺を、裏切ったんだよっ……逃げるんじゃ、ねえよ……！！", H_NORMAL, 1);
		com->text_insert("アカツキ", "――――――――そうか", H_NORMAL, 0);
		com->text_insert("アカツキ", "（――俺が――他人を変えたんだ。）", H_MOUSOU, 0);
		com->text_insert("アカツキ", "これだったか……違和感の原因は――", H_NORMAL, 0);
		com->text_insert("ハンダ", "……………………", H_NORMAL, 1);
		com->text_insert("アカツキ", "あの時――宮利根との約束で――第三者じゃいられなくなったんだ。", H_NORMAL, 0);
		com->text_insert("ハンダ", "へらへらしてんじゃねえよ！！ずっと、ずっと！！かかわってきたくせに！！", H_NORMAL, 1);
		com->text_insert("アカツキ", "――――――――", H_NORMAL, 0);
		com->text_insert("アカツキ", "――――――――あぁ", H_NORMAL, 0);
		com->text_insert("アカツキ", "（俺も、ずっと照らされてたんだったな。）", H_MOUSOU, 0);
		com->text_insert("ハンダ", "わかったかっ……わかったかよ、亮也……", H_NORMAL, 1);
		com->text_insert("アカツキ", "ああ…………", H_NORMAL, 0);
		com->text_insert("ハンダ", "お前は、俺の親友なんだよ……", H_NORMAL, 1);
		com->text_insert("ハンダ", "お前と一緒にZ校長倒して、それで終わりなんだよ…………", H_NORMAL, 1);
		com->text_insert("アカツキ", "そう、みたいだな…………", H_NORMAL, 0);
		com->text_insert("ハンダ", "やれよ亮也、俺と、一緒に。", H_NORMAL, 1);
		com->text_insert("アカツキ", "ああ。行こうか。", H_NORMAL, 0);
	}
	play_bgm(bgm[0], 12, 79);
	move(12, 0, 100, 3);
	move(49, 1, 180, 6);
	finish(10);
	finish(11);
	finish(79);
	text_play();
}
void MainMission20::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("アカツキ", "…………朝、か。", H_NORMAL, 0);
		com->text_insert("アカツキ", "――学校、だるいな…………", H_NORMAL, 0);
		com->text_insert("アカツキ", "（本当にＺ校長が死んだら、その日くらいは学校休みになったりするのだろうか。）", H_NORMAL, 0);
		com->text_insert("アカツキ", "――まあ、どうでもいいか。", H_NORMAL, 0);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
		com->text_insert("アカツキ", "（ああ、半田を待たせることになる。早く準備して行くか。）", H_NORMAL, 0);
	}
	event(1, "event/アカツキ起床.png");//挿絵
	finish(6);
	text_play();
}

///////デイリーミッション//////////
int rand_te_card(int te_kind) {
	int k[5] = {0, 0, 0, 0, 0};
	int r;
	switch (te_kind) {
	case O_T:
		k[0] = 22; k[1] = 20; k[2] = 30;
		r = GetRand(2);
		return k[r];
	case P_T:
		k[0] = 7; k[1] = 22;
		r = GetRand(1);
		return k[r];
	case Q_T:
		k[0] = 24; k[1] = 22; k[2] = 22;
		r = GetRand(2);
		return k[r];
	case R_T:
		k[0] = 6; k[1] = 22;
		r = GetRand(1);
		return k[r];
	case S_T:
		k[0] = 8; k[1] = 22; k[2] = 30;
		r = GetRand(2);
		return k[r];
	case T_T:
		k[0] = 5; k[1] = 12; k[2] = 22;
		r = GetRand(2);
		return k[r];
	case U_T:
		k[0] = 13; k[1] = 27; k[2] = 27;
		r = GetRand(2);
		return k[r];
	case V_T:
		k[0] = 10; k[1] = 14; k[2] = 22;
		r = GetRand(2);
		return k[r];
	case W_T:
		k[0] = 19; k[1] = 4; k[2] = 22;
		r = GetRand(2);
		return k[r];
	case X_T:
		k[0] = 2; k[1] = 3; k[2] = 23;
		r = GetRand(2);
		return k[r];
	case Y_T:
		k[0] = 9; k[1] = 18; k[2] = 28; k[3] = 18; k[4] = 28;
		r = GetRand(4);
		return k[r];
	case Z_T:
		k[0] = 23; k[1] = 18; k[2] = 25;
		r = GetRand(2);
		return k[r];
	}
	return 0;
}
//DM1
//先生の種類とカード
DailyMission1::DailyMission1(MissionText& detail) :
	Mission(detail)
{
	set_rare_rate(80, 20, 0, 0);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 3;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(100, 0, 0, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
//バトルだけをする
int DailyMission1::play() {
	switch (now) {
	case 0:
		result = bse->battle_loop(kind);//使用したキャラをkindに入れて教えてもらう
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 1:
		return result;
		break;
	}
	return 0;
}
void DailyMission1::text1() {
}
void DailyMission1::text2() {
}

DailyMission2::DailyMission2(MissionText& detail) :
	Mission(detail)
{
	set_rare_rate(70, 30, 0, 0);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(100, 0, 0, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
//バトルだけをする
int DailyMission2::play() {
	switch (now) {
	case 0:
		result = bse->battle_loop(kind);//使用したキャラをkindに入れて教えてもらう
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 1:
		return result;
		break;
	}
	return 0;
}
void DailyMission2::text1() {
}
void DailyMission2::text2() {
}

DailyMission3::DailyMission3(MissionText& detail) :
	Mission(detail)
{
	set_rare_rate(63, 32, 5, 0);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(70, 30, 0, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
//バトルだけをする
int DailyMission3::play() {
	switch (now) {
	case 0:
		result = bse->battle_loop(kind);//使用したキャラをkindに入れて教えてもらう
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 1:
		return result;
		break;
	}
	return 0;
}
void DailyMission3::text1() {
}
void DailyMission3::text2() {
}

DailyMission4::DailyMission4(MissionText& detail) :
	Mission(detail)
{
	set_rare_rate(55, 35, 10, 0);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(30, 70, 0, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
//バトルだけをする
int DailyMission4::play() {
	switch (now) {
	case 0:
		result = bse->battle_loop(kind);//使用したキャラをkindに入れて教えてもらう
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 1:
		return result;
		break;
	}
	return 0;
}
void DailyMission4::text1() {
}
void DailyMission4::text2() {
}

DailyMission5::DailyMission5(MissionText& detail) :
	Mission(detail)
{
	set_rare_rate(45, 40, 13, 2);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 100, 0, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
//バトルだけをする
int DailyMission5::play() {
	switch (now) {
	case 0:
		result = bse->battle_loop(kind);//使用したキャラをkindに入れて教えてもらう
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 1:
		return result;
		break;
	}
	return 0;
}
void DailyMission5::text1() {
}
void DailyMission5::text2() {
}

DailyMission6::DailyMission6(MissionText& detail) :
	Mission(detail)
{
	set_rare_rate(35, 45, 15, 5);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 70, 30, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
//バトルだけをする
int DailyMission6::play() {
	switch (now) {
	case 0:
		result = bse->battle_loop(kind);//使用したキャラをkindに入れて教えてもらう
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 1:
		return result;
		break;
	}
	return 0;
}
void DailyMission6::text1() {
}
void DailyMission6::text2() {
}

DailyMission7::DailyMission7(MissionText& detail) :
	Mission(detail)
{
	set_rare_rate(22, 60, 15, 8);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 30, 70, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
//バトルだけをする
int DailyMission7::play() {
	switch (now) {
	case 0:
		result = bse->battle_loop(kind);//使用したキャラをkindに入れて教えてもらう
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 1:
		return result;
		break;
	}
	return 0;
}
void DailyMission7::text1() {
}
void DailyMission7::text2() {
}

DailyMission8::DailyMission8(MissionText& detail) :
	Mission(detail)
{
	set_rare_rate(10, 65, 15, 10);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 0, 100, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
//バトルだけをする
int DailyMission8::play() {
	switch (now) {
	case 0:
		result = bse->battle_loop(kind);//使用したキャラをkindに入れて教えてもらう
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 1:
		return result;
		break;
	}
	return 0;
}
void DailyMission8::text1() {
}
void DailyMission8::text2() {
}

DailyMission9::DailyMission9(MissionText& detail) :
	Mission(detail)
{
	set_rare_rate(0, 58, 20, 12);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 0, 70, 30);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
//バトルだけをする
int DailyMission9::play() {
	switch (now) {
	case 0:
		result = bse->battle_loop(kind);//使用したキャラをkindに入れて教えてもらう
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 1:
		return result;
		break;
	}
	return 0;
}
void DailyMission9::text1() {
}
void DailyMission9::text2() {
}

DailyMission10::DailyMission10(MissionText& detail) :
	Mission(detail)
{
	set_rare_rate(0, 50, 30, 20);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 0, 30, 70);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
//バトルだけをする
int DailyMission10::play() {
	switch (now) {
	case 0:
		result = bse->battle_loop(kind);//使用したキャラをkindに入れて教えてもらう
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 1:
		return result;
		break;
	}
	return 0;
}
void DailyMission10::text1() {
}
void DailyMission10::text2() {
}

//イベントミッション
EventMission1::EventMission1(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/EMc.ogg");
	volume_bgm(1);
	set_rare_rate(63, 32, 5, 0);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(70, 30, 0, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission1::play() {
	return after_text();
}
void EventMission1::text1() {
}
void EventMission1::text2() {
	if (com == NULL) {
		int x[10] = { 100,200,-100,-100,-100,300,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("オワダ", "幼稚園の頃ってさー、気楽だったよね～。", H_NORMAL, 5);
		com->text_insert("ハンダ", "まあ、勉強もしなくてよかったからな。", H_NORMAL, 1);
		com->text_insert("オワダ", "裕斗ってそんなに勉強してたっけ？", H_NORMAL, 5);
		com->text_insert("ハンダ", "ぐ…………してない…………", H_NORMAL, 1);
		com->text_insert("ハンダ", "でも、あの頃はただ走ってるだけでも楽しかったよな。", H_NORMAL, 1);
		com->text_insert("オワダ", "……………………？", H_NORMAL, 5);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
		com->text_insert("ハンダ", "なぜ疑問符！？まさかの共感ゼロ！？", H_NORMAL, 1);
		com->text_insert("ハンダ", "いや俺だけ！？何も考えずに走ってるだけでもなぜか楽しかったじゃん！？", H_NORMAL, 1);
		com->text_insert("オワダ", "ごめん、うそうそ。なんとなくわかる、それ。", H_NORMAL, 5);
		com->text_insert("ハンダ", "そっか…………そう、だよな。", H_NORMAL, 1);
		com->text_insert("オワダ", "走るだけってのは極端だけどさ…………なんだろう、", H_NORMAL, 5);
		com->text_insert("オワダ", "昔楽しかったこととか、思い返せば何が面白いんだろう、ってこと多いよね。", H_NORMAL, 5);
		com->text_insert("ハンダ", "そうそう、なんか寂しいんだよなあ。それが。", H_NORMAL, 1);
		com->text_insert("オワダ", "でも、逆もあるんだよね～", H_NORMAL, 5);
		com->text_insert("ハンダ", "逆？", H_NORMAL, 1);
		com->text_insert("オワダ", "幼稚園で裕斗がおもらししたの、思い出したら面白いけど、当時はなんとも思わなかったわ。", H_NORMAL, 5);
		com->text_insert("ハンダ", "ばっ…………覚えてたのかよ！！", H_NORMAL, 1);
		com->text_insert("オワダ", "あはははは！", H_NORMAL, 5);
		com->text_insert("ハンダ", "思い出しても面白くねえよ、そんなこと！！", H_NORMAL, 1);
		com->text_insert("アカツキ", "（なんとも思われなかったって、一番恥ずかしいやつ…………）", H_MOUSOU, 0);
	}
	play_bgm(bgm[0], 1, 21);
	finish(21);
	text_play();
}

EventMission2::EventMission2(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/EMe.ogg");
	volume_bgm(1);
	set_rare_rate(63, 32, 5, 0);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(70, 30, 0, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission2::play() {
	return after_text();
}
void EventMission2::text1() {
}
void EventMission2::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,400,-100,-100,100,-100,-100,-100,300 };
		com = new Text(x, -100);
		com->text_insert("トモチ", "ねえ、太田って好きな人いる？", H_NORMAL, 9);
		com->text_insert("オオタ", "ああ。俺は茨川が好きだ。", H_NORMAL, 2);
		com->text_insert("オワダ", "すがすがしいぐらいあっさり言ったー！", H_NORMAL, 5);
		com->text_insert("オオタ", "へへ。俺は仲間に隠し事はしねーよ。", H_NORMAL, 2);
		com->text_insert("トモチ", "私もよ。茨川に伝えとくね。", H_NORMAL, 9);
		com->text_insert("オオタ", "それだけは勘弁してくれ…………", H_NORMAL, 2);
		com->text_insert("トモチ", "大丈夫だよ。彼女も太田が好きだから。", H_NORMAL, 9);
		com->text_insert("オオタ", "え、マジで！？嘘だろ！？", H_NORMAL, 2);
		com->text_insert("トモチ", "うん、嘘。", H_NORMAL, 9);
		com->text_insert("オオタ", "……………………", H_NORMAL, 2);
		com->text_insert("トモチ", "あっははははは！", H_NORMAL, 9);
		com->text_insert("オオタ", "……………………", H_NORMAL, 2);
		com->text_insert("オワダ", "お、太田、ドンマイ。", H_NORMAL, 5);
		com->text_insert("オオタ", "……………………", H_NORMAL, 2);
		com->text_insert("オワダ", "ね、ね、今のが嘘ってだけでさ。もしかしたらほんとに好きかもしれないよ？", H_NORMAL, 5);
		com->text_insert("オオタ", "……………………うぅ…………", H_NORMAL, 2);
		com->text_insert("オワダ", "お、落ち込みすぎだろ！？情けねえ！！", H_NORMAL, 5);
		com->text_insert("トモチ", "最っ高の反応！！ははははは！！", H_NORMAL, 9);
		com->text_insert("オワダ", "あんたねえ……………………", H_NORMAL, 5);
		com->text_insert("オオタ", "へっ……いいさ。自力であいつに認めてもらうさ。", H_NORMAL, 2);
		com->text_insert("オワダ", "割とあっさり立ち直った…………", H_NORMAL, 5);
		com->text_insert("トモチ", "ま、誰にも言わないよ。頑張りたまえ、太田君？", H_NORMAL, 9);
		com->text_insert("オオタ", "砥用のやろお……俺の純粋な心を弄びやがって…………", H_NORMAL, 2);
		com->text_insert("オワダ", "うーん、やっぱ違和感ありまくり。ともっちゃん、あんなテンションの人じゃなかったのに……", H_NORMAL, 5);
		com->text_insert("オオタ", "はあ！？まじで、元からじゃねえの、あれ！！", H_NORMAL, 2);
		com->text_insert("オワダ", "うん、去年同じクラスだったけど、めっちゃまともだったよ。", H_NORMAL, 5);
		com->text_insert("オオタ", "嘘だろ…………想像できねえ…………", H_NORMAL, 2);
		com->text_insert("オワダ", "髪も結んでなくて、清楚な感じでね？おとなしくてクラスの人気者だったのよ？", H_NORMAL, 5);
		com->text_insert("オオタ", "ありえねえ…………それが演技だったらまだ納得できるかも…………", H_NORMAL, 2);
	}
	play_bgm(bgm[0], 1, 29);
	move(22, 9, 700, 5);
	finish(29);
	text_play();
}

EventMission3::EventMission3(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/EMb.ogg");
	volume_bgm(1);
	set_rare_rate(63, 32, 5, 0);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(70, 30, 0, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission3::play() {
	return after_text();
}
void EventMission3::text1() {
}
void EventMission3::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,-100,200,-100,-100,400,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("ウチモト", "ねーねー台羅ー、今度勉強教えてよー。", H_NORMAL, 7);
		com->text_insert("タイラ", "ん、次のテスト勉強か？", H_NORMAL, 4);
		com->text_insert("タイラ", "教えるのは全然いいが……普段小和田に教わってるんじゃないのか？", H_NORMAL, 4);
		com->text_insert("ウチモト", "んーそうなんだけどねー、全然成績上がんないの。", H_NORMAL, 7);
		com->text_insert("タイラ", "俺だったらもっと良くなるって思ってるわけか？", H_NORMAL, 4);
		com->text_insert("ウチモト", "うん！台羅はめっちゃ成績良いし、きっと教えるのもうまいよ！", H_NORMAL, 7);
		com->text_insert("タイラ", "小和田も成績良いし、あまり期待しない方がいいと思うぞ。", H_NORMAL, 4);
		com->text_insert("タイラ", "小和田から俺に変えても、成績が急激に変わるとは思えない。", H_NORMAL, 4);
		com->text_insert("ウチモト", "いいっていいって、やってみないと分かんないし。", H_NORMAL, 7);
		com->text_insert("タイラ", "確かにそれはそうだ。", H_NORMAL, 4);
		com->text_insert("タイラ", "じゃあいつにしようか？俺は塾がない日ならいつでもいいが。", H_NORMAL, 4);
		com->text_insert("ウチモト", "私は塾も行ってないし、テスト前は部活もないから、台羅の都合に合わせるよ。", H_NORMAL, 7);
		com->text_insert("ウチモト", "テスト前のどっかで一時間、一番都合がいい時間教えて？", H_NORMAL, 7);
		com->text_insert("タイラ", "ん……？一時間でいいのか、遠慮しなくていいんだが。", H_NORMAL, 4);
		com->text_insert("ウチモト", "あはは、遠慮はしてないよー、杏子にもおんなじ時間付き合ってもらってたし。", H_NORMAL, 7);
		com->text_insert("タイラ", "……………………なるほど", H_NORMAL, 4);
		com->text_insert("タイラ", "…………内本、一時間じゃ誰に教わっても大して成績は上がらないぞ。", H_NORMAL, 4);
		com->text_insert("ウチモト", "え…………", H_NORMAL, 7);
		com->text_insert("タイラ", "（なんだその本気のキョトンとした顔は……）", H_MOUSOU, 4);
	}
	play_bgm(bgm[0], 1, 19);
	finish(19);
	text_play();
}

EventMission4::EventMission4(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/EMd.ogg");
	volume_bgm(1);
	set_rare_rate(63, 32, 5, 0);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(70, 30, 0, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission4::play() {
	return after_text();
}
void EventMission4::text1() {
}
void EventMission4::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,100,200,-100,350,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("ミヤトネ", "はあ……………………", H_NORMAL, 6);
		com->text_insert("イシ", "ど、どうしたの？", H_NORMAL, 3);
		com->text_insert("ミヤトネ", "あ、ご、ごめんなさい。", H_NORMAL, 6);
		com->text_insert("ミヤトネ", "ちょっと……今回のテストの成績が悪かったのを引きずっちゃって……", H_NORMAL, 6);
		com->text_insert("イシ", "へ、へえ。宮利根さんでも調子悪いときあるんだ。", H_NORMAL, 3);
		com->text_insert("イシ", "ちなみに……何位だったか訊いていい？", H_NORMAL, 3);
		com->text_insert("ミヤトネ", "あ、はい……１２位……でした。", H_NORMAL, 6);
		com->text_insert("イシ", "……………………", H_NORMAL, 3);
		com->text_insert("タイラ", "ちょっと期待したか、３９位。", H_NORMAL, 4);
		com->text_insert("イシ", "し、してない！！てか順位で呼ぶな！！", H_NORMAL, 3);
		com->text_insert("ミヤトネ", "えっと……台羅、君は何位でした……？", H_NORMAL, 6);
		com->text_insert("タイラ", "俺は４位だった。", H_NORMAL, 4);
		com->text_insert("ミヤトネ", "そうですか…………", H_NORMAL, 6);
		com->text_insert("イシ", "……き、気にしなくていいって！こいつ、意外と真面目に塾行ってるし！", H_NORMAL, 3);
		com->text_insert("ミヤトネ", "あ、ごめんなさい、競争心とか、持ってるわけじゃなくて……", H_NORMAL, 6);
		com->text_insert("ミヤトネ", "勉強方法とか……コツとか、訊きたいなって……", H_NORMAL, 6);
		com->text_insert("イシ", "（やばい、台羅は宮利根さんほど勉強してないぞ。）", H_MOUSOU, 3);
		com->text_insert("イシ", "（それを知ったらショックを受けるんじゃ…………）", H_MOUSOU, 3);
		com->text_insert("タイラ", "授業中に先生の口ぶりとかから、テストで出る問題を予想するのは結構効くな。", H_NORMAL, 4);
		com->text_insert("ミヤトネ", "な、なるほど…………！", H_NORMAL, 6);
		com->text_insert("イシ", "なるほど！！", H_NORMAL, 3);
	}
	play_bgm(bgm[0], 1, 21);
	finish(21);
	text_play();
}

EventMission5::EventMission5(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/EMb.ogg");
	volume_bgm(1);
	set_rare_rate(55, 35, 10, 0);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(30, 70, 0, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission5::play() {
	return after_text();
}
void EventMission5::text1() {
}
void EventMission5::text2() {
	if (com == NULL) {
		int x[10] = { 200,-100,-100,-100,-100,-100,-100,-100,400,-100 };
		com = new Text(x, -100);
		com->text_insert("イバラカワ", "まったく。どうして半田は私に頼んだの。", H_NORMAL, 8);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
		com->text_insert("イバラカワ", "赤月とだってあんま一緒に戦ったことないのに。", H_NORMAL, 8);
		com->text_insert("イバラカワ", "もっと別の編成があったでしょ。もっとちゃんと考えてほしいわ。", H_NORMAL, 8);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
		com->text_insert("イバラカワ", "あんたもそう思わない？", H_NORMAL, 8);
		com->text_insert("アカツキ", "いや…………別に…………", H_NORMAL, 0);
		com->text_insert("イバラカワ", "……ま、いいわ。", H_NORMAL, 8);
		com->text_insert("イバラカワ", "大体半田はいつも詰めが甘いと思うのよ。", H_NORMAL, 8);
		com->text_insert("イバラカワ", "この前だってそう。もっと先生との相性を考えてたらあんなに苦戦しなかった。", H_NORMAL, 8);
		com->text_insert("アカツキ", "…………あ…………", H_NORMAL, 0);
		com->text_insert("イバラカワ", "なに", H_NORMAL, 8);
		com->text_insert("アカツキ", "いや…………", H_NORMAL, 0);
		com->text_insert("イバラカワ", "なにか文句あんの？", H_NORMAL, 8);
		com->text_insert("アカツキ", "別に…………", H_NORMAL, 0);
		com->text_insert("イバラカワ", "あんたもね、たまには半田に反論してもいいと思う。", H_NORMAL, 8);
		com->text_insert("イバラカワ", "一番近くにいるあんたがちゃんと言うのは大事でしょ。", H_NORMAL, 8);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
	}
	play_bgm(bgm[0], 1, 18);
	finish(18);
	text_play();
}

EventMission6::EventMission6(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/EMe.ogg");
	volume_bgm(1);
	set_rare_rate(55, 35, 10, 0);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(30, 70, 0, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission6::play() {
	return after_text();
}
void EventMission6::text1() {
}
void EventMission6::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,200,300,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("オオタ", "なあ賢介、また今度遊ぼうぜ！", H_NORMAL, 2);
		com->text_insert("イシ", "え、いいけど……どうして？", H_NORMAL, 3);
		com->text_insert("オオタ", "どうして？遊ぶのに理由なんていらないだろー？", H_NORMAL, 2);
		com->text_insert("イシ", "そ、そうだけど、ちょっと驚いたんだ。", H_NORMAL, 3);
		com->text_insert("イシ", "太田が僕を誘ってくるなんて思ってもなかった。", H_NORMAL, 3);
		com->text_insert("オオタ", "ははは、共闘した仲だろ、普通だって！", H_NORMAL, 2);
		com->text_insert("イシ", "ごめん、僕がひねくれてるだけだ。", H_NORMAL, 3);
		com->text_insert("イシ", "僕なんかといいのかな、て思っちゃうんだ。", H_NORMAL, 3);
		com->text_insert("オオタ", "あー、わかるぜ、その気持ち！", H_NORMAL, 2);
		com->text_insert("イシ", "いや――いやいやいや！わからないでしょ、君には！", H_NORMAL, 3);
		com->text_insert("イシ", "君は僕とは正反対の人種じゃんか！", H_NORMAL, 3);
		com->text_insert("オオタ", "あー、今でもちょっとはあるぜ？自分に自信がない感じ。", H_NORMAL, 2);
		com->text_insert("イシ", "ははは……そんな、無理に共感してくれなくてもいいのに……", H_NORMAL, 3);
		com->text_insert("オオタ", "そんなことねえよ。俺だって昔はおんなじ感じだったしな。", H_NORMAL, 2);
		com->text_insert("オオタ", "賢介って昔の俺に似てるんだよな。", H_NORMAL, 2);
		com->text_insert("イシ", "え？――嘘、だよね？なんでそんな真面目な口調で――", H_NORMAL, 3);
		com->text_insert("オオタ", "もちろんどっちが良いか悪いかなんてないぜ？", H_NORMAL, 2);
		com->text_insert("イシ", "あ、きっと大げさに言ってるんだよね？", H_NORMAL, 3);
		com->text_insert("オオタ", "いや、俺はそんなつもりねーよ？嘘だってつかねえ。", H_NORMAL, 2);
		com->text_insert("イシ", "いやいやいやいや！！", H_NORMAL, 3);
	}
	play_bgm(bgm[0], 1, 20);
	finish(20);
	text_play();
}

EventMission7::EventMission7(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/EMa.ogg");
	volume_bgm(1);
	set_rare_rate(55, 35, 10, 0);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(30, 70, 0, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission7::play() {
	return after_text();
}
void EventMission7::text1() {
}
void EventMission7::text2() {
	if (com == NULL) {
		int x[10] = { -100,200,-100,-100,-100,-100,-100,500,300,-100 };
		com = new Text(x, -100);
		com->text_insert("ハンダ", "ごめんね、急に協力頼んじゃって。", H_NORMAL, 1);
		com->text_insert("イバラカワ", "ま、他にいないなら仕方ないでしょ。", H_NORMAL, 8);
		com->text_insert("ウチモト", "全然いいよー！これからも遠慮せずに頼んでよ！", H_NORMAL, 7);
		com->text_insert("イバラカワ", "私はなるべくやめてほしいけど……", H_NORMAL, 8);
		com->text_insert("ハンダ", "ごめん、みんな自分の都合があるのに……", H_NORMAL, 1);
		com->text_insert("イバラカワ", "今日は、なにもないからいいけど……これからはあまり期待しないで。", H_NORMAL, 8);
		com->text_insert("ウチモト", "あ、私は早く帰らないと！", H_NORMAL, 7);
		com->text_insert("ハンダ", "用事あったんだ。ほんとごめん！", H_NORMAL, 1);
		com->text_insert("ウチモト", "いいよー。早く帰ってプリティーキュアーズ観ないと！", H_NORMAL, 7);
		com->text_insert("ハンダ", "ん？――プリティー…………？", H_NORMAL, 1);
		com->text_insert("ウチモト", "うん、毎週観てるんだー！", H_NORMAL, 7);
		com->text_insert("ウチモト", "日曜にやってるんだけど、起きれないから録画してるの。今週はまだ観てないだ！", H_NORMAL, 7);
		com->text_insert("ハンダ", "うん、そのアニメ自体は知ってるけど…………", H_NORMAL, 1);
		com->text_insert("ハンダ", "（ねえ茨川さん、あれって小さい子が観るアニメだと思ってたんだけど結構中学生でも観るの？）", H_NORMAL, 1);
		com->text_insert("イバラカワ", "（知らないし。私も昔観てたけど今はもう観てない。）", H_NORMAL, 8);
		com->text_insert("イバラカワ", "（――あ、でもオタクのクラスメイトが観てたな――）", H_NORMAL, 8);
		com->text_insert("ハンダ", "内本さんってさ、他にもアニメ観てるの？", H_NORMAL, 1);
		com->text_insert("ウチモト", "他――？うーん、今毎週観てるのはプリキュアーズだけかなー。", H_NORMAL, 7);
		com->text_insert("ハンダ", "（今は、か。）", H_NORMAL, 1);
		com->text_insert("イバラカワ", "（ま、彼女のことだし、単純に中身が子供なだけかもよ。）", H_NORMAL, 8);
		com->text_insert("ハンダ", "（確かに、オレもそんな気がする。）", H_NORMAL, 1);
		com->text_insert("イバラカワ", "（どっちにせよ、詮索する必要もないでしょ。）", H_NORMAL, 8);
		com->text_insert("ハンダ", "（そうだよね。ごめん、ついちょっと気になっちゃった。）", H_NORMAL, 1);
	}
	play_bgm(bgm[0], 1, 23);
	finish(23);
	text_play();
}

EventMission8::EventMission8(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/EMb.ogg");
	volume_bgm(1);
	set_rare_rate(55, 35, 10, 0);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(30, 70, 0, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission8::play() {
	return after_text();
}
void EventMission8::text1() {
}
void EventMission8::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,-100,-100,-100,400,-100,-100,500 };
		com = new Text(x, -100);
		com->text_insert("ミヤトネ", "はあ……………………", H_NORMAL, 6);
		com->text_insert("トモチ", "ふ、君は運がいい。ため息を吐いたとき隣に私がいたのだから。", H_NORMAL, 9);
		com->text_insert("ミヤトネ", "え、な、なんですか！？", H_NORMAL, 6);
		com->text_insert("トモチ", "悩みがあるのだろう？私に話すといい。", H_NORMAL, 9);
		com->text_insert("ミヤトネ", "いや、そういうつもりじゃ――いや、悩みはあるけど――", H_NORMAL, 6);
		com->text_insert("トモチ", "――――――――", H_NORMAL, 9);
		com->text_insert("ミヤトネ", "――えっと、じゃあ一つだけ…………", H_NORMAL, 6);
		com->text_insert("トモチ", "何でもいいよ", H_NORMAL, 9);
		com->text_insert("ミヤトネ", "効率のいい勉強の仕方を教えてください！", H_NORMAL, 6);
		com->text_insert("トモチ", "一か月に必要な勉強量を一日で終わらせればいい！！", H_NORMAL, 9);
		com->text_insert("トモチ", "そうしたら次の月まで遊びまくれる！！以上！！", H_NORMAL, 9);
		com->text_insert("ミヤトネ", "答えになってないし、無茶すぎる！！", H_NORMAL, 6);
		com->text_insert("トモチ", "一日で一か月分の勉強をやればいいだけよ。", H_NORMAL, 9);
		com->text_insert("トモチ", "一日で一気にやったほうが効率的でしょ？", H_NORMAL, 9);
		com->text_insert("ミヤトネ", "その、一日で終わらせる方法が知りたいです…………", H_NORMAL, 6);
		com->text_insert("トモチ", "工夫することだ！！", H_NORMAL, 9);
		com->text_insert("ミヤトネ", "その詳細が知りたいんです！！", H_NORMAL, 6);
		com->text_insert("トモチ", "やればできる！！", H_NORMAL, 9);
		com->text_insert("ミヤトネ", "…………はあ……そもそも、私は別に勉強時間を減らしたいわけじゃないんです。", H_NORMAL, 6);
		com->text_insert("トモチ", "なんと。遊ぶ時間はいらないのか？", H_NORMAL, 9);
		com->text_insert("ミヤトネ", "遊ばないです。勉強しかしないんで。", H_NORMAL, 6);
		com->text_insert("トモチ", "なんと……………………！！", H_NORMAL, 9);
		com->text_insert("ミヤトネ", "はあ、やっぱり、才能ないのかなあ。", H_NORMAL, 6);
		com->text_insert("トモチ", "…………では、私が効率のいい遊び方を教えよう。", H_NORMAL, 9);
		com->text_insert("ミヤトネ", "遊び方？――いや、私は――", H_NORMAL, 6);
		com->text_insert("トモチ", "では行こう。私についてくるのだ！！", H_NORMAL, 9);
		com->text_insert("ミヤトネ", "勉強方法じゃないならいいです！！それに早く帰って勉強しないと！！", H_NORMAL, 6);
		com->text_insert("トモチ", "門限か！？ではこうして話してる時間が惜しい。", H_NORMAL, 9);
		com->text_insert("トモチ", "数分でいい、ついてくるのだ！！", H_NORMAL, 9);
		com->text_insert("ミヤトネ", "いや、いいですって！いいです！！――いいから！！", H_NORMAL, 6);
		com->text_insert("ミヤトネ", "やめろーーーー！！！", H_NORMAL, 6);
	}
	play_bgm(bgm[0], 1, 31);
	move(28, 9, 440, 5);
	move(30, 9, 700, 3);
	move(30, 6, 700, 3);
	finish(31);
	text_play();
}

EventMission9::EventMission9(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/EMc.ogg");
	volume_bgm(1);
	set_rare_rate(45, 40, 13, 2);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 100, 0, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission9::play() {
	return after_text();
}
void EventMission9::text1() {
}
void EventMission9::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,200,-100,300,-100,-100,400,-100 };
		com = new Text(x, -100);
		com->text_insert("イシ", "……………………", H_NORMAL, 3);
		com->text_insert("オワダ", "……ねえ、なんで伊志しゃべらないの？", H_NORMAL, 5);
		com->text_insert("イシ", "いや、その…………しゃべることないし……", H_NORMAL, 3);
		com->text_insert("オワダ", "なんで？台羅とかとはいつもしゃべってるじゃん。", H_NORMAL, 5);
		com->text_insert("イシ", "いや……あいつは、その…………", H_NORMAL, 3);
		com->text_insert("オワダ", "え、なに？", H_NORMAL, 5);
		com->text_insert("イシ", "え、あ…………", H_NORMAL, 3);
		com->text_insert("オワダ", "緊張してる？異性と話すの苦手？", H_NORMAL, 5);
		com->text_insert("イシ", "あ、そう。うん…………", H_NORMAL, 3);
		com->text_insert("イバラカワ", "――ふっ", H_NORMAL, 8);
		com->text_insert("イシ", "（笑われた――）", H_MOUSOU, 3);
		com->text_insert("オワダ", "大丈夫、緊張することないって。私も天子ちゃんもフレンドリーよ。", H_NORMAL, 5);
		com->text_insert("イバラカワ", "いや、私は別に――", H_NORMAL, 8);
		com->text_insert("イシ", "……………………", H_NORMAL, 3);
		com->text_insert("オワダ", "ほら、なにか話題振ってみてよ！", H_NORMAL, 5);
		com->text_insert("イシ", "……………………", H_NORMAL, 3);
		com->text_insert("オワダ", "ほら、なんでもいいから、学校のこととか！！", H_NORMAL, 5);
		com->text_insert("イシ", "………………P先生が", H_NORMAL, 3);
		com->text_insert("オワダ", "うん、うん！", H_NORMAL, 5);
		com->text_insert("イシ", "よく不良にいじめられてるけど、かわいそうだよね。", H_NORMAL, 3);
		com->text_insert("イバラカワ", "――ふふ", H_NORMAL, 8);
		com->text_insert("オワダ", "うん！私もそう思う！", H_NORMAL, 5);
		com->text_insert("オワダ", "でもさー、本人にも原因はあるのは明白だし、仕方ない気がする。伊志もそう思う？", H_NORMAL, 5);
		com->text_insert("イシ", "うん、本人にも原因は少なからずあるけど、一番は家庭環境じゃないかな？", H_NORMAL, 3);
		com->text_insert("オワダ", "え？――どういうこと？", H_NORMAL, 5);
		com->text_insert("イシ", "不良ってさ、家庭環境に問題があること多いんだよ。例えば両親も元ヤンだったり。", H_NORMAL, 3);
		com->text_insert("オワダ", "え、そっち――？", H_NORMAL, 5);
		com->text_insert("イシ", "そう思うと、彼らも家庭環境が良ければ、人をいじめるかわいそうな人間にならずに済んだかもしれないよね。", H_NORMAL, 3);
		com->text_insert("オワダ", "あ、かわいそうって、そっち――？", H_NORMAL, 5);
		com->text_insert("イバラカワ", "根暗…………ひねくれてる…………", H_NORMAL, 8);
		com->text_insert("イシ", "え、ええ！？確かにそうだけど、なぜ今の会話からそうなるの！？", H_NORMAL, 3);
		com->text_insert("イバラカワ", "正直会話も成立してなかったし……", H_NORMAL, 8);
		com->text_insert("イシ", "ええ！？なんで！？", H_NORMAL, 3);
		com->text_insert("オワダ", "――あっははははは！！伊志っておもしろすぎ！！", H_NORMAL, 5);
		com->text_insert("イシ", "また笑われた……………………", H_NORMAL, 3);
		com->text_insert("イバラカワ", "ま、これも個性だと思えばいいんじゃない？", H_NORMAL, 8);
		com->text_insert("イシ", "ふ、腑に落ちない……", H_NORMAL, 3);
	}
	play_bgm(bgm[0], 1, 37);
	finish(37);
	text_play();
}

EventMission10::EventMission10(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/EMa.ogg");
	volume_bgm(1);
	set_rare_rate(45, 40, 13, 2);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 100, 0, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission10::play() {
	return after_text();
}
void EventMission10::text1() {
}
void EventMission10::text2() {
	if (com == NULL) {
		int x[10] = { -100,100,-100,-100,-100,-100,300,-100,-100,400 };
		com = new Text(x, -100);
		com->text_insert("ハンダ", "二人とも、今日はありがとう。", H_NORMAL, 1);
		com->text_insert("ミヤトネ", "ど、どうも――", H_NORMAL, 6);
		com->text_insert("トモチ", "――前から言おうと思ってたんだけど、半田君ってさー", H_NORMAL, 9);
		com->text_insert("ハンダ", "え、な、なに？", H_NORMAL, 1);
		com->text_insert("トモチ", "普段みんなのリーダー的なポジションで振舞ってるけどさー", H_NORMAL, 9);
		com->text_insert("トモチ", "本当はそんなキャラじゃなかったりして", H_NORMAL, 9);
		com->text_insert("ミヤトネ", "――え、そ、そうなんですか？", H_NORMAL, 6);
		com->text_insert("ハンダ", "え、いや、そんなことないけど？どうしてそう思うんだ？", H_NORMAL, 1);
		com->text_insert("トモチ", "なんか時々、慣れてない感じがするんだよねー。", H_NORMAL, 9);
		com->text_insert("ハンダ", "いや、そんな砥用の直感で言われてもなあ。", H_NORMAL, 1);
		com->text_insert("トモチ", "そう、確かに、あくまで予想だったのよ。", H_NORMAL, 9);
		com->text_insert("トモチ", "でも今日、休み時間にあなたの教室の前通ったときに", H_NORMAL, 9);
		com->text_insert("トモチ", "教室の隅で一人だったから", H_NORMAL, 9);
		com->text_insert("ミヤトネ", "えー、それは意外です…………", H_NORMAL, 6);
		com->text_insert("トモチ", "そもそも普段のメンバー以外と話してるの見たことないんだよねー。", H_NORMAL, 9);
		com->text_insert("トモチ", "ふふふ、面白いよね。結構無理してリーダーしてるんじゃない？", H_NORMAL, 9);
		com->text_insert("ミヤトネ", "いや、それでも予想ですよ！一人でいたのもたまたまかもしれないですよね！？", H_NORMAL, 6);
		com->text_insert("ハンダ", "……………………", H_NORMAL, 1);
		com->text_insert("ミヤトネ", "あまり多くの人とつるまないタイプなだけですよね！？", H_NORMAL, 6);
		com->text_insert("ハンダ", "……………………", H_NORMAL, 1);
		com->text_insert("ミヤトネ", "ええ！？黙っちゃった！？", H_NORMAL, 6);
	}
	play_bgm(bgm[0], 1, 21);
	move(3, 9, 200, 5);
	finish(21);
	text_play();
}

EventMission11::EventMission11(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/EMd.ogg");
	volume_bgm(1);
	set_rare_rate(45, 40, 13, 2);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 100, 0, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission11::play() {
	return after_text();
}
void EventMission11::text1() {
}
void EventMission11::text2() {
	if (com == NULL) {
		int x[10] = { 100,-100,400,-100,-100,-100,-100,200,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("オオタ", "おつかれ、内本！！", H_NORMAL, 2);
		com->text_insert("ウチモト", "うん、おつかれー！", H_NORMAL, 7);
		com->text_insert("オオタ", "亮也もありがとな！", H_NORMAL, 2);
		com->text_insert("アカツキ", "……………………ああ。", H_NORMAL, 0);
		com->text_insert("オオタ", "……………………", H_NORMAL, 2);
		com->text_insert("オオタ", "（やっぱ亮也はすげえ無口だな）", H_MOUSOU, 2);
		com->text_insert("オオタ", "（裕斗は慣れてるみてえだが、俺はまだ違和感あるぜ。）", H_MOUSOU, 2);
		com->text_insert("ウチモト", "赤月君、今日調子よさそうだったね！！", H_NORMAL, 7);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
		com->text_insert("ウチモト", "いやー、私も悪くはなかったんだけどね、何か良いことあった？", H_NORMAL, 7);
		com->text_insert("アカツキ", "別に…………", H_NORMAL, 0);
		com->text_insert("ウチモト", "そっかー！そんな日ってあるよねー！", H_NORMAL, 7);
		com->text_insert("アカツキ", "…………", H_NORMAL, 0);
		com->text_insert("ウチモト", "え、ない？私だけかなー？あると思うんだけどなー。", H_NORMAL, 7);
		com->text_insert("オオタ", "……………………", H_NORMAL, 2);
		com->text_insert("オオタ", "（内本はすげえなあ…………）", H_MOUSOU, 2);
	}
	play_bgm(bgm[0], 1, 16);
	finish(16);
	text_play();
}

EventMission12::EventMission12(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/EMc.ogg");
	volume_bgm(1);
	set_rare_rate(45, 40, 13, 2);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 100, 0, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission12::play() {
	return after_text();
}
void EventMission12::text1() {
}
void EventMission12::text2() {
	if (com == NULL) {
		int x[10] = { -100,200,-100,-100,300,450,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("ハンダ", "うーん、いい作戦が思いついたんだけど、ちょっと問題あるなあ。", H_NORMAL, 1);
		com->text_insert("オワダ", "へー、どんな作戦？", H_NORMAL, 5);
		com->text_insert("ハンダ", "授業中に誰もいない部屋に罠を設置して放課後そこで戦うんだ。", H_NORMAL, 1);
		com->text_insert("オワダ", "なかなかずるいね……", H_NORMAL, 5);
		com->text_insert("タイラ", "となると、どうやって授業中に抜け出すかってとこだな。", H_NORMAL, 4);
		com->text_insert("ハンダ", "そうなんだ。トイレって言っても、罠仕掛けるのに時間かかりそうだし……", H_NORMAL, 1);
		com->text_insert("オワダ", "じゃあ保健室に行くって言うしかないんじゃない？", H_NORMAL, 5);
		com->text_insert("ハンダ", "罠設置にどれだけ時間がかかるか……５０分以内にできるかな……", H_NORMAL, 1);
		com->text_insert("オワダ", "一回の授業中にできないなら無理でしょ。", H_NORMAL, 5);
		com->text_insert("タイラ", "分担すればいい。一限に半田、二限に俺って感じでな。", H_NORMAL, 4);
		com->text_insert("オワダ", "確かに。でも私はパスで。授業抜けるのはいや。", H_NORMAL, 5);
		com->text_insert("ハンダ", "手際よく一人でできる人じゃないとだめだ。やっぱりこの作戦は無理か。", H_NORMAL, 1);
		com->text_insert("タイラ", "自分で言うのもなんだが、俺じゃダメか？", H_NORMAL, 4);
		com->text_insert("ハンダ", "え、いいのか？授業受けれないぞ？", H_NORMAL, 1);
		com->text_insert("タイラ", "全然いいさ。むしろやりたいな。", H_NORMAL, 4);
		com->text_insert("オワダ", "マジ？意外…………", H_NORMAL, 5);
		com->text_insert("タイラ", "放課後前の５，６限で誰もいなくて、部活でも使われない教室を調べよう。", H_NORMAL, 4);
		com->text_insert("タイラ", "あ、半田は大丈夫か？無理なら砥用にでも頼むか。", H_NORMAL, 4);
		com->text_insert("ハンダ", "いやいや、いけるいける！むしろ俺もやりたい！", H_NORMAL, 1);
		com->text_insert("オワダ", "はあ……止めないけどさ、あんたらそれでいいの？", H_NORMAL, 5);
		com->text_insert("ハンダ", "いやー、授業抜け出すのってやってみたかったんだよなあ。", H_NORMAL, 1);
		com->text_insert("タイラ", "他のクラスの授業風景をのぞき見してみたいな。", H_NORMAL, 4);
		com->text_insert("ハンダ", "あ、俺も！それもやっておこう！", H_NORMAL, 1);
		com->text_insert("オワダ", "あれ！？なんか目的変わってない！？", H_NORMAL, 5);
	}
	play_bgm(bgm[0], 1, 24);
	finish(24);
	text_play();
}

EventMission13::EventMission13(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/EMe.ogg");
	volume_bgm(1);
	set_rare_rate(35, 45, 15, 5);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 70, 30, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission13::play() {
	return after_text();
}
void EventMission13::text1() {
}
void EventMission13::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,200,-100,-100,-100,-100,-100,400 };
		com = new Text(x, -100);
		com->text_insert("トモチ", "伊志君ってさー、異性に興味とかあるの？", H_NORMAL, 9);
		com->text_insert("イシ", "（また始まった…………）", H_MOUSOU, 3);
		com->text_insert("イシ", "いや、別にそんなに。", H_NORMAL, 3);
		com->text_insert("トモチ", "あー、興味あるやつだこれ。", H_NORMAL, 9);
		com->text_insert("イシ", "ないって！", H_NORMAL, 3);
		com->text_insert("トモチ", "それはねー、典型的な嘘パターンだよ。そこは「多少」とか言うのがいいよ。", H_NORMAL, 9);
		com->text_insert("イシ", "う、うざい…………", H_NORMAL, 3);
		com->text_insert("トモチ", "あー傷ついたわー。これって好きな女子に意地悪言っちゃうやつ？", H_NORMAL, 9);
		com->text_insert("イシ", "まさか。ありえないよ。", H_NORMAL, 3);
		com->text_insert("トモチ", "おー、華麗に受け流すねー。", H_NORMAL, 9);
		com->text_insert("イシ", "慣れたんだよ。君の言動にはもう惑わされない。", H_NORMAL, 3);
		com->text_insert("トモチ", "ちょっといつもより早口になってるけどね。", H_NORMAL, 9);
		com->text_insert("イシ", "え！？…………あ", H_NORMAL, 3);
		com->text_insert("トモチ", "あっはっははは！", H_NORMAL, 9);
		com->text_insert("イシ", "はあ……そりゃあさ、異性に全く興味がないわけじゃないよ。", H_NORMAL, 3);
		com->text_insert("イシ", "大抵の人はそうでしょ？", H_NORMAL, 3);
		com->text_insert("トモチ", "そうだよ。だから隠す必要なかったよね。伊志君が勝手に自滅しただけだよね？", H_NORMAL, 9);
		com->text_insert("イシ", "あっはっは……そうだよ、そうだよ。", H_NORMAL, 3);
		com->text_insert("イシ", "僕だってさ、T先生みたいな大人な女性に憧れるし。", H_NORMAL, 3);
		com->text_insert("トモチ", "っ――――", H_NORMAL, 9);
		com->text_insert("トモチ", "――――うそ――", H_NORMAL, 9);
		com->text_insert("イシ", "う、だめか。騙されないか。", H_NORMAL, 3);
		com->text_insert("トモチ", "――あ、ははは！無謀すぎ！", H_NORMAL, 9);
		com->text_insert("イシ", "くそー、いつか見返してやる…………", H_NORMAL, 3);
		com->text_insert("トモチ", "あははは――――ふぅ", H_NORMAL, 9);
	}
	play_bgm(bgm[0], 1, 25);
	finish(25);
	text_play();
}

EventMission14::EventMission14(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/EMb.ogg");
	volume_bgm(1);
	set_rare_rate(35, 45, 15, 5);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 70, 30, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission14::play() {
	return after_text();
}
void EventMission14::text1() {
}
void EventMission14::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,-100,300,-100,200,-100,400,-100 };
		com = new Text(x, -100);
		com->text_insert("イバラカワ", "ああもう、オオタのやつ……", H_NORMAL, 8);
		com->text_insert("ミヤトネ", "太田君がどうかしたんですか？", H_NORMAL, 6);
		com->text_insert("イバラカワ", "いっつも私にからんでくるのよ。まったく、うっとおしい。", H_NORMAL, 8);
		com->text_insert("ミヤトネ", "はは……そうなんですか……", H_NORMAL, 6);
		com->text_insert("イバラカワ", "――なに", H_NORMAL, 8);
		com->text_insert("ミヤトネ", "え、いや、仲いいんだなって思って――", H_NORMAL, 6);
		com->text_insert("イバラカワ", "よくない！！", H_NORMAL, 8);
		com->text_insert("タイラ", "ふ、茨川はいつも太田の話ばっかだよな。", H_NORMAL, 4);
		com->text_insert("イバラカワ", "は、はあ！？そんなことないし！！", H_NORMAL, 8);
		com->text_insert("タイラ", "実際、太田の話をしてる時が一番テンション高いしな。", H_NORMAL, 4);
		com->text_insert("イバラカワ", "い、いみ分かんない！ばっかじゃないの！？", H_NORMAL, 8);
		com->text_insert("タイラ", "ほらな、やっぱりまんざらでもないように見える。", H_NORMAL, 4);
		com->text_insert("ミヤトネ", "あはは……", H_NORMAL, 6);
		com->text_insert("イバラカワ", "ああもう、うざいうざい！！あんた、太田の話はしないで！", H_NORMAL, 8);
		com->text_insert("タイラ", "いいが……自分から言い出したんだろ……", H_NORMAL, 4);
		com->text_insert("イバラカワ", "ちょっと愚痴を吐いただけ。", H_NORMAL, 8);
		com->text_insert("タイラ", "ふっ", H_NORMAL, 4);
		com->text_insert("イバラカワ", "――――もう、「ふっ」て言うのも禁止！！", H_NORMAL, 8);
		com->text_insert("タイラ", "わかった。", H_NORMAL, 4);
		com->text_insert("イバラカワ", "……………………", H_NORMAL, 8);
		com->text_insert("タイラ", "……………………", H_NORMAL, 4);
		com->text_insert("イバラカワ", "ああ、だめ！！黙るのも禁止！！", H_NORMAL, 8);
		com->text_insert("タイラ", "なぜそんな親の仇のように俺から自由を奪う――", H_NORMAL, 4);
	}
	play_bgm(bgm[0], 1, 23);
	finish(23);
	text_play();
}

EventMission15::EventMission15(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/EMd.ogg");
	volume_bgm(1);
	set_rare_rate(35, 45, 15, 5);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 70, 30, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission15::play() {
	return after_text();
}
void EventMission15::text1() {
}
void EventMission15::text2() {
	if (com == NULL) {
		int x[10] = { 300,400,-100,-100,-100,-100,-100,-100,-100,500 };
		com = new Text(x, -100);
		com->text_insert("トモチ", "ねーねー、赤月ってなんでそんなにしゃべんないの？", H_NORMAL, 9);
		com->text_insert("ハンダ", "ああ、昔はもう少し口数が多かったけどだんだん減ってきたんだ。", H_NORMAL, 1);
		com->text_insert("トモチ", "へーそうなんだー、ねーどうしてー？どうしてどうしてー？", H_NORMAL, 9);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
		com->text_insert("ハンダ", "別にいいだろ、理由なんて。そういうやつなんだよ。", H_NORMAL, 1);
		com->text_insert("トモチ", "人見知り？ではないよね、半田ともしゃべらないんだし。", H_NORMAL, 9);
		com->text_insert("トモチ", "もしかしてしゃべらない俺カッケー的な奴？中二病？", H_NORMAL, 9);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
		com->text_insert("トモチ", "あれー、ひょっとして怒った？", H_NORMAL, 9);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
		com->text_insert("ハンダ", "いや、怒らないよ。怒ってるの見たことないし。", H_NORMAL, 1);
		com->text_insert("トモチ", "ふーん、そうなのかなー？そうなのかなー？", H_NORMAL, 9);
		com->text_insert("ハンダ", "な、別に怒ってなんかないよな？", H_NORMAL, 1);
		com->text_insert("アカツキ", "……………………", H_NORMAL, 0);
		com->text_insert("トモチ", "……………………", H_NORMAL, 9);
		com->text_insert("ハンダ", "……あ、あれ………………？", H_NORMAL, 1);
	}
	play_bgm(bgm[0], 1, 16);
	finish(16);
	text_play();
}

EventMission16::EventMission16(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/EMc.ogg");
	volume_bgm(1);
	set_rare_rate(35, 45, 15, 5);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 70, 30, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission16::play() {
	return after_text();
}
void EventMission16::text1() {
}
void EventMission16::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,200,-100,-100,300,450,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("オワダ", "太田！！明日は絶対持ってきてよね！！", H_NORMAL, 5);
		com->text_insert("オワダ", "三者面談の希望時間届よ！出してないの太田だけよ！！", H_NORMAL, 5);
		com->text_insert("オオタ", "あー、わかってる、わかってるって！！", H_NORMAL, 2);
		com->text_insert("オワダ", "それといっつも靴のかかと踏んでる！ほら今も片足！！", H_NORMAL, 5);
		com->text_insert("オオタ", "いや、今直そうとしてるとこだって！！", H_NORMAL, 2);
		com->text_insert("オワダ", "ちゃんと靴紐ほどいてから穿いたほうがいいんじゃない？", H_NORMAL, 5);
		com->text_insert("オオタ", "いいだろ、俺の自由だ！！", H_NORMAL, 2);
		com->text_insert("オワダ", "あと――", H_NORMAL, 5);
		com->text_insert("オオタ", "もう勘弁してくれ！！", H_NORMAL, 2);
		com->text_insert("オワダ", "あ、待て！！", H_NORMAL, 5);
		com->text_insert("オワダ", "――ったく", H_NORMAL, 5);
		com->text_insert("ウチモト", "ははは……、小和田さんは太田君に厳しいですね。", H_NORMAL, 6);
		com->text_insert("オワダ", "あいつはいい加減すぎるのよ。気になって仕方ないわ。", H_NORMAL, 5);
		com->text_insert("ウチモト", "でもなんだかんだいって太田君って、ちゃんとするんですよね。", H_NORMAL, 6);
		com->text_insert("オワダ", "私が言わなきゃ怪しいけどね……", H_NORMAL, 5);
	}
	play_bgm(bgm[0], 1, 14);
	move(9, 2, -300, 7);
	move(10, 5, 200, 6);
	finish(15);
	text_play();
}

EventMission17::EventMission17(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/EMa.ogg");
	volume_bgm(1);
	set_rare_rate(22, 60, 15, 8);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 30, 70, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission17::play() {
	return after_text();
}
void EventMission17::text1() {
}
void EventMission17::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,150,-100,300,-100,-100,450,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("オオタ", "悪いな、内本。手伝ってもらって。裕斗が誰か呼べってうるさいんだ。", H_NORMAL, 2);
		com->text_insert("ウチモト", "全然いいよ！！太田の頼みならなんだって聞くよ！！", H_NORMAL, 7);
		com->text_insert("オオタ", "へ、なんだよ。そんな特別扱いみてえな……照れるぜ。でも俺には心に決めた人が――", H_NORMAL, 2);
		com->text_insert("ウチモト", "もちろんだよ！太田は私と同じ「頭悪い族」だからね！", H_NORMAL, 7);
		com->text_insert("オオタ", "え？", H_NORMAL, 2);
		com->text_insert("タイラ", "ふっ", H_NORMAL, 4);
		com->text_insert("ウチモト", "太田は一番の仲間だと思ってるよ！テストも１７０位くらいでしょ？", H_NORMAL, 7);
		com->text_insert("オオタ", "そこまで悪くねえよ！１５０位くらいだ！", H_NORMAL, 2);
		com->text_insert("ウチモト", "えー？そうなのー？じゃあ私より良いじゃん。", H_NORMAL, 7);
		com->text_insert("オオタ", "俺は結構勉強してるんだぜ。一緒にしないでほしいぜ。", H_NORMAL, 2);
		com->text_insert("ウチモト", "私だってやってるよ！", H_NORMAL, 7);
		com->text_insert("オオタ", "なに！？で、でも俺の方がやってるはずだ！！", H_NORMAL, 2);
		com->text_insert("ウチモト", "いやいや、こうみえて私、かなりやってるよー？", H_NORMAL, 7);
		com->text_insert("タイラ", "ちなみに二人は先週の中間テストは何位だったんだ？", H_NORMAL, 4);
		com->text_insert("オオタ", "……………………", H_NORMAL, 2);
		com->text_insert("ウチモト", "……………………", H_NORMAL, 7);
		com->text_insert("タイラ", "二人とも自信ないのかよ……", H_NORMAL, 4);
	}
	play_bgm(bgm[0], 1, 17);
	finish(17);
	text_play();
}

EventMission18::EventMission18(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/EMe.ogg");
	volume_bgm(1);
	set_rare_rate(22, 60, 15, 8);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 30, 70, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission18::play() {
	return after_text();
}
void EventMission18::text1() {
}
void EventMission18::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,200,-100,-100,-100,300,400,-100 };
		com = new Text(x, -100);
		com->text_insert("イシ", "いやー、流石内本さん。動きがスピーディだね。", H_NORMAL, 3);
		com->text_insert("ウチモト", "えへへ、ありがとう！", H_NORMAL, 7);
		com->text_insert("イシ", "内本さんなら時間も越えられそうだよ。", H_NORMAL, 3);
		com->text_insert("ウチモト", "え、どういうこと！？タイムスリップしちゃうの！？", H_NORMAL, 7);
		com->text_insert("イシ", "相対性理論から考えると理論上不可能じゃないんだ。", H_NORMAL, 3);
		com->text_insert("ウチモト", "そー、た？", H_NORMAL, 7);
		com->text_insert("イバラカワ", "気にしないで。彼、今日数学の授業で先生の雑談を聞いてからあの調子なのよ。", H_NORMAL, 8);
		com->text_insert("イシ", "あれは凄い理論なんだ。別に運動方程式が間違ってるわけじゃないんだけど、あれはまた新しい理論で――", H_NORMAL, 3);
		com->text_insert("ウチモト", "伊志君は間違ったこと言ってるってこと？", H_NORMAL, 7);
		com->text_insert("イバラカワ", "そうじゃないけど、授業で聞いただけの浅い知識で語ってるの。", H_NORMAL, 8);
		com->text_insert("ウチモト", "はーん……私、よくわかんないや。", H_NORMAL, 7);
		com->text_insert("イバラカワ", "私だってわかんないわ。", H_NORMAL, 8);
	}
	play_bgm(bgm[0], 1, 12);
	finish(12);
	text_play();
}

EventMission19::EventMission19(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/EMa.ogg");
	volume_bgm(1);
	set_rare_rate(22, 60, 15, 8);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 30, 70, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission19::play() {
	return after_text();
}
void EventMission19::text1() {
}
void EventMission19::text2() {
	if (com == NULL) {
		int x[10] = { 100,200,400,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("ハンダ", "どうしたんだよ、太田。なんかテンション低いな？", H_NORMAL, 1);
		com->text_insert("オオタ", "今日の給食のプリンを一個しか食えなかったからだ……", H_NORMAL, 2);
		com->text_insert("ハンダ", "ああ、そっちのクラスも余ったんだ。", H_NORMAL, 1);
		com->text_insert("オオタ", "気持ちでは負けてねえんだよ。じゃんけんじゃなくて面接なら俺のプリンだった。", H_NORMAL, 2);
		com->text_insert("ハンダ", "面接で決めるなんて聞いたことないな……", H_NORMAL, 1);
		com->text_insert("オオタ", "なんだよ、裕斗はプリン勝ち取れたのか？", H_NORMAL, 2);
		com->text_insert("ハンダ", "俺はそもそも参加してない。一個で十分だ。", H_NORMAL, 1);
		com->text_insert("オオタ", "はあ、大人ぶりやがって。本当は欲しいくせによお。", H_NORMAL, 2);
		com->text_insert("ハンダ", "いや、そんなこと…………", H_NORMAL, 1);
		com->text_insert("オオタ", "はっ、待て、もしかしてそれが普通なのか…………？", H_NORMAL, 2);
		com->text_insert("オオタ", "考えてみれば、小学生のときより参加する人が減ってる気がするぜ……", H_NORMAL, 2);
		com->text_insert("オオタ", "そうだよな、裕斗だけじゃない。きっと亮也だって参加してねえよな？", H_NORMAL, 2);
		com->text_insert("アカツキ", "ああ。", H_NORMAL, 0);
		com->text_insert("オオタ", "へっ、俺も大人にならねえとな。今度からはほどほどにするぜ。", H_NORMAL, 2);
		com->text_insert("ハンダ", "別に気にしなくていいと思うけどな。", H_NORMAL, 1);
		com->text_insert("ハンダ", "俺の場合は、この前、杏子が俺にプリン作ってくれたんだ。", H_NORMAL, 1);
		com->text_insert("ハンダ", "だから、正直給食のプリンが魅力的に感じなくなったんだ。", H_NORMAL, 1);
		com->text_insert("オオタ", "の、惚気てんじゃねえ！！", H_NORMAL, 2);
	}
	play_bgm(bgm[0], 1, 18);
	finish(18);
	text_play();
}

EventMission20::EventMission20(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm確保
	bgm[0] = LoadSoundMem("bgm/EMd.ogg");
	volume_bgm(1);
	set_rare_rate(22, 60, 15, 8);//報酬のカードレア割合
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//先生一人が持つカード枚数
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 30, 70, 0);//先生のカードのレア度
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission20::play() {
	return after_text();
}
void EventMission20::text1() {
}
void EventMission20::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,200,300,450,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("オワダ", "ねえ、伊志はその髪、床屋で切ってるの？", H_NORMAL, 5);
		com->text_insert("イシ", "え、そ、そうだけど…………", H_NORMAL, 3);
		com->text_insert("オワダ", "伊志は顔いいんだから、もうちょっといいとこで散髪しなよ。", H_NORMAL, 5);
		com->text_insert("イシ", "い、いや別に興味ないし……", H_NORMAL, 3);
		com->text_insert("オワダ", "興味とかじゃなくてさ、もっとこう、オーラを出すために？", H_NORMAL, 5);
		com->text_insert("イシ", "いや、いいよ、そういうの……", H_NORMAL, 3);
		com->text_insert("オワダ", "きっと持ち物だってシンプルで暗いのばっかでしょ？", H_NORMAL, 5);
		com->text_insert("オワダ", "もうちょっとおしゃれしてみたら？世界変わるよ？", H_NORMAL, 5);
		com->text_insert("イシ", "いいよ、変わらなくて…………", H_NORMAL, 3);
		com->text_insert("オワダ", "ほら、台羅は割とちゃんとしてるじゃない？", H_NORMAL, 5);
		com->text_insert("タイラ", "そうだな……わりと、な。", H_NORMAL, 4);
		com->text_insert("イシ", "ちゃんとしてるってなんだよ！まるで僕がちゃんとしてないみたいに！！", H_NORMAL, 3);
		com->text_insert("オワダ", "ごめんごめん。そういうつもりじゃないの。", H_NORMAL, 5);
		com->text_insert("イシ", "だいたい、不潔じゃなければなんだっていいだろ！？僕の自由だ！！", H_NORMAL, 3);
		com->text_insert("タイラ", "まあまあ、落ち着けって。", H_NORMAL, 4);
		com->text_insert("オワダ", "もちろん、自由だよ。ごめんね、ちょっとしたアドバイスよ。", H_NORMAL, 5);
		com->text_insert("タイラ", "おしゃれは気が向いたときにすればいいんじゃないか。", H_NORMAL, 4);
		com->text_insert("イシ", "発言内容に怒ってるんじゃない！", H_NORMAL, 3);
		com->text_insert("イシ", "ただ僕は、おしゃれしてる人からの謎の上から目線が死ぬほど嫌いなんだ！！", H_NORMAL, 3);
		com->text_insert("タイラ", "……………………", H_NORMAL, 4);
		com->text_insert("オワダ", "……………………っあ――", H_NORMAL, 5);
		com->text_insert("オワダ", "――ごめん、なんか。いろいろと、ごめん。", H_NORMAL, 5);
	}
	play_bgm(bgm[0], 1, 22);
	finish(22);
	text_play();
}