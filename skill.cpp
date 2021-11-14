#include"skill.h"
#include"character.h"
#include"DxLib.h"
#include<algorithm>

const int AVOID_RATE = 70;//回避する確率
const double COUNTER_RATE = 1.2;//カウンターの倍率

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

//----スキル発動関数-----//
//対象の陣営に生存者がいるかどうか
bool Skill::survivor(Character* c) {
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (c[i].get_hp() > 0)return true;
	}
	return false;
}

//スキル適用対象を1体決める 挑発やステルスも考慮する
int Skill::aim_decide(Character* c) {
	int aim = 0;//num == 0が暫定
	int candidate[3] = { -1,-1,-1 };//優先度-1が最低で、選ばれない
	int max_priority = 0;//3人中最も高い優先度
	//まずは優先度を計算
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (c[i].get_hp() > 0) {
			candidate[i] = 1;//まずは暫定普通の優先度
			if (c[i].get_state()->get_kind() == PROVOCATION) {
				candidate[i] = 2;//挑発は優先度高い
			}
			else if (c[i].get_state()->get_kind() == HIDE) {
				candidate[i] = 0;//ステルスは優先度低い
			}
			max_priority = max(max_priority, candidate[i]);
		}
	}
	//最も高い優先度を持つキャラのみをcandidateに左詰めする
	int now = 0;//最も高い優先度を持つキャラの数
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (candidate[i] == max_priority) {
			candidate[now] = i;//キャラの番号を入れる
			now++;
		}
	}
	return candidate[GetRand(now - 1)];
}

//攻撃関連
//攻撃をかわしたりカウンターしたり 防御力も考慮する d_flagがfalseなら考慮しない
void Skill::attack(Character& self, Character& c, int damage, int s_hit, int s_kaihi, int c_hit, bool d_flag) {
	if (c.get_state()->avoid() && GetRand(99) + 1 <= AVOID_RATE) {//攻撃をかわされた場合
		c.set_effect(15);//回避エフェクト
		PlaySoundMem(s_kaihi, DX_PLAYTYPE_BACK);//回避音
	}
	else {//攻撃を与える
		c.dec_hp(damage, d_flag);//hpが減る
		c.set_effect(0);
		c.cancel();//攻撃キャンセル
		if (c.get_state()->counter()) {//相手がカウンター状態なら
			self.dec_hp((int)(damage * COUNTER_RATE), d_flag);//hpが減る
			self.set_effect(9);
			PlaySoundMem(c_hit, DX_PLAYTYPE_BACK);//カウンター音
		}
		PlaySoundMem(s_hit, DX_PLAYTYPE_BACK);//hpが減る音
	}
}

//バフデバフ関連
//バフかデバフか判定して効果音とエフェクトを決める
void Skill::buff(Character& c, Change& set, int s_buff, int s_debuff, int kind) {
	if (set.get_value() >= 0) {
		c.set_effect(kind);//バフのエフェクト
		PlaySoundMem(s_buff, DX_PLAYTYPE_BACK);//バフの効果音
	}
	else {
		c.set_effect(kind+1);//デバフのエフェクト
		PlaySoundMem(s_debuff, DX_PLAYTYPE_BACK);//デバフの効果音
	}
}

//状態異常関連
//かわせる状態異常付与スキルもある かわせないスキルの場合はavoid_flagをfalseにするべし cancelも同様
void Skill::state(Character& c, Change& set, int s_hit, int s_kaihi, bool avoid_flag, bool cancel_flag) {
	if (avoid_flag && c.get_state()->avoid() && GetRand(99) + 1 <= AVOID_RATE) {//攻撃をかわされた場合
		c.set_effect(15);//回避エフェクト
		PlaySoundMem(s_kaihi, DX_PLAYTYPE_BACK);//回避音
	}
	else {//攻撃を与える
		if (set.demerit() && c.get_state()->vaccination()) {//予防接種で回避
			c.set_effect(16);//エフェクト
			return;
		}
		c.set_state(set);//セット
		switch (set.get_kind()) {
		case NORMAL:
		case VACCINATION:
			c.set_effect(16);//エフェクト
			break;
		case POISON:
			c.set_effect(7);//エフェクト
			break;
		case HIDE:
			c.set_effect(12);//エフェクト
			break;
		case PROVOCATION:
			c.set_effect(11);//エフェクト
			break;
		case AVOID:
			c.set_effect(8);//エフェクト
			break;
		case COUNTER:
			c.set_effect(10);//エフェクト
			break;
		case ENDURE:
			c.set_effect(13);//エフェクト
			break;
		case PARALYSIS:
			c.set_effect(14);//エフェクト
			break;
		}
		if (cancel_flag) { c.cancel(); }//攻撃キャンセル
		PlaySoundMem(s_hit, DX_PLAYTYPE_BACK);//hpが減る音
	}
}


//////////各スキルここから////////////////////
//1体に通常攻撃
void Damage1::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound){
	if (!survivor(enemy))return;//敵が全滅している
	int aim = target;
	if (aim == -1) { aim = aim_decide(enemy); }//対象を決める
	int damage = power;
	damage = self[num].attack_value(damage);//攻撃力バフデバフ補正
	attack(self[num], enemy[aim], damage, sound[0], sound[9], sound[11]);
}

//1体に防御力を無視した攻撃
void Damage2::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(enemy))return;//敵が全滅している
	int aim = target;
	if (aim == -1) { aim = aim_decide(enemy); }//対象を決める
	int damage = power;
	damage = self[num].attack_value(damage);//攻撃力バフデバフ補正
	attack(self[num], enemy[aim], damage, sound[0], sound[9], sound[11], false);
}

//1体に2回攻撃
void DamageDouble1::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(enemy))return;
	int aim = target;
	if (aim == -1) { aim = aim_decide(enemy); }//対象を決める
	int damage = power;
	damage = self[num].attack_value(damage);//攻撃力バフデバフ補正
	attack(self[num], enemy[aim], damage, sound[0], sound[9], sound[11]);//1回目
	attack(self[num], enemy[aim], damage, sound[0], sound[9], sound[11]);//2回目
}

//1体に2回攻撃
void DamageDouble2::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(enemy))return;
	int aim = target;
	if (aim == -1) { aim = aim_decide(enemy); }//対象を決める
	int damage = power;
	damage = self[num].attack_value(damage);//攻撃力バフデバフ補正
	attack(self[num], enemy[aim], damage, sound[0], sound[9], sound[11], false);//1回目
	attack(self[num], enemy[aim], damage, sound[0], sound[9], sound[11], false);//2回目
}

//全体攻撃
void DamageAll::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(enemy))return;
	int damage = power;
	damage = self[num].attack_value(damage);//攻撃力バフデバフ補正
	//全員が対象
	for (int i = 0; i < MAX_WITH_BATTLE; i++) { 
		if (enemy[i].get_hp() <= 0)continue;//hp <= 0は無効
		attack(self[num], enemy[i], damage, sound[0], sound[9], sound[11]);
	}
}

//攻撃バフ全体
void AttackUpAll_5sec::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(self))return;
	int v = power;
	//味方全員が対象
	Change c(ATTACK_NORMAL, 300, v);//バフの情報を持つオブジェクト
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (self[i].get_hp() <= 0)continue;//hp <= 0は無効
		self[i].set_attack(c);//バフをセット
		buff(self[i], c, sound[1], sound[2], E_ATTACK_UP);//バフデバフのセットと効果音
	}
}
//攻撃デバフ一体
void AttackDown_5sec::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(enemy))return;
	int v = -power;//デバフはすなわち負のバフ
	//敵一体が対象
	int aim = target;
	if (aim == -1) { aim = aim_decide(enemy); }//対象を決める
	Change c(ATTACK_NORMAL, 300, v);//デバフの情報を持つオブジェクト
	enemy[aim].set_attack(c);
	buff(enemy[aim], c, sound[1], sound[2], E_ATTACK_UP);//バフデバフのセットと効果音
}

//防御バフ全体
void DeffenseUpAll_5sec::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(self))return;
	int v = power;
	//味方全員が対象
	Change c(DEFFENSE_NORMAL, 300, v);
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (self[i].get_hp() <= 0)continue;//hp <= 0は無効
		self[i].set_deffense(c);
		buff(self[i], c, sound[3], sound[4], E_DEFFENSE_UP);
	}
}
//防御デバフ一体
void DeffenseDown_5sec::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(enemy))return;
	int v = -power;
	//敵一体が対象
	int aim = target;
	if (aim == -1) { aim = aim_decide(enemy); }//対象を決める
	Change c(DEFFENSE_NORMAL, 300, v);
	enemy[aim].set_deffense(c);
	buff(enemy[aim], c, sound[3], sound[4], E_DEFFENSE_UP);
}

//スピードバフ全体
void SpeedUpAll_5sec::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(self))return;
	int v = power;
	//味方全員が対象
	Change c(SPEED_NORMAL, 300, v);
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (self[i].get_hp() <= 0)continue;//hp <= 0は無効
		self[i].set_speed(c);//バフをセット
		buff(self[i], c, sound[5], sound[6], E_SPEED_UP);
	}
}
//スピードデバフ一体
void SpeedDown_5sec::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(enemy))return;
	int v = -power;
	//敵一体が対象
	int aim = target;
	if (aim == -1) { aim = aim_decide(enemy); }//対象を決める
	Change c(SPEED_NORMAL, 300, v);
	enemy[aim].set_speed(c);//デバフをセット
	buff(enemy[aim], c, sound[5], sound[6], E_SPEED_UP);
}

//マヒ一体
void Paralysis::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(enemy))return;
	int v = power;
	//敵一体が対象
	int aim = target;
	if (aim == -1) { aim = aim_decide(enemy); }//対象を決める
	Change c(PARALYSIS, v, v);
	state(enemy[aim], c, sound[7], sound[9]);
}
//毒一体
void Poison_5sec::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(enemy))return;
	int v = power;
	//敵一体が対象
	int aim = target;
	if (aim == -1) { aim = aim_decide(enemy); }//対象を決める
	Change c(POISON, 300, v);
	state(enemy[aim], c, sound[8], sound[9], true, false);//キャンセルは起きない(false)
}
//自身に我慢付与
void EndureSelf::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	int v = power;
	Change c(ENDURE, v, v);
	state(self[num], c, sound[17], sound[9], false, false);
	Change c2(DEFFENSE_RATE, v, 30);//ダメージは30％減少
	self[num].set_deffense(c2);
	Change c3(SPEED_RATE, v, 30);//スピードが30％上昇
	self[num].set_speed(c3);
}
//自身に回避付与
void AvoidSelf::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	int v = power;
	Change c(AVOID, v, v);
	state(self[num], c, sound[10], sound[9], false, false);
}
//自身にカウンター付与
void CounterSelf::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	int v = power;
	Change c(COUNTER, v, v);
	state(self[num], c, sound[12], sound[9], false, false);
}
//自身にステルス付与
void HideSelf::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	int v = power;
	Change c(HIDE, v, v);
	state(self[num], c, sound[14], sound[9], false, false);
	Change c2(ATTACK_RATE, v, 30);//攻撃力が30％上昇
	self[num].set_attack(c2);
	Change c3(SPEED_RATE, v, 30);//スピードが30％上昇
	self[num].set_speed(c3);
}
//自身に挑発付与
void ProvocationSelf_30::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	int v = power;
	Change c(PROVOCATION, v, v);
	state(self[num], c, sound[13], sound[9], false, false);
	Change c2(DEFFENSE_RATE, v, 30);//ダメージは30％減少
	self[num].set_deffense(c2);
}

//味方全体の体力を回復
void HpRepair::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	int v = -power;//回復だから負のダメージ
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (self[i].get_hp() <= 0)continue;//hp <= 0は無効
		self[i].change_hp(v, false);
	}
	PlaySoundMem(sound[16], DX_PLAYTYPE_BACK);//音
}

//アイテムで使用者のHPを回復
void HpRepairItem::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	int v = -power;//回復だから負のダメージ
	self[num].change_hp(v, false);
	PlaySoundMem(sound[16], DX_PLAYTYPE_BACK);//音
}

//味方全体の状態異常を回復
void StateRepair::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	int v = power;
	//味方全員が対象
	Change c(VACCINATION, v, v);//予防接種
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (self[i].get_hp() <= 0 || self[i].get_state()->merit())continue;//hp <= 0は無効
		state(self[i], c, -1, -1, false, false);//キャンセルと回避は起きない(false)
		PlaySoundMem(sound[15], DX_PLAYTYPE_BACK);//hpが減る音
	}
}

//シロナ
void Shirona::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(enemy))return;//敵が全滅している
	int aim = target;
	if (aim == -1) { aim = aim_decide(enemy); }//対象を決める
	int damage = power;
	attack(self[num], enemy[aim], damage, sound[0], sound[9], sound[11]);

	if (self[num].get_hp() <= 0)return;
	int v = 60;
	//使用者が対象
	Change c(DEFFENSE_NORMAL, 300, v);
	self[num].set_deffense(c);
	buff(self[num], c, sound[3], sound[4], E_DEFFENSE_UP);
}

//クロエ
void Kuroe::skill_trigger(Character* self, Character* enemy, int num, int target, int* sound) {
	if (!survivor(enemy))return;//敵が全滅している
	int aim = target;
	if (aim == -1) { aim = aim_decide(enemy); }//対象を決める
	int damage = power;
	attack(self[num], enemy[aim], damage, sound[0], sound[9], sound[11]);

	if (self[num].get_hp() <= 0)return;
	int v = 60;
	//使用者が対象
	Change c(ATTACK_NORMAL, 300, v);//バフの情報を持つオブジェクト
	self[num].set_attack(c);//バフをセット
	buff(self[num], c, sound[1], sound[2], E_ATTACK_UP);//バフデバフのセットと効果音
}