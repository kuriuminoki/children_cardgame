#ifndef INCLUDED_BATTLE_H
#define INCLUDED_BATTLE_H

#include"character.h"
#include<vector>

enum {
	LOSE = -1,
	CONTINUE,
	WIN,
	INTERVAL,
	CANCEL//戻るボタンで戻った
};

//コンストラクタの引数でキャラの情報を6人分受け取り、
//バトルをおこなう。
//キャラを順番付けして画面に配置する役目も担う。
class BattleField {
private:
	bool load_flag = false;//非同期処理に対応するため、音の大きさを変えるのは後回し
	bool flag = false;//戦闘中はtrue,一時停止中がfalse
	//0,1,2の順番付けはここで初期化によっておこなう
	Friend student[MAX_WITH_BATTLE] = {0, 1, 2};//生徒3人
	Enemy teacher[MAX_WITH_BATTLE] = {0, 1, 2};//先生3人
	int round;//ラウンド
	std::queue<int> wait_num;//発動待ちのキャラ,空ならflag = true
	int target;//狙う敵 -1はなし
	int hand;//現在実行中のキャラ-1はなし
	void picture_card_trigger();
private:
	int haikei;//背景の画像
	int* effect_handle[EFFECT_NUM];//エフェクトのアニメーション各4つ
	int trigger_handle1;//カード発動効果音
	int trigger_handle2;
	int* skill_sound_handle;//ダメージなどのサウンド
	int target_sound;
	int target_cancel;
	int icon[15];//攻撃力と防御力バフデバフのアイコン
private:
	int bgm;//前半のBGM
	int music;//後半の声付き唄
private:
	int finish_cnt = 0;//戦闘終了時の硬直
	int font_handle;
private:
	void target_decide();
	void target_hpzero();
	void picture_target();
	int result(Deck after_deck[3]);
	bool finish_ok();
	void reset();
public:
	BattleField(Friend* st, Enemy* te);
	~BattleField();
	int test(Deck after_deck[3]);
	void picture_bar();
	void queue_trigger();
	void deck_reflesh(Deck& new_deck, int index);
	void play_music(bool interval_flag);
	void get_remain_hp(int remain_hp[]);
};

#endif
