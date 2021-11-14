#ifndef INCLUDED_CARD_H
#define INCLUDED_CARD_H

#include"skill.h"


#define DECK_MAX 5		//�f�b�L�ɃZ�b�g�ł���J�[�h�̍ő吔9
#define STORE_MAX 1000	//�ۊǂł���J�[�h�̍ő吔100

//�����ς݂̃J�[�h��
const int CARD_SUM[11] = { 3,3,3,3,3,3,3,3,3,3,0 };

enum {
	C_RARE,
	B_RARE,
	A_RARE,
	S_RARE
};

///////�J�[�h///////////
class Card {
protected:
	int kind;//�J�[�h�̎��
	int rare;//�J�[�h�̃��A���e�B
	int hp;//�X�e�[�^�X
	int power;//�J�[�h�̌��ʂ̐�Βl(�U���͂�f�o�t�̈З�)
	int speed;//�J�[�h���o���Ă��甭���܂ł̎���
	int rigid;//�d������
	int handle;//�摜�n���h��
	Skill* ability;//�J�[�h�̌���kind�ɉ����Ĕh���N���X�����
public:
	Card(int k = 0, int ra = 0, int h = 0, int p = 0, int s = 0, int r = 0);
	~Card();
	void null_ability();//�Z�[�u�f�[�^�����[�h�����Ƃ��ɂ̂ݎg��
	void operator=(const Card& other);//�R�s�[�p
	void set_kind(int x);//kind���Z�b�g
	void set_handle(int h);
	int get_kind();
	int get_hp();
	int get_power();
	int get_speed();
	int get_rigid();
	int get_rare();
	void load_card(bool load_flag = true);//�摜�̃��[�h kind�ɉ����ăX�L���擾
	int get_handle();
	Skill* get_ability();
	void trigger(Character* self, Character* enemy, int num, int target, int* handle);
	void Card::set_state(int h, int p, int s, int r);
	void set_rare(int c_rate, int b_rate, int a_rate, int s_rate);
public:
	bool update();//�J�[�h�̃A�b�v�f�[�g
};

///////�f�b�L�̓J�[�h�̏W���̂ɂ����Ȃ�//////////
class Deck {
private:
	int size;//�f�b�L�̃J�[�h����
	Card* cards;//�f�b�L�{��
public:
	Deck();//�R���X�g���N�^�ŃJ�[�h�͈�x��ɂ���
	~Deck();
	void operator=(const Deck& other);//�R�s�[�p
	Card* get_cards();
	bool empty();
	void get_last(int& last);
	void sort();
	int partition(int left, int right);
	void quick_sort(int left, int right);
	int get_size();
	int get_hp();
	void set_size(int n);
	void set_card(Card& card, int i);
	void load_cards();
};

void reward_card(int kind, Card& c, int c_rate = 25, int b_rate = 25, int a_rate = 25, int s_rate = 25, int card_kind = -1);
int sell_money(int kind, int rare = C_RARE);
int buy_money(int kind);
void normal_status(int kind, Card& c);

#endif