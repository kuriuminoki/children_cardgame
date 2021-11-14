#include"character.h"
#include"DxLib.h"

#define HP_BAR_HEIGHT 10
#define HP_BAR_LEN 120
#define WAIT_BAR_HEIGHT 10
#define HP_X 40
#define HP_Y 230
#define EX -18
#define EY -190//-38

void icon_judge(std::vector<Change>& c, bool& buff, bool& debuff);
void state_judge(int x, int y, int handle[], int state, int value);

////��ԕω��N���X/////////////////////////
Change::Change(int k, int c, int v) {
	kind = k;
	cnt = c;
	value = v;
}

void Change::operator=(const Change& other) {
	kind = other.kind;
	cnt = other.cnt;
	value = other.value;
}

//��ԕω��̃J�E���g������
int Change::cnt_dec() {
	if (cnt > 0)cnt--;
	if (cnt == 0)return true;
	return false;
}

int Change::get_kind() {
	return kind;
}
int Change::get_value() {
	return value;
}

//�U���͊֘A
void Change::attack(int& sum) {
	switch (kind) {
	case ATTACK_NORMAL://���Z
		sum += value;//value�����Ȃ�o�t sum(�^�_���[�W)�͑���
		break;
	case ATTACK_RATE://����
		sum = (int)(sum * ((100.0 + value) / 100));//value�����Ȃ�o�t value%����
		break;
	}
}

//�h��͊֘A
void Change::deffense(int& sum) {
	switch (kind) {
	case DEFFENSE_NORMAL://���Z
		sum -= value;//value�����Ȃ�o�t sum(�_���[�W)�͌���
		break;
	case DEFFENSE_RATE://����
		sum = (int)(sum * ((100.0 - value) / 100));//value�����Ȃ�o�t value%����
		break;
	}
}

//�X�s�[�h�֘A
void Change::speed(int& sum) {
	switch (kind) {
	case SPEED_NORMAL://���Z
		sum -= value;//value�����Ȃ�o�t sum(�҂�����)�͒Z���Ȃ�
		break;
	case SPEED_RATE://����
		sum = (int)(sum * ((100.0 - value) / 100));//value�����Ȃ�o�t value%����
		break;
	}
}

//��Ԉُ�֘A
//��Ԉُ�ɂ���ē����Ȃ��Ȃ��Ă���Ȃ�false��Ԃ�
bool Change::can_move() {
	switch (kind) {
	case PARALYSIS://�}�q
		return false;
	}
	return true;
}
//��Ԉُ�ɂ��_���[�W���
int Change::damage() {
	switch (kind) {
	case POISON://��
		if (cnt % 60 == 1) {
			return value;//����I�Ƀ_���[�W
		}
		break;
	}
	return 0;//�_���[�W�Ȃ�
}
//�L�����Z����ς��邱�Ƃ��ł����ԂȂ�true��Ԃ�
bool Change::endure() {
	switch (kind) {
	case ENDURE:
	case PROVOCATION:
		return true;
	}
	return false;
}
//�U�������킹���ԂȂ�true��Ԃ�
bool Change::avoid() {
	switch (kind) {
	case AVOID:
		return true;
	}
	return false;
}
//��Ԉُ�����킹���ԂȂ�true��Ԃ�
bool Change::vaccination() {
	switch (kind) {
	case VACCINATION:
		return true;
	}
	return false;
}
//�J�E���^�[�ł����ԂȂ�true��Ԃ�
bool Change::counter() {
	switch (kind) {
	case COUNTER:
		return true;
	}
	return false;
}
//�s���v�������炷��Ԉُ�������Ă���Ȃ�true��Ԃ�
bool Change::demerit() {
	switch (kind) {
	case POISON:
	case PARALYSIS:
		return true;
	}
	return false;
}
//���v�������炷��Ԉُ�������Ă���Ȃ�true��Ԃ�
bool Change::merit() {
	switch (kind) {
	case COUNTER:
	case AVOID:
	case ENDURE:
	case PROVOCATION:
	case HIDE:
		return true;
	}
	return false;
}
/////////////////////////////////////////////

//�o�t�ƃf�o�t�����ꂼ�ꑶ�݂��邩���m�F�@����Ȃ�true
void icon_judge(std::vector<Change>& c, bool& buff, bool& debuff) {
	for (unsigned int i = 0; i < c.size(); i++) {
		if (c[i].get_value() >= 0)buff = true;
		else { debuff = true; }
		if (buff && debuff)break;
	}
}

//��Ԉُ�̃A�C�R����\��
void state_judge(int x, int y, int handle[], int state, int value) {
	const double scale = 0.7;
	switch (state) {
	case NORMAL:
		break;
	case POISON:
		if (value >= 0) {//��
			DrawRotaGraph(x, y, scale, 0, handle[6], TRUE, FALSE);
		}
		else {//��
			DrawRotaGraph(x, y, scale, 0, handle[7], TRUE, FALSE);
		}
		break;
	case PARALYSIS://���
		DrawRotaGraph(x, y, scale, 0, handle[8], TRUE, FALSE);
		break;
	case HIDE://�X�e���X
		DrawRotaGraph(x, y, scale, 0, handle[11], TRUE, FALSE);
		break;
	case PROVOCATION://����
		DrawRotaGraph(x, y, scale, 0, handle[10], TRUE, FALSE);
		break;
	case AVOID://���
		DrawRotaGraph(x, y, scale, 0, handle[9], TRUE, FALSE);
		break;
	case COUNTER://�J�E���^�[
		DrawRotaGraph(x, y, scale, 0, handle[12], TRUE, FALSE);
		break;
	case ENDURE://�䖝
		DrawRotaGraph(x, y, scale, 0, handle[13], TRUE, FALSE);
		break;
	case VACCINATION://�\�h�ڎ�
		DrawRotaGraph(x, y, scale, 0, handle[14], TRUE, FALSE);
		break;
	}
}

/////////�L�����N�^�[�֘A//////////////////
Character::Character() {
	max_hp = 0;
	hp = 0;
	php = 0;
	wait_cnt = 0;
	event_cnt = 0;
	kind = 0;
	next = -1;
	now = 0;
	wait_goal = 10;
	effect_cnt = 0;
	effect_kind = -1;
	cancel_flag = false;
	stand_handle = -1;
	damage_font = CreateFontToHandle(NULL, 25, 7);//���������Ƃ��̃t�H���g
}

Friend::Friend(int n) {
	num = n;
	switch (num) {
	case 0:
		x = 0;
		y = 230;
		break;
	case 1:
		x = 200;
		y = 230;
		break;
	case 2:
		x = 400;
		y = 230;
		break;
	}
}

Enemy::Enemy(int n) {
	num = n;
	switch (num) {
	case 0:
		x = 40;
		y = 0;
		break;
	case 1:
		x = 240;
		y = 0;
		break;
	case 2:
		x = 440;
		y = 0;
		break;
	}
}

//BF���󂯎�����L�����̏��������ŗp�ӂ����I�u�W�F�N�g��
//�R�s�[���邽�߂Ɏg���Bnum��x,y��BF������w�肵�������Ō���
void Character::operator=(const Character& other) {
	kind = other.kind;
	if (kind == 0)return;//�L�����Ȃ�
	huku = other.huku;
	wait_cnt = other.wait_cnt;
	deck = other.deck;
	stand_handle = other.stand_handle;
	max_hp = deck.get_hp();
	hp = max_hp;
	php = max_hp;
	size_x = other.size_x;
	size_y = other.size_y;
}

void Friend::load_handle() {
	switch (kind) {//kind��huku�ɉ����ă��[�h����
	case AKATSUKI:
		stand_handle = LoadGraph("stand/�A�J�c�L.png");
		break;
	case HANDA:
		stand_handle = LoadGraph("stand/�n���_.png");
		break;
	case OOTA:
		stand_handle = LoadGraph("stand/�I�I�^.png");
		break;
	case ISHI:
		stand_handle = LoadGraph("stand/�C�V.png");
		break;
	case TAIRA:
		stand_handle = LoadGraph("stand/�^�C��.png");
		break;
	case OWADA:
		stand_handle = LoadGraph("stand/�I���_.png");
		break;
	case MIYATONE:
		stand_handle = LoadGraph("stand/�~���g�l.png");
		break;
	case UCHIMOTO:
		stand_handle = LoadGraph("stand/�E�`���g.png");
		break;
	case IBARAKAWA:
		stand_handle = LoadGraph("stand/�C�o���J��.png");
		break;
	case TOMOCHI:
		stand_handle = LoadGraph("stand/�g���`.png");
		break;
	}
	GetGraphSize(stand_handle, &size_x, &size_y);
	size_x = (int)(size_x * 0.75);
	size_y = (int)(size_y * 0.75);
}

void Enemy::load_handle() {
	switch (kind) {
	case O_T:
		stand_handle = LoadGraph("enemy/O.png");
		break;
	case P_T:
		stand_handle = LoadGraph("enemy/P.png");
		break;
	case Q_T:
		stand_handle = LoadGraph("enemy/Q.png");
		break;
	case R_T:
		stand_handle = LoadGraph("enemy/R.png");
		break;
	case S_T:
		stand_handle = LoadGraph("enemy/S.png");
		break;
	case T_T:
		stand_handle = LoadGraph("enemy/T.png");
		break;
	case U_T:
		stand_handle = LoadGraph("enemy/U.png");
		break;
	case V_T:
		stand_handle = LoadGraph("enemy/V.png");
		break;
	case W_T:
		stand_handle = LoadGraph("enemy/W.png");
		break;
	case X_T:
		stand_handle = LoadGraph("enemy/X.png");
		break;
	case Y_T:
		stand_handle = LoadGraph("enemy/Y.png");
		break;
	case Z_T:
		stand_handle = LoadGraph("enemy/Z.png");
		break;
	}
	GetGraphSize(stand_handle, &size_x, &size_y);
	size_x = (int)(size_x * 0.75);
	size_y = (int)(size_y * 0.75);
}

//�����G�\��
void Friend::picture() {
	if (stand_handle != -1 && hp == 0) {//hp��0�ɂȂ��Ă��ꂽ�Ƃ�
		SetDrawBright(100, 100, 100);
		DrawRotaGraph2(x, y, 0, 0, 1, 0, stand_handle, TRUE, FALSE);
		SetDrawBright(255, 255, 255);
	}
	else if (event_cnt > 0 && event_cnt < ONE_SECOND) {
		if(event_cnt % 4 < 2){ SetDrawBright(100, 100, 100); }
		DrawRotaGraph2(x, y, 0, 0, 1, 0, stand_handle, TRUE, FALSE);
		SetDrawBright(255, 255, 255);
	}
	else if (next == -1 && wait_cnt > 0) {
		SetDrawBright(100, 100, 100);
		DrawRotaGraph2(x, y, 0, 0, 1, 0, stand_handle, TRUE, FALSE);
		SetDrawBright(255, 255, 255);
	}
	else {
		DrawRotaGraph2(x, y, 0, 0, 1, 0, stand_handle, TRUE, FALSE);
	}
}

//�}�E�X�J�[�\���������G�ɏd�Ȃ��Ă��邩�ǂ���
int Character::mouse_hit() {
	int hand_x, hand_y;
	GetMousePoint(&hand_x, &hand_y);
	if (hand_x >= x && hand_x < x + size_x && hand_y >= y && hand_y < y + size_y) {
		return num;
	}
	return -1;
}

//�}�E�X�J�[�\�����d�Ȃ��Ă���Ȃ�g�ň͂�
void Character::picture_flame() {
	if (mouse_hit() == num) {
		DrawBox(x, y, x + size_x, y + size_y, GetColor(0, 255, 255), FALSE);
	}
}

//�^�[�Q�b�e�B���O�G
void Character::picture_red_circle() {
	const int r = 40;
	const int cx = (int)(x + (size_x * 0.75 / 2)) + 20;
	const int cy = (int)(y + (size_y * 0.75 / 2)) + 50;
	DrawCircle(cx, cy, r + 1, GetColor(255,0,0), false);
	DrawCircle(cx, cy, r, GetColor(255, 0, 0), false);
	DrawCircle(cx, cy, r - 1, GetColor(255, 0, 0), false);
	DrawLine(cx - r, cy - 1, cx + r, cy - 1, GetColor(255, 0, 0));
	DrawLine(cx - r, cy, cx + r, cy, GetColor(255, 0, 0));
	DrawLine(cx - r, cy + 1, cx + r, cy + 1, GetColor(255, 0, 0));
	DrawLine(cx - 1, cy - r, cx - 1, cy + r, GetColor(255, 0, 0));
	DrawLine(cx, cy - r, cx, cy + r, GetColor(255, 0, 0));
	DrawLine(cx + 1, cy - r, cx + 1, cy + r, GetColor(255, 0, 0));
}

//�����G�\��
void Enemy::picture() {
	picture_flame();
	if (stand_handle != -1 && hp == 0) {
		SetDrawBright(100, 100, 100);
		DrawRotaGraph2(x, y + 20, 0, 0, 0.75, 0, stand_handle, TRUE, FALSE);
		SetDrawBright(255, 255, 255);
	}
	else if (event_cnt > 0 && event_cnt < ONE_SECOND) {
		if (event_cnt % 4 < 2) { SetDrawBright(100, 100, 100); }
		DrawRotaGraph2(x, y + 20, 0, 0, 0.75, 0, stand_handle, TRUE, FALSE);
		SetDrawBright(255, 255, 255);
	}
	else if (next == -1 && wait_cnt > 0) {
		SetDrawBright(100, 100, 100);
		DrawRotaGraph2(x, y + 20, 0, 0, 0.75, 0, stand_handle, TRUE, FALSE);
		SetDrawBright(255, 255, 255);
	}
	else {
		DrawRotaGraph2(x, y + 20, 0, 0, 0.75, 0, stand_handle, TRUE, FALSE);
	}
}

//set�֘A
void Character::kind_set(int k) {
	kind = k;
	load_handle();//�摜�̃��[�h
}

void Character::huku_set(int h) {
	huku = h;
	load_handle();//�摜�̃��[�h
}

void Friend::picture_hp() {
	int hp_rate = (int)((double)HP_BAR_LEN * ((double)hp / (double)max_hp));
	int hp_rate2 = (int)((double)HP_BAR_LEN * ((double)php / (double)max_hp));
	if (max_hp == 0) { hp_rate = 0; hp_rate2 = 0; }
	int px = x + HP_X, py = y + HP_Y;
	DrawBox(px - 6, py - 3,px + HP_BAR_LEN + 6, py + HP_BAR_HEIGHT + 3, GetColor(0, 0, 0), TRUE);//�g
	DrawBox(px - 5, py - 2, px + HP_BAR_LEN + 5, py + HP_BAR_HEIGHT + 2, GetColor(100, 100, 100), TRUE);//�g
	if (hp <= php) {//�_���[�W
		DrawBox(px, py, px + hp_rate, py + HP_BAR_HEIGHT, GetColor(0, 255, 0), TRUE);//��hp
		DrawBox(px + hp_rate, py, px + hp_rate2, py + HP_BAR_HEIGHT, GetColor(255, 0, 0), TRUE);//��php
		DrawBox(px + hp_rate2, py, px + HP_BAR_LEN, py + HP_BAR_HEIGHT, GetColor(0, 0, 0), TRUE);//�N����
	}
	else {//��
		DrawBox(px, py, px + hp_rate2, py + HP_BAR_HEIGHT, GetColor(0, 255, 0), TRUE);//��hp
		DrawBox(px + hp_rate2, py, px + hp_rate, py + HP_BAR_HEIGHT, GetColor(100, 100, 255), TRUE);//���Fphp
		DrawBox(px + hp_rate, py, px + HP_BAR_LEN, py + HP_BAR_HEIGHT, GetColor(0, 0, 0), TRUE);//�N����
	}
	if (next != -1) {
		DrawFormatString(x + 31, y + 11, GetColor(0, 0, 0), "���s�҂�:%d����", next + 1);
		DrawFormatString(x + 29, y + 9, GetColor(0, 0, 0), "���s�҂�:%d����", next + 1);
		DrawFormatString(x + 30, y + 10, GetColor(255, 255, 255), "���s�҂�:%d����", next + 1);
	}
}

void Friend::picture_wait() {
	int wait_rate;
	if (wait_goal == 0) { wait_rate = HP_BAR_LEN; }
	else { wait_rate = (int)((double)HP_BAR_LEN * ((double)wait_cnt / (double)wait_goal)); }
	int px = x + HP_X, py = y + HP_Y - 20;
	DrawBox(px - 6, py - 3, px + HP_BAR_LEN + 6, py + HP_BAR_HEIGHT + 3, GetColor(0, 0, 0), TRUE);
	DrawBox(px - 5, py - 2, px + HP_BAR_LEN + 5, py + HP_BAR_HEIGHT + 2, GetColor(100, 100, 100), TRUE);
	DrawBox(px, py, px + wait_rate, py + HP_BAR_HEIGHT, GetColor(255, 100, 100), TRUE);
	if (next != -1 || wait_cnt == 0) {
		DrawBox(px + wait_rate, py, px + HP_BAR_LEN, py + HP_BAR_HEIGHT, GetColor(0, 0, 255), TRUE);
	}
	else {
		DrawBox(px + wait_rate, py, px + HP_BAR_LEN, py + HP_BAR_HEIGHT, GetColor(255, 255, 0), TRUE);
	}
}

void Friend::picture_state() {
	if (kind == -1)return;
	if ((next == -1 && wait_cnt > 0 && wait_cnt % 8 < 4) || hp <= 0) {
		DrawBox(x + 54, y + 124, x + 141, y + 151, GetColor(255, 50, 50), TRUE);
		DrawBox(x + 55, y + 125, x + 140, y + 150, GetColor(100, 100, 100), TRUE);
		if (hp > 0) {
			DrawFormatString(x + 73, y + 130, GetColor(255, 255, 0), "�d����");
		}
		else {
			DrawFormatString(x + 63, y + 130, GetColor(255, 0, 0), "�s���s�\");
		}
	}
	//�f�o�b�O�p
	/*DrawBox(x + 54, y + 174, x + 161, y + 201, GetColor(255, 50, 50), TRUE);
	DrawBox(x + 55, y + 175, x + 160, y + 200, GetColor(150, 150, 150), TRUE);
	DrawFormatString(x + 63, y + 180, GetColor(255, 255, 255), "hp = %d", hp);*/
}

void Friend::picture_icon(int icon[]) {
	int px = x + HP_X, py = y + HP_Y - 40;
	bool buff[6] = { false };
	icon_judge(attack, buff[0], buff[1]);
	icon_judge(deffense, buff[2], buff[3]);
	icon_judge(speed, buff[4], buff[5]);
	for (int i = 0; i < 6; i++) {
		if (buff[i]) {
			DrawRotaGraph(px + 25 * i, py, 0.6, 0, icon[i], TRUE, FALSE);
		}
	}
	state_judge(px - 20, py + 35, icon, state.get_kind(), state.get_value());
}

void Enemy::picture_hp() {
	int hp_rate = (int)((double)HP_BAR_LEN * ((double)hp / (double)max_hp));
	int hp_rate2 = (int)((double)HP_BAR_LEN * ((double)php / (double)max_hp));
	if (max_hp == 0) { hp_rate = 0; hp_rate2 = 0; }
	int px = x + HP_X + EX, py = y + HP_Y + EY - 10;
	DrawBox(px - 6, py - 3, px + HP_BAR_LEN + 6, py + HP_BAR_HEIGHT + 3, GetColor(0, 0, 0), TRUE);
	DrawBox(px - 5, py - 2, px + HP_BAR_LEN + 5, py + HP_BAR_HEIGHT + 2, GetColor(100, 100, 100), TRUE);
	if (hp <= php) {//�_���[�W
		DrawBox(px, py, px + hp_rate, py + HP_BAR_HEIGHT, GetColor(0, 255, 0), TRUE);
		DrawBox(px + hp_rate, py, px + hp_rate2, py + HP_BAR_HEIGHT, GetColor(255, 0, 0), TRUE);
		DrawBox(px + hp_rate2, py, px + HP_BAR_LEN, py + HP_BAR_HEIGHT, GetColor(0, 0, 0), TRUE);
	}
	else {
		DrawBox(px, py, px + hp_rate2, py + HP_BAR_HEIGHT, GetColor(0, 255, 0), TRUE);
		DrawBox(px + hp_rate2, py, px + hp_rate, py + HP_BAR_HEIGHT, GetColor(100, 100, 255), TRUE);
		DrawBox(px + hp_rate, py, px + HP_BAR_LEN, py + HP_BAR_HEIGHT, GetColor(0, 0, 0), TRUE);
	}
	if (next != -1) {
		DrawFormatString(x + 9, y + 209, GetColor(0, 0, 0), "���s�҂�:%d����", next + 1);
		DrawFormatString(x + 11, y + 211, GetColor(0, 0, 0), "���s�҂�:%d����", next + 1);
		DrawFormatString(x + 10, y + 210, GetColor(255, 255, 255), "���s�҂�:%d����", next + 1);
	}
}

void Enemy::picture_wait() {
	int wait_rate;
	if (wait_goal == 0) { wait_rate = HP_BAR_LEN; }
	else { wait_rate = (int)((double)HP_BAR_LEN * ((double)wait_cnt / (double)wait_goal)); }
	int px = x + HP_X + EX, py = y + HP_Y - 30 + EY;
	DrawBox(px - 6, py - 3, px + HP_BAR_LEN + 6, py + HP_BAR_HEIGHT + 3, GetColor(0, 0, 0), TRUE);
	DrawBox(px - 5, py - 2, px + HP_BAR_LEN + 5, py + HP_BAR_HEIGHT + 2, GetColor(100, 100, 100), TRUE);
	DrawBox(px, py, px + wait_rate, py + HP_BAR_HEIGHT, GetColor(255, 100, 100), TRUE);
	if (next != -1 || wait_cnt == 0) {
		DrawBox(px + wait_rate, py, px + HP_BAR_LEN, py + HP_BAR_HEIGHT, GetColor(0, 0, 255), TRUE);
	}
	else {
		DrawBox(px + wait_rate, py, px + HP_BAR_LEN, py + HP_BAR_HEIGHT, GetColor(255, 255, 0), TRUE);
	}
}

void Enemy::picture_state() {
	if (kind == -1)return;
	if ((next == -1 && wait_cnt > 0 && wait_cnt % 8 < 4) || hp <= 0) {
		DrawBox(x + 36, y + 114, x + 123, y + 141, GetColor(255, 50, 50), TRUE);
		DrawBox(x + 37, y + 115, x + 122, y + 140, GetColor(100, 100, 100), TRUE);
		if (hp > 0) {
			DrawFormatString(x + 55, y + 120, GetColor(255, 255, 0), "�d����");
		}
		else {
			DrawFormatString(x + 45, y + 120, GetColor(255, 0, 0), "�s���s�\");
		}
	}
	//DrawBox(x + 36, y + 164, x + 123, y + 191, GetColor(255, 50, 50), TRUE);
	//DrawBox(x + 37, y + 165, x + 122, y + 190, GetColor(150, 150, 150), TRUE);
	//DrawFormatString(x + 45, y + 170, GetColor(255, 255, 255), "deffense = %d", deffense.size());
}

void Enemy::picture_icon(int icon[]) {
	int px = x + HP_X + EX, py = y + HP_Y + 20 + EY;
	bool buff[6] = { false };
	icon_judge(attack, buff[0], buff[1]);
	icon_judge(deffense, buff[2], buff[3]);
	icon_judge(speed, buff[4], buff[5]);
	for (int i = 0; i < 6; i++) {
		if (buff[i]) {
			DrawRotaGraph(px + 25 * i, py, 0.6, 0, icon[i], TRUE, FALSE);
		}
	}
	state_judge(px - 20, py - 35, icon, state.get_kind(), state.get_value());
}

void Character::picture_damage(bool flag) {
	std::queue<std::pair<int, int> > que;
	int color;
	std::string value;
	std::pair<int, int> now;
	while (!damage_value.empty()) {
		now = damage_value.front();
		damage_value.pop();
		while (!damage_value.empty() && damage_value.front().second == now.second) {
			now.first += damage_value.front().first;
			damage_value.pop();
		}
		if (flag) {
			now.second++;
		}
		if (now.second < DAMAGE_COUNT) { que.push(now); }
		if (now.first > 0) { 
			color = GetColor(255, 0, 0);
			value = std::to_string(now.first);
		}
		else if (now.first == 0) { 
			color = GetColor(0, 0, 255);
			value = std::to_string(now.first);
		}
		else { 
			color = GetColor(0, 255, 0);
			value = std::to_string(now.first * -1);
		}
		DrawStringToHandle(x+40, y+150-(now.second/2), value.c_str() , color, damage_font);
	}
	damage_value = que;
}

int Character::get_num() {
	return num;
}

int Character::get_kind() {
	return kind;
}

int Character::get_hp() {
	return hp;
}

int Character::get_max_hp() {
	return max_hp;
}

int Character::get_wait() {
	return wait_cnt;
}

Deck* Character::get_cards() {
	return &deck;
}

int Character::get_now() {
	return now;
}

void Character::set_max_hp(int value) {
	max_hp = value;
}

void Character::set_effect(int k) {
	effect_kind = k;
	effect_cnt = EFFECT_COUNT;
}

void  Character::reset() {
	wait_cnt = 0;
	event_cnt = 0;
	next = -1;
	now = 0;
	wait_goal = 10;
	effect_cnt = 0;
	effect_kind = -1;
	cancel_flag = false;
}

//�G�t�F�N�g�̃A�j���[�V�����𐧌�
void Character::effect_counting() {
	if (effect_cnt > 0) {
		effect_cnt--;
		if (!effect_cnt)effect_kind = -1;
	}
}

//�J�[�h�����𐧌䂷��厖�Ȋ֐�
void Character::counting(std::queue<int>& wait_num) {
	c_flag();//�U���L�����Z��
	//�d�����I���A���J�[�h���c���Ă��āA�������Ă��āA����Ԉُ�œ����Ȃ��Ȃ��Ă��Ȃ��Ȃ�
	if (wait_cnt == 0 && now <= deck.get_size() && hp > 0 && state.can_move()) {
		if (next == -1) {//next�Ɏ��ɔ�������J�[�h�̈ʒu����
			do {
				next = now++;
			} while (now <= deck.get_size() && deck.get_cards()[next].get_kind() == 0);
			if (next < deck.get_size() && next >= 0) {//�J�[�h�����O�̍d��
				wait_cnt = deck.get_cards()[next].get_speed();//�J�[�h�����҂�
				wait_cnt = speed_value(wait_cnt);//�o�t�f�o�t�̕␳
				wait_goal = wait_cnt;
			}
		}
		else {//�J�[�h����
			wait_num.push(num_return());//���ԑ҂���
		}
	}
	else if(wait_cnt > 0){
		wait_cnt--;//�҂����ԁA�d�����Ԃ̏���
	}
	if (next != -1 && next >= deck.get_size())next = -1;
}

//0-5�̐�����0-2�ɕϊ�����(���k�̏ꍇ)
int Friend::num_return() {
	return num;
}

//0-5�̐�����0-2�ɕϊ�����(�搶�̏ꍇ)
int Enemy::num_return() {
	return num + MAX_WITH_BATTLE;
}

//�J�[�h������ʌ�A�X�L�������ۂɔ������A�J�[�h�̔������I������
void Character::skill_trigger(Character* student, Character* teacher, int target, int* handle) {
	if (event_cnt >= TRIGGER_COUNT) {//�J�[�h������ʏI����A�܂��X�L���𔭓����Ă��Ȃ��Ȃ�
		//�X�L���̌��ʂ𔭓�
		card_trigger(student, teacher, target, handle);
		//�I������
		next = -1;
		wait_cnt = deck.get_cards()[now - 1].get_rigid();//�d�����Ԃ̔���
		wait_cnt = speed_value(wait_cnt);//�o�t�f�o�t�̕␳
		wait_goal = wait_cnt;
		event_cnt = 0;//���̃J�[�h�ɔ�����cnt���Z�b�g
	}//�X�L���������܂߂��A�J�[�h���������S�ɏI��
}

//�U�����L�����Z������ �U�����󂯂����ȂǂɃL�����Z��
void Character::cancel() {
	cancel_flag = true;
}
//���ۂɃL�����Z����K�p����
void Character::c_flag() {
	if (cancel_flag) {
		//�L�����Z������Ԉُ�őς���Ȃ��Ȃ�
		if (!state.endure() || next == -1) {//�ς���Ԃł��d���̓L�����Z��
			next = -1;
			wait_cnt = 0;
		}
		cancel_flag = false;
	}
}

//�J�[�h�������̉�ʕ\��
int Friend::picture_trigger(int sound_handle) {
	int h = deck.get_cards()[next].get_handle();
	if (event_cnt < TRIGGER_COUNT) {
		SetDrawMode(DX_DRAWMODE_NEAREST);
		std::string str = deck.get_cards()[next].get_ability()->get_text();
		event_cnt++;
		if (event_cnt == ONE_SECOND)	PlaySoundMem(sound_handle, DX_PLAYTYPE_BACK);
		//�\��
		if (event_cnt < ONE_SECOND) {
			//�����G�_�Œ�
		}
		else if (event_cnt < ONE_SECOND+(TRIGGER_COUNT - ONE_SECOND) / 4) {
			int dx = 640 - (640 / ((TRIGGER_COUNT - ONE_SECOND) / 4)) * (event_cnt - ONE_SECOND);
			DrawBox(0, 130, 640, 360, GetColor(100, 100,255), TRUE);//0, 130, 640, 360
			DrawRotaGraph2(270 + dx, 200, 0, 0, 1, 0, h, TRUE, FALSE);
			DrawFormatString(340 - str.size() * 5 + dx, 160, GetColor(0, 0, 0), str.c_str());
		}
		else {
			DrawBox(0, 130, 640, 360, GetColor(100, 100, 255), TRUE);
			DrawRotaGraph2(270, 200, 0, 0, 1, 0, h, TRUE, FALSE);//1.2
			DrawFormatString(340 - str.size() * 5, 160, GetColor(0, 0, 0), str.c_str());//y:150
		}
		SetDrawMode(DX_DRAWMODE_BILINEAR);
	}
	else {
		return true;
	}
	return false;
}

int Enemy::picture_trigger(int sound_handle) {
	if (event_cnt < TRIGGER_COUNT) {
		std::string str = deck.get_cards()[next].get_ability()->get_text();
		event_cnt++;
		if (event_cnt == ONE_SECOND)	PlaySoundMem(sound_handle, DX_PLAYTYPE_BACK);
		//�\��
		if (event_cnt < ONE_SECOND) {
			//�����G�_�Œ�
		}
		else if (event_cnt < ONE_SECOND + (TRIGGER_COUNT - ONE_SECOND) / 4) {
			int dx = 640 - (640 / ((TRIGGER_COUNT - ONE_SECOND) / 4)) * (event_cnt - ONE_SECOND);
			DrawBox(0, 130, 640, 360, GetColor(100, 100, 255), TRUE);
			DrawFormatString(340 - str.size() * 5 + dx, 160, GetColor(0, 0, 0), str.c_str());
		}
		else {
			DrawBox(0, 130, 640, 360, GetColor(100, 100, 255), TRUE);
			DrawFormatString(340 - str.size() * 5, 160, GetColor(0, 0, 0), str.c_str());
		}
	}
	else {
		return true;
	}
	return false;
}


//�J�[�h�̃X�L���𔭓�����
void Friend::card_trigger(Character* student, Character* teacher, int target, int* handle) {
	get_cards()->get_cards()[next].trigger(student, teacher, num, target, handle);
}

void Enemy::card_trigger(Character* student, Character* teacher, int target, int* handle) {
	get_cards()->get_cards()[next].trigger(teacher, student, num, -1, handle);
}

//HP�����炷�֐� �h��͂��l������ d_flag��false�Ȃ�l�����Ȃ��f�t�H���g��true
void Character::dec_hp(int damage, bool d_flag) {
	if(d_flag){ damage = deffense_value(damage); }//�h��͂̉e��
	if(damage >= 0){ 
		hp -= damage;
		damage_value.push(std::make_pair(damage, 0));//�_���[�W�̒l�\���p
	}
	if (hp < 0) { hp = 0; }
}

void Character::change_hp(int value, bool d_flag) {
	if (value == 0)return;
	if (d_flag) { value = deffense_value(value); }//�h��͂̉e��
	hp -= value;
	damage_value.push(std::make_pair(value, 0));//�_���[�W�̒l�\���p
	if (hp < 0) { hp = 0; }
	if (hp > max_hp) { hp = max_hp; }
}

void Character::seek_hp() {
	if (php > hp) {//�_���[�W
		php--;
	}
	else if (php < hp) {//��
		php++;
	}
}

std::vector<Change>* Character::get_attack() {
	return &attack;
}

std::vector<Change>* Character::get_deffence() {
	return &deffense;
}

std::vector<Change>* Character::get_speed() {
	return &speed;
}

Change* Character::get_state() {
	return &state;
}

int Character::get_effect() {
	return effect_kind;
}

//�G�t�F�N�g�\���֘A
void Character::picture_effect(int handle1, int handle2, int handle3, int handle4) {
	if (effect_kind == -1) {
		return;
	}
	switch (effect_cnt / (EFFECT_COUNT / EFFECT_NUM)) {
	case 0:
		effect_draw(handle4);
		break;
	case 1:
		effect_draw(handle3);
		break;
	case 2:
		effect_draw(handle2);
		break;
	case 3:
		effect_draw(handle1);
		break;
	}
}

void Friend::effect_draw(int handle) {
	DrawRotaGraph2(x + 10, y + 20, 0, 0, 0.3, 0, handle, TRUE, FALSE);
}

void Enemy::effect_draw(int handle) {
	DrawRotaGraph2(x - 10, y + 50, 0, 0, 0.3, 0, handle, TRUE, FALSE);
}

void Character::set_attack(Change& add) {
	attack.push_back(add);
}

void Character::set_deffense(Change& add) {
	deffense.push_back(add);
}

void Character::set_speed(Change& add) {
	speed.push_back(add);
}

void Character::set_state(Change& add) {
	state = add;
}

//�o�t�f�o�t�Ȃǂ�cnt�̐���ƏI������
void Character::state_cnt() {
	for (unsigned i = 0; i < attack.size(); i++) {
		if (attack[i].cnt_dec()) {
			attack.erase(attack.begin() + i);
			attack.shrink_to_fit();
		}
	}
	for (unsigned i = 0; i < deffense.size(); i++) {
		if (deffense[i].cnt_dec()) {
			deffense.erase(deffense.begin() + i);
			deffense.shrink_to_fit();
		}
	}
	for (unsigned int i = 0; i < speed.size(); i++) {
		if (speed[i].cnt_dec()) {
			speed.erase(speed.begin() + i);
			speed.shrink_to_fit();
		}
	}
	if (state.cnt_dec()) { state = Change(); }
	change_hp(state.damage(), false);//�łȂǂɂ��_���[�W���
}

//�o�t�f�o�t�ɂ��U���͕␳�̍��v��Ԃ�
int Character::attack_value(int power) {
	int sum = power;
	for (unsigned int i = 0; i < attack.size(); i++) {
		attack[i].attack(sum);
	}
	return (sum > 0) ? sum : 0;
}

//�o�t�f�o�t�ɂ��h��͕␳�̍��v��Ԃ�
int Character::deffense_value(int power) {
	int sum = power;
	for (unsigned int i = 0; i < deffense.size(); i++) {
		deffense[i].deffense(sum);
	}
	return (sum > 0) ? sum : 0;
}

//�o�t�f�o�t�ɂ��X�s�[�h�␳�̍��v��Ԃ�
int Character::speed_value(int power) {
	int sum = power;
	for (unsigned int i = 0; i < speed.size(); i++) {
		speed[i].speed(sum);
	}
	return (sum > 0) ? sum : 1;//�Œ�ł�1��wait����
}