#ifndef INCLUDED_MENU_H
#define INCLUDED_MENU_H

#include"shop.h"
#include"data.h"

#define TODAY_MAX 3//����ɒ񎦂����~�b�V�����̐�
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
private://�Z�[�u����f�[�^
	int money;
	Deck* deck;
	Deck* store;
	Deck* item;//�A�C�e���q��
	bool clear[M_TOTAL];//�e�~�b�V�������N���A�������ǂ���
	int day;//�����ڂ�
	int today_mission[TODAY_MAX];//�����ł���~�b�V�����̎��
	MissionText* daily_mission[DAYLY_MISSION];//DM
private:
	int cnt;//���t�ύX�C�x���g�̃J�E���g
	int m_kind;//�~�b�V�����̎��
	MissionText* task_detail;//�����̃~�b�V�����T�v
	Mission* task;//�~�b�V����
	Shop* shop;//�V���b�v
	DataBase* db;//�f�[�^
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
	void picture_reflesh();//�摜�ƃt�H���g�����ׂč폜
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