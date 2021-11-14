#ifndef INCLUDED_SEQUENCE_H
#define INCLUDED_SEQUENCE_H

#include"battle.h"

class Bsequence {
private:
	int stand_handle[10];//10�l���̗����G �ݒ肵���ߑ����i�[
	int s_kettei, s_modoru;//����A�߂鉹
private:
	int focus;//���삵�悤�Ƃ��Ă���L������kind
	int kind[3];//���k�O�l�̎��
	bool fixed[3];//���k�̎�ނ��Œ�Ȃ�true
private:
	Friend student[3];//BattleField�֓n�����k�̏��
	Enemy teacher[3];//mission�N���X���p�ӂ���G
	Deck st_deck[10];//���k10�l���̃f�b�L
	Deck store[10];//10�l���̃J�[�h�q��
	Deck item;//�A�C�e���q��
	Deck after_deck[3];//�o�g���ŃA�C�e���Ȃǂ��������
	int remain_hp[3];//���k�̎c��HP
private:
	BattleField* bf;//student teacher��n���Đ���
private:
	void st_deck_set();//st_deck��student�փR�s�[
	void st_kind_set(int another_kind[]);//kind��student�փR�s�[ �g�����L������m�邽�߂�another�ɂ�
	void dec_item();
public:
	Bsequence(Enemy te[3], int st_kind[3], bool st_fixed[3]);
	~Bsequence();
	//WIN�܂���LOSE��Ԃ��I����ĂȂ��Ȃ�0 �g�����L������m�邽�߂�kind���n��
	int battle_loop(int another_kind[]);
	//��V���󂯎���ďI������
	void battle_end(int kind, Card& c);
};

#endif
