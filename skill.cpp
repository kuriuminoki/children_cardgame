#include"skill.h"
#include"character.h"
#include"DxLib.h"
#include<algorithm>

const int AVOID_RATE = 70;//�������m��
const double COUNTER_RATE = 1.2;//�J�E���^�[�̔{��

Skill::Skill() {
	power = 0;
}

void Skill::operator=(const Skill& other) {
	text = other.text;
	power = other.power;
}

void Skill::set_text(std::string s) {
	text = s;
}

void Skill::set_power(int p) {
	power = p;
}

int Skill::get_power() {
	return power;
}

const char* Skill::get_text() {
	return text.c_str();
}

//----�X�L�������֐�-----//
//�Ώۂ̐w�c�ɐ����҂����邩�ǂ���
bool Skill::survivor(Character* c) {
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (c[i].get_hp() > 0)return true;
	}
	return false;
}

//�X�L���K�p�Ώۂ�1�̌��߂� ������X�e���X���l������
int Skill::aim_decide(Character* c) {
	int aim = 0;//num == 0���b��
	int candidate[3] = { -1,-1,-1 };//�D��x-1���Œ�ŁA�I�΂�Ȃ�
	int max_priority = 0;//3�l���ł������D��x
	//�܂��͗D��x���v�Z
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (c[i].get_hp() > 0) {
			candidate[i] = 1;//�܂��͎b�蕁�ʂ̗D��x
			if (c[i].get_state()->get_kind() == PROVOCATION) {
				candidate[i] = 2;//�����͗D��x����
			}
			else if (c[i].get_state()->get_kind() == HIDE) {
				candidate[i] = 0;//�X�e���X�͗D��x�Ⴂ
			}
			max_priority = max(max_priority, candidate[i]);
		}
	}
	//�ł������D��x�����L�����݂̂�candidate�ɍ��l�߂���
	int now = 0;//�ł������D��x�����L�����̐�
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (candidate[i] == max_priority) {
			candidate[now] = i;//�L�����̔ԍ�������
			now++;
		}
	}
	return candidate[GetRand(now - 1)];
}

//�U���֘A
//�U�������킵����J�E���^�[������ �h��͂��l������ d_flag��false�Ȃ�l�����Ȃ�
void Skill::attack(Character& self, Character& c, int damage, int s_hit, int s_kaihi, int c_hit, bool d_flag) {
	if (c.get_state()->avoid() && GetRand(99) + 1 <= AVOID_RATE) {//�U�������킳�ꂽ�ꍇ
		c.set_effect(15);//����G�t�F�N�g
		PlaySoundMem(s_kaihi, DX_PLAYTYPE_BACK);//�����
	}
	else {//�U����^����
		c.dec_hp(damage, d_flag);//hp������
		c.set_effect(0);
		c.cancel();//�U���L�����Z��
		if (c.get_state()->counter()) {//���肪�J�E���^�[��ԂȂ�
			self.dec_hp((int)(damage * COUNTER_RATE), d_flag);//hp������
			self.set_effect(9);
			PlaySoundMem(c_hit, DX_PLAYTYPE_BACK);//�J�E���^�[��
		}
		PlaySoundMem(s_hit, DX_PLAYTYPE_BACK);//hp�����鉹
	}
}

//�o�t�f�o�t�֘A
//�o�t���f�o�t�����肵�Č��ʉ��ƃG�t�F�N�g�����߂�
void Skill::buff(Character& c, Change& set, int s_buff, int s_debuff, int kind) {
	if (set.get_value() >= 0) {
		c.set_effect(kind);//�o�t�̃G�t�F�N�g
		PlaySoundMem(s_buff, DX_PLAYTYPE_BACK);//�o�t�̌��ʉ�
	}
	else {
		c.set_effect(kind+1);//�f�o�t�̃G�t�F�N�g
		PlaySoundMem(s_debuff, DX_PLAYTYPE_BACK);//�f�o�t�̌��ʉ�
	}
}

//��Ԉُ�֘A
//���킹���Ԉُ�t�^�X�L�������� ���킹�Ȃ��X�L���̏ꍇ��avoid_flag��false�ɂ���ׂ� cancel�����l
void Skill::state(Character& c, Change& set, int s_hit, int s_kaihi, bool avoid_flag, bool cancel_flag) {
	if (avoid_flag && c.get_state()->avoid() && GetRand(99) + 1 <= AVOID_RATE) {//�U�������킳�ꂽ�ꍇ
		c.set_effect(15);//����G�t�F�N�g
		PlaySoundMem(s_kaihi, DX_PLAYTYPE_BACK);//�����
	}
	else {//�U����^����
		if (set.demerit() && c.get_state()->vaccination()) {//�\�h�ڎ�ŉ��
			c.set_effect(16);//�G�t�F�N�g
			return;
		}
		c.set_state(set);//�Z�b�g
		switch (set.get_kind()) {
		case NORMAL:
		case VACCINATION:
			c.set_effect(16);//�G�t�F�N�g
			break;
		case POISON:
			c.set_effect(7);//�G�t�F�N�g
			break;
		case HIDE:
			c.set_effect(12);//�G�t�F�N�g
			break;
		case PROVOCATION:
			c.set_effect(11);//�G�t�F�N�g
			break;
		case AVOID:
			c.set_effect(8);//�G�t�F�N�g
			break;
		case COUNTER:
			c.set_effect(10);//�G�t�F�N�g
			break;
		case ENDURE:
			c.set_effect(13);//�G�t�F�N�g
			break;
		case PARALYSIS:
			c.set_effect(14);//�G�t�F�N�g
			break;
		}
		if (cancel_flag) { c.cancel(); }//�U���L�����Z��
		PlaySoundMem(s_hit, DX_PLAYTYPE_BACK);//hp�����鉹
	}
}


//////////�e�X�L����������////////////////////
//1�̂ɒʏ�U��
void Damage1::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound){
	if (!survivor(enemy))return;//�G���S�ł��Ă���
	int aim = target;
	if (aim == -1) { aim = aim_decide(enemy); }//�Ώۂ����߂�
	int damage = power;
	damage = self[num].attack_value(damage);//�U���̓o�t�f�o�t�␳
	attack(self[num], enemy[aim], damage, sound[0], sound[9], sound[11]);
}

//1�̂ɖh��͂𖳎������U��
void Damage2::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(enemy))return;//�G���S�ł��Ă���
	int aim = target;
	if (aim == -1) { aim = aim_decide(enemy); }//�Ώۂ����߂�
	int damage = power;
	damage = self[num].attack_value(damage);//�U���̓o�t�f�o�t�␳
	attack(self[num], enemy[aim], damage, sound[0], sound[9], sound[11], false);
}

//1�̂�2��U��
void DamageDouble1::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(enemy))return;
	int aim = target;
	if (aim == -1) { aim = aim_decide(enemy); }//�Ώۂ����߂�
	int damage = power;
	damage = self[num].attack_value(damage);//�U���̓o�t�f�o�t�␳
	attack(self[num], enemy[aim], damage, sound[0], sound[9], sound[11]);//1���
	attack(self[num], enemy[aim], damage, sound[0], sound[9], sound[11]);//2���
}

//1�̂�2��U��
void DamageDouble2::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(enemy))return;
	int aim = target;
	if (aim == -1) { aim = aim_decide(enemy); }//�Ώۂ����߂�
	int damage = power;
	damage = self[num].attack_value(damage);//�U���̓o�t�f�o�t�␳
	attack(self[num], enemy[aim], damage, sound[0], sound[9], sound[11], false);//1���
	attack(self[num], enemy[aim], damage, sound[0], sound[9], sound[11], false);//2���
}

//�S�̍U��
void DamageAll::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(enemy))return;
	int damage = power;
	damage = self[num].attack_value(damage);//�U���̓o�t�f�o�t�␳
	//�S�����Ώ�
	for (int i = 0; i < MAX_WITH_BATTLE; i++) { 
		if (enemy[i].get_hp() <= 0)continue;//hp <= 0�͖���
		attack(self[num], enemy[i], damage, sound[0], sound[9], sound[11]);
	}
}

//�U���o�t�S��
void AttackUpAll_5sec::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(self))return;
	int v = power;
	//�����S�����Ώ�
	Change c(ATTACK_NORMAL, 300, v);//�o�t�̏������I�u�W�F�N�g
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (self[i].get_hp() <= 0)continue;//hp <= 0�͖���
		self[i].set_attack(c);//�o�t���Z�b�g
		buff(self[i], c, sound[1], sound[2], E_ATTACK_UP);//�o�t�f�o�t�̃Z�b�g�ƌ��ʉ�
	}
}
//�U���f�o�t���
void AttackDown_5sec::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(enemy))return;
	int v = -power;//�f�o�t�͂��Ȃ킿���̃o�t
	//�G��̂��Ώ�
	int aim = target;
	if (aim == -1) { aim = aim_decide(enemy); }//�Ώۂ����߂�
	Change c(ATTACK_NORMAL, 300, v);//�f�o�t�̏������I�u�W�F�N�g
	enemy[aim].set_attack(c);
	buff(enemy[aim], c, sound[1], sound[2], E_ATTACK_UP);//�o�t�f�o�t�̃Z�b�g�ƌ��ʉ�
}

//�h��o�t�S��
void DeffenseUpAll_5sec::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(self))return;
	int v = power;
	//�����S�����Ώ�
	Change c(DEFFENSE_NORMAL, 300, v);
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (self[i].get_hp() <= 0)continue;//hp <= 0�͖���
		self[i].set_deffense(c);
		buff(self[i], c, sound[3], sound[4], E_DEFFENSE_UP);
	}
}
//�h��f�o�t���
void DeffenseDown_5sec::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(enemy))return;
	int v = -power;
	//�G��̂��Ώ�
	int aim = target;
	if (aim == -1) { aim = aim_decide(enemy); }//�Ώۂ����߂�
	Change c(DEFFENSE_NORMAL, 300, v);
	enemy[aim].set_deffense(c);
	buff(enemy[aim], c, sound[3], sound[4], E_DEFFENSE_UP);
}

//�X�s�[�h�o�t�S��
void SpeedUpAll_5sec::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(self))return;
	int v = power;
	//�����S�����Ώ�
	Change c(SPEED_NORMAL, 300, v);
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (self[i].get_hp() <= 0)continue;//hp <= 0�͖���
		self[i].set_speed(c);//�o�t���Z�b�g
		buff(self[i], c, sound[5], sound[6], E_SPEED_UP);
	}
}
//�X�s�[�h�f�o�t���
void SpeedDown_5sec::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(enemy))return;
	int v = -power;
	//�G��̂��Ώ�
	int aim = target;
	if (aim == -1) { aim = aim_decide(enemy); }//�Ώۂ����߂�
	Change c(SPEED_NORMAL, 300, v);
	enemy[aim].set_speed(c);//�f�o�t���Z�b�g
	buff(enemy[aim], c, sound[5], sound[6], E_SPEED_UP);
}

//�}�q���
void Paralysis::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(enemy))return;
	int v = power;
	//�G��̂��Ώ�
	int aim = target;
	if (aim == -1) { aim = aim_decide(enemy); }//�Ώۂ����߂�
	Change c(PARALYSIS, v, v);
	state(enemy[aim], c, sound[7], sound[9]);
}
//�ň��
void Poison_5sec::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(enemy))return;
	int v = power;
	//�G��̂��Ώ�
	int aim = target;
	if (aim == -1) { aim = aim_decide(enemy); }//�Ώۂ����߂�
	Change c(POISON, 300, v);
	state(enemy[aim], c, sound[8], sound[9], true, false);//�L�����Z���͋N���Ȃ�(false)
}
//���g�ɉ䖝�t�^
void EndureSelf::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	int v = power;
	Change c(ENDURE, v, v);
	state(self[num], c, sound[17], sound[9], false, false);
	Change c2(DEFFENSE_RATE, v, 30);//�_���[�W��30������
	self[num].set_deffense(c2);
	Change c3(SPEED_RATE, v, 30);//�X�s�[�h��30���㏸
	self[num].set_speed(c3);
}
//���g�ɉ��t�^
void AvoidSelf::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	int v = power;
	Change c(AVOID, v, v);
	state(self[num], c, sound[10], sound[9], false, false);
}
//���g�ɃJ�E���^�[�t�^
void CounterSelf::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	int v = power;
	Change c(COUNTER, v, v);
	state(self[num], c, sound[12], sound[9], false, false);
}
//���g�ɃX�e���X�t�^
void HideSelf::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	int v = power;
	Change c(HIDE, v, v);
	state(self[num], c, sound[14], sound[9], false, false);
	Change c2(ATTACK_RATE, v, 30);//�U���͂�30���㏸
	self[num].set_attack(c2);
	Change c3(SPEED_RATE, v, 30);//�X�s�[�h��30���㏸
	self[num].set_speed(c3);
}
//���g�ɒ����t�^
void ProvocationSelf_30::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	int v = power;
	Change c(PROVOCATION, v, v);
	state(self[num], c, sound[13], sound[9], false, false);
	Change c2(DEFFENSE_RATE, v, 30);//�_���[�W��30������
	self[num].set_deffense(c2);
}

//�����S�̗̂̑͂���
void HpRepair::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	int v = -power;//�񕜂����畉�̃_���[�W
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (self[i].get_hp() <= 0)continue;//hp <= 0�͖���
		self[i].change_hp(v, false);
	}
	PlaySoundMem(sound[16], DX_PLAYTYPE_BACK);//��
}

//�A�C�e���Ŏg�p�҂�HP����
void HpRepairItem::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	int v = -power;//�񕜂����畉�̃_���[�W
	self[num].change_hp(v, false);
	PlaySoundMem(sound[16], DX_PLAYTYPE_BACK);//��
}

//�����S�̂̏�Ԉُ����
void StateRepair::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	int v = power;
	//�����S�����Ώ�
	Change c(VACCINATION, v, v);//�\�h�ڎ�
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (self[i].get_hp() <= 0 || self[i].get_state()->merit())continue;//hp <= 0�͖���
		state(self[i], c, -1, -1, false, false);//�L�����Z���Ɖ���͋N���Ȃ�(false)
		PlaySoundMem(sound[15], DX_PLAYTYPE_BACK);//hp�����鉹
	}
}

//�V���i
void Shirona::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(enemy))return;//�G���S�ł��Ă���
	int aim = target;
	if (aim == -1) { aim = aim_decide(enemy); }//�Ώۂ����߂�
	int damage = power;
	attack(self[num], enemy[aim], damage, sound[0], sound[9], sound[11]);

	if (self[num].get_hp() <= 0)return;
	int v = 60;
	//�g�p�҂��Ώ�
	Change c(DEFFENSE_NORMAL, 300, v);
	self[num].set_deffense(c);
	buff(self[num], c, sound[3], sound[4], E_DEFFENSE_UP);
}

//�N���G
void Kuroe::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(enemy))return;//�G���S�ł��Ă���
	int aim = target;
	if (aim == -1) { aim = aim_decide(enemy); }//�Ώۂ����߂�
	int damage = power;
	attack(self[num], enemy[aim], damage, sound[0], sound[9], sound[11]);

	if (self[num].get_hp() <= 0)return;
	int v = 60;
	//�g�p�҂��Ώ�
	Change c(ATTACK_NORMAL, 300, v);//�o�t�̏������I�u�W�F�N�g
	self[num].set_attack(c);//�o�t���Z�b�g
	buff(self[num], c, sound[1], sound[2], E_ATTACK_UP);//�o�t�f�o�t�̃Z�b�g�ƌ��ʉ�
}