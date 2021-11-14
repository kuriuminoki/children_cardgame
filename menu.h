#ifndef INCLUDED_MENU_H
#define INCLUDED_MENU_H

#include"shop.h"
#include"data.h"

#define TODAY_MAX 3//一日に提示されるミッションの数
#define DAYLY_MISSION 10
#define DAY_COUNT 240

enum {
	M_SELECT = 1,
	M_PLAY,
	CUS,
	SHOP,
	DATA
};

class CardGame {
private://セーブするデータ
	int money;
	Deck* deck;
	Deck* store;
	Deck* item;//アイテム倉庫
	bool clear[M_TOTAL];//各ミッションをクリアしたかどうか
	int day;//何日目か
	int today_mission[TODAY_MAX];//今日できるミッションの種類
	MissionText* daily_mission[DAYLY_MISSION];//DM
private:
	int cnt;//日付変更イベントのカウント
	int m_kind;//ミッションの種類
	MissionText* task_detail;//今日のミッション概要
	Mission* task;//ミッション
	Shop* shop;//ショップ
	DataBase* db;//データ
private:
	int* stand_handle;
	int kettei_sound, modoru_sound;
	int font_handle;
	int bgm;
private:
	int mouse_handle;
	int mouse_x, mouse_y;
public:
	CardGame();
	~CardGame();
	void CMain();
public:
	void init_mission();
	bool play_mission();
	void end_mission();
private:
	void picture_reflesh();//画像とフォントをすべて削除
	void sound_reflesh();
	void init_cards();
	void end_cards();
	void chara_focus(int& focus, int& now);
	void menu_key(int hand_x, int hand_y, int& indicate);
	void mission_key(int hand_x, int hand_y, int& indicate);
	void mission_decide();
	void menu_click(int indicate, int& now);
	void picture_menu();
	void picture_result(int money, Card& c);
public:
	void picture_mouse();
};

#endif