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

/////////カードが持つスキル/////////
class Skill {
protected:
	int power;//威力
private:
	std::string text;//カードの説明文
protected:
	void attack(Character& self, Character& c, int damage, int s_hit, int s_kaihi, int c_hit, bool d_flag = true);
	void buff(Character& c, Change& set, int s_buff, int s_debuff, int kind);
	void state(Character& c, Change& set, int s_hit, int s_kaihi, bool avoid_flag = true, bool cancel_flag = true);
public:
	Skill();
	void operator=(const Skill& other);//コピー用
	void set_text(std::string s);
	void set_power(int p);
	int get_power();
	const char* get_text();
	virtual void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) = 0;
	bool survivor(Character* c);
	int aim_decide(Character* c);
};

//////スキルの派生クラス　具体的な能力を記載///////////
class Damage1 ://敵1体に普通の攻撃をするスキル
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};

class Damage2 ://敵1体に防御力を無視した攻撃をするスキル
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};

class DamageDouble1 ://敵1体に通常攻撃2回をするスキル
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};

class DamageDouble2 ://敵1体に防御力を無視した攻撃2回をするスキル
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};

class DamageAll ://敵全体に普通の攻撃をするスキル
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};

class AttackUpAll_5sec ://味方全体の攻撃力を上げるスキル(5sec)
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class AttackDown_5sec ://敵一体の攻撃力を下げるスキル(5sec)
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};

class DeffenseUpAll_5sec ://味方全体の防御力を上げるスキル(5sec)
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class DeffenseDown_5sec ://敵一体の防御力を下げるスキル(5sec)
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};

class SpeedUpAll_5sec ://味方全体のスピードを上げるスキル(5sec)
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class SpeedDown_5sec ://敵一体のスピードを下げるスキル(5sec)
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};

class Paralysis ://敵一体をマヒさせるスキル威力が効果時間になる
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class Poison_5sec ://敵一体を毒にするスキル
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class EndureSelf ://自身に我慢を付与する
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class AvoidSelf ://自身に回避を付与する
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class CounterSelf ://自身にカウンターを付与する
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class HideSelf ://自身にステルスを付与する
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class ProvocationSelf_30 ://自身に挑発を付与する
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class HpRepair ://味方全体のHPを回復
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class HpRepairItem ://アイテムで使用者のHPを回復
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class StateRepair ://味方全体の状態異常を回復
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class Shirona ://シロナ
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};
class Kuroe ://クロエ
	public Skill {
public:
	void skill_trigger(Character* self, Character* enemy, int num, int target, int* sound);
};

#endif