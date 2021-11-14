#ifndef INCLUDED_CHARACTER_H
#define INCLUDED_CHARACTER_H

#include"card.h"
#include<queue>
#include<vector>

//最大チーム人数 3vs3
#define MAX_WITH_BATTLE 3

#define ONE_SECOND 30//一秒 60
#define TRIGGER_COUNT 100//カード発動画面の表示時間 180

#define EFFECT_COUNT 20//エフェクトの表示時間
#define EFFECT_NUM 4//エフェクトのアニメーション,フレーム数

#define DAMAGE_COUNT 80//ダメージの値表示時間

enum {
	AKATSUKI = 1,
	HANDA,
	OOTA,
	ISHI,
	TAIRA,
	OWADA,
	MIYATONE,
	UCHIMOTO,
	IBARAKAWA,
	TOMOCHI
};

enum {
	O_T = 1,
	P_T,
	Q_T,
	R_T,
	S_T,
	T_T,
	U_T,
	V_T,
	W_T,
	X_T,
	Y_T,
	Z_T
};

enum {//状態異常
	NORMAL,		//正常
	PARALYSIS,	//マヒ
	POISON,		//毒
	HIDE,		//ステルス
	PROVOCATION,//挑発
	AVOID,		//回避
	COUNTER,	//カウンター
	ENDURE,		//キャンセル不可
	VACCINATION	//予防接種
};

enum {//攻撃力関連のスキル
	ATTACK_NORMAL,	//単なる加算減算
	ATTACK_RATE		//割合で変化10%アップなど
};
enum {//防御力関連のスキル
	DEFFENSE_NORMAL,	//単なる加算減算
	DEFFENSE_RATE		//割合で変化10%アップなど
};
enum {//スピード関連のスキル
	SPEED_NORMAL,	//単なる加算減算
	SPEED_RATE		//割合で変化10%アップなど
};

//バフデバフ状態異常の情報セットクラス
class Change {//バフ、デバフ
private:
	int kind;//種類
	int cnt;//有効時間残り
	int value;//威力
public:
	void operator=(const Change& other);
	Change(int k = NORMAL, int c = 0, int v = 0);
	int cnt_dec();
	int get_kind();
	int get_value();
	void attack(int& sum);
	void deffense(int& sum);
	void speed(int& sum);
	bool can_move();
	int damage();
	bool endure();
	bool avoid();
	bool vaccination();
	bool counter();
	bool demerit();
	bool merit();
};

//生徒と先生の基底クラス
class Character {
protected:
	int num;//何番目か
	int kind;//キャラの種類
	int huku;//服の種類
	int max_hp;
	int hp;
	int php;//表示されるhp hpを追いかける
	int wait_cnt;//硬直時間
	int wait_goal;//目標待ち時間 バーの表示に利用
	int event_cnt;//カード発動画面の表示時間カウント
	int next;//次に発動しようとしているカード 発動後0に戻る
	int now;//デッキで次に発動するカードのインデックス
	int x, y;//立ち絵が表示される座標
	Deck deck;//デッキ
	int effect_cnt;//エフェクトのカウント
	int effect_kind;//エフェクトの種類 -1はなし
	std::queue<std::pair<int, int> > damage_value;//damageの値と、表示した時間
	int damage_font;
	bool cancel_flag;
protected:
	std::vector<Change> attack;//攻撃補正
	std::vector<Change> deffense;//防御補正
	std::vector<Change> speed;//スピード補正
	Change state;//状態異常
protected:
	int stand_handle;//立ち絵のハンドル
	int size_x, size_y;//画像のサイズ
protected:
	virtual void load_handle() = 0;
	virtual void effect_draw(int handle) = 0;
	void c_flag();//キャンセルする
public:
	Character();
	void operator=(const Character& other);
	void reset();
	void effect_counting();//エフェクトのアニメーションを制御
	void counting(std::queue<int>& wait_num);//wait_cntの制御
	virtual int num_return() = 0;//0-5からnumへの変換
	void skill_trigger(Character* student, Character* teacher, int target, int* handle);//実際にカードを発動する関数
	virtual void card_trigger(Character* student, Character* teacher, int target, int* handle) = 0;//スキルを発動
	void cancel();//cancel_flagをtrueにする
	void seek_hp();//phpがhpを追いかける
public:
	int mouse_hit();
	void picture_flame();
	void picture_red_circle();
	virtual void picture() = 0;//立ち絵
	virtual void picture_hp() = 0;//バー
	virtual void picture_wait() = 0;//バー
	virtual void picture_state() = 0;
	virtual void picture_icon(int icon[]) = 0;
	virtual int picture_trigger(int sound_handle) = 0;//カード発動画面
	void picture_effect(int handle1, int handle2, int handle3, int handle4);
	void picture_damage(bool flag);
public:
	Deck* get_cards();
	int get_num();
	int get_kind();
	int get_hp();
	int get_max_hp();
	int get_wait();
	int get_now();
	std::vector<Change>* get_attack();
	std::vector<Change>* get_deffence();
	std::vector<Change>* get_speed();
	Change* get_state();
	int get_effect();
public:
	void kind_set(int k);
	void huku_set(int h);
	void set_max_hp(int value);
	void set_effect(int k);
public:
	//バフデバフ状態異常関連
	void set_attack(Change& add);
	void set_deffense(Change& add);
	void set_speed(Change& add);
	void set_state(Change& add);
	void state_cnt();
	int attack_value(int power = 0);
	int deffense_value(int power = 0);
	int speed_value(int power = 0);
public:
	void dec_hp(int damage, bool d_flag = true);//体力を減らす
	void change_hp(int damage, bool d_flag = true);//体力の増減（回復もあり）
};

//生徒のクラス
class Friend :
	public Character {
public:
	Friend(int n = 0);
private:
	void load_handle();
	void card_trigger(Character* student, Character* teacher, int target, int* handle);
	void effect_draw(int handle);
public:
	void picture();
	void picture_hp();
	void picture_wait();
	void picture_state();
	void picture_icon(int icon[]);
	int num_return();
	int picture_trigger(int sound_handle);//カード発動画面
};

//先生のクラス
class Enemy :
	public Character {
public:
	Enemy(int n = 0);
private:
	void load_handle();
	void card_trigger(Character* student, Character* teacher, int target, int* handle);
	void effect_draw(int handle);
public:
	void picture();
	void picture_hp();
	void picture_wait();
	void picture_state();
	void picture_icon(int icon[]);
	int num_return();
	int picture_trigger(int sound_handle);//カード発動画面
};

#endif