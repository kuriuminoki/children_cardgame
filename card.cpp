#include"card.h"
#include"character.h"
#include"DxLib.h"
#include<map>
#include<utility>

//�J�[�h�̃X�e�[�^�X�ɃA�b�v�f�[�g��K�p
bool Card::update() {
	switch (kind) {
	case 16:
		if (power < 10) {
			int h = hp, s = speed, r = rigid;
			set_state(1, 240, 1, 1);
			hp = h; speed = s; rigid = r;
			return true;
		}
		break;
	}
	return false;
}

void swap(Card* x, Card* y) {
	Card tmp;
	tmp = *x;
	*x = *y;
	*y = tmp;
}

int Deck::partition(int left, int right) {
	int i, j, pivot;
	i = left;
	j = right + 1;
	pivot = left;   // �擪�v�f��pivot�Ƃ���

	do {
		int p = cards[pivot].get_kind() * 10 - cards[pivot].get_rare();
		do { i++; } while (cards[i].get_kind() * 10 - cards[i].get_rare() < p);
		do { j--; } while (p < cards[j].get_kind() * 10 - cards[j].get_rare());
		// pivot��菬�������̂����ցA�傫�����̂��E��
		if (i < j) { swap(&cards[i], &cards[j]); }
	} while (i < j);

	swap(&cards[pivot], &cards[j]);   //pivot���X�V

	return j;
}

/* �N�C�b�N�\�[�g */
void Deck::quick_sort(int left, int right) {
	int pivot;

	if (left < right) {
		pivot = partition(left, right);
		quick_sort(left, pivot - 1);   // pivot�����ɍċA�I�ɃN�C�b�N�\�[�g
		quick_sort(pivot + 1, right);
	}
}

////////�f�b�L�֘A////////////

Deck::Deck() {
	size = 10;
	cards = new Card[size];
}

Deck::~Deck() {
	delete[] cards;
}

void Deck::operator=(const Deck& other) {
	size = other.size;
	Card* c = new Card[size];
	delete[] cards;
	cards = c;
	for (int i = 0; i < size; i++) {
		cards[i] = other.cards[i];//�J�[�h���R�s�[
	}
}

Card* Deck::get_cards() {
	return cards;
}

int Deck::get_size() {
	return size;
}

//�f�b�L�Ɉꖇ�ȏ�̃J�[�h���Ȃ����true �A�C�e���̓J�[�h�ł͂Ȃ�
bool Deck::empty() {
	for (int i = 0; i < size; i++) {
		if (cards[i].get_kind() > 0) {
			return false;
		}
	}
	return true;
}

//�f�b�L�̃J�[�h�̖��������߂�
void Deck::set_size(int n) {
	size = n;
	if (size > 0) {
		Card* c = new Card[size];
		delete[] cards;
		cards = c;
	}
	for (int i = 0; i < size; i++) {
		cards[i].set_kind(0);//�ŏ��̓J�[�h����̏��
	}
}

//�J�[�h���ꖇ�A�w��̃C���f�b�N�X�̃J�[�h�փR�s�[
void Deck::set_card(Card& card, int i) {
	if (i >= size || i < 0) { return; }
	cards[i] = card;
	//cards[i].load_card();
}

//�f�b�L�̃J�[�h�����ׂă��[�h�@�������摜�͊e��ވ�񂸂�
void Deck::load_cards() {
	int n=0;
	int sum;
	std::map<double, int> s;
	for (int i = 0; i < size; i++) {
		std::map<double, int>::iterator ite;
		double key = cards[i].get_kind() + (cards[i].get_rare() / 10.0);
		ite = s.find(key);
		if (ite == s.end()) {//�V���ɏo�������J�[�h
			cards[i].load_card();
			s.insert(std::make_pair(key, cards[i].get_handle()));
		}
		else {//���Ƀ��[�h�ς݂̃J�[�h
			n++;
			cards[i].set_handle(ite->second);
			cards[i].load_card(false);
		}
		sum = i;
	}
	sum = 0;
	n = 0;
}

//�f�b�L�̍��vHP��Ԃ�
int Deck::get_hp() {
	int hp = 0;
	for (int i = 0; i < size; i++) {
		if (cards[i].get_kind() != 0) {
			hp += cards[i].get_hp();
		}
	}
	return hp;
}

//�f�b�L�̖����𒲂ׂ�
void Deck::get_last(int& last) {
	last = size;//�J�[�h�͖��^��
	for (int i = 0; i < size; i++) {
		if (cards[i].get_kind() == 0) {
			last = i;//�J�[�h�̓C���f�b�N�Xlast���ŏ��̋�
			break;
		}
	}
}

void Deck::sort() {
	int last;
	get_last(last);
	quick_sort(0, last - 1);
}

//////////////////////////////////////
/////////�e�J�[�h�֘A��������/////////

Card::Card(int k, int ra, int h, int p, int s, int r) {
	kind = k;
	rare = ra;
	hp = h;
	power = p;
	speed = s;
	rigid = r;
	ability = NULL;
}

Card::~Card() {
	if (ability != NULL) {
		delete ability;
	}
}

//�Z�[�u�f�[�^�����[�h�����Ƃ��ɂ̂ݎg��
void Card::null_ability() {
	ability = NULL;
}

//�J�[�h�̃R�s�[
//����ŏ��߂ăf�b�L�̃J�[�h����ޕt�������
void Card::operator=(const Card& other) {
	hp = other.hp;
	kind = other.kind;
	rare = other.rare;
	power = other.power;
	speed = other.speed;
	rigid = other.rigid;
	if (ability == NULL) {
		load_card(false);
	}
	else {
		*ability = *other.ability;
	}
	handle = other.handle;
}

void Card::set_kind(int x) {
	kind = x;
}

void Card::set_handle(int h) {
	handle = h;
}

int Card::get_hp() {
	return hp;
}

int Card::get_power() {
	return power;
}

int Card::get_kind() {
	return kind;
}

int Card::get_speed() {
	return speed;
}

int Card::get_rigid() {
	return rigid;
}

int Card::get_handle() {
	return handle;
}

int Card::get_rare() {
	return rare;
}

Skill* Card::get_ability() {
	return ability;
}

void Card::trigger(Character* self, Character* enemy, int num, int target, int* handle) {
	ability->skill_trigger(self, enemy, num, target, handle);
	if (kind < 0) {
		kind = 0;//�A�C�e������
	}
}

const int SPEED_MIN = 10;
//�J�[�h�̃X�e�[�^�X�𗐐����܂߂Ĉꊇ�Z�b�g����
void Card::set_state(int h, int p, int s, int r) {
	double ra;
	double add;
	switch (rare) {
	case C_RARE:
		add = 1.0;
		break;
	case B_RARE:
		add = 1.4;
		break;
	case A_RARE:
		add = 1.4 * 1.4;
		break;
	case S_RARE:
		add = 1.4 * 1.4 * 1.4;
		break;
	}

	ra = 1.0 + (GetRand(49) + 1) / 100.0;//1.0�`1.5�{
	hp = (int)(h * ra * add);

	ra = 1.0 + (GetRand(49) + 1) / 100.0;//1.0�`1.5�{
	power = (int)(p * ra * add);

	ra = 1.0 + (GetRand(49) + 1) / 100.0;//1.0�`1.5�{
	speed = (int)(s * ra - (add-1.0)*10);
	if (speed < SPEED_MIN) { speed = SPEED_MIN; }//���̒l�ɂ͂Ȃ�Ȃ�

	ra = 1.0 + (GetRand(49) + 1) / 100.0;//1.0�`1.5�{
	rigid = (int)(r * ra - (add-1.0)*10);
	if (rigid < SPEED_MIN) { rigid = SPEED_MIN; }
}

//�^����ꂽ�m�������A���e�B�ɓK�p���ăZ�b�g����
void Card::set_rare(int c_rate, int b_rate, int a_rate, int s_rate) {
	int sum = c_rate + b_rate + a_rate + s_rate;
	int r = GetRand(sum-1);
	if (r < c_rate) {
		rare = C_RARE;
	}
	else if (r < c_rate + b_rate) {
		rare = B_RARE;
	}
	else if (r < c_rate + b_rate + a_rate) {
		rare = A_RARE;
	}
	else {
		rare = S_RARE;
	}
}

//�L�����̎�ނɉ����ă����_����������card_kind�ŃJ�[�h�̎�ނ�Ԃ�
void reward_card(int kind, Card& c, int c_rate, int b_rate, int a_rate, int s_rate, int card_kind) {
	c.set_rare(c_rate, b_rate, a_rate, s_rate);
	if (card_kind > 0) {
		normal_status(card_kind, c);
	}
	else {
		//�Ⴆ�΃A�J�c�L(kind == 1)�Ȃ�J�[�h�̎�ނ� 1, 11, 21, 31...
		int r = GetRand(CARD_SUM[kind - 1] - 1) * 10 + kind;
		c.set_kind(r);
		normal_status(r, c);
	}
}

//�e�J�[�h�̔��p���z
int sell_money(int kind, int rare) {
	if (kind == 0)return 0;
	switch (kind) {
	case -1:
		return 10;
	case -2:
	case -3:
		return 50;
	}
	if (kind >= 1 && kind <= 30) {
		return 10 * (rare+1) * (rare+1);
	}
	return 0;
}

//�V���b�v�Ŕ����̂ɕK�v�ȋ��z�i�A�C�e���̂݁j
int buy_money(int kind) {
	switch (kind) {
	case -1:
		return 200;
		break;
	case -2:
	case -3:
		return 500;
		break;
	}
	return 0;
}

//�J�[�h�̊�{�X�e�[�^�X
void normal_status(int kind, Card& c) {
	if (kind == 0)return;
	switch (kind) {
	//�A�C�e��
	case -1://�񕜖�
		c.set_state(0, 80, 30, 30);
		break;
	case -2://�V���i
		c.set_state(0, 80, 50, 10);
		break;
	case -3://�N���G
		c.set_state(0, 80, 50, 10);
		break;
	//�A�J�c�L
	case 1:
		c.set_state(100, 30, 100, 115);
		break;
	case 11:
		c.set_state(100, 55, 95, 110);
		break;
	case 21:
		c.set_state(100, 27, 90, 110);
		break;
	//�n���_
	case 2:
		c.set_state(100, 50, 60, 70);
		break;
	case 12:
		c.set_state(100, 20, 100, 120);
		break;
	case 22:
		c.set_state(100, 50, 100, 125);
		break;
	//�I�I�^
	case 3:
		c.set_state(110, 70, 100, 120);
		break;
	case 13:
		c.set_state(110, 240, 50, 30);
		break;
	case 23:
		c.set_state(110, 90, 125, 140);
		break;
	//�C�V
	case 4:
		c.set_state(100, 240, 100, 150);
		break;
	case 14:
		c.set_state(100, 15, 90, 120);
		break;
	case 24:
		c.set_state(100, 25, 85, 90);
		break;
	//�^�C��
	case 5:
		c.set_state(97, 50, 65, 85);
		break;
	case 15:
		c.set_state(100, 240, 40, 90);
		break;
	case 25:
		c.set_state(115, 65, 110, 130);
		break;
	//�I���_
	case 6:
		c.set_state(100, 30, 120, 150);
		break;
	case 16:
		c.set_state(100, 240, 60, 60);//c.set_state(100, 1, 60, 60);
		break;
	case 26:
		c.set_state(100, 60, 90, 90);
		break;
	//�~���g�l
	case 7:
		c.set_state(120, 40, 60, 70);
		break;
	case 17:
		c.set_state(120, 180, 70, 90);
		break;
	case 27:
		c.set_state(120, 50, 113, 125);
		break;
	//�E�`���g
	case 8:
		c.set_state(100, 33, 50, 60);
		break;
	case 18:
		c.set_state(100, 45, 25, 80);
		break;
	case 28:
		c.set_state(100, 50, 50, 83);
		break;
	//�C�o���J��
	case 9:
		c.set_state(95, 180, 50, 32);
		break;
	case 19:
		c.set_state(100, 240, 63, 70);
		break;
	case 29:
		c.set_state(90, 60, 70, 100);
		break;
	//�g���`
	case 10:
		c.set_state(105, 50, 63, 85);
		break;
	case 20:
		c.set_state(105, 65, 95, 105);
		break;
	case 30:
		c.set_state(105, 55, 85, 95);
		break;
	}
}

//�J�[�h�̃��[�h kind�̈�̈ʂŃL�����𔻒f���邱��
void Card::load_card(bool load_flag) {
	std::string path;
	switch (rare) {
	case C_RARE:
		path = "card/C/";
		break;
	case B_RARE:
		path = "card/B/";
		break;
	case A_RARE:
		path = "card/A/";
		break;
	case S_RARE:
		path = "card/S/";
		break;
	}
	if (kind == 0)return;
	switch (kind) {
	//�A�C�e��
	case -1:
		ability = new HpRepairItem;
		ability->set_text("�g�p�҂̗̑͂���");
		if (load_flag)handle = LoadGraph("card/item/�񕜖�.png");
		break;
	case -2:
		ability = new Shirona;
		ability->set_text("�V���i���U���������Ȃ��A�g�p�҂̖h��͂�60�㏸����");
		if (load_flag)handle = LoadGraph("card/item/�V���i.png");
		break;
	case -3:
		ability = new Kuroe;
		ability->set_text("�N���G���U���������Ȃ��A�g�p�҂̍U���͂�60�㏸����");
		if (load_flag)handle = LoadGraph("card/item/�N���G.png");
		break;
	/////�A�J�c�L/////
	case 1:
		ability = new DamageDouble1;
		ability->set_text("�G1�̂ւ̒ʏ�U��(2��)");
		path += "�A�J�c�L1.png";
		break;
	case 11:
		ability = new Damage2;
		ability->set_text("�G1�̂ւ̖h��͂𖳎������U��");
		path += "�A�J�c�L2.png";
		break;
	case 21:
		ability = new DamageDouble2;
		ability->set_text("�G1�̂ւ̖h��͂𖳎������U��(2��)");
		path += "�A�J�c�L3.png";
		break;
	/////�n���_/////
	case 2:
		ability = new AttackUpAll_5sec;
		ability->set_text("�����S�̂̍U���͏㏸(5�b��)");
		path += "�n���_1.png";
		break;
	case 12:
		ability = new HpRepair;
		ability->set_text("�����S�̗̂̑͂���");
		path += "�n���_2.png";
		break;
	case 22:
		ability = new Damage1;
		ability->set_text("�G1�̂ւ̒ʏ�U��");
		path += "�n���_3.png";
		break;
	/////�I�I�^/////
	case 3:
		ability = new Damage1;
		ability->set_text("�G1�̂֋��߂̒ʏ�U��");
		path += "�I�I�^1.png";
		break;
	case 13:
		ability = new EndureSelf;
		ability->set_text("�U�����L�����Z�����ꂸ�A�h��͂ƃX�s�[�h���R�O���A�b�v");
		path += "�I�I�^2.png";
		break;
	case 23:
		ability = new Damage1;
		ability->set_text("�G1�̂ւ��Ȃ苭���ʏ�U��");
		path += "�I�I�^3.png";
		break;
	/////�C�V/////
	case 4:
		ability = new Paralysis;
		ability->set_text("�G1�̂Ƀ}�q��t�^");
		path += "�C�V1.png";
		break;
	case 14:
		ability = new Poison_5sec;
		ability->set_text("�G1�̂�5�b�Ԃ̓ł�t�^");
		path += "�C�V2.png";
		break;
	case 24:
		ability = new SpeedUpAll_5sec;
		ability->set_text("�����S�̂̃X�s�[�h�㏸(5�b��)");
		path += "�C�V3.png";
		break;
	/////�^�C��/////
	case 5:
		ability = new AttackDown_5sec;
		ability->set_text("�G1�̂̍U���͌���(5�b��)");
		path += "�^�C��1.png";
		break;
	case 15:
		ability = new HideSelf;
		ability->set_text("�G�ɑ_���ɂ����Ȃ�A�U���͂ƃX�s�[�h���R�O���A�b�v");
		path += "�^�C��2.png";
		break;
	case 25:
		ability = new Damage1;
		ability->set_text("�G1�̂ւ̒ʏ�U��");
		path += "�^�C��3.png";
		break;
	/////�I���_/////
	case 6:
		ability = new DamageAll;
		ability->set_text("�G�S�̂ւ̒ʏ�U��");
		path += "�I���_2.png";
		break;
	case 16:
		ability = new StateRepair;
		ability->set_text("�L�v�ȏ�Ԉُ�������Ȃ������S���ɗ\�h�ڎ��t�^");
		path += "�I���_1.png";
		break;
	case 26:
		ability = new Damage1;
		ability->set_text("�G1�̂ւ̒ʏ�U��");
		path += "�I���_3.png";
		break;
	/////�~���g�l
	case 7:
		ability = new DeffenseUpAll_5sec;
		ability->set_text("�����S�̖̂h��͏㏸(5�b��)");
		path += "�~���g�l1.png";
		break;
	case 17:
		ability = new ProvocationSelf_30;
		ability->set_text("�G�ɑ_���₷���Ȃ�A�_���[�W�͂R�O���J�b�g�����");
		path += "�~���g�l2.png";
		break;
	case 27:
		ability = new Damage1;
		ability->set_text("�G1�̂ւ̒ʏ�U��");
		path += "�~���g�l3.png";
		break;
	/////�E�`���g/////
	case 8:
		ability = new SpeedDown_5sec;
		ability->set_text("�G1�̂̃X�s�[�h����(5�b��)");
		path += "�E�`���g1.png";
		break;
	case 18:
		ability = new Damage1;
		ability->set_text("�G1�̂ւ̂��Ȃ�f�����ʏ�U��");
		path += "�E�`���g2.png";
		break;
	case 28:
		ability = new Damage1;
		ability->set_text("�G1�̂ւ̑f�����ʏ�U��");
		path += "�E�`���g3.png";
		break;
	/////�C�o���J��/////
	case 9:
		ability = new AvoidSelf;
		ability->set_text("��莞�ԍU����70%%�̊m���ŉ��");
		path += "�C�o���J��3.png";
		break;
	case 19:
		ability = new CounterSelf;
		ability->set_text("��莞�ԓG����̍U����1.2�{�Ŕ�������");
		path += "�C�o���J��2.png";
		break;
	case 29:
		ability = new Damage1;
		ability->set_text("�G1�̂ւ̒ʏ�U��");
		path += "�C�o���J��1.png";
		break;
	/////�g���`/////
	case 10:
		ability = new DeffenseDown_5sec;
		ability->set_text("�G1�̖̂h��͌���(5�b��)");
		path += "�g���`1.png";
		break;
	case 20:
		ability = new Damage1;
		ability->set_text("�G1�̂֏������߂̒ʏ�U��");
		path += "�g���`2.png";
		break;
	case 30:
		ability = new Damage1;
		ability->set_text("�G1�̂֏����f�����ʏ�U��");
		path += "�g���`3.png";
		break;
	}
	if (load_flag && kind > 0)handle = LoadGraph(path.c_str());
	ability->set_power(power);
}