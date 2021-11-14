#include"battle.h"
#include"DxLib.h"
#include"control.h"

#define FINISH_COUNT 180
#define MUSIC_START 3//何ラウンド目から音楽始まるか

int BattleField::test(Deck after_deck[3]) {
	if (!load_flag && GetASyncLoadNum() == 0) {
		ChangeVolumeSoundMem(255 * 55 / 100, trigger_handle1);
		ChangeVolumeSoundMem(255 * 60 / 100, trigger_handle2);
		ChangeVolumeSoundMem(255 * 35 / 100, target_sound);
		ChangeVolumeSoundMem(255 * 55 / 100, target_cancel);
		for (int i = 0; i < 18; i++) { ChangeVolumeSoundMem(255 * 60 / 100, skill_sound_handle[i]); }
		ChangeVolumeSoundMem(255 * 45 / 100, skill_sound_handle[0]);
		ChangeVolumeSoundMem(255 * 50 / 100, skill_sound_handle[7]);
		ChangeVolumeSoundMem(255 * 100 / 100, skill_sound_handle[14]);
		ChangeVolumeSoundMem(255 * 60 / 100, music);
		ChangeVolumeSoundMem(255 * 65 / 100, bgm);
		load_flag = true;
	}
	play_music(false);
	target_decide();//ターゲッティング
	target_hpzero();//HPがゼロになったらターゲット解除
	queue_trigger();//カード発動による一時停止への切り替え
	if (flag) {//バトル進行中
		for (int i = 0; i < MAX_WITH_BATTLE; i++) {
			if (student[i].get_kind()) {
				student[i].skill_trigger(student, teacher, target, skill_sound_handle);//スキルの効果適用
			}
			if (teacher[i].get_kind()) {
				teacher[i].skill_trigger(student, teacher, -1, skill_sound_handle);//スキルの効果適用
			}
			target_hpzero();//HPがゼロになったらターゲット解除
		}
		for (int i = 0; i < MAX_WITH_BATTLE; i++) {
			if (student[i].get_kind()) {
				student[i].seek_hp();//phpをhpに近づける
				student[i].effect_counting();//エフェクト
				student[i].state_cnt();//状態異常などのカウント
				student[i].counting(wait_num);//待機
			}
			if (teacher[i].get_kind()) {
				teacher[i].seek_hp();
				teacher[i].effect_counting();
				teacher[i].state_cnt();
				teacher[i].counting(wait_num);//待機
			}
		}
	}
	else {//一時停止中

	}
	//描画フェーズ
	DrawRotaGraph(0, 0, 1, 0, haikei, TRUE);
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		int k;
		if (student[i].get_kind()) {
			student[i].picture();
			k = student[i].get_effect();
			student[i].picture_effect(effect_handle[0][k], effect_handle[1][k], effect_handle[2][k], effect_handle[3][k]);
			student[i].picture_damage(flag);
		}
		if (teacher[i].get_kind()) {
			teacher[i].picture();
			k = teacher[i].get_effect();
			teacher[i].picture_effect(effect_handle[0][k], effect_handle[1][k], effect_handle[2][k], effect_handle[3][k]);
			teacher[i].picture_damage(flag);
		}
	}
	picture_target();
	picture_bar();
	//DrawFormatString(0, 0, GetColor(255, 255, 255), "next = %d", student[0].next);
	//DrawFormatString(100, 0, GetColor(255, 255, 255), "now = %d", student[0].now);
	DrawLine(0, 232, 640, 232, GetColor(255, 255, 0));
	DrawLine(0, 233, 640, 233, GetColor(0, 0, 0));
	DrawLine(0, 234, 640, 234, GetColor(255, 255, 0));
	picture_card_trigger();//カード発動画面 終了したらflagがtrueに
	return result(after_deck);
}

const int SKILL_SOUND_SUM = 18;
//コンストラクタ
BattleField::BattleField(Friend* st, Enemy* te) {
	font_handle = CreateFontToHandle(NULL, 75, 7);//勝ち負けとかのフォント
	flag = true;//デバッグ用にtrueスタート
	target = -1;
	hand = -1;
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		student[i] = st[i];
		teacher[i] = te[i];
	}
	round = 1;//ラウンド1からスタート
	//背景のロード
	haikei = LoadGraph("battle_picture/背景.png");
	//カード発動画面の音
	trigger_handle1 = LoadSoundMem("sound/カード発動1.wav");
	//ChangeVolumeSoundMem(255 * 40 / 100, trigger_handle1);
	trigger_handle2 = LoadSoundMem("sound/カード発動2.wav");
	//ChangeVolumeSoundMem(255 * 40 / 100, trigger_handle2);
	target_sound = LoadSoundMem("sound/ターゲッティング.wav");
	//ChangeVolumeSoundMem(255 * 20 / 100, target_sound);
	target_cancel = LoadSoundMem("sound/ターゲッティングキャンセル.wav");
	//ChangeVolumeSoundMem(255 * 50 / 100, target_cancel);
	//バフデバフ状態異常のアイコン
	icon[0] = LoadGraph("battle_picture/攻撃力上昇.png");
	icon[1] = LoadGraph("battle_picture/攻撃力下降.png");
	icon[2] = LoadGraph("battle_picture/防御力上昇.png");
	icon[3] = LoadGraph("battle_picture/防御力下降.png");
	icon[4] = LoadGraph("battle_picture/スピード上昇.png");
	icon[5] = LoadGraph("battle_picture/スピード下降.png");
	icon[6] = LoadGraph("battle_picture/毒.png");
	icon[7] = LoadGraph("battle_picture/回復.png");
	icon[8] = LoadGraph("battle_picture/麻痺.png");
	icon[9] = LoadGraph("battle_picture/回避.png");
	icon[10] = LoadGraph("battle_picture/挑発.png");
	icon[11] = LoadGraph("battle_picture/ステルス.png");
	icon[12] = LoadGraph("battle_picture/カウンター.png");
	icon[13] = LoadGraph("battle_picture/我慢.png");
	icon[14] = LoadGraph("battle_picture/予防接種.png");
	//スキル発動の効果音
	skill_sound_handle = new int[SKILL_SOUND_SUM];
	skill_sound_handle[0] = LoadSoundMem("sound/斬撃.wav");
	skill_sound_handle[1] = LoadSoundMem("sound/攻撃力アップ.wav");
	skill_sound_handle[2] = LoadSoundMem("sound/攻撃力ダウン.wav");
	skill_sound_handle[3] = LoadSoundMem("sound/防御力アップ.wav");
	skill_sound_handle[4] = LoadSoundMem("sound/防御力ダウン.wav");
	skill_sound_handle[5] = LoadSoundMem("sound/スピードアップ.wav");
	skill_sound_handle[6] = LoadSoundMem("sound/スピードダウン.wav");
	skill_sound_handle[7] = LoadSoundMem("sound/麻痺.wav");
	skill_sound_handle[8] = LoadSoundMem("sound/毒.wav");
	skill_sound_handle[9] = LoadSoundMem("sound/回避.wav");
	skill_sound_handle[10] = LoadSoundMem("sound/回避付与.wav");
	skill_sound_handle[11] = LoadSoundMem("sound/カウンター.wav");
	skill_sound_handle[12] = LoadSoundMem("sound/カウンター付与.wav");
	skill_sound_handle[13] = LoadSoundMem("sound/挑発付与.wav");
	skill_sound_handle[14] = LoadSoundMem("sound/ステルス付与.wav");
	skill_sound_handle[15] = LoadSoundMem("sound/状態異常回復.wav");
	skill_sound_handle[16] = LoadSoundMem("sound/回復.wav");
	skill_sound_handle[17] = LoadSoundMem("sound/我慢付与.wav");
	for (int i = 0; i < 18; i++) { ChangeVolumeSoundMem(255 * 50 / 100, skill_sound_handle[i]); }
	/*ChangeVolumeSoundMem(255 * 40 / 100, skill_sound_handle[0]);
	ChangeVolumeSoundMem(255 * 45 / 100, skill_sound_handle[7]);
	ChangeVolumeSoundMem(255 * 100 / 100, skill_sound_handle[14]);*/
	//エフェクトのアニメーション
	for (int i = 0; i < EFFECT_NUM; i++)effect_handle[i] = new int[17];
	effect_handle[0][0] = LoadGraph("battle_picture/オレンジ1.png");
	effect_handle[1][0] = LoadGraph("battle_picture/オレンジ2.png");
	effect_handle[2][0] = LoadGraph("battle_picture/オレンジ3.png");
	effect_handle[3][0] = LoadGraph("battle_picture/オレンジ4.png");
	effect_handle[0][1] = LoadGraph("battle_picture/攻撃力上昇1.png");
	effect_handle[1][1] = LoadGraph("battle_picture/攻撃力上昇2.png");
	effect_handle[2][1] = LoadGraph("battle_picture/攻撃力上昇3.png");
	effect_handle[3][1] = LoadGraph("battle_picture/攻撃力上昇4.png");
	effect_handle[0][2] = LoadGraph("battle_picture/攻撃力減少1.png");
	effect_handle[1][2] = LoadGraph("battle_picture/攻撃力減少2.png");
	effect_handle[2][2] = LoadGraph("battle_picture/攻撃力減少3.png");
	effect_handle[3][2] = LoadGraph("battle_picture/攻撃力減少4.png");
	effect_handle[0][3] = LoadGraph("battle_picture/防御力上昇1.png");
	effect_handle[1][3] = LoadGraph("battle_picture/防御力上昇2.png");
	effect_handle[2][3] = LoadGraph("battle_picture/防御力上昇3.png");
	effect_handle[3][3] = LoadGraph("battle_picture/防御力上昇4.png");
	effect_handle[0][4] = LoadGraph("battle_picture/防御力減少1.png");
	effect_handle[1][4] = LoadGraph("battle_picture/防御力減少2.png");
	effect_handle[2][4] = LoadGraph("battle_picture/防御力減少3.png");
	effect_handle[3][4] = LoadGraph("battle_picture/防御力減少4.png");
	effect_handle[0][5] = LoadGraph("battle_picture/スピード上昇1.png");
	effect_handle[1][5] = LoadGraph("battle_picture/スピード上昇2.png");
	effect_handle[2][5] = LoadGraph("battle_picture/スピード上昇3.png");
	effect_handle[3][5] = LoadGraph("battle_picture/スピード上昇4.png");
	effect_handle[0][6] = LoadGraph("battle_picture/スピード減少1.png");
	effect_handle[1][6] = LoadGraph("battle_picture/スピード減少2.png");
	effect_handle[2][6] = LoadGraph("battle_picture/スピード減少3.png");
	effect_handle[3][6] = LoadGraph("battle_picture/スピード減少4.png");
	effect_handle[0][7] = LoadGraph("battle_picture/毒1.png");
	effect_handle[1][7] = LoadGraph("battle_picture/毒2.png");
	effect_handle[2][7] = LoadGraph("battle_picture/毒3.png");
	effect_handle[3][7] = LoadGraph("battle_picture/毒4.png");
	effect_handle[0][8] = LoadGraph("battle_picture/回避1.png");
	effect_handle[1][8] = LoadGraph("battle_picture/回避2.png");
	effect_handle[2][8] = LoadGraph("battle_picture/回避3.png");
	effect_handle[3][8] = LoadGraph("battle_picture/回避4.png");
	effect_handle[0][9] = LoadGraph("battle_picture/カウンターヒット1.png");
	effect_handle[1][9] = LoadGraph("battle_picture/カウンターヒット2.png");
	effect_handle[2][9] = LoadGraph("battle_picture/カウンターヒット3.png");
	effect_handle[3][9] = LoadGraph("battle_picture/カウンターヒット4.png");
	effect_handle[0][10] = LoadGraph("battle_picture/カウンター1.png");
	effect_handle[1][10] = LoadGraph("battle_picture/カウンター2.png");
	effect_handle[2][10] = LoadGraph("battle_picture/カウンター3.png");
	effect_handle[3][10] = LoadGraph("battle_picture/カウンター4.png");
	effect_handle[0][11] = LoadGraph("battle_picture/挑発1.png");
	effect_handle[1][11] = LoadGraph("battle_picture/挑発2.png");
	effect_handle[2][11] = LoadGraph("battle_picture/挑発3.png");
	effect_handle[3][11] = LoadGraph("battle_picture/挑発4.png");
	effect_handle[0][12] = LoadGraph("battle_picture/ステルス1.png");
	effect_handle[1][12] = LoadGraph("battle_picture/ステルス2.png");
	effect_handle[2][12] = LoadGraph("battle_picture/ステルス3.png");
	effect_handle[3][12] = LoadGraph("battle_picture/ステルス4.png");
	effect_handle[0][13] = LoadGraph("battle_picture/我慢1.png");
	effect_handle[1][13] = LoadGraph("battle_picture/我慢2.png");
	effect_handle[2][13] = LoadGraph("battle_picture/我慢3.png");
	effect_handle[3][13] = LoadGraph("battle_picture/我慢4.png");
	effect_handle[0][14] = LoadGraph("battle_picture/麻痺1.png");
	effect_handle[1][14] = LoadGraph("battle_picture/麻痺2.png");
	effect_handle[2][14] = effect_handle[0][14];
	effect_handle[3][14] = effect_handle[1][14];
	effect_handle[0][15] = LoadGraph("battle_picture/回避成功1.png");
	effect_handle[1][15] = LoadGraph("battle_picture/回避成功2.png");
	effect_handle[2][15] = LoadGraph("battle_picture/回避成功3.png");
	effect_handle[3][15] = LoadGraph("battle_picture/回避成功4.png");
	effect_handle[0][16] = LoadGraph("battle_picture/状態異常回復1.png");
	effect_handle[1][16] = LoadGraph("battle_picture/状態異常回復2.png");
	effect_handle[2][16] = LoadGraph("battle_picture/状態異常回復3.png");
	effect_handle[3][16] = LoadGraph("battle_picture/状態異常回復4.png");
	switch (GetRand(4)) {
	case 0:
		music = LoadSoundMem("music/12345_full.mp3");
		break;
	case 1:
		music = LoadSoundMem("music/kobune_full.mp3");
		break;
	case 2:
		music = LoadSoundMem("music/noapusa_full.mp3");
		break;
	case 3:
		music = LoadSoundMem("music/star_full.mp3");
		break;
	case 4:
		music = LoadSoundMem("music/zinnia_full.mp3");
		break;
	}
	ChangeVolumeSoundMem(255 * 60 / 100, music);
	switch (GetRand(4)) {
	case 0:
		bgm = LoadSoundMem("music/event33.ogg");
		break;
	case 1:
		bgm = LoadSoundMem("music/event34.ogg");
		break;
	case 2:
		bgm = LoadSoundMem("music/event42.ogg");
		break;
	case 3:
		bgm = LoadSoundMem("music/event46.ogg");
		break;
	case 4:
		bgm = LoadSoundMem("music/event7.ogg");
		break;
	}
	ChangeVolumeSoundMem(255 * 65 / 100, bgm);
}

BattleField::~BattleField() {
	DeleteFontToHandle(font_handle);//フォントデータの削除
	for (int i = 0; i < SKILL_SOUND_SUM; i++) {
		DeleteSoundMem(skill_sound_handle[i]);
	}
	delete[] skill_sound_handle;
	for (int i = 0; i < EFFECT_NUM; i++)delete[] effect_handle[i];
	DeleteSoundMem(trigger_handle1);
	DeleteSoundMem(trigger_handle2);
	DeleteSoundMem(target_sound);
	DeleteSoundMem(target_cancel);
}

//インターバル明けのデッキ更新
void BattleField::deck_reflesh(Deck& new_deck, int index) {
	*student[index].get_cards() = new_deck;
}

//インターバルに入る前
void BattleField::reset() {
	flag = true;
	finish_cnt = 0;
	target = -1;
	round++;
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		student[i].reset();
		teacher[i].reset();
	}
	if (round == MUSIC_START) {
		StopSoundMem(bgm);
	}
}

//生徒の残りhpを教える
void BattleField::get_remain_hp(int remain_hp[]) {
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		remain_hp[i] = student[i].get_hp();
	}
}

//音楽を流す
void BattleField::play_music(bool interval_flag) {
	if (round < MUSIC_START) {
		if (CheckSoundMem(bgm) == 0) {
			PlaySoundMem(bgm, DX_PLAYTYPE_BACK);
		}
	}
	else if(!interval_flag || round > MUSIC_START){
		if (CheckSoundMem(music) == 0) {
			PlaySoundMem(music, DX_PLAYTYPE_BACK);
		}
	}
}

//終了・インターバルへ移行してもいいかどうか
bool BattleField::finish_ok() {
	bool ok = true;
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (student[i].get_wait() > 0 || teacher[i].get_wait() > 0) {
			ok = false; break;
		}
	}
	return (ok && wait_num.empty() && hand == -1);
}

int BattleField::result(Deck after_deck[3]) {
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (student[i].get_hp() > 0) {
			break;
		}
		if (i == MAX_WITH_BATTLE - 1) {
			flag = false; finish_cnt++;
			DrawBox(0, 130, 640, 350, GetColor(100, 100, 100), TRUE);
			if(finish_cnt <= 60)DrawStringToHandle(640 - finish_cnt * 7, 200, "LOSE", GetColor(0, 0, 255), font_handle);
			else DrawStringToHandle(220, 200, "LOSE", GetColor(0, 0, 255), font_handle);
			if (finish_cnt >= FINISH_COUNT) {
				for (int i = 0; i < 3; i++) { after_deck[i] = *student[i].get_cards(); }
				StopSoundMem(bgm);
				StopSoundMem(music);
				return LOSE;//敗北
			}
		}
	}
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (teacher[i].get_hp() > 0) {
			break;
		}
		if (i == MAX_WITH_BATTLE - 1) {
			flag = false; finish_cnt++;
			DrawBox(0, 130, 640, 350, GetColor(100, 100, 100), TRUE);
			if (finish_cnt <= 60)DrawStringToHandle(640 - finish_cnt * 7, 200, "WIN", GetColor(255, 0, 0), font_handle);
			else DrawStringToHandle(220, 200, "WIN", GetColor(255, 0, 0), font_handle);
			if (finish_cnt >= FINISH_COUNT) {
				for (int i = 0; i < 3; i++) { after_deck[i] = *student[i].get_cards(); }
				StopSoundMem(bgm);
				StopSoundMem(music);
				return WIN;//勝利
			}
		}
	}
	if (!finish_ok())return CONTINUE;
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		int n = student[i].get_now();
		int s = student[i].get_cards()->get_size();
		int k = student[i].get_kind();
		if (n <= s && k != 0) {
			if (student[i].get_hp() > 0) {
				return CONTINUE;
			}
		}
		int n2 = teacher[i].get_now();
		int s2 = teacher[i].get_cards()->get_size();
		int k2 = teacher[i].get_kind();
		if (n2 <= s2 && k2 != 0) {
			if (teacher[i].get_hp() > 0) {
				return CONTINUE;
			}
		}
	}
	flag = false; finish_cnt++;
	DrawBox(0, 130, 640, 350, GetColor(100,100,100),TRUE);
	if (finish_cnt <= 60)DrawStringToHandle(640 - finish_cnt * 8, 200, "INTERVAL", GetColor(255, 0, 255), font_handle);
	else DrawStringToHandle(160, 200, "INTERVAL", GetColor(255, 0, 255), font_handle);
	if (finish_cnt >= FINISH_COUNT) {
		reset();
		for (int i = 0; i < 3; i++) { after_deck[i] = *student[i].get_cards(); }
		return INTERVAL;//インターバルへ
	}
	return CONTINUE;
}

//体力バーと待機時間バー、アイコンを表示
void BattleField::picture_bar() {
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (student[i].get_kind()) {
			student[i].picture_hp();
			student[i].picture_wait();
			student[i].picture_state();
			student[i].picture_icon(icon);
		}
		if (teacher[i].get_kind()) {
			teacher[i].picture_hp();
			teacher[i].picture_wait();
			teacher[i].picture_state();
			teacher[i].picture_icon(icon);
		}
	}
}

void BattleField::picture_card_trigger() {
	if (!flag) {
		if (hand != -1) {//キャラがカードを発動
			if (hand < MAX_WITH_BATTLE) {//studentが発動
				if (student[hand].picture_trigger(trigger_handle2)) {
					flag = true;
					hand = -1;
				}
			}
			else {//先生が発動
				if (teacher[hand - MAX_WITH_BATTLE].picture_trigger(trigger_handle2)) {
					flag = true;
					hand = -1;
				}
			}
		}
	}
}

void BattleField::queue_trigger() {
	if(!wait_num.empty() && flag) {//キューがからになるまでカード発動
		flag = false;//一時停止
		hand = wait_num.front();//キューから取り出す
		wait_num.pop();
		PlaySoundMem(trigger_handle1, DX_PLAYTYPE_BACK);
	}
}

//クリックでターゲッティング
void BattleField::target_decide() {
	int m = left_click();
	int a = -1;
	//立ち絵とカーソルが重なっているならa == numとなるはず
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		int x = teacher[i].mouse_hit();
		if (a == -1)a = x;
	}
	//左クリックが押された瞬間
	if (m == 1) { 
		if (a == target || a == -1) {
			if (target == -1)return;
			target = -1;//現在ターゲッティングしている敵なら解除
			PlaySoundMem(target_cancel, DX_PLAYTYPE_BACK);//音
		}
		else {
			target = a;//新たに敵をターゲッティング
			PlaySoundMem(target_sound, DX_PLAYTYPE_BACK);//音
		}
	}
}

void BattleField::target_hpzero() {
	if (target != -1 && teacher[target].get_hp() <= 0)target = -1;//死んでいるなら解除
}

void BattleField::picture_target() {
	if (target == -1)return;
	teacher[target].picture_red_circle();
}