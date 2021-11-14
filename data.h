#ifndef INCLUDED_DATA_H
#define INCLUDED_DATA_H

#include"mission.h"

enum {//各画面
	DATA_MAIN = 0,
	DATA_CHARA,
	DATA_CLEAR,
	DATA_OP,
	DATA_ED
};

class DataBase {
private:
	int student_handle[10];
	int teacher_handle[12];
	int other_handle[2];//シロナやクロエの画像を入れる予定
	int kettei_sound;
	bool clear[M_TOTAL];
	int font_handle;
	int em_sum;//クリアしたEMの数
	Op* op;
	Ed* ed;
private:
	int hand_x, hand_y;
	int now;//今どの画面か
private://キャラのデータ画面で使う
	int c_now = -1;
	int c_num = 0;
	int handle = -1;//キャラの絵
	std::string text;//キャラの説明文
public:
	DataBase(bool clear_data[]);
	~DataBase();
public:
	bool play();
	int get_now();
private:
	bool watch_clear();
	bool watch_character();
};

#endif
