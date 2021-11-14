#ifndef INCLUDED_CHARACTER_H
#define INCLUDED_CHARACTER_H

#include"card.h"
#include<queue>
#include<vector>

//�ő�`�[���l�� 3vs3
#define MAX_WITH_BATTLE 3

#define ONE_SECOND 30//��b 60
#define TRIGGER_COUNT 100//�J�[�h������ʂ̕\������ 180

#define EFFECT_COUNT 20//�G�t�F�N�g�̕\������
#define EFFECT_NUM 4//�G�t�F�N�g�̃A�j���[�V����,�t���[����

#define DAMAGE_COUNT 80//�_���[�W�̒l�\������

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

enum {//��Ԉُ�
	NORMAL,		//����
	PARALYSIS,	//�}�q
	POISON,		//��
	HIDE,		//�X�e���X
	PROVOCATION,//����
	AVOID,		//���
	COUNTER,	//�J�E���^�[
	ENDURE,		//�L�����Z���s��
	VACCINATION	//�\�h�ڎ�
};

enum {//�U���͊֘A�̃X�L��
	ATTACK_NORMAL,	//�P�Ȃ���Z���Z
	ATTACK_RATE		//�����ŕω�10%�A�b�v�Ȃ�
};
enum {//�h��͊֘A�̃X�L��
	DEFFENSE_NORMAL,	//�P�Ȃ���Z���Z
	DEFFENSE_RATE		//�����ŕω�10%�A�b�v�Ȃ�
};
enum {//�X�s�[�h�֘A�̃X�L��
	SPEED_NORMAL,	//�P�Ȃ���Z���Z
	SPEED_RATE		//�����ŕω�10%�A�b�v�Ȃ�
};

//�o�t�f�o�t��Ԉُ�̏��Z�b�g�N���X
class Change {//�o�t�A�f�o�t
private:
	int kind;//���
	int cnt;//�L�����Ԏc��
	int value;//�З�
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

//���k�Ɛ搶�̊��N���X
class Character {
protected:
	int num;//���Ԗڂ�
	int kind;//�L�����̎��
	int huku;//���̎��
	int max_hp;
	int hp;
	int php;//�\�������hp hp��ǂ�������
	int wait_cnt;//�d������
	int wait_goal;//�ڕW�҂����� �o�[�̕\���ɗ��p
	int event_cnt;//�J�[�h������ʂ̕\�����ԃJ�E���g
	int next;//���ɔ������悤�Ƃ��Ă���J�[�h ������0�ɖ߂�
	int now;//�f�b�L�Ŏ��ɔ�������J�[�h�̃C���f�b�N�X
	int x, y;//�����G���\���������W
	Deck deck;//�f�b�L
	int effect_cnt;//�G�t�F�N�g�̃J�E���g
	int effect_kind;//�G�t�F�N�g�̎�� -1�͂Ȃ�
	std::queue<std::pair<int, int> > damage_value;//damage�̒l�ƁA�\����������
	int damage_font;
	bool cancel_flag;
protected:
	std::vector<Change> attack;//�U���␳
	std::vector<Change> deffense;//�h��␳
	std::vector<Change> speed;//�X�s�[�h�␳
	Change state;//��Ԉُ�
protected:
	int stand_handle;//�����G�̃n���h��
	int size_x, size_y;//�摜�̃T�C�Y
protected:
	virtual void load_handle() = 0;
	virtual void effect_draw(int handle) = 0;
	void c_flag();//�L�����Z������
public:
	Character();
	void operator=(const Character& other);
	void reset();
	void effect_counting();//�G�t�F�N�g�̃A�j���[�V�����𐧌�
	void counting(std::queue<int>& wait_num);//wait_cnt�̐���
	virtual int num_return() = 0;//0-5����num�ւ̕ϊ�
	void skill_trigger(Character* student, Character* teacher, int target, int* handle);//���ۂɃJ�[�h�𔭓�����֐�
	virtual void card_trigger(Character* student, Character* teacher, int target, int* handle) = 0;//�X�L���𔭓�
	void cancel();//cancel_flag��true�ɂ���
	void seek_hp();//php��hp��ǂ�������
public:
	int mouse_hit();
	void picture_flame();
	void picture_red_circle();
	virtual void picture() = 0;//�����G
	virtual void picture_hp() = 0;//�o�[
	virtual void picture_wait() = 0;//�o�[
	virtual void picture_state() = 0;
	virtual void picture_icon(int icon[]) = 0;
	virtual int picture_trigger(int sound_handle) = 0;//�J�[�h�������
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
	//�o�t�f�o�t��Ԉُ�֘A
	void set_attack(Change& add);
	void set_deffense(Change& add);
	void set_speed(Change& add);
	void set_state(Change& add);
	void state_cnt();
	int attack_value(int power = 0);
	int deffense_value(int power = 0);
	int speed_value(int power = 0);
public:
	void dec_hp(int damage, bool d_flag = true);//�̗͂����炷
	void change_hp(int damage, bool d_flag = true);//�̗͂̑����i�񕜂�����j
};

//���k�̃N���X
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
	int picture_trigger(int sound_handle);//�J�[�h�������
};

//�搶�̃N���X
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
	int picture_trigger(int sound_handle);//�J�[�h�������
};

#endif