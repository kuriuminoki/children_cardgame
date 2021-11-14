#ifndef INCLUDED_SKILL_H
#define INCLUDED_SKILL_H

#include<string>

class Change;
class Character;

enum {
	E_ORANGE = 0,
	E_ATTACK_UP,
	E_ATTACK_DOWN,
	E_DEFFENSE_UP,
	E_DEFFENSE_DOWN,
	E_SPEED_UP,
	E_SPEED_DOWN
};

/////////�J�[�h�����X�L��/////////
class Skill {
protected:
	int power;//�З�
private:
	std::string text;//�J�[�h�̐�����
protected:
	void attack(Character& self, Character& c, int damage, int s_hit, int s_kaihi, int c_hit, bool d_flag = true);
	void buff(Character& c, Change& set, int s_buff, int s_debuff, int kind);
	void state(Character& c, Change& set, int s_hit, int s_kaihi, bool avoid_flag = true, bool cancel_flag = true);
public:
	Skill();
	void operator=(const Skill& other);//�R�s�[�p
	void set_text(std::string s);
	void set_power(int p);
	int get_power();
	const char* get_text();
	virtual void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) = 0;
	bool survivor(Character* c);
	int aim_decide(Character* c);
};

//////�X�L���̔h���N���X�@��̓I�Ȕ\�͂��L��///////////
class Damage1 ://�G1�̂ɕ��ʂ̍U��������X�L��
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};

class Damage2 ://�G1�̂ɖh��͂𖳎������U��������X�L��
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};

class DamageDouble1 ://�G1�̂ɒʏ�U��2�������X�L��
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};

class DamageDouble2 ://�G1�̂ɖh��͂𖳎������U��2�������X�L��
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};

class DamageAll ://�G�S�̂ɕ��ʂ̍U��������X�L��
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};

class AttackUpAll_5sec ://�����S�̂̍U���͂��グ��X�L��(5sec)
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class AttackDown_5sec ://�G��̂̍U���͂�������X�L��(5sec)
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};

class DeffenseUpAll_5sec ://�����S�̖̂h��͂��グ��X�L��(5sec)
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class DeffenseDown_5sec ://�G��̖̂h��͂�������X�L��(5sec)
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};

class SpeedUpAll_5sec ://�����S�̂̃X�s�[�h���グ��X�L��(5sec)
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class SpeedDown_5sec ://�G��̂̃X�s�[�h��������X�L��(5sec)
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};

class Paralysis ://�G��̂��}�q������X�L���З͂����ʎ��ԂɂȂ�
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class Poison_5sec ://�G��̂�łɂ���X�L��
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class EndureSelf ://���g�ɉ䖝��t�^����
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class AvoidSelf ://���g�ɉ����t�^����
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class CounterSelf ://���g�ɃJ�E���^�[��t�^����
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class HideSelf ://���g�ɃX�e���X��t�^����
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class ProvocationSelf_30 ://���g�ɒ�����t�^����
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class HpRepair ://�����S�̂�HP����
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class HpRepairItem ://�A�C�e���Ŏg�p�҂�HP����
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class StateRepair ://�����S�̂̏�Ԉُ����
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class Shirona ://�V���i
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class Kuroe ://�N���G
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};

#endif