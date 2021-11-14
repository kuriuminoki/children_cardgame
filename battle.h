#ifndef INCLUDED_BATTLE_H
#define INCLUDED_BATTLE_H

#include"character.h"
#include<vector>

enum {
	LOSE = -1,
	CONTINUE,
	WIN,
	INTERVAL,
	CANCEL//�߂�{�^���Ŗ߂���
};

//�R���X�g���N�^�̈����ŃL�����̏���6�l���󂯎��A
//�o�g���������Ȃ��B
//�L���������ԕt�����ĉ�ʂɔz�u�����ڂ��S���B
class BattleField {
private:
	bool load_flag = false;//�񓯊������ɑΉ����邽�߁A���̑傫����ς���̂͌��
	bool flag = false;//�퓬����true,�ꎞ��~����false
	//0,1,2�̏��ԕt���͂����ŏ������ɂ���Ă����Ȃ�
	Friend student[MAX_WITH_BATTLE] = {0, 1, 2};//���k3�l
	Enemy teacher[MAX_WITH_BATTLE] = {0, 1, 2};//�搶3�l
	int round;//���E���h
	std::queue<int> wait_num;//�����҂��̃L����,��Ȃ�flag = true
	int target;//�_���G -1�͂Ȃ�
	int hand;//���ݎ��s���̃L����-1�͂Ȃ�
	void picture_card_trigger();
private:
	int haikei;//�w�i�̉摜
	int* effect_handle[EFFECT_NUM];//�G�t�F�N�g�̃A�j���[�V�����e4��
	int trigger_handle1;//�J�[�h�������ʉ�
	int trigger_handle2;
	int* skill_sound_handle;//�_���[�W�Ȃǂ̃T�E���h
	int target_sound;
	int target_cancel;
	int icon[15];//�U���͂Ɩh��̓o�t�f�o�t�̃A�C�R��
private:
	int bgm;//�O����BGM
	int music;//�㔼�̐��t���S
private:
	int finish_cnt = 0;//�퓬�I�����̍d��
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
