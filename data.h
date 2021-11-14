#ifndef INCLUDED_DATA_H
#define INCLUDED_DATA_H

#include"mission.h"

enum {//�e���
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
	int other_handle[2];//�V���i��N���G�̉摜������\��
	int kettei_sound;
	bool clear[M_TOTAL];
	int font_handle;
	int em_sum;//�N���A����EM�̐�
	Op* op;
	Ed* ed;
private:
	int hand_x, hand_y;
	int now;//���ǂ̉�ʂ�
private://�L�����̃f�[�^��ʂŎg��
	int c_now = -1;
	int c_num = 0;
	int handle = -1;//�L�����̊G
	std::string text;//�L�����̐�����
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
