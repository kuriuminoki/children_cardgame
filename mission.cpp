#include"mission.h"
#include"DxLib.h"
#include"control.h"

const char* student_name(int kind) {
	switch (kind) {
	case -1:
		return "�s��";
		break;
	case 1:
		return "�Ԍ�";
		break;
	case 2:
		return "���c";
		break;
	case 3:
		return "���c";
		break;
	case 4:
		return "�Ɏu";
		break;
	case 5:
		return "�䗅";
		break;
	case 6:
		return "���a�c";
		break;
	case 7:
		return "�{����";
		break;
	case 8:
		return "���{";
		break;
	case 9:
		return "���";
		break;
	case 10:
		return "�u�p";
		break;
	}
	return "���R";
}

const char* teacher_name(int kind) {
	switch (kind) {
	case 1:
		return "O�搶";
		break;
	case 2:
		return "P�搶";
		break;
	case 3:
		return "Q�搶";
		break;
	case 4:
		return "R�搶";
		break;
	case 5:
		return "S�搶";
		break;
	case 6:
		return "T�搶";
		break;
	case 7:
		return "U�搶";
		break;
	case 8:
		return "V�搶";
		break;
	case 9:
		return "W�搶";
		break;
	case 10:
		return "X�搶";
		break;
	case 11:
		return "Y�搶";
		break;
	case 12:
		return "Z�Z��";
		break;
	case 13:
		return "A�搶";
		break;
	case 14:
		return "B�搶";
		break;
	case 15:
		return "����";
		break;
	}
	return "�Ȃ�";
}

Mission::Mission(MissionText& detail) {
	now = 0;
	result = 0;
	com = NULL;
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		kind[i] = 0;
		fixed[i] = false;
	}
	for (int i = 0; i < 3; i++) {
		if (detail.get_student()[i] != 0) {
			kind[i] = detail.get_student()[i];
			fixed[i] = true;
			if (kind[i] == -1) { kind[i] = 0; }
		}
		teacher[i].kind_set(detail.get_teacher()[i]);
	}
	reward_money = detail.get_reward();
	bgm = NULL;
}

Mission::~Mission() {
	delete bse;
	if (bgm != NULL) {
		delete[] bgm;
	}
}

//play�̑O�ɕK��1��ĂԂ���
void Mission::init() {
	for (int i = 0; i < 3; i++) {//�f�b�L�̊e�J�[�h�̉摜�Ȃǂ����[�h
		teacher[i].get_cards()->load_cards();
	}
	bse = new Bsequence(teacher, kind, fixed);
}

void Mission::kind_set(int first, int second, int third) {
	teacher[0].kind_set(first);
	teacher[1].kind_set(second);
	teacher[2].kind_set(third);
}

void Mission::size_set(int first, int second, int third) {
	teacher[0].get_cards()->set_size(first);//�f�b�L�̃J�[�h����
	teacher[1].get_cards()->set_size(second);//�f�b�L�̃J�[�h����
	teacher[2].get_cards()->set_size(third);//�f�b�L�̃J�[�h����
}

void Mission::st_set(bool f, bool s, bool t, int first, int second, int third) {
	fixed[0] = f;
	fixed[1] = s;
	fixed[2] = t;
	kind[0] = first;
	kind[1] = second;
	kind[2] = third;
}

//�g�p�����L�����̐�1�`3 kind�����Ɋ񂹂�
int student_sum(int kind[]) {
	int sum = 0;
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (kind[i] != 0) {
			kind[sum] = kind[i];
			sum++;
		}
	}
	return sum;
}

//��b�p�[�g �I�������܂ł���
void Mission::text_play() {
	if (com->play() == 1) {
		now++;
		delete com;
		com = NULL;
	}
}

//�L�����̈ړ�
void Mission::move(int num, int kind, int gx, int speed) {
	if (com->get_num() == num && (num == 0 || left_click() == 1 || !com->get_flag())) {
		com->move(kind, gx, speed);//�ړ�
	}
}
//�C�x���g�̑}�G
void Mission::event(int num, const char* path) {
	if (com->get_num() == num && (left_click() == 1 || !com->get_flag())) {
		if (left_click() == 1 && com->get_flag()) {
			com->set_event(LoadGraph(path));
		}
		com->event();
	}
}
//��ʏI��
void Mission::finish(int num) {
	if (com->get_num() == num && (left_click() == 1 || !com->get_flag())) {
		com->finish();
	}
}

//num1 - num2 �܂Ŏw���BGM���J��Ԃ�����
void Mission::play_bgm(int handle, int num1, int num2) {
	if (right_click() == 60) {
		StopSoundMem(handle);
	}
	else if (com->get_num() >= num1 && com->get_num() < num2) {
		if (CheckSoundMem(handle) == 0) {
			PlaySoundMem(handle, DX_PLAYTYPE_BACK);
		}
	}
	else if((right_click() == 60 || (left_click() == 1 || !com->get_flag())) && CheckSoundMem(handle) != 0){
		StopSoundMem(handle);
	}
}

void Mission::volume_bgm(int sum) {
	for (int i = 0; i < sum; i++) {
		ChangeVolumeSoundMem(255 * 60 / 100, bgm[i]);
	}
}

//��V���󂯎�� �O������menu���Ăяo��
bool Mission::reward(int result, int& money, Card& c) {
	if (result == WIN) {
		money += reward_money;
		int sum = student_sum(kind);//��������k�͒N�H
		if (sum == 0)return true;
		int r;
		r = GetRand(sum * 3) % sum;//�ǂ̃L�����̃J�[�h���Hr = GetRand(sum - 1)
		reward_card(kind[r], c, rare[0], rare[1], rare[2], rare[3]);//��V�̃J�[�h����ɓ����
		c.load_card();
		bse->battle_end(kind[r], c);//�o�g���V�[�P���X�I������ �J�[�h��q�ɂ֓���ăZ�[�u
	}
	else {
		money += reward_money / 10;
	}

	return true;
}


int* MissionText::get_student() {
	return kind;
}
int* MissionText::get_teacher() {
	return teacher;
}
int MissionText::get_reward() {
	return reward;
}

//�~�b�V�����̏ڍׂ�\������
const int BLACK = GetColor(0, 0, 0);
bool MissionText::picture_mission(int x, int y, int hand_x, int hand_y, bool clear) {
	const int WIDE = 200, HEIGHT = 250;
	bool mouse_flag = false;
	if (hand_x >= x && hand_x <= x + WIDE && hand_y >= y && hand_y <= y + HEIGHT) {
		DrawBox(x - 2, y - 2, x + WIDE + 2, y + HEIGHT + 2, GetColor(200, 200, 0), TRUE);
		mouse_flag = true;
	}
	DrawBox(x, y, x + WIDE, y + HEIGHT, GetColor(150, 150, 150), TRUE);
	DrawBox(x+3, y+28, x + WIDE-3, y + HEIGHT-143, GetColor(100, 100, 250), TRUE);
	x += 2;
	DrawFormatString(x, y + 5, BLACK, name.data());
	std::string str[4];
	unsigned int now = 0;
	while (now < text.length()) {
		str[now / 22] += text[now];
		now++;
	}
	for (int i = 0; i < 4; i++) {
		DrawFormatString(x+3, y + 30 + (i * 20), BLACK, str[i].data());
	}
	DrawFormatString(x, y + HEIGHT - 140, BLACK, "��Փx:%d", difficult);
	DrawFormatString(x, y + HEIGHT - 120, BLACK, "��V:%d", reward);
	DrawFormatString(x, y + HEIGHT - 100, BLACK, "�Œ�L����:");
	DrawFormatString(x + 10, y + HEIGHT - 80, BLACK, "%s, %s, %s", student_name(kind[0]), student_name(kind[1]), student_name(kind[2]));
	DrawFormatString(x, y + HEIGHT - 60, BLACK, "�G:");
	DrawFormatString(x + 10, y + HEIGHT - 40, BLACK, "%s, %s, %s", teacher_name(teacher[0]), teacher_name(teacher[1]), teacher_name(teacher[2]));
	if(clear){ DrawFormatString(x, y + HEIGHT - 20, GetColor(255, 0, 0), "�N���A�ς�"); }
	return mouse_flag;
}

void Mission::set_rare_rate(int c_rate, int b_rate, int a_rate, int s_rate) {
	rare[0] = c_rate;
	rare[1] = b_rate;
	rare[2] = a_rate;
	rare[3] = s_rate;
}

void MissionText::set_st(int first, int second, int third) {
	kind[0] = first;
	kind[1] = second;
	kind[2] = third;
}
void MissionText::set_te(int first, int second, int third) {
	teacher[0] = first;
	teacher[1] = second;
	teacher[2] = third;
}
void MissionText::set_te_random() {
	int candidate[Z_T];
	for (int i = 0; i < Z_T; i++) {
		candidate[i] = i + 1;
	}
	for (int k = 0; k < 10; k++) {
		for (int i = Z_T; i > 1; i--) {
			int j = GetRand(100) % i;
			i--;
			int t = candidate[i];
			candidate[i] = candidate[j];
			candidate[j] = t;
		}
	}
	for (int i = 0; i < 3; i++) {
		teacher[i] = candidate[i];
	}
}

//���C���~�b�V�����̏͂��܂�����r�؂�
void chapter_cancel(int* now, const bool clear[]) {
	if (*now != MM1 && *now != MM4 && *now != MM7 && *now != MM10 && *now != MM14 && *now != MM17 && *now != MM20) {
		if (!clear[*now - 1]) {
			*now = NO_MISSION;//�͂̎n�߈ȊO�͑O�̓����N���A����K�v�A��
		}
	}
	if (*now >= MM10 && *now <= MM12 && !clear[MM9]) {
		*now = NO_MISSION;//3�͂��N���A���Ȃ���4�͂ł��Ȃ�
	}
	if (*now >= MM14 && *now <= MM16 && !clear[MM6]) {
		*now = NO_MISSION;//3�͂��N���A���Ȃ���5�͂ł��Ȃ�
	}
	if (*now >= MM17 && *now <= MM19 && (!clear[MM3] || !clear[MM12])) {
		*now = NO_MISSION;//1,3�͂��N���A���Ȃ���6�͂ł��Ȃ�
	}
	if (*now == MM20 && (!clear[MM3] || !clear[MM16] || !clear[MM19])) {
		*now = NO_MISSION;//1,5,6�͂��N���A���Ȃ���7�͂ł��Ȃ�
	}
}
//�����̃~�b�V���������߂�i���[���ɉ����āj
void mission_rule(int today_mission[], bool clear[], int day) {
	if (day == 1 && !clear[MM1]) {//��ԍŏ�
		today_mission[0] = MM1;
		today_mission[1] = NO_MISSION;
		today_mission[2] = NO_MISSION;
		return;
	}
	//���C���~�b�V����
	today_mission[0] = day-1;
	chapter_cancel(&today_mission[0], clear);
	if (today_mission[0] == NO_MISSION) {//���C���~�b�V�������r�؂�Ă���EM�������_��
		today_mission[0] = GetRand(EM20 - EM1) + EM1;
	}
	//�T�u�~�b�V����
	if (day == 1) {
		//today_mission[1] = SM1;
	}
	else if (clear[day - 2]) {
		//today_mission[1] = day + SM1 - 1;
	}
	if(today_mission[1] == NO_MISSION) {//�T�u�~�b�V�������r�؂�Ă���EM�������_��
		do{
			today_mission[1] = GetRand(EM20 - EM1) + EM1;
		} while (today_mission[0] == today_mission[1]);
		//today_mission[1] = EM1;
	}
	//�C�x���g�~�b�V����
	do{
		today_mission[2] = GetRand(EM20 - EM1) + EM1;
	} while (today_mission[0] == today_mission[2] || today_mission[1] == today_mission[2]);
}

//�퓬�O�ƌ�Ɉ�񂸂̉�b������p�^�[��
int Mission::normal_text() {
	switch (now) {
	case 0:
		text1();
		break;
	case 1:
		result = bse->battle_loop(kind);//�g�p�����L������kind�ɓ���ċ����Ă��炤
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 2:
		text2();
		break;
	case 3:
		return result;
		break;
	}
	return 0;
}

//�퓬��Ɉ���b������p�^�[��
int Mission::after_text() {
	switch (now) {
	case 0:
		result = bse->battle_loop(kind);//�g�p�����L������kind�ɓ���ċ����Ă��炤
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 1:
		text2();
		break;
	case 2:
		return result;
		break;
	}
	return 0;
}

//kind�ɉ����ă~�b�V�������m��
void get_mission(Mission*& m, int kind, MissionText& detail) {
	switch (kind) {
	case MM1:
		m = new MainMission1(detail);
		break;
	case MM2:
		m = new MainMission2(detail);
		break;
	case MM3:
		m = new MainMission3(detail);
		break;
	case MM4:
		m = new MainMission4(detail);
		break;
	case MM5:
		m = new MainMission5(detail);
		break;
	case MM6:
		m = new MainMission6(detail);
		break;
	case MM7:
		m = new MainMission7(detail);
		break;
	case MM8:
		m = new MainMission8(detail);
		break;
	case MM9:
		m = new MainMission9(detail);
		break;
	case MM10:
		m = new MainMission10(detail);
		break;
	case MM11:
		m = new MainMission11(detail);
		break;
	case MM12:
		m = new MainMission12(detail);
		break;
	case MM13:
		m = new MainMission13(detail);
		break;
	case MM14:
		m = new MainMission14(detail);
		break;
	case MM15:
		m = new MainMission15(detail);
		break;
	case MM16:
		m = new MainMission16(detail);
		break;
	case MM17:
		m = new MainMission17(detail);
		break;
	case MM18:
		m = new MainMission18(detail);
		break;
	case MM19:
		m = new MainMission19(detail);
		break;
	case MM20:
		m = new MainMission20(detail);
		break;
	case SM1:

		break;
	case DM1:
		m = new DailyMission1(detail);
		break;
	case DM2:
		m = new DailyMission2(detail);
		break;
	case DM3:
		m = new DailyMission3(detail);
		break;
	case DM4:
		m = new DailyMission4(detail);
		break;
	case DM5:
		m = new DailyMission5(detail);
		break;
	case DM6:
		m = new DailyMission6(detail);
		break;
	case DM7:
		m = new DailyMission7(detail);
		break;
	case DM8:
		m = new DailyMission8(detail);
		break;
	case DM9:
		m = new DailyMission9(detail);
		break;
	case DM10:
		m = new DailyMission10(detail);
		break;
	case EM1:
		m = new EventMission1(detail);
		break;
	case EM2:
		m = new EventMission2(detail);
		break;
	case EM3:
		m = new EventMission3(detail);
		break;
	case EM4:
		m = new EventMission4(detail);
		break;
	case EM5:
		m = new EventMission5(detail);
		break;
	case EM6:
		m = new EventMission6(detail);
		break;
	case EM7:
		m = new EventMission7(detail);
		break;
	case EM8:
		m = new EventMission8(detail);
		break;
	case EM9:
		m = new EventMission9(detail);
		break;
	case EM10:
		m = new EventMission10(detail);
		break;
	case EM11:
		m = new EventMission11(detail);
		break;
	case EM12:
		m = new EventMission12(detail);
		break;
	case EM13:
		m = new EventMission13(detail);
		break;
	case EM14:
		m = new EventMission14(detail);
		break;
	case EM15:
		m = new EventMission15(detail);
		break;
	case EM16:
		m = new EventMission16(detail);
		break;
	case EM17:
		m = new EventMission17(detail);
		break;
	case EM18:
		m = new EventMission18(detail);
		break;
	case EM19:
		m = new EventMission19(detail);
		break;
	case EM20:
		m = new EventMission20(detail);
		break;
	default:
		m = new MainMission1(detail);
		break;
	}
}

//////////�e�~�b�V�����̒ǉ������͈ȉ�����/////////////////
MissionText::MissionText(int m_kind) {
	const int FREE = 0;
	difficult = -1;
	switch (m_kind) {
	case MM1:
		name = "�`���[�g���A��";
		text = "�n���_�F�퓬�̊�{���m�F���悤�I";
		set_st(AKATSUKI, HANDA, -1);
		set_te(0, O_T, 0);
		difficult = 1;
		reward = 30;
		break;
	case MM2:
		name = "����������Ă��邩�H";
		text = "�n���_�F�ȂɁA���c�����Ȃ��H�c�c���������A�����A������I";
		set_st(OOTA, FREE, FREE);
		set_te(0, Q_T, 0);
		difficult = 1;
		reward = 50;
		break;
	case MM3:
		name = "���̐搶�A���\�ɂ�";
		text = "�A�J�c�L�F�c�c�c�c�c�c�c�c";
		set_st(-1, AKATSUKI, -1);
		set_te(0, X_T, 0);
		difficult = 4;
		reward = 200;
		break;
	case MM4:
		name = "�O����";
		text = "�n���_�F���R�ɁA�Ƃ͌��������ǁ\�\�܂�����삳��A��l�Ő킢�ɍs���Ȃ�āc�c�c�c";
		set_st(IBARAKAWA, OOTA, FREE);
		set_te(S_T, T_T, 0);
		difficult = 1;
		reward = 50;
		break;
	case MM5:
		name = "�T";
		text = "�C�o���J���F�������ƏI��点�ċA��B";
		set_st(IBARAKAWA, OWADA, UCHIMOTO);
		set_te(O_T, P_T, Q_T);
		difficult = 1;
		reward = 50;
		break;
	case MM6:
		name = "�₷�v���o�A�󂷖�";
		text = "�I�I�^�F�ւ��A����ς����A�ق��Ƃ��˂��ȁB";
		set_st(IBARAKAWA, OOTA, -1);
		set_te(U_T, V_T, 0);
		difficult = 2;
		reward = 100;
		break;
	case MM7:
		name = "�n�܂�͉\�ƂƂ���";
		text = "�C�V�F�����͓u�p����Ƃ����B�|���Ȃ��B";
		set_st(ISHI, TOMOCHI, -1);
		set_te(0, W_T, 0);
		difficult = 1;
		reward = 50;
		break;
	case MM8:
		name = "��������";
		text = "�E�`���g�F�����H������[�A���v����[�B";
		set_st(ISHI, UCHIMOTO, -1);
		set_te(0, O_T, 0);
		difficult = 1;
		reward = 50;
		break;
	case MM9:
		name = "�����l�ԃg���`�X�G";
		text = "�C�V�F�ǂ����悤�c�c�c�c���Ȃ��΂��B";
		set_st(ISHI, TOMOCHI, -1);
		set_te(P_T, R_T, S_T);
		difficult = 2;
		reward = 100;
		break;
	case MM10:
		name = "�����Ⴋ�A����Ǒ��~��";
		text = "�^�C���F�~�b�V�������B���v�A���͂��邳�B";
		set_st(ISHI, TAIRA, 0);
		set_te(X_T, O_T, Q_T);
		difficult = 2;
		reward = 100;
		break;
	case MM11:
		name = "���̓����̎������č�";
		text = "�g���`�F�܂��n���_���ĂȂ����[�B�����I��点�����񂾂��ǂȁ[�B";
		set_st(TOMOCHI, HANDA, 0);
		set_te(S_T, V_T, 0);
		difficult = 2;
		reward = 100;
		break;
	case MM12:
		name = "��";
		text = "�^�C���F��x�A���������Ęb���ׂ��Ȃ񂾂낤�ȁB";
		set_st(TAIRA, 0, 0);
		set_te(0, W_T, 0);
		difficult = 3;
		reward = 150;
		break;
	case MM13:
		name = "���s����I�F��";
		text = "�C�V�F�����񂾁c�c�c�c����ŁA�����񂾁B";
		set_st(ISHI, 0, 0);
		set_te(P_T, R_T, 0);
		difficult = 3;
		reward = 150;
		break;
	case MM14:
		name = "�q�����������";
		text = "�n���_�F�����͓��ɕ�����ă~�b�V�������B�ǎq�A�������͂�낵���B";
		set_st(HANDA, AKATSUKI, MIYATONE);
		set_te(Y_T, T_T, 0);
		difficult = 2;
		reward = 100;
		break;
	case MM15:
		name = "�q����F��";
		text = "�C�o���J���F���c�c�c�c�c";
		set_st(HANDA, AKATSUKI, IBARAKAWA);
		set_te(V_T, X_T, 0);
		difficult = 3;
		reward = 150;
		break;
	case MM16:
		name = "���@���L���[�ɕ���";
		text = "�A�J�c�L�F�c�c�c�c�c�c�c�c";
		set_st(HANDA, OWADA, UCHIMOTO);
		set_te(0, Q_T, 0);
		difficult = 3;
		reward = 150;
		break;
	case MM17:
		name = "���m���������|�̍���";
		text = "�I���_�F�����͓��Ԃ����ƈꏏ�����B";
		set_st(AKATSUKI, OWADA, MIYATONE);
		set_te(O_T, U_T, P_T);
		difficult = 4;
		reward = 200;
		break;
	case MM18:
		name = "����";
		text = "�^�C���F�u�p�����邵�A�T�|�[�g�ɂ��O����̂����������ȁB";
		set_st(OWADA, TOMOCHI, TAIRA);
		set_te(Y_T, R_T, 0);
		difficult = 4;
		reward = 200;
		break;
	case MM19:
		name = "�{����������";
		text = "�~���g�l�F���߂�Ȃ������߂�Ȃ������߂�Ȃ����\�\";
		set_st(AKATSUKI, MIYATONE, -1);
		set_te(0, T_T, 0);
		difficult = 4;
		reward = 200;
		break;
	case MM20:
		name = "�Ă���t�̋q";
		text = "�n���_�F�c�c�c�c�c�c�c�c";
		set_st(AKATSUKI, HANDA, -1);
		set_te(0, Z_T, 0);
		difficult = 5;
		reward = 250;
		break;
	case SM1:

		break;
	case DM1:
		name = "�f�C���[�~�b�V����";
		text = "{�Q���ҕ�W��}";
		set_st(0, 0, 0);
		set_te_random();
		difficult = 1;
		reward = 50;
		break;
	case DM2:
		name = "�f�C���[�~�b�V����";
		text = "{�Q���ҕ�W��}";
		set_st(0, 0, 0);
		set_te_random();
		difficult = 2;
		reward = 100;
		break;
	case DM3:
		name = "�f�C���[�~�b�V����";
		text = "{�Q���ҕ�W��}";
		set_st(0, 0, 0);
		set_te_random();
		difficult = 3;
		reward = 150;
		break;
	case DM4:
		name = "�f�C���[�~�b�V����";
		text = "{�Q���ҕ�W��}";
		set_st(0, 0, 0);
		set_te_random();
		difficult = 4;
		reward = 200;
		break;
	case DM5:
		name = "�f�C���[�~�b�V����";
		text = "{�Q���ҕ�W��}";
		set_st(0, 0, 0);
		set_te_random();
		difficult = 5;
		reward = 250;
		break;
	case DM6:
		name = "�f�C���[�~�b�V����";
		text = "{�Q���ҕ�W��}";
		set_st(0, 0, 0);
		set_te_random();
		difficult = 6;
		reward = 300;
		break;
	case DM7:
		name = "�f�C���[�~�b�V����";
		text = "{�Q���ҕ�W��}";
		set_st(0, 0, 0);
		set_te_random();
		difficult = 7;
		reward = 350;
		break;
	case DM8:
		name = "�f�C���[�~�b�V����";
		text = "{�Q���ҕ�W��}";
		set_st(0, 0, 0);
		set_te_random();
		difficult = 8;
		reward = 400;
		break;
	case DM9:
		name = "�f�C���[�~�b�V����";
		text = "{�Q���ҕ�W��}";
		set_st(0, 0, 0);
		set_te_random();
		difficult = 9;
		reward = 450;
		break;
	case DM10:
		name = "�f�C���[�~�b�V����";
		text = "{�Q���ҕ�W��}";
		set_st(0, 0, 0);
		set_te_random();
		difficult = 10;
		reward = 500;
		break;
	}
	if (difficult != -1) { return; }
	switch (m_kind) {
	case EM1:
		name = "�C�x���g�~�b�V�����P";
		text = "�I���_�F�o���Ă�H����������ˁ`";
		set_st(1, 2, 6);
		set_te(O_T, P_T, T_T);
		difficult = 3;
		reward = 150;
		break;
	case EM2:
		name = "�C�x���g�~�b�V�����Q";
		text = "�g���`�F�˂��A�D���Ȑl�Ƃ�����́H";
		set_st(3, 6, 10);
		set_te(O_T, Q_T, V_T);
		difficult = 3;
		reward = 150;
		break;
	case EM3:
		name = "�C�x���g�~�b�V�����R";
		text = "�E�`���g�F���[�܂��e�X�g�߂��ȁ[�B";
		set_st(8, 5, 0);
		set_te(O_T, S_T, U_T);
		difficult = 3;
		reward = 150;
		break;
	case EM4:
		name = "�C�x���g�~�b�V�����S";
		text = "�~���g�l�F�͂��c�c�����΂��c�c";
		set_st(4, 5, 7);
		set_te(O_T, T_T, Y_T);
		difficult = 3;
		reward = 150;
		break;
	case EM5:
		name = "�C�x���g�~�b�V�����T";
		text = "�C�o���J���F�܂������B�ǂ����Ď��ɗ��ނ́B";
		set_st(1, 9, 0);
		set_te(O_T, U_T, X_T);
		difficult = 4;
		reward = 200;
		break;
	case EM6:
		name = "�C�x���g�~�b�V�����U";
		text = "�I�I�^�F����A�܂����x�V�ڂ����I";
		set_st(3, 4, 0);
		set_te(P_T, Q_T, W_T);
		difficult = 4;
		reward = 200;
		break;
	case EM7:
		name = "�C�x���g�~�b�V�����V";
		text = "�n���_�F���߂�A�}�ɋ��͗��񂶂���āB";
		set_st(2, 8, 9);
		set_te(P_T, R_T, U_T);
		difficult = 4;
		reward = 200;
		break;
	case EM8:
		name = "�C�x���g�~�b�V�����W";
		text = "�~���g�l�F�u�p������āA������ƕ|�������c�c";
		set_st(7, 10, 0);
		set_te(P_T, U_T, Z_T);
		difficult = 4;
		reward = 200;
		break;
	case EM9:
		name = "�C�x���g�~�b�V�����X";
		text = "�C�V�F�i���킠�A���q��l�Ɓc�c�|���I�I�j";
		set_st(4, 6, 9);
		set_te(Q_T, S_T, W_T);
		difficult = 5;
		reward = 250;
		break;
	case EM10:
		name = "�C�x���g�~�b�V�����P�O";
		text = "�n���_�F��l�Ƃ��A���肪�Ƃ��B�܂���낵���B";
		set_st(2, 7, 10);
		set_te(Q_T, T_T, Z_T);
		difficult = 5;
		reward = 250;
		break;
	case EM11:
		name = "�C�x���g�~�b�V�����P�P";
		text = "�A�J�c�L�F�c�c�c�c�c�c�c�c";
		set_st(1, 3, 8);
		set_te(Q_T, W_T, X_T);
		difficult = 5;
		reward = 250;
		break;
	case EM12:
		name = "�C�x���g�~�b�V�����P�Q";
		text = "�n���_�F������킪�v���������I";
		set_st(2, 5, 6);
		set_te(R_T, S_T, V_T);
		difficult = 5;
		reward = 250;
		break;
	case EM13:
		name = "�C�x���g�~�b�V�����P�R";
		text = "�C�V�F�u�p����Ƃ��c�c�C��t���Ȃ��ƁB";
		set_st(4, 10, 0);
		set_te(R_T, U_T, X_T);
		difficult = 6;
		reward = 300;
		break;
	case EM14:
		name = "�C�x���g�~�b�V�����P�S";
		text = "�^�C���F���Ƃ��c�c���������΂��܂肵��ׂ�@��Ȃ������ȁB";
		set_st(5, 9, 7);
		set_te(S_T, T_T, V_T);
		difficult = 6;
		reward = 300;
		break;
	case EM15:
		name = "�C�x���g�~�b�V�����P�T";
		text = "�g���`�F�Ԍ����Ă��[�A�Ȃ�ł���Ȃɂ���ׂ�Ȃ��́[�H";
		set_st(1, 2, 10);
		set_te(S_T, V_T, W_T);
		difficult = 6;
		reward = 300;
		break;
	case EM16:
		name = "�C�x���g�~�b�V�����P�U";
		text = "�I�I�^�F�����I�I�����A���ق��Ă���I�I";
		set_st(3, 6, 7);
		set_te(T_T, U_T, W_T);
		difficult = 6;
		reward = 300;
		break;
	case EM17:
		name = "�C�x���g�~�b�V�����P�V";
		text = "�E�`���g�F���c�ɂ͒��Ԉӎ������Ă��I";
		set_st(3, 5, 8);
		set_te(T_T, V_T, X_T);
		difficult = 7;
		reward = 350;
		break;
	case EM18:
		name = "�C�x���g�~�b�V�����P�W";
		text = "�C�V�F�A�C���V���^�C�����Đ�����I";
		set_st(4, 8, 9);
		set_te(T_T, W_T, Y_T);
		difficult = 7;
		reward = 350;
		break;
	case EM19:
		name = "�C�x���g�~�b�V�����P�X";
		text = "���c�F���[�������傤�I�I���Ă˂��I�I";
		set_st(1, 2, 3);
		set_te(U_T, V_T, Z_T);
		difficult = 7;
		reward = 350;
		break;
	case EM20:
		name = "�C�x���g�~�b�V�����Q�O";
		text = "�I���_�F�ˁ[�A�������v���񂾂��ǂ��[�B";
		set_st(4, 5, 6);
		set_te(V_T, X_T, Y_T);
		difficult = 7;
		reward = 350;
		break;
	}
}

//�~�b�V�������Ƃ̐搶�̏����ȉ���
//MM1
//�搶�̎�ނƃJ�[�h
MainMission1::MainMission1(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/piano21.mp3");
	volume_bgm(1);
	set_rare_rate(100, 0, 0, 0);
	size_set(9, 9, 9);
	teacher[1].get_cards()->set_card(Card(22, 0, 300, 30, 120, 110), 0);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 30, 120, 110), 1);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 30, 120, 110), 2);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 30, 120, 110), 3);
}
//��b�ƃo�g��������
int MainMission1::play() {
	return normal_text();
}
void MainMission1::text1() {
	if (com == NULL) {
		int x[10] = { 10,500,-100,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�n���_", "���ꂩ�疈���搶�Ɛ���Ă������ƂɂȂ邯�ǁA�܂��͐퓬�̊�{���m�F�����B", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�n���_", "����͖l�Ɨ���̓�l��O�搶�Ɛ킨���B", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�n���_", "�퓬�͊e���k���ő�T���̃J�[�h�ō\�������f�b�L���g���Ă����Ȃ���B", H_NORMAL, 1);
		com->text_insert("�n���_", "�J�[�h�͈ꖇ�ڂ��珇�Ԃɔ�������Ă�����B", H_NORMAL, 1);
		com->text_insert("�n���_", "�������A�J�[�h�𔭓�����̂ɂ͎��Ԃ�������B", H_NORMAL, 1);
		com->text_insert("�n���_", "��������̂ɂ����鎞�Ԃ̓J�[�h�ɂ���ĈႤ��B", H_NORMAL, 1);
		com->text_insert("�n���_", "�J�[�h�𔭓����悤�Ƃ��Ă���Ƃ���ɑ��肩��̍U�����󂯂Ă��܂��ƁA�J�[�h�͔��������Ɏ��̃J�[�h�Ɉڂ��Ă��܂��B", H_NORMAL, 1);
		com->text_insert("�n���_", "������A�J�[�h�̔������ԁA������̍d�����Ԃ��l���ăf�b�L����邱�Ƃ��厖����B", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�n���_", "���k�Ɛ搶�̑S�����Ō�̃J�[�h�܂Ŕ���������A������x�f�b�L��g�݂Ȃ�����B", H_NORMAL, 1);
		com->text_insert("�n���_", "���肪�ǂꂭ�炢�̃^�C�~���O�łǂ�ȋZ���g���Ă��������l�����ăf�b�L����蒼���ƗL�����B", H_NORMAL, 1);
		com->text_insert("�n���_", "�܂��A�K����芵�����Ă��ƂŁA�����s�������B", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�c�c�c�c", H_NORMAL, 0);
	}
	move(15, 0, -500, 1);//�ړ�
	move(15, 1, -500, 5);//�ړ�
	finish(15);
	play_bgm(bgm[0], 1, 15);
	text_play();
}
void MainMission1::text2() {
	if (com == NULL) {
		int x[10] = { 10,500,-100,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�n���_", "���������ɂł����ˁB", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�n���_", "�퓬��͕�V�����炦���B", H_NORMAL, 1);
		com->text_insert("�n���_", "������������V�����������ɁA�J�[�h���ꖇ�����_���ł��炦��B", H_MOUSOU, 1);
		com->text_insert("�n���_", "���炦��J�[�h�̎�ނ́A��������k�̂����̒N���̂���B", H_MOUSOU, 1);
		com->text_insert("�n���_", "��V��J�[�h�𔄂��Ď�ɓ��ꂽ�����̓A�C�e���𔃂��̂Ɏg����B", H_MOUSOU, 1);
		com->text_insert("�n���_", "�A�C�e���͈�x�g������������Ⴄ���ǁA�N�̃f�b�L�ɂ��g�ݍ��߂邵�A���͂���B", H_MOUSOU, 1);
	}
	//move(1, 9, 300, 5);//�ړ�
	//event(3, "event/�g���`�f�[�g���U.png");//�}�G
	text_play();
}

//MM2
//�搶�̎�ނƃJ�[�h
MainMission2::MainMission2(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[2];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/piano32.mp3");
	bgm[1] = LoadSoundMem("bgm/piano28.mp3");
	volume_bgm(2);
	set_rare_rate(100, 0, 0, 0);
	size_set(9, 9, 9);
	teacher[1].get_cards()->set_card(Card(24, 0, 1000, 50, 90, 100), 0);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 120, 110), 1);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 120, 110), 2);
	teacher[1].get_cards()->set_card(Card(24, 0, 0, 50, 90, 100), 3);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 120, 110), 4);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 120, 110), 5);
}
//��b�ƃo�g��������
int MainMission2::play() {
	return normal_text();
}
void MainMission2::text1() {
	if (com == NULL) {
		int x[10] = { 10,500,-100,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�n���_", "�����A���c�I���ǂ��ɂ���񂾁I�H", H_NORMAL, 1);
		com->text_insert("�I�I�^", "���[�T�l�A�킢�s���Ă邺�[�B", H_NORMAL, 2);
		com->text_insert("�I�I�^", "���������Q�搶�Ƃ�肠���Ă��邺�B", H_NORMAL, 2);
		com->text_insert("�n���_", "�͂��I�H�҂Ă�A����ȁA���Ƃ��c�c", H_NORMAL, 1);
		com->text_insert("�I�I�^", "���[���[����ʂɂ��B", H_NORMAL, 2);
		com->text_insert("�I�I�^", "�����Ɠ|������݂�Ȃ������邾��H", H_NORMAL, 2);
		com->text_insert("�n���_", "���������A��肶��A�Ȃ��c�c�I�I", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�n���_", "�͂��A�킩�����B���߂Ă�����Ƒ҂��Ă���B", H_NORMAL, 1);
		com->text_insert("�n���_", "�������A�������ɉ��l������B", H_NORMAL, 1);
		com->text_insert("�n���_", "������A��l�œ˂����܂Ȃ��ł����B", H_NORMAL, 1);
		com->text_insert("�I�I�^", "���������A�ʂɂ������āB", H_NORMAL, 2);
		com->text_insert("�n���_", "���ނ���A�҂��ĂĂ���B", H_NORMAL, 1);
		com->text_insert("�I�I�^", "�c�c�c�c��[������B", H_NORMAL, 2);
	}
	play_bgm(bgm[0], 1, 14);
	finish(14);
	text_play();
}
void MainMission2::text2() {
	if (com == NULL) {
		int x[10] = { 10,500,-100,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�n���_", "�c�c�c�c�͂��A�܂��������c�́c�c", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�S�z�Ȃ̂��H", H_NORMAL, 0);
		com->text_insert("�n���_", "�܂��A�ȁc�c������������Ă킯����Ȃ��B", H_NORMAL, 1);
		com->text_insert("�n���_", "����͌�ł݂�ȂɌ������Ƃ��Ă��񂾂��ǁA�Ȃ�ׂ���l�Ő키�̂͂�߂Ăق����񂾁B", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�n���_", "���ƁA�키�O�͐�΂ɖl�ɋ����Ăق����񂾁B", H_NORMAL, 1);
		com->text_insert("�n���_", "���ɁA�w�搶�Ȃ񂩂͐�΂Ɉ�l�Ő�����Ⴞ�߂��B���̐l�͂�΂��B", H_NORMAL, 1);
		com->text_insert("�n���_", "���O�ɏ��������Đ키�ׂ����肾�B", H_NORMAL, 1);
		com->text_insert("�n���_", "�܂��A����Ƃ͕t�������������c�c�����K�v���Ȃ��Ƃ͎v�����ǁc�c", H_NORMAL, 1);
		com->text_insert("�n���_", "����͌�ł݂�Ȃɂ������Ƃ���B", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�����B", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "�i���c�́\�\�j", H_MOUSOU, 0);
		com->text_insert("�A�J�c�L", "�i�ǂ����āA�搶�Ɛ킨���Ȃ�Č����o�����񂾂낤���B�j", H_MOUSOU, 0);
		com->text_insert("�A�J�c�L", "�i�܂��A�I���͐[���l����K�v�Ȃ��B�j", H_MOUSOU, 0);
		com->text_insert("�A�J�c�L", "�i�����ʂ�A�������c�̌������Ă����΂����B�j", H_MOUSOU, 0);
	}
	play_bgm(bgm[1], 1, 16);
	finish(16);
	text_play();
}

//MM3
//�搶�̎�ނƃJ�[�h
MainMission3::MainMission3(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[2];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/piano29.mp3");
	bgm[1] = LoadSoundMem("bgm/piano22.mp3");
	volume_bgm(2);
	set_rare_rate(70, 21, 8, 1);
	size_set(9, 9, 9);
	teacher[1].get_cards()->set_card(Card(3, 0, 900, 90, 130, 110), 0);//hp500
	teacher[1].get_cards()->set_card(Card(3, 0, 0, 90, 130, 110), 1);
	teacher[1].get_cards()->set_card(Card(2, 0, 0, 50, 90, 100), 2);
	teacher[1].get_cards()->set_card(Card(23, 0, 0, 110, 160, 140), 3);
	teacher[1].get_cards()->set_card(Card(3, 0, 0, 90, 130, 110), 4);
}
//��b�ƃo�g��������
int MainMission3::play() {
	return normal_text();
}
void MainMission3::text1() {
	if (com == NULL) {
		int x[10] = { 10,500,-100,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�n���_", "�ȂɁc�c�I�H�킩�����A�������s���B", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�n���_", "���߂񗺖�A������ƃI���_�̂Ƃ��s���Ă���B", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�����B", H_NORMAL, 0);
		com->text_insert("�n���_", "�c�c�c�c�ق�ƁA���߂�B�����͐�A���ĂĂ�����B", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�����B", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "�i�c�c�c�c�A�邩�B�j", H_MOUSOU, 0);
		com->text_insert("�A�J�c�L", "�c�c�c�c��", H_NORMAL, 0);
		com->text_insert("X�搶", "�����A���O�́c�c�c�c�Ԍ����B�����́H", H_NORMAL, 10);
		com->text_insert("�A�J�c�L", "�A��ł�", H_NORMAL, 0);
		com->text_insert("X�搶", "�����A�������B", H_NORMAL, 10);
		com->text_insert("�A�J�c�L", "���Ⴀ�A�搶�A���悤�Ȃ�B", H_NORMAL, 0);
		com->text_insert("X�搶", "�������炢��������ǂ����H", H_NORMAL, 10);
		com->text_insert("�A�J�c�L", "�c�c�c�c", H_NORMAL, 0);
		com->text_insert("X�搶", "���O�A�q�����q��������˂����B�j�炵���˂��B", H_NORMAL, 10);
		com->text_insert("�A�J�c�L", "�c�c����Ȃ́A�I���̏���ł��傤", H_NORMAL, 0);
		com->text_insert("X�搶", "���H�c�c���O�Ȃ񂾁A���̑ԓx�́B", H_NORMAL, 10);
		com->text_insert("�A�J�c�L", "�i�����H������Ȃ��B�j", H_MOUSOU, 0);
		com->text_insert("�A�J�c�L", "�i���͕����ɓ���Ƃ�����Ȃ��Ƃ��A����Ȃ��Ƃ��������ƈႤ���E�����Ă�񂾁B�j", H_MOUSOU, 0);
		com->text_insert("�A�J�c�L", "�i����ł����͂��Ȃ̂Ɂc�c�Ȃ񂾁A���̉՗����́B�j", H_MOUSOU, 0);
		com->text_insert("X�搶", "�����A�Ȃ񂾂����Ă񂾂�I�I", H_NORMAL, 10);
		com->text_insert("�A�J�c�L", "�i�q�����q�������Ƃ��A�j�炵���Ƃ��A����Ȃ��Ƃ��ǂ������Ă����͂����B�j", H_MOUSOU, 0);
		com->text_insert("�A�J�c�L", "�i����Ȃ̂Ɂc�c�Ȃ�Ȃ񂾁B�����ɕ������B�j", H_MOUSOU, 0);
		com->text_insert("X�搶", "�����A�Ȃɖق��Ă񂾁I�I�Ȃ߂Ă�̂��I�I", H_NORMAL, 10);
		com->text_insert("�A�J�c�L", "�c�c�搶���Ă��A�����{���ȁB", H_NORMAL, 0);
	}
	play_bgm(bgm[0], 1, 25);
	move(6, 1, -300, 6);//�ړ�
	move(7, 0, 700, 5);//�ړ�
	move(8, 0, 400, 5);//�ړ�
	move(8, 10, 300, 5);//�ړ�
	move(12, 0, 50, 5);//�ړ�
	finish(7);
	text_play();
}
void MainMission3::text2() {
	if (com == NULL) {
		int x[10] = { 300,-100,-100,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, 500);
		com->text_insert("X�搶", "�����c�c���A���c�c�c�c", H_NORMAL, 10);
		com->text_insert("�A�J�c�L", "�c�c�c�c", H_NORMAL, 0);
		com->text_insert("X�搶", "���O���c�c�c�c���O�c�c�I�I", H_NORMAL, 10);
		com->text_insert("�A�J�c�L", "�c�c�c�c", H_NORMAL, 0);
		com->text_insert("X�搶", "�������I�I�c�c��΁@�^���āc�c�I�I", H_NORMAL, 10);
		com->text_insert("�A�J�c�L", "�搶�͂����Ŏ��ʂ񂾁B�\�\�����ŁA�I��肾�B", H_NORMAL, 0);
		com->text_insert("X�搶", "�c�c�c�c���O�c�c�c�c", H_NORMAL, 10);
		com->text_insert("X�搶", "�c�c���Ⴂ���Ă񂾂�A���O�́c�c�c�c", H_NORMAL, 10);
		com->text_insert("�A�J�c�L", "���񂽂́c�c�N�Y���B�ז��ȑ��݂��B", H_NORMAL, 0);
		com->text_insert("X�搶", "�X�点�Ă񂾂�A���O�́c�c", H_NORMAL, 10);
		com->text_insert("X�搶", "���O�́A��l�ɔ��R���Ă邾���̐��ӋC�ȃK�L���B", H_NORMAL, 10);
		com->text_insert("�A�J�c�L", "���񂽂�������Ȃ��\�\", H_NORMAL, 0);
		com->text_insert("X�搶", "�́c�c�c�c�H", H_NORMAL, 10);
		com->text_insert("�A�J�c�L", "�搶���A�݂�Ȃ��A���c���\�\", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "�ǂ����������N�Y�΂����c�c�c�c", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "�݂�Ȃ݂�ȁA���˂΂����c�c�c�c", H_NORMAL, 0);
		com->text_insert("X�搶", "�c�c�c�c�c�c�c�c", H_NORMAL, 10);
		com->text_insert("�A�J�c�L", "�c�c�c�c", H_NORMAL, 0);
		com->text_insert("X�搶", "���O�c�c�c�c�����Ă�c�c�c�c", H_NORMAL, 10);
	}
	play_bgm(bgm[1], 14, 19);
	finish(19);
	text_play();
}

//MM4
//�搶�̎�ނƃJ�[�h
MainMission4::MainMission4(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/piano40.mp3");
	volume_bgm(1);
	set_rare_rate(100, 0, 0, 0);
	size_set(9, 9, 9);
	teacher[0].get_cards()->set_card(Card(8, 0, 300, 30, 70, 90), 0);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 60, 110, 110), 1);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 60, 110, 110), 2);
	teacher[0].get_cards()->set_card(Card(8, 0, 0, 30, 70, 90), 3);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 60, 110, 110), 4);

	teacher[1].get_cards()->set_card(Card(22, 0, 300, 55, 120, 100), 0);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 55, 120, 100), 1);
	teacher[1].get_cards()->set_card(Card(5, 0, 0, 40, 80, 70), 2);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 55, 120, 100), 3);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 55, 120, 100), 4);
}
//��b�ƃo�g��������
int MainMission4::play() {
	return normal_text();
}
void MainMission4::text1() {
	if (com == NULL) {
		int x[10] = { -100,-100,400,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�n���_", "�I�I�^�A���������H", H_NORMAL, 1);
		com->text_insert("�I�I�^", "�����������A�~�b�V�������H", H_NORMAL, 2);
		com->text_insert("�n���_", "�C�o���J����S�搶��T�搶��|���̂�C�����񂾂��A�N���U�킸��l�ōs�����񂾁B", H_NORMAL, 1);
		com->text_insert("�I�I�^", "�c�c�c�c�������A�킩�����B�����ɍs���΂����񂾂ȁH", H_NORMAL, 2);
		com->text_insert("�n���_", "�����B���Ƃł���Έ�l�A�������ɑ���B���񂾂��B", H_NORMAL, 1);
		com->text_insert("�I�I�^", "�C����I�I�������������I�I", H_NORMAL, 2);
	}
	move(6, 2, -200, 8);//�ړ�
	finish(6);
	text_play();
}
void MainMission4::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,100,-100,-100,-100,-100,-100,300,-100 };
		com = new Text(x, -100);
		com->text_insert("�C�o���J��", "�ǂ�����", H_NORMAL, 8);
		com->text_insert("�I�I�^", "����͂������̃Z���t�����ẮB�Ȃ�ň�l�Ő킨���Ƃ����񂾂�B", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "�ʂɁc�c�����ł���B���̏���B", H_NORMAL, 8);
		com->text_insert("�I�I�^", "��ŗT�l�ɂ�������Ǝv�����ǂȁc�c��l�Ő키�Ȃ�B", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "���񂽂����̑O���c�ɒ��ӂ��ꂽ���ĕ��������ǁB�l�̂��ƌ�����H", H_NORMAL, 8);
		com->text_insert("�I�I�^", "�I���̂��Ƃ͊֌W�Ȃ��B�Ƃɂ����A���ꂩ��̓I���̂��ƌĂׂ�B", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "�Ȃɂ���c�c�ق��Ƃ��Ă�B", H_NORMAL, 8);
		com->text_insert("�I�I�^", "�ق��Ƃ��ˁ[��B��������Ȃ��āI", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "�������Ă�킯����ˁ[��I�o�J�I�I", H_NORMAL, 8);
		com->text_insert("�I�I�^", "���A�����I�I", H_NORMAL, 2);
		com->text_insert("�I�I�^", "�c�c�c�c�B", H_NORMAL, 2);
	}
	play_bgm(bgm[0], 1, 11);
	move(9, 8, 800, 6);//�ړ�
	move(10, 2, 300, 6);//�ړ�
	finish(11);
	text_play();
}

//MM5
//�搶�̎�ނƃJ�[�h
MainMission5::MainMission5(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/piano40.mp3");
	volume_bgm(1);
	set_rare_rate(90, 10, 0, 0);
	size_set(9, 9, 9);
	teacher[0].get_cards()->set_card(Card(22, 0, 200, 50, 110, 100), 0);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 50, 110, 100), 1);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 50, 110, 100), 2);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 50, 110, 100), 3);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 50, 110, 100), 4);

	teacher[1].get_cards()->set_card(Card(7, 0, 200, 30, 90, 60), 0);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 50, 100, 110), 1);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 50, 100, 110), 2);
	teacher[1].get_cards()->set_card(Card(7, 0, 0, 30, 90, 60), 3);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 50, 100, 110), 4);

	teacher[2].get_cards()->set_card(Card(22, 0, 200, 50, 120, 120), 0);
	teacher[2].get_cards()->set_card(Card(24, 0, 0, 20, 90, 110), 1);
	teacher[2].get_cards()->set_card(Card(3, 0, 0, 60, 140, 140), 2);
	teacher[2].get_cards()->set_card(Card(22, 0, 0, 50, 120, 120), 3);
	teacher[2].get_cards()->set_card(Card(22, 0, 0, 50, 120, 120), 4);
}
//��b�ƃo�g��������
int MainMission5::play() {
	return normal_text();
}
void MainMission5::text1() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,-100,-100,100,-100,200,400,-100 };
		com = new Text(x, -100);
		com->text_insert("�I���_", "�����A������I��l�Ƃ������͂����H", H_NORMAL, 5);
		com->text_insert("�E�`���g", "�I�b�P�[����[�I�I", H_NORMAL, 7);
		com->text_insert("�C�o���J��", "����B", H_NORMAL, 8);
		com->text_insert("�I���_", "�c�c�c�c�悵�A���Ⴀ�s�������B", H_NORMAL, 5);
	}
	move(4, 5, -400, 4);//�ړ�
	move(4, 7, -400, 4);//�ړ�
	move(4, 8, -400, 4);//�ړ�
	finish(4);
	text_play();
}
void MainMission5::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,-100,-100,100,-100,200,300,-100 };
		com = new Text(x, -100);
		com->text_insert("�E�`���g", "������[�I���肪�Ƃ��ǎq�I", H_NORMAL, 7);
		com->text_insert("�I���_", "����B���܂��A�g��ꂽ��ˁB", H_NORMAL, 5);
		com->text_insert("�E�`���g", "�V�q���������肪�Ƃ��I�܂���낵���ˁI", H_NORMAL, 7);
		com->text_insert("�C�o���J��", "����B", H_NORMAL, 8);
		com->text_insert("�C�o���J��", "���Ⴀ�A�܂��B", H_NORMAL, 8);
		com->text_insert("�I���_", "�c�c�c�c", H_NORMAL, 5);
		com->text_insert("�E�`���g", "����A���������ēV�q����񌳋C�Ȃ��H�C�̂������ȁH", H_NORMAL, 7);
		com->text_insert("�I���_", "���C���Ȃ��̂͋C�̂�������Ȃ��Ǝv�����ǁc�c��������Ȋ����ˁB", H_NORMAL, 5);
		com->text_insert("�E�`���g", "���������c�c�m���ɂ�����������Ȃ��B���i���猳�C�c�c�c�c", H_NORMAL, 7);
		com->text_insert("�E�`���g", "����H���C���Ȃ��Ƃ������N�[���Ȃ����H", H_NORMAL, 7);
		com->text_insert("�I���_", "�킩��Ȃ��c�c������������Ȃ����ǁc�c", H_NORMAL, 5);
		com->text_insert("�E�`���g", "���ǁH", H_NORMAL, 7);
		com->text_insert("�I���_", "�Ȃ񂾂��c�c�V�q�����Ƃ͋������������B", H_NORMAL, 5);
		com->text_insert("�C�o���J��", "�\�\�͂��c�c�c�c", H_NORMAL, 8);
		com->text_insert("�C�o���J��", "�c�c�_�������āA�킩���Ă�̂Ɂc�c�c�c", H_NORMAL, 8);
		com->text_insert("�C�o���J��", "�\�\����Ȃ��Ⴂ���Ȃ���ˁc�c�{���́B", H_NORMAL, 8);
		com->text_insert("�C�o���J��", "�݂�Ȃ��D�����Ȃ���΂����̂Ɂ\�\", H_NORMAL, 8);
	}
	play_bgm(bgm[0], 1, 13);
	move(5, 8, -200, 6);//�ړ�
	move(13, 5, -300, 300);//�ړ�
	move(13, 7, -300, 300);//�ړ�
	move(14, 8, 400, 6);//�ړ�
	finish(17);
	text_play();
}

//MM6
//�搶�̎�ނƃJ�[�h
MainMission6::MainMission6(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[2];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/piano36.mp3");
	bgm[1] = LoadSoundMem("bgm/piano41.mp3");
	volume_bgm(2);
	set_rare_rate(80, 20, 0, 0);
	size_set(9, 9, 9);
	teacher[0].get_cards()->set_card(Card(13, 0, 600, 200, 80, 60), 0);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 45, 110, 100), 1);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 45, 110, 100), 2);
	teacher[0].get_cards()->set_card(Card(13, 0, 0, 200, 80, 60), 3);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 45, 110, 100), 4);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 45, 110, 100), 5);

	teacher[1].get_cards()->set_card(Card(14, 0, 400, 13, 100, 120), 0);
	teacher[1].get_cards()->set_card(Card(10, 0, 0, 40, 100, 110), 1);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 40, 120, 80), 2);
	teacher[1].get_cards()->set_card(Card(10, 0, 0, 40, 90, 60), 3);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 50, 120, 80), 4);
}
//��b�ƃo�g��������
int MainMission6::play() {
	return normal_text();
}
void MainMission6::text1() {
	if (com == NULL) {
		int x[10] = { -100,-100,-300,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�I�I�^", "�������I�\�\�����҂Ă��āI�I", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "�\�\�\�\�\�\�\�\", H_NORMAL, 8);
		com->text_insert("�I�I�^", "�搶�|���ɍs���񂾂�I�H�����s�����I�I", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "�͂��\�\�����A�������Č������ł���B", H_NORMAL, 8);
		com->text_insert("�I�I�^", "�I�����Ăׂ��Č���������H", H_NORMAL, 2);
		com->text_insert("�I�I�^", "�Ȃ񂾂�[�A�Ȃ�ł���ȉ�������񂾂�I", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "�������Ă�킯����Ȃ��I�I", H_NORMAL, 8);
		com->text_insert("�C�o���J��", "���񂽂ق�ƃo�J�ł���I�I", H_NORMAL, 8);
		com->text_insert("�I�I�^", "�������ȁc�c�I���͌����Ă����Ȃ��ƕ�����˂��B", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "�\�\�\�\�\�\�\�\", H_NORMAL, 8);
		com->text_insert("�I�I�^", "�����Ă����A�Ȃ�ł݂�Ȃ𗊂낤�Ƃ��Ȃ��񂾁H", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "�\�\�\�\�\�\�\�\", H_NORMAL, 8);
		com->text_insert("�I�I�^", "�I���́\�\���̂��Ɓc�c�c�c�厖�ɑz���Ă�񂾂�B", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "���́\�\�\�\�\�\�\�\", H_NORMAL, 8);
		com->text_insert("�C�o���J��", "�\�\���͂��񂽂̂��ƁA�厖�ɑz���ĂȂ��B", H_NORMAL, 8);
		com->text_insert("�I�I�^", "�c�c�c�c�����A����c�c", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "���Ⴂ�A���Ȃ��ŁB�����Ȃ킯����Ȃ��B", H_NORMAL, 8);
		com->text_insert("�C�o���J��", "���Ԃ��Ƃ͎v���Ă�B�ł��\�\", H_NORMAL, 8);
		com->text_insert("�C�o���J��", "����ȏ㒇�ǂ��́A�Ȃ�Ȃ��B", H_NORMAL, 8);
		com->text_insert("�I�I�^", "�c�c�c�c���Ⴀ�\�\", H_NORMAL, 2);
		com->text_insert("�I�I�^", "�F�߂Ă��炦��悤�A�撣��˂��ƂȁI", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "�\�\�\�\�\�\�\�\", H_NORMAL, 8);
		com->text_insert("�I�I�^", "�悵�A���Ⴀ�s�������I�I", H_NORMAL, 2);
	}
	play_bgm(bgm[0], 1, 23);
	move(1, 8, 400, 7);//�ړ�
	move(1, 2, 200, 7);//�ړ�
	move(2, 2, 300, 6);//�ړ�
	finish(23);
	text_play();
}
void MainMission6::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,250,-100,-100,-100,-100,-100,500,-100 };
		com = new Text(x, -100);
		com->text_insert("�I�I�^", "�ӂ��c�c�c�c���A������I�I", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "�c�c�c�c������B", H_NORMAL, 8);
		com->text_insert("�I�I�^", "�c�c�c�c���Ⴀ�A�܂������ȁB", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "�\�\�ǂ�����", H_NORMAL, 8);
		com->text_insert("�I�I�^", "���H", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "���́A���Ȃ������₵�������R��H", H_NORMAL, 8);
		com->text_insert("�C�o���J��", "����ł��A�ǂ����Ď��ɍ\���́H", H_NORMAL, 8);
		com->text_insert("�I�I�^", "�\�\����́\�\", H_NORMAL, 2);
		com->text_insert("�I�I�^", "���O�̂��Ƃ��\�\", H_NORMAL, 2);
		com->text_insert("�I�I�^", "�\�\����A�I���݂͂�Ȃ̂��Ƃ��A��ԑ厖�����炾�B", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "�\�\�͂�A�悭����ȃN�T�C���ƌ�����ˁB", H_NORMAL, 8);
		com->text_insert("�I�I�^", "�ւցc�c���́c�c", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "�c�c�c�c�ق�ƁA�o�J�݂���", H_NORMAL, 8);
		com->text_insert("�I�I�^", "�\�\�\�\�\�\�\�\", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "�\�\�����]�Z�������Ă̂͒m���Ă�H", H_NORMAL, 8);
		com->text_insert("�I�I�^", "���H�\�\���������΁A��N�̎��͂��Ȃ������悤�ȁc�c�c�c", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "���͋��N�����̊w�Z�ɓ]�Z���Ă����B", H_NORMAL, 8);
		com->text_insert("�C�o���J��", "���ꂪ�A���񂽂����Ƌ�����u�����R�B", H_NORMAL, 8);
		com->text_insert("�I�I�^", "�����ƁA���́c�c", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "�\�\�����Əڂ�������Ȃ��ƁA�킩��Ȃ���ˁB", H_NORMAL, 8);
		com->text_insert("�I�I�^", "�c�c�c�c�B", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "�O�̊w�Z�ɁA�e�F�ƌĂׂ�l�������́B", H_NORMAL, 8);
		com->text_insert("�C�o���J��", "���́c�c���̗F�B�̂��Ƃ������ƖY����Ȃ��B", H_NORMAL, 8);
		com->text_insert("�C�o���J��", "�����Ƃ����Ɓc�c�Y����Ȃ��c�c", H_NORMAL, 8);
		com->text_insert("�C�o���J��", "���̊w�Z�ŗF�B���ł��Ă��\�\�P�Ԃ̗F�B�ɂ͂Ȃ�Ȃ��B", H_NORMAL, 8);
		com->text_insert("�C�o���J��", "���̐e�F�Ɣ�ׂĂ��܂��B", H_NORMAL, 8);
		com->text_insert("�C�o���J��", "���̐l�ɔ�ׂ��璇�ǂ��Ȃ��A�C������Ȃ����āc�c�c�c�B", H_NORMAL, 8);
		com->text_insert("�I�I�^", "���c�c�c�c�B", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "�ŒႾ���Ă킩���Ă�B�ł��\�\�\�\�\�\", H_NORMAL, 8);
		com->text_insert("�C�o���J��", "���͖Y��Ȃ��B�\�\�Y�ꂽ���A�Ȃ���c�c�c�c�B", H_NORMAL, 8);
		com->text_insert("�I�I�^", "��������I�I", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "�\�\�\�\�\�\�\�\", H_NORMAL, 8);
		com->text_insert("�I�I�^", "�I�����A�ς��Ă��I�I", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "�\�\�\�\�\�\�\�\", H_NORMAL, 8);
		com->text_insert("�I�I�^", "�I���͒m��˂����ǁA���̗F�B�͑厖�����A�Y�ꂿ�Ⴂ���˂��I", H_NORMAL, 2);
		com->text_insert("�I�I�^", "�\�\�ł��I�I�I��������Ȃ����炢�A���ǂ��Ȃ��Ă݂���I�I", H_NORMAL, 2);
		com->text_insert("�I�I�^", "�]�Z���Ă悩�����A�Ȃ�Ďv�����Ƃ͐�΂ɂȂ��I�I", H_NORMAL, 2);
		com->text_insert("�I�I�^", "���ǁA�����ł̐������悩�������āA�v���Ă��炦�邭�炢�̑��݂ɂȂ�I�I", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "�\�\�\�\�\�\�\�\", H_NORMAL, 8);
		com->text_insert("�I�I�^", "�ߋ��͕ς��邱�Ƃ��ł��˂����ǁc�c���߂č����炢�͕ς��悤���B", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "�\�\�\�\���\�\", H_NORMAL, 8);
		com->text_insert("�I�I�^", "�c�c�c�c", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "�\�\�\�\�\�\����āA�݂��\�\", H_NORMAL, 8);
		com->text_insert("�I�I�^", "�����B", H_NORMAL, 2);
		com->text_insert("�C�o���J��", "�������Ȃ�c�c����Ă݂��I�I", H_NORMAL, 8);
		com->text_insert("�I�I�^", "�����I�I", H_NORMAL, 2);
	}
	play_bgm(bgm[1], 31, 46);
	move(3, 2, 200, 6);//�ړ�
	move(30, 2, 220, 6);//�ړ�
	move(32, 2, 240, 6);//�ړ�
	move(37, 2, 260, 6);//�ړ�
	finish(46);
	text_play();
}

//MM7
//�搶�̎�ނƃJ�[�h
MainMission7::MainMission7(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[2];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/piano27.mp3");
	bgm[1] = LoadSoundMem("bgm/piano22.mp3");
	volume_bgm(2);
	set_rare_rate(100, 0, 0, 0);
	size_set(9, 9, 9);
	teacher[1].get_cards()->set_card(Card(19, 0, 800, 200, 70, 60), 0);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 110, 100), 1);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 110, 100), 2);
	teacher[1].get_cards()->set_card(Card(4, 0, 0, 180, 80, 80), 3);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 110, 100), 4);
	teacher[1].get_cards()->set_card(Card(19, 0, 0, 200, 70, 60), 5);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 110, 100), 6);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 110, 100), 7);
}
//��b�ƃo�g��������
int MainMission7::play() {
	return normal_text();
}
void MainMission7::text1() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,200,-100,-100,-100,-100,-100,400 };
		com = new Text(x, -100);
		com->text_insert("�g���`", "�Ɏu�N�A�����͂�낵���ˁ[�B", H_NORMAL, 9);
		com->text_insert("�C�V", "���A�����B���Ⴀ�A�s�������B", H_NORMAL, 3);
		com->text_insert("�g���`", "�ǂ������́H�ْ����Ă�́H", H_NORMAL, 9);
		com->text_insert("�C�V", "�܁A�܂��A�ˁB", H_NORMAL, 3);
		com->text_insert("�g���`", "�c�c�c�c", H_NORMAL, 9);
		com->text_insert("�C�V", "�c�c�c�c", H_NORMAL, 3);
		com->text_insert("�g���`", "������W�搶��l��������ˁH", H_NORMAL, 9);
		com->text_insert("�C�V", "���A�����B�k�قɂ���炵���B", H_NORMAL, 3);
		com->text_insert("�g���`", "�����͊����H���M����H", H_NORMAL, 9);
		com->text_insert("�C�V", "�S�z�ł́A���邯�ǁB�������Ă͓̂������B", H_NORMAL, 3);
		com->text_insert("�g���`", "���v��B�����l�Ԃ̎������Ă�񂾂���B", H_NORMAL, 9);
		com->text_insert("�C�V", "�́A�͂́c�c�c�c����͂��肪�Ƃ��B", H_NORMAL, 3);
		com->text_insert("�g���`", "���A���������ċ^���Ă�H���̂��Ƃ���܂�m��Ȃ��H", H_NORMAL, 9);
		com->text_insert("�C�V", "�^���Ă͂Ȃ���c�c�ł��A����܂�m��Ȃ����ȁc�c�c�c�B", H_NORMAL, 3);
		com->text_insert("�g���`", "�͂��A������ˁ[�B�N���X�Ⴄ����ˁ[�B", H_NORMAL, 9);
		com->text_insert("�C�V", "�Z��ł鐢�E���Ⴄ����ˁB", H_NORMAL, 3);
		com->text_insert("�g���`", "���A����͔ڋ��ȍl�����ˁ[�Ɏu�N�B", H_NORMAL, 9);
		com->text_insert("�g���`", "�܂��A���Ɠ������x���܂ł����l�Ȃ�ĂقƂ�ǂ��Ȃ��̂����������ǂˁB", H_NORMAL, 9);
	}
	play_bgm(bgm[0], 1, 18);
	move(8, 9, 380, 6);//�ړ�
	finish(18);
	text_play();
}
void MainMission7::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,400,-100,-100,-100,-100,-100, 100 };
		com = new Text(x, -100);
		com->text_insert("�C�V", "�͂��A�͂��c�c�c�c�B", H_NORMAL, 3);
		com->text_insert("�g���`", "�Ɏu�N������[�B���v�[�H", H_NORMAL, 9);
		com->text_insert("�C�V", "�͂��A�͂��B�������傤�A�ԁc�c�B", H_NORMAL, 3);
		com->text_insert("�g���`", "���[��A�Ɏu�N�g���[�j���O�Ƃ��������������񂶂�Ȃ��H", H_NORMAL, 9);
		com->text_insert("�C�V", "�܂��m���Ɂc�c���̂܂܂����΂���ȁc�c", H_NORMAL, 3);
		com->text_insert("�C�V", "�����͂܂������A������키�搶�͍�����苭���������c�c", H_NORMAL, 3);
		com->text_insert("�g���`", "�\�\�����A������́A���ƂȂ񂾂��ǂ��B", H_NORMAL, 9);
		com->text_insert("�g���`", "������̃~�b�V�����A��߂Ƃ��Ȃ��H", H_NORMAL, 9);
		com->text_insert("�C�V", "���H�Ȃ�ŁH", H_NORMAL, 3);
		com->text_insert("�g���`", "�����A�w�Z�̖k�ق��������邩��B", H_NORMAL, 9);
		com->text_insert("�C�V", "�\�\�\�\�\�\�\�\�́H", H_NORMAL, 3);
		com->text_insert("�g���`", "�~�b�V�����Ȃ񂩂���Ă��犪�����܂�Ď��񂶂Ⴄ��B", H_NORMAL, 9);
		com->text_insert("�C�V", "�R�ɂ��Ă����肷�����c�c�Ȃ񂩗p���ł�����́H", H_NORMAL, 3);
		com->text_insert("�g���`", "��������ȃo�J�ȉR���Ǝv���H", H_NORMAL, 9);
		com->text_insert("�C�V", "�c�c���Ⴀ���ɖ{�����Ƃ��āA�Ȃ�ł���Ȃ��Ɓc�c�ƁA�u�p���񂪒m���Ă�񂾂�B", H_NORMAL, 3);
		com->text_insert("�g���`", "������Ɖ\�ŕ������́B�N�̎�ɂ����̂��͂킩��Ȃ��B", H_NORMAL, 9);
		com->text_insert("�C�V", "�܂��܂��������c�c�c�c�������������Ȃ�đ傲�ƁA�w�Z���̂��̂̊�@���B", H_NORMAL, 3);
		com->text_insert("�g���`", "������A�����߂ĕc���Ă�ł��ꂽ�ˁI", H_NORMAL, 9);
		com->text_insert("�C�V", "�c�c�c�c�c�c�c�c", H_NORMAL, 3);
		com->text_insert("�C�V", "����ϐM�����Ȃ��B�䗅�Ƃ����c�������Ȃ�܂������c�c�������ǌN�̌������Ƃ͐M�p�ł��Ȃ��B", H_NORMAL, 3);
		com->text_insert("�g���`", "�Ђǁ[���B", H_NORMAL, 9);
		com->text_insert("�g���`", "���낢��ƁA�Ђǁ[���B", H_NORMAL, 9);
		com->text_insert("�C�V", "������̃~�b�V�����A����ȂɌ��Ȃ�Q�����Ȃ��Ă�����B", H_NORMAL, 3);
		com->text_insert("�C�V", "�ʂ̐l�U������B���c�ɂ��l����`���Ƃ��B", H_NORMAL, 3);
		com->text_insert("�g���`", "������[�A��������񂾂��āB���񂶂Ⴄ��H", H_NORMAL, 9);
		com->text_insert("�C�V", "���c�c�M���ĂȂ����ǁA��΂ɉR�Ƃ͌����؂�Ȃ��̂��炢�c�c", H_NORMAL, 3);
		com->text_insert("�C�V", "���ꂪ�R���Ƃ�����N�A���������Ǝv���񂾂��ǁc�c", H_NORMAL, 3);
		com->text_insert("�g���`", "�R����Ȃ����āB�M���Ă�[�B", H_NORMAL, 9);
		com->text_insert("�g���`", "������͊w�Z���痣�ꂽ�Ƃ��Ńf�[�g���悤��I", H_NORMAL, 9);
		com->text_insert("�C�V", "�\�\���̂����c�c", H_NORMAL, 3);
		com->text_insert("�g���`", "������̗U����B�ґ򂾂Ǝv��Ȃ��H", H_NORMAL, 9);
		com->text_insert("�C�V", "�\�\����ς�", H_NORMAL, 3);
		com->text_insert("�g���`", "�H", H_NORMAL, 9);
		com->text_insert("�C�V", "�N�́A���������B�����Ă�B�T�C�R�p�X���B", H_NORMAL, 3);
		com->text_insert("�g���`", "�\�\�\�\�\�\�\�\", H_NORMAL, 9);
		com->text_insert("�C�V", "���Ⴀ�A�����͂��肪�Ƃ��B������͕ʂ̐l�ƃ~�b�V�����s������B", H_NORMAL, 3);
		com->text_insert("�g���`", "�\�\�\�\�\�\�\�\", H_NORMAL, 9);
	}
	play_bgm(bgm[1], 11, 31);
	move(17, 9, 150, 6);//�ړ�
	move(22, 3, 450, 6);//�ړ�
	move(28, 9, 200, 6);//�ړ�
	move(33, 3, 500, 6);//�ړ�
	move(36, 3, 800, 6);//�ړ�
	finish(37);
	text_play();
}

//MM8
//�搶�̎�ނƃJ�[�h
MainMission8::MainMission8(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[2];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/piano21.mp3");
	bgm[1] = LoadSoundMem("bgm/piano33.mp3");
	volume_bgm(2);
	set_rare_rate(90, 10, 0, 0);
	size_set(9, 9, 9);
	teacher[1].get_cards()->set_card(Card(22, 0, 800, 60, 90, 110), 0);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 90, 110), 1);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 90, 110), 2);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 90, 110), 3);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 90, 110), 4);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 90, 110), 5);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 60, 90, 110), 6);
}
//��b�ƃo�g��������
int MainMission8::play() {
	return normal_text();
}
void MainMission8::text1() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,200,-100,-100,-100,400,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�C�V", "���{����A�����͂�낵���B", H_NORMAL, 3);
		com->text_insert("�E�`���g", "����A��낵���I�I", H_NORMAL, 7);
		com->text_insert("�C�V", "�}�ł��߂�B�u�p���񂪗p���Ő����Q���ł��Ȃ��݂����ŁB", H_NORMAL, 3);
		com->text_insert("�E�`���g", "�����Ȃ񂾁H���͑S�R���v����A�C�ɂ��Ȃ��ŁI", H_NORMAL, 7);
		com->text_insert("�E�`���g", "�������������H�����Ɩ����A��낵���ˁI�I", H_NORMAL, 7);
		com->text_insert("�C�V", "����B���肪�Ƃ��B", H_NORMAL, 3);
	}
	move(6, 3, -500, 6);//�ړ�
	move(6, 7, -500, 6);//�ړ�
	finish(6);
	text_play();
}
void MainMission8::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,100,-100,-100,-100,400,-100, -100 };
		com = new Text(x, -100);
		com->text_insert("�C�V", "�͂��A�͂��A�͂��c�c�c�c", H_NORMAL, 3);
		com->text_insert("�E�`���g", "������[�I���쏕��������I", H_NORMAL, 7);
		com->text_insert("�C�V", "�͂��A�͂��A���肪�A�Ƃ��c�c�B", H_NORMAL, 3);
		com->text_insert("�E�`���g", "���v�H�Ȃɂ����ݕ������Ă��悤���H", H_NORMAL, 7);
		com->text_insert("�C�V", "���v�c�c���肪�Ƃ��B", H_NORMAL, 3);
		com->text_insert("�E�`���g", "�����H�������������͑��߂ɐQ�����������ˁI", H_NORMAL, 7);
		com->text_insert("�C�V", "����B�����͂��肪�Ƃ��B�܂��A�����c�c", H_NORMAL, 3);
		com->text_insert("�E�`���g", "����A���Ⴀ�ˁI", H_NORMAL, 7);
		com->text_insert("�E�`���g", "�c�c���I���������΂��I", H_NORMAL, 7);
		com->text_insert("�C�V", "��A�ǂ����A�����́H", H_NORMAL, 3);
		com->text_insert("�E�`���g", "�u�p�������Đ����p��������́H", H_NORMAL, 7);
		com->text_insert("�C�V", "���A����c�c�܂��c�c", H_NORMAL, 3);
		com->text_insert("�E�`���g", "��[�A����͍������ȁ[�B", H_NORMAL, 7);
		com->text_insert("�E�`���g", "���A�u�p�����ɗ��ݎ����Ă�񂾂��ǂȁ[�B", H_NORMAL, 7);
		com->text_insert("�C�V", "���A���A�����Ȃ́H", H_NORMAL, 3);
		com->text_insert("�E�`���g", "���[���B���\�d�v�ȗ��ݎ��Ȃ񂾁[�B", H_NORMAL, 7);
		com->text_insert("�C�V", "�c�c�c�c", H_NORMAL, 3);
		com->text_insert("�E�`���g", "�d���Ȃ���ˁ[�B�c�O�����ǉ�����������B", H_NORMAL, 7);
		com->text_insert("�C�V", "�\�\���A���߂�I", H_NORMAL, 3);
		com->text_insert("�E�`���g", "���H", H_NORMAL, 7);
		com->text_insert("�C�V", "�p�����Ă����̂́A�R�Ȃ񂾁c�c", H_NORMAL, 3);
		com->text_insert("�E�`���g", "���A�����Ȃ񂾁I�悩�����[�B", H_NORMAL, 7);
		com->text_insert("�C�V", "�c�c�c�c", H_NORMAL, 3);
		com->text_insert("�E�`���g", "�c�c����H���Ⴀ�A�Ȃ�ō��������A���Ƀ~�b�V�������񂾂́H", H_NORMAL, 7);
		com->text_insert("�C�V", "����́c�c�c�c", H_NORMAL, 3);
		com->text_insert("�E�`���g", "�\�\�܁A�S�R�������ǂˁI�����������s����I", H_NORMAL, 7);
		com->text_insert("�C�V", "�c�c�c�c", H_NORMAL, 3);
		com->text_insert("�E�`���g", "�c�c�c�c�Ɏu����H", H_NORMAL, 7);
		com->text_insert("�C�V", "�c�c�c�c���߂�A���́c�c�c�c", H_NORMAL, 3);
		com->text_insert("�E�`���g", "�ǂ������́H���k�Ȃ牽�ł�������I", H_NORMAL, 7);
		com->text_insert("�C�V", "���͂��c�c�I�u�p���񂪁I", H_NORMAL, 3);
		com->text_insert("�C�V", "�����A�w�Z�̖k�ق�����������Č����������񂾁I�I", H_NORMAL, 3);
		com->text_insert("�E�`���g", "���I�H", H_NORMAL, 7);
		com->text_insert("�C�V", "�ق�ƖŒ��ꒃ����I�I�����A�����Ă�񂾁I�I", H_NORMAL, 3);
		com->text_insert("�C�V", "������΂��z����I�I�ꏏ�Ƀ~�b�V�����s���̂����������񂾁I�I", H_NORMAL, 3);
		com->text_insert("�E�`���g", "�Ɏu������m���Ă��񂾁[�I�I", H_NORMAL, 7);
		com->text_insert("�C�V", "�\�\�\�\�\�\�\�\", H_NORMAL, 3);
		com->text_insert("�E�`���g", "�����Ǝv��Ȃ��H�w�Z���j�I�I", H_NORMAL, 7);
		com->text_insert("�E�`���g", "��킸�ɐ搶����C�ɂ�ꂿ�Ⴄ�񂾂�I", H_NORMAL, 7);
		com->text_insert("�E�`���g", "�u�p�����ɂ͓����A���͂𗊂�ł��񂾁[�I", H_NORMAL, 7);
		com->text_insert("�E�`���g", "�ł��Ɏu������m�����Ȃ�A���������������͂��Ă���Ȃ��H", H_NORMAL, 7);
		com->text_insert("�E�`���g", "�����A�k�قŐ搶�������t���Ăق����́I", H_NORMAL, 7);
		com->text_insert("�E�`���g", "����ŁA���j�M���M���Ŗk�ق��瓦���āA�Ȃ�ׂ���������̐搶��|���́I�I", H_NORMAL, 7);
		com->text_insert("�E�`���g", "�u�p�����ƈɎu����͓��ɕ����ꂽ�Ƃ��āc�c�c�c", H_NORMAL, 7);
		com->text_insert("�E�`���g", "�搶���R�l���Ђ������炻�ꂾ���łU�l��ꂿ�Ⴄ�ˁI�I", H_NORMAL, 7);
		com->text_insert("�E�`���g", "���[�y���݂��ˁ[�I�݂�ȋ����Ċ�Ԃ�I�I", H_NORMAL, 7);
	}
	play_bgm(bgm[0], 1, 18);
	play_bgm(bgm[1], 37, 46);
	move(8, 7, 450, 6);//�ړ�
	move(18, 3, 150, 6);//�ړ�
	move(33, 3, 200, 6);//�ړ�
	move(35, 7, 400, 6);//�ړ�
	move(37, 7, 350, 6);//�ړ�
	move(38, 7, 300, 6);//�ړ�
	event(42, "event/�E�`���g�{��.png");//�}�G
	finish(46);
	text_play();
}

//MM9
//�搶�̎�ނƃJ�[�h
MainMission9::MainMission9(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[2];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/piano36.mp3");
	bgm[1] = LoadSoundMem("bgm/piano26.mp3");
	volume_bgm(2);
	set_rare_rate(80, 20, 0, 0);
	size_set(9, 9, 9);
	teacher[0].get_cards()->set_card(Card(22, 0, 250, 60, 100, 120), 0);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 60, 100, 120), 1);
	teacher[0].get_cards()->set_card(Card(7, 0, 0, 35, 80, 90), 2);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 60, 100, 120), 3);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 60, 100, 120), 4);

	teacher[1].get_cards()->set_card(Card(22, 0, 250, 65, 110, 110), 0);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 65, 110, 110), 1);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 65, 110, 110), 2);
	teacher[1].get_cards()->set_card(Card(6, 0, 0, 25, 140, 110), 3);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 65, 110, 110), 4);

	teacher[2].get_cards()->set_card(Card(22, 0, 250, 50, 100, 110), 0);
	teacher[2].get_cards()->set_card(Card(8, 0, 0, 20, 90, 80), 1);
	teacher[2].get_cards()->set_card(Card(22, 0, 0, 50, 100, 110), 2);
	teacher[2].get_cards()->set_card(Card(8, 0, 0, 20, 90, 80), 3);
	teacher[2].get_cards()->set_card(Card(22, 0, 0, 50, 100, 110), 4);
}
//��b�ƃo�g��������
int MainMission9::play() {
	return normal_text();
}
void MainMission9::text1() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,200,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, 500);
		com->text_insert("�C�V", "�͂��A�͂��A�͂��c�c�c�c", H_NORMAL, 3);
		com->text_insert("�C�V", "�i�ǂ����悤�A�ǂ����悤�c�c�c�c�I�I�j", H_MOUSOU, 3);
		com->text_insert("�C�V", "�i���ǒN�ɂ��c�c�䗅�ɂ���ł������邱�Ƃ��ł��Ȃ������c�c�j", H_MOUSOU, 3);
		com->text_insert("�C�V", "�ǂ�����΁c�c�ǂ�����΁c�c�c�c", H_NORMAL, 3);
		com->text_insert("�C�V", "�i���������l��l�Ő搶�O�l�������t���邱�Ƃ��疳�����c�c�I�j", H_MOUSOU, 3);
		com->text_insert("�C�V", "�i���̂����Z�ɂ����j�c�c���߂��A�l�c�c�c�c�I�I�j", H_MOUSOU, 3);
		com->text_insert("�C�V", "���ǁc�c�c�c", H_NORMAL, 3);
		com->text_insert("�C�V", "�i�u�p����̌��������Ƃ͉R����Ȃ������c�c�ޏ��͋����Ă邩��^�����͎̂d���Ȃ����ǁc�c�j", H_MOUSOU, 3);
		com->text_insert("�C�V", "�܂����A���{����܂ŋ����Ă��Ȃ�āc�c�c�c�I�I", H_NORMAL, 3);
		com->text_insert("�C�V", "�i����ς薳�����c�c�I�I������ł��x���Ȃ��A�����c�c�j", H_MOUSOU, 3);

		com->text_insert("�g���`", "�\�\�����c�c����ς�A���Ă��񂾁B", H_NORMAL, 9);
		com->text_insert("�C�V", "�\�\�\�\�\�\�\�\��", H_NORMAL, 3);
		com->text_insert("�g���`", "����ق��c�c�Ɏu�N�c�c", H_NORMAL, 9);
		com->text_insert("�C�V", "�ǂ��A���āc�c�c�c", H_NORMAL, 3);
		com->text_insert("�C�V", "�i�{���{���̎p�A���؂�A���̓u�p���񂪁c�c�j", H_MOUSOU, 3);
		com->text_insert("�C�V", "�\�\�l�́c�c�c�c", H_NORMAL, 3);
		com->text_insert("�g���`", "���A�ڂ����Ƃ��Ȃ��B�s���`�Ȃ�ł���H", H_NORMAL, 9);
		com->text_insert("�g���`", "��������������v�B�������Ɠ|���H", H_NORMAL, 9);
	}
	play_bgm(bgm[0], 1, 18);
	move(10, 9, 100, 3);//�ړ�
	move(17, 9, 300, 6);//�ړ�
	finish(18);
	text_play();
}
void MainMission9::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,100,-100,-100,-100,-100,-100, 200 };
		com = new Text(x, -100);
		com->text_insert("�C�V", "�͂����A�͂��c�c�c�c�A������c�c�c�c", H_NORMAL, 3);
		com->text_insert("�g���`", "�˂��A������ƊO�łȂ��H��ꂿ������B�����I", H_NORMAL, 9);
		com->text_insert("�C�V", "�����A��������ĉB���Ȃ��Ă�����c�c�B", H_NORMAL, 3);
		com->text_insert("�g���`", "�\�\�\�\�\�\�\�\", H_NORMAL, 9);
		com->text_insert("�C�V", "���{����ɋ��͂𗊂܂�Ă��񂾂�ˁc�c�c�c", H_NORMAL, 3);
		com->text_insert("�g���`", "�\�\�����A����ς����m���Ă��́B", H_NORMAL, 9);
		com->text_insert("�C�V", "���낻�딚������񂾂�ˁH�}�����B", H_NORMAL, 3);
		com->text_insert("�g���`", "�����ˁ[�c�c�c�c", H_NORMAL, 9);
		com->text_insert("�C�V", "�c�c�c�c", H_NORMAL, 3);
		com->text_insert("�g���`", "�c�c�c�c", H_NORMAL, 9);
		com->text_insert("�C�V", "�c�c�c�c�c�c�c�c", H_NORMAL, 3);
		com->text_insert("�g���`", "�c�c�c�c�c�c�c�c", H_NORMAL, 9);
		com->text_insert("�C�V", "�c�c�c�c�����A���Ȃ��c�c�c�c", H_NORMAL, 3);
		com->text_insert("�g���`", "���{�̌��Ԃ肩�炵�āA����Ȃɂ������񔚒e�p�ӂ��Ă�킯����Ȃ�������������ˁ[�c�c�c�c", H_NORMAL, 9);
		com->text_insert("�C�V", "�܂����c�c�c�c�u�p�A����c�c", H_NORMAL, 3);
		com->text_insert("�g���`", "����ς���{�ɂ͂т����肵���[�H", H_NORMAL, 9);
		com->text_insert("�g���`", "�Z�ɔ����Ȃ�Ă˂��B�������̎����т����肵����B", H_NORMAL, 9);
		com->text_insert("�C�V", "�ǂ����āc�c�c�c", H_NORMAL, 3);
		com->text_insert("�C�V", "���{�����ᔻ���Ȃ��́c�c�c�c�H", H_NORMAL, 3);
		com->text_insert("�C�V", "����Ɂc�c�c�c�ǂ����Ĕ��c�Ƃ��Ɍ���Ȃ������񂾂�B", H_NORMAL, 3);
		com->text_insert("�g���`", "�Ɏu�N�����ĒN�ɂ�����Ȃ������ł���H", H_NORMAL, 9);
		com->text_insert("�g���`", "�����Ƃ݂�ȍ������邵�A���{���݂�ȂɌ����邩������Ȃ��ł���[�H", H_NORMAL, 9);
		com->text_insert("�C�V", "�ł��A�ޏ��͂��ꂾ���̂��Ƃ�������킯�����c�c�c�c", H_NORMAL, 3);
		com->text_insert("�g���`", "�Ɏu�N�����Ă킩���Ă邭���Ɂ[�B", H_NORMAL, 9);
		com->text_insert("�g���`", "�������{�Ɉ��C�͂Ȃ���ˁB", H_NORMAL, 9);
		com->text_insert("�C�V", "�c�c�c�c", H_NORMAL, 3);
		com->text_insert("�g���`", "����̌��́A���ꂪ�����ƒN���s�K�ɂȂ�Ȃ��I���������̂�B", H_NORMAL, 9);
		com->text_insert("�C�V", "�c�c�c�c", H_NORMAL, 3);
		com->text_insert("�C�V", "�c�c�˂��A�u�p����c�c", H_NORMAL, 3);
		com->text_insert("�g���`", "��H", H_NORMAL, 9);
		com->text_insert("�C�V", "���߂�c�c�l���Ⴂ���Ă������c�c���Ă��B", H_NORMAL, 3);
		com->text_insert("�g���`", "�Ɏu���[��", H_NORMAL, 9);
		com->text_insert("�C�V", "�u�p������āA����ς�c�c�c�c", H_NORMAL, 3);
		com->text_insert("�g���`", "�Ɏu���[�[�[�[�[��I�I�I", H_NORMAL, 9);
		com->text_insert("�C�V", "���A�ȁA�ȂɁI�H", H_NORMAL, 3);
		com->text_insert("�g���`", "�f�[�g�A���悤��I", H_NORMAL, 9);
	}
	play_bgm(bgm[1], 16, 36);
	move(8, 3, -1400, 6);//�ړ�
	move(8, 9, -1300, 6);//�ړ�
	move(9, 3, 400, 3000);//�ړ�
	move(9, 9, 500, 3000);//�ړ�
	finish(8);
	finish(36);
	event(35, "event/�g���`�f�[�g���U.png");//�}�G
	text_play();
}

//MM10
//�搶�̎�ނƃJ�[�h
MainMission10::MainMission10(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/piano21.mp3");
	//bgm[1] = LoadSoundMem("bgm/piano26.mp3");
	volume_bgm(1);
	set_rare_rate(80, 20, 0, 0);
	size_set(9, 9, 9);
	teacher[0].get_cards()->set_card(Card(3, 0, 400, 60, 100, 120), 0);
	teacher[0].get_cards()->set_card(Card(3, 0, 0, 60, 100, 120), 1);
	teacher[0].get_cards()->set_card(Card(2, 0, 0, 30, 80, 90), 2);
	teacher[0].get_cards()->set_card(Card(3, 0, 0, 60, 100, 120), 3);
	teacher[0].get_cards()->set_card(Card(3, 0, 0, 60, 100, 120), 4);

	teacher[1].get_cards()->set_card(Card(22, 0, 400, 45, 110, 110), 0);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 45, 110, 110), 1);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 45, 110, 110), 2);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 45, 110, 110), 3);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 45, 110, 110), 4);

	teacher[2].get_cards()->set_card(Card(22, 0, 400, 50, 120, 110), 0);
	teacher[2].get_cards()->set_card(Card(24, 0, 0, 30, 80, 90), 1);
	teacher[2].get_cards()->set_card(Card(22, 0, 0, 50, 120, 110), 2);
	teacher[2].get_cards()->set_card(Card(22, 0, 0, 50, 120, 110), 3);
	teacher[2].get_cards()->set_card(Card(22, 0, 0, 50, 120, 110), 4);
}
//��b�ƃo�g��������
int MainMission10::play() {
	return normal_text();
}
void MainMission10::text1() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,200,300,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�C�V", "���߂�䗅�B���͂��Ă�����āB", H_NORMAL, 3);
		com->text_insert("�^�C��", "�S�R�������B���ł������Ă���B", H_NORMAL, 4);
		com->text_insert("�C�V", "�ł��c�c�c�c�����m�́H", H_NORMAL, 3);
		com->text_insert("�^�C��", "�����͏m����Ȃ��B����Ɂc�c", H_NORMAL, 4);
		com->text_insert("�^�C��", "�m���炢�x�ނ��B�����ŕ׋�����΂����B", H_NORMAL, 4);
		com->text_insert("�C�V", "���ǎ��Ɨ������������Ȃ�����H", H_NORMAL, 3);
		com->text_insert("�^�C��", "�ӁA����Ȃ̌���C�ɂ��邱�Ƃ���Ȃ��B", H_NORMAL, 4);
		com->text_insert("�C�V", "�����͌����Ă����c�c�c�c", H_NORMAL, 3);
		com->text_insert("�^�C��", "�e������ɏm���������Ă邾�����B", H_NORMAL, 4);
		com->text_insert("�^�C��", "�V�т�F�B�̕����׋������厖���B", H_NORMAL, 4);
		com->text_insert("�C�V", "�͂́c�c���΁A�V�˂͌������Ƃ��Ⴄ�ȁB", H_NORMAL, 3);
		
	}
	finish(11);
	text_play();
}
void MainMission10::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,400,500,-100,-100,-100,-100, -100 };
		com = new Text(x, -100);
		com->text_insert("�C�V", "�͂��A�͂��c�c�����͂��肪�Ƃ��B", H_NORMAL, 3);
		com->text_insert("�^�C��", "�\���葁���I������B�܂�����܂Ŏ��Ԃ���ȁB", H_NORMAL, 4);
		com->text_insert("�^�C��", "�ǂ�������ċA��Ȃ����H", H_NORMAL, 4);
		com->text_insert("�C�V", "�����ˁB�c�c�ł����v�H", H_NORMAL, 3);
		com->text_insert("�^�C��", "�����H�\�\�e�ɓ{���Ȃ������Ă��Ƃ��H", H_NORMAL, 4);
		com->text_insert("�C�V", "�����B����O�ł��x��������V��ł���ċ^����񂶂�Ȃ����H", H_NORMAL, 3);
		com->text_insert("�^�C��", "����͐S�z���������B���̐e�͂����܂Ō������Ȃ��B", H_NORMAL, 4);
		com->text_insert("�^�C��", "�������������Ă��w�Z�Ɏc���ĕ׋����Ă����Č����΂����B", H_NORMAL, 4);
		com->text_insert("�C�V", "�c�c�c�c", H_NORMAL, 3);
		com->text_insert("�^�C��", "�\�\�܂����̓��̂��ƋC�ɂ��Ă�̂��H", H_NORMAL, 4);
		com->text_insert("�C�V", "����\�\���v�B�s�������B", H_NORMAL, 3);
		com->text_insert("�^�C��", "�����B", H_NORMAL, 4);
		com->text_insert("�^�C��", "�i���v�c�c�c�c���B�j", H_MOUSOU, 4);
	
	}
	play_bgm(bgm[0], 1, 13);
	move(13, 3, -1400, 6);//�ړ�
	move(13, 4, -1300, 6);//�ړ�
	finish(13);
	text_play();
}

//MM11
//�搶�̎�ނƃJ�[�h
MainMission11::MainMission11(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/piano37.mp3");
	//bgm[1] = LoadSoundMem("bgm/piano26.mp3");
	volume_bgm(1);
	set_rare_rate(75, 25, 0, 0);
	size_set(9, 9, 9);
	teacher[0].get_cards()->set_card(Card(8, 0, 700, 40, 80, 100), 0);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 60, 120, 120), 1);
	teacher[0].get_cards()->set_card(Card(8, 0, 0, 40, 80, 100), 2);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 60, 120, 120), 3);
	teacher[0].get_cards()->set_card(Card(8, 0, 0, 40, 80, 100), 4);

	teacher[1].get_cards()->set_card(Card(14, 0, 700, 13, 90, 110), 0);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 65, 110, 110), 1);
	teacher[1].get_cards()->set_card(Card(14, 0, 0, 13, 90, 110), 2);
	teacher[1].get_cards()->set_card(Card(10, 0, 0, 20, 80, 100), 3);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 65, 110, 110), 4);
}
//��b�ƃo�g��������
int MainMission11::play() {
	return normal_text();
}
void MainMission11::text1() {
	if (com == NULL) {
		int x[10] = { -100,700,-100,-100,-100,-100,-100,-100,-100,200 };
		com = new Text(x, -100);
		com->text_insert("�g���`", "�����͊w�Z�I�������Ɏu�N���炩���ɍs�����Ǝv�����̂Ɂc�c", H_NORMAL, 9);
		com->text_insert("�g���`", "�i���c�͂��ƈ�l�ɐ���������Č����Ă����ǁA�ǂ������c�c�j", H_MOUSOU, 9);
		com->text_insert("�g���`", "�������Ăق����ȁc�c�c�c", H_NORMAL, 9);
		com->text_insert("�g���`", "�i�܂����c�͏��Ȃ��Ƃ��x������l�ł͂Ȃ����ǁA�������ƏI��点�ċA�肽���Ȃ��B�j", H_MOUSOU, 9);
		com->text_insert("�g���`", "�Ȃ񂩍����́c�c�c�c���C�N���Ȃ��Ȃ��B", H_NORMAL, 9);
	}
	move(5, 1, 400, 3);
	finish(5);
	text_play();
}
void MainMission11::text2() {
	if (com == NULL) {
		int x[10] = { -100,200,-100,-100,-100,-100,-100,-100,-100, 400 };
		com = new Text(x, -100);
		com->text_insert("�n���_", "������B�܂������B", H_NORMAL, 1);
		com->text_insert("�g���`", "������[�B", H_NORMAL, 9);

		com->text_insert("�g���`", "�\�\���A�Ɏu�N����[��I", H_NORMAL, 9);
		com->text_insert("�C�V", "���A���A�u�p����B", H_NORMAL, 3);
		com->text_insert("�g���`", "�ǂ������́H����Ȏ��Ԃ܂Ŏc���āB", H_NORMAL, 9);
		com->text_insert("�C�V", "�����A�ψ���̎d���������āB", H_NORMAL, 3);
		com->text_insert("�g���`", "�����Ȃ񂾁[�B���Ⴀ������A��́H", H_NORMAL, 9);
		com->text_insert("�C�V", "����B�܂��c�c�c�c", H_NORMAL, 3);
		com->text_insert("�g���`", "���Ⴀ�ꏏ�Ɂ\�\����A�䗅�N�ƋA��̂��ȁH", H_NORMAL, 9);
		com->text_insert("�C�V", "����A�����c�c���߂�B", H_NORMAL, 3);
		com->text_insert("�g���`", "���₢��A�C�ɂ��Ȃ��ŁB�䗅�N�́H�ǂ��ɂ���́H", H_NORMAL, 9);
		com->text_insert("�C�V", "�l���ψ���̎d���I���܂Ő}�����ŕ׋�������āB", H_NORMAL, 3);
		com->text_insert("�C�V", "���}�����֌������Ƃ��Ȃ񂾁B", H_NORMAL, 3);
		com->text_insert("�g���`", "�Ȃ�قǂˁB", H_NORMAL, 9);
		com->text_insert("�C�V", "����A���Ⴀ�A�ˁB�܂������c�c", H_NORMAL, 3);
		com->text_insert("�g���`", "�܂��ˁ[�c�c", H_NORMAL, 9);
		com->text_insert("�g���`", "�c�c�˂��Ɏu�N�A�ǂ��������́H", H_NORMAL, 9);
		com->text_insert("�C�V", "���A�����c�c�H", H_NORMAL, 3);
		com->text_insert("�g���`", "�Ȃ񂩌��C�Ȃ������Ɍ��������炳�B���k�Ȃ畷���Ă������H", H_NORMAL, 9);
		com->text_insert("�C�V", "����A���v�B���肪�Ƃ��B", H_NORMAL, 3);
		com->text_insert("�g���`", "�c�c�c�c", H_NORMAL, 9);
		com->text_insert("�C�V", "�c�c�c�c", H_NORMAL, 3);
		com->text_insert("�C�V", "�c�c���́A���B���Ⴀ������u���Ă����H", H_NORMAL, 3);
		com->text_insert("�g���`", "�������B�Ȃ�ł��I", H_NORMAL, 9);
		com->text_insert("�C�V", "��l�ɂȂ���Ăǂ��������Ƃ��Ǝv���H", H_NORMAL, 3);
		com->text_insert("�g���`", "��l�ɂȂ���Ăǂ��������Ƃ��c�c�c�c�H", H_NORMAL, 9);
		com->text_insert("�g���`", "�ȂɈɎu�N�A��l�ɂȂ�́H�����S�������ˁB", H_NORMAL, 9);
		com->text_insert("�C�V", "��l�ɂȂ�̂��Ă������Ƃ��ȁH", H_NORMAL, 3);
		com->text_insert("�g���`", "�c�c�c�c", H_NORMAL, 9);
		com->text_insert("�g���`", "�c�c�Ȃ�قǁA�ˁB", H_NORMAL, 9);
		com->text_insert("�g���`", "���̌��������킹�Ă��炤�Ɓc�c�c�c���R�������Ƃ��Ǝv���B", H_NORMAL, 9);
		com->text_insert("�g���`", "����͒N�����ʂ�ׂ����ł��邵�A��l�ɂȂ�Ȃ�������Љ�ɏo���Ƃ��ɍ��邩��B", H_NORMAL, 9);
		com->text_insert("�g���`", "�ł��c�c��l�ɂȂ邱�Ƃ��\�\�K���Ƃ͌���Ȃ��B", H_NORMAL, 9);
		com->text_insert("�C�V", "�c�c�c�c", H_NORMAL, 3);
		com->text_insert("�g���`", "���̗\�z�Ƃ��Ắc�c����ȂƂ��낾��ˁB�Ɏu�N���Y��ł�̂́B", H_NORMAL, 9);
		com->text_insert("�C�V", "�c�c�c�c���΁A�u�p���񂾁B", H_NORMAL, 3);
		com->text_insert("�g���`", "�܁A���͊����l�Ԃ�����ˁB�\�\�ŁA���̎���̓���������", H_NORMAL, 9);
		com->text_insert("�g���`", "��l�ɂȂ���Ă����̂́c�c�䖝���邱�Ƃ��Ǝv���ˁA���́B", H_NORMAL, 9);
		com->text_insert("�C�V", "�\�\�䖝�A���c�c�c�c�B", H_NORMAL, 3);
		com->text_insert("�g���`", "�܂��H���݂����Ȋ����l�ԂȂ�܂������Ɏu�N�݂����ȃ_���_���l�Ԃ��Ⴀ�Y��œ��R�ˁB", H_NORMAL, 9);
		com->text_insert("�C�V", "�͂́c�c�c�c", H_NORMAL, 3);
		com->text_insert("�g���`", "�c�c�܁A�����܂Ŏ��̍l�������ǂˁB", H_NORMAL, 9);
		com->text_insert("�g���`", "����͒N�����ʂ铹���Ǝv����B�����ɔY�݋ꂵ�݂��܂��A�Ɏu����N�B", H_NORMAL, 9);
		com->text_insert("�C�V", "�c�c�c�c�䖝���邱�Ƃ���l�ɂȂ邱�Ƃ����Ă����Ȃ�", H_NORMAL, 3);
		com->text_insert("�C�V", "�F�B�ƍ��܂Œʂ�ɕt���������Ƃ��䖝���Ȃ��Ƃ����Ȃ���ˁH", H_NORMAL, 3);
		com->text_insert("�g���`", "�c�c�c�c", H_NORMAL, 9);
		com->text_insert("�g���`", "�c�c�Ɏu�N�A�����", H_NORMAL, 9);
		com->text_insert("�g���`", "���́u�F�B�v�ƒ��ژb���ׂ����Ƃ�", H_NORMAL, 9);
	}
	play_bgm(bgm[0], 3, 48);
	move(2, 1, -1400, 4);//�ړ�
	move(2, 9, 1200, 6);//�ړ�
	move(3, 3, 200, 1000);//�ړ�
	move(3, 9, 800, 1000);//�ړ�
	move(4, 9, 350, 6);//�ړ�
	move(16, 3, 100, 3);//�ړ�
	move(26, 9, 300, 6);//�ړ�
	finish(2);
	finish(48);
	text_play();
}

//MM12
//�搶�̎�ނƃJ�[�h
MainMission12::MainMission12(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[2];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/piano24.mp3");
	bgm[1] = LoadSoundMem("bgm/piano31.mp3");
	volume_bgm(2);
	set_rare_rate(70, 25, 5, 0);
	size_set(9, 9, 9);
	teacher[1].get_cards()->set_card(Card(19, 0, 1800, 240, 70, 80), 0);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 75, 110, 110), 1);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 75, 110, 110), 2);
	teacher[1].get_cards()->set_card(Card(19, 0, 0, 240, 70, 80), 3);
	teacher[1].get_cards()->set_card(Card(4, 0, 0, 180, 100, 130), 4);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 75, 110, 110), 5);
	teacher[1].get_cards()->set_card(Card(19, 0, 0, 240, 70, 80), 6);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 75, 110, 110), 7);
}
//��b�ƃo�g��������
int MainMission12::play() {
	return normal_text();
}
void MainMission12::text1() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,-100,300,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�^�C��", "�i����ς�����̂͂��̓������������������B�j", H_MOUSOU, 4);
		com->text_insert("�^�C��", "�i�{�l�����ɒ��ڑ��k���邱�Ƃ͂Ȃ��������\�\�j", H_MOUSOU, 4);
		com->text_insert("�^�C��", "�i���̓�����A����͂����炳�܂ɉ��Ƌ�����u���悤�ɂȂ����B�j", H_MOUSOU, 4);
		com->text_insert("�^�C��", "�i�c��������\�\�������͂����ƈꏏ�������B�j", H_MOUSOU, 4);
		com->text_insert("�^�C��", "�i����͍����ς��Ȃ��B�����S�������֌W�Ƃ����킯�ł͂Ȃ��B�j", H_MOUSOU, 4);
		com->text_insert("�^�C��", "�i���́c�c�悭�����΁A��l�ɂȂ����Ƃ������Ƃ��낤���B�j", H_MOUSOU, 4);
		com->text_insert("�^�C��", "�i���̓��c�c���̐e���A�V�тɗU���p�x��������ƌ���ɒ��ӂ������̓��\�\�j", H_MOUSOU, 4);
		com->text_insert("�^�C��", "�i����͎��Ԃ̗�������������񂾂낤�B�j", H_MOUSOU, 4);
		com->text_insert("�^�C��", "�i�����\�\����͉����ꏏ�������B�j", H_MOUSOU, 4);
		com->text_insert("�^�C��", "�i�����Ƃ����ƁA��l�ɂȂ��Ă���������悤�ȓ��X�͑����Ȃ��ƁB���������񂾁B�j", H_MOUSOU, 4);
		com->text_insert("�^�C��", "�i�C�����͂킩��B����A���O�̍l���͊Ԉ���Ă��Ȃ��B�j", H_MOUSOU, 4);
		com->text_insert("�^�C��", "�i�����ȁA����A���͂܂��q���ł������񂾂�B�j", H_MOUSOU, 4);
		com->text_insert("�^�C��", "�i���͂܂��A���O�ƃo�J�������X���߂��������񂾂�B�j", H_MOUSOU, 4);
		com->text_insert("�^�C��", "�i���̐e�����΂��悤���A�搶���{�낤���A���B�ɂ͊֌W�Ȃ������͂����낤�H�j", H_MOUSOU, 4);
		com->text_insert("�^�C��", "�i�\�\�����Č���A���O��1�����A����I�ɊԈ���Ă���B�j", H_MOUSOU, 4);
		com->text_insert("�^�C��", "�i���O�͉��̂��Ƃ�V�˂��Ƃ������A�{���̓V�˂͂��O�̂悤�Ȃ�̂��Ƃ������񂾂�B�j", H_MOUSOU, 4);
		com->text_insert("�^�C��", "�i�m���ɉ��͕׋��ʂ̂��Ɋw�N�P���̏��ʈȊO���Ƃ������Ƃ��Ȃ��B�j", H_MOUSOU, 4);
		com->text_insert("�^�C��", "�i�����ȁA�e�X�g�Ȃ�Ă��̂͗��_�I�ɍl���ē����𓱂��΁A��΂ɐ�����������������񂾂�B�j", H_MOUSOU, 4);
		com->text_insert("�^�C��", "�i����A���O�͓V�˂��B�j", H_MOUSOU, 4);
		com->text_insert("�^�C��", "�i���܂ŁA���O�͉��x���V�����V�тŉ��̐��E��ς��Ă��ꂽ�ȁB�j", H_MOUSOU, 4);
		com->text_insert("�^�C��", "�i�Ȃ��A����B������x�A���ɖ����Ă����B�j", H_MOUSOU, 4);
		com->text_insert("�^�C��", "�i�^�ʖڂȓ��X�Ȃ�đދ����B�h���I�ȓ��X�����ɂ����A����B�j", H_MOUSOU, 4);
	}
	play_bgm(bgm[0], 1, 22);
	finish(22);
	text_play();
}
void MainMission12::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,-100,200,-100,-100,-100,-100,-100 };
		com = new Text(x, 500);
		com->text_insert("W�搶", "���A���c�c�c�c���́A�K�L���c�c�I�I", H_NORMAL, 10);
		com->text_insert("W�搶", "�Ă߂���݂����ȕs�ǂ́c�c�Љ�s�K���҂Ȃ񂾂�I�I", H_NORMAL,10);
		com->text_insert("�^�C��", "�K�L�c�c�c�c���c�c�c�c", H_NORMAL, 4);
		com->text_insert("�^�C��", "�c�c�c�c", H_NORMAL, 4);
		com->text_insert("�^�C��", "�Ȃ��A�搶�c�c�c�c", H_NORMAL, 4);
		com->text_insert("�^�C��", "���񂽂́c�c�c�c���̕ӂɗ����Ă�΂���ŉ����ł���H", H_NORMAL, 4);
		com->text_insert("W�搶", "�����c�c�I�H�������Ă񂾂Ă߂��c�c�I�I", H_NORMAL, 10);
		com->text_insert("�^�C��", "��������ȁc�c���̕ӂ̐΂���Ŋۈ���y���߂�l������񂾂���c�c", H_NORMAL, 4);
		com->text_insert("W�搶", "�c�c�c�c", H_NORMAL, 10);
		com->text_insert("�^�C��", "���O���l�ɂ́A��������ȁB", H_NORMAL, 4);
		com->text_insert("�^�C��", "��l�ɂƂ��āA�΂���́A�����̐΂��낾�B", H_NORMAL, 4);
	}
	play_bgm(bgm[1], 1, 11);
	finish(11);
	text_play();
}

//MM13
//�搶�̎�ނƃJ�[�h
MainMission13::MainMission13(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[2];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/piano24.mp3");
	bgm[1] = LoadSoundMem("bgm/pianoMM13.mp3");
	volume_bgm(2);
	set_rare_rate(70, 25, 5, 0);
	size_set(9, 9, 9);
	teacher[0].get_cards()->set_card(Card(7, 0, 900, 30, 80, 90), 0);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 70, 100, 120), 1);
	teacher[0].get_cards()->set_card(Card(7, 0, 0, 30, 80, 90), 2);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 70, 100, 120), 3);
	teacher[0].get_cards()->set_card(Card(7, 0, 0, 30, 80, 90), 4);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 70, 100, 120), 5);

	teacher[1].get_cards()->set_card(Card(6, 0, 900, 25, 140, 110), 0);
	teacher[1].get_cards()->set_card(Card(6, 0, 0, 25, 140, 110), 1);
	teacher[1].get_cards()->set_card(Card(6, 0, 0, 25, 140, 110), 2);
	teacher[1].get_cards()->set_card(Card(6, 0, 0, 25, 140, 110), 3);
	teacher[1].get_cards()->set_card(Card(6, 0, 0, 25, 140, 110), 4);
	teacher[1].get_cards()->set_card(Card(6, 0, 0, 25, 140, 110), 5);
}
//��b�ƃo�g��������
int MainMission13::play() {
	return normal_text();
}
void MainMission13::text1() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,250,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�C�V", "�i�^�ʖڂȓz���Ȃ��Ă����Ǝv���Ă��B�j", H_NORMAL, 3);
		com->text_insert("�C�V", "�i�^�C���͕׋����̂��D���Ȃ킯����Ȃ��B�j", H_NORMAL, 3);
		com->text_insert("�C�V", "�i�����A�����������������̂͑S���A�m�I�Ȃ��̂������B�j", H_NORMAL, 3);
		com->text_insert("�C�V", "�i�ŏ��̂�����S�O�����������B�j", H_NORMAL, 3);
		com->text_insert("�C�V", "�i�l�Ȃ񂩂ƗV��łāA���S�ł͑ދ����Ă�񂶂�Ȃ������ĕs���������B�j", H_NORMAL, 3);
		com->text_insert("�C�V", "�i�ł��A���v���΁\�\�����͂����{�C�Ŋy����ł��񂾂낤�ȁB�j", H_NORMAL, 3);
		com->text_insert("�C�V", "�i�����ǁA�^�C���̕ꂳ��ɒ��ӂ���Ă���A�^�C���ƗV�Ԃ��Ƃ��܂��S�O���悤�ɂȂ����B�j", H_NORMAL, 3);
		com->text_insert("�C�V", "�i�v���o�����񂾁B�\�\�����͖l�Ɛ����Ă鐢�E���Ⴄ���āB�j", H_NORMAL, 3);
		com->text_insert("�C�V", "�i�l�Ȃ񂩂ƕt�������Ă���\�\�����̏����ɓD�������Ⴄ�񂾁B�j", H_NORMAL, 3);
		com->text_insert("�C�V", "�i�����́\�\�V�˂��B�l�͑�����l�ɂȂ��āA��C��ǂ܂Ȃ��Ƃ����Ȃ��񂾁B�j", H_NORMAL, 3);
		com->text_insert("�C�V", "�i�����̗��e��S�z������̂��A�������搶�ɓ{����̂��A�{�����肦�Ȃ����Ȃ񂾁B�j", H_NORMAL, 3);
	}
	play_bgm(bgm[0], 1, 11);
	finish(11);
	text_play();
}
void MainMission13::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,300,700,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�^�C��", "����A�����A�邾��H�ꏏ�ɋA�낤���B", H_NORMAL, 4);
		com->text_insert("�C�V", "�����B������Ƌ����ɉו����ɍs������A������Ƒ҂��ĂāB", H_NORMAL, 3);
		com->text_insert("�^�C��", "�c�c�c�c�Ȃ�����", H_NORMAL, 4);
		com->text_insert("�C�V", "���H", H_NORMAL, 3);
		com->text_insert("�^�C��", "���O�́\�\�S�O���Ă��ȁB���Ƃ�ނ��Ƃ��B", H_NORMAL, 4);
		com->text_insert("�C�V", "�Ȃ񂾂�c�c�c�c�}�ɁB", H_NORMAL, 3);
		com->text_insert("�^�C��", "�Ȃ񂾂낤�ȁc�c�c�c�����Ƙb�����Ǝv���Ă͂����񂾂��c�c", H_NORMAL, 4);
		com->text_insert("�^�C��", "����Ȃ�ɁA�����̒��ōl���𐮗��ł����񂾁B", H_NORMAL, 4);
		com->text_insert("�^�C��", "������\�\�v���؂��č��A�b�������Ďv�����񂾂낤�ȁB", H_NORMAL, 4);
		com->text_insert("�C�V", "�������c�c�c�c�l���A����B", H_NORMAL, 3);
		com->text_insert("�^�C��", "����͓s���������ȁB", H_NORMAL, 4);
		com->text_insert("�C�V", "�\�\�䗅�A����ς�l�ƍ��܂Œʂ�t�������̂͂悭�Ȃ��Ǝv���񂾁B", H_NORMAL, 3);
		com->text_insert("�^�C��", "���́A���e���C�ɂ��Ă�̂��H", H_NORMAL, 4);
		com->text_insert("�C�V", "�Ⴄ��B������񂻂�����邯�ǁc�c", H_NORMAL, 3);
		com->text_insert("�C�V", "��Ԃ́A�䗅�̏�������B�������A�S�z���Ă�񂾁B", H_NORMAL, 3);
		com->text_insert("�^�C��", "���̏�����S�z�c�c�H����͂��肪�������ǂȁA�]�v�Ȃ����b���B", H_NORMAL, 4);
		com->text_insert("�C�V", "�́c�c�c�c�H�Ȃ񂾂�A�l�̋C���m�炸�ɁI", H_NORMAL, 3);
		com->text_insert("�^�C��", "���̏�����S�z����͎̂����Ɛe�����ŏ\�����B", H_NORMAL, 4);
		com->text_insert("�^�C��", "����͉��̏�����S�z���āA����ɍ��y���ނ��Ƃ���߂�����̂��H", H_NORMAL, 4);
		com->text_insert("�C�V", "�������Ă񂾂�I�I�ӂ����Ă�񂶂�Ȃ��̂��I�H", H_NORMAL, 3);
		com->text_insert("�^�C��", "�c�c�c�c", H_NORMAL, 4);
		com->text_insert("�C�V", "�׋������ɂȂ����̂��I�H�ʂ̂��Ƃɓ����悤���Ă̂��I�H", H_NORMAL, 3);
		com->text_insert("�C�V", "���������Ȃ�����I�I���O�͓V�˂Ȃ̂ɁI�I", H_NORMAL, 3);
		com->text_insert("�^�C��", "�c�c�c�c��", H_NORMAL, 4);
		com->text_insert("�C�V", "�������������񂾂�I", H_NORMAL, 3);
		com->text_insert("�^�C��", "�e�������������Ă��Ȃ��Ďv�����񂾁B", H_NORMAL, 4);
		com->text_insert("�C�V", "�c�c�c�c", H_NORMAL, 3);
		com->text_insert("�C�V", "�c�c�c�c��l�ɂȂ���Ă����̂́A�䖝���邱�Ɓc�c", H_NORMAL, 3);
		com->text_insert("�^�C��", "�c�c�c�c", H_NORMAL, 4);
		com->text_insert("�C�V", "�l���A�����v����B", H_NORMAL, 3);
		com->text_insert("�^�C��", "�������c�c��������������Ȃ炻��ł������B", H_NORMAL, 4);
		com->text_insert("�C�V", "�c�c�c�c", H_NORMAL, 3);
		com->text_insert("�^�C��", "�\�\���͂����A���Ȃ��Ƃ����͂܂�����ƗV��ł��������Ďv�����������B", H_NORMAL, 4);
		com->text_insert("�C�V", "�䗅�c�c�c�c", H_NORMAL, 3);
		com->text_insert("�^�C��", "����͂ǂ��������񂾁H����������Ăق����B", H_NORMAL, 4);
		com->text_insert("�^�C��", "����͋C�ɂ������Ȃ񂾂�B���ہA�܂����͐��ш����Ȃ��ĂȂ�����H", H_NORMAL, 4);
		com->text_insert("�C�V", "�l�́c�c�c�c", H_NORMAL, 3);
		com->text_insert("�C�V", "�V�сA������c�c�c�c�{���́c�c�I", H_NORMAL, 3);
		com->text_insert("�^�C��", "�c�c�c�c������", H_NORMAL, 4);
		com->text_insert("�^�C��", "�\�\�悩�����c�c�c�c���̈ꌾ�������ĂȁB", H_NORMAL, 4);
		com->text_insert("�C�V", "�c�c�c�c", H_NORMAL, 3);
		com->text_insert("�^�C��", "����ω��B���čō��̃R���r�����", H_NORMAL, 4);
	}
	play_bgm(bgm[1], 1, 42);
	move(0, 4, 500, 3);
	move(2, 3, 200, 3);
	move(16, 3, 250, 6);
	move(19, 3, 300, 6);
	move(21, 3, 350, 6);
	event(40, "event/�^�C���ƃC�V.png");//�}�G
	finish(42);
	text_play();
}

//MM14
//�搶�̎�ނƃJ�[�h
MainMission14::MainMission14(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[2];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/piano32.mp3");
	bgm[1] = LoadSoundMem("bgm/piano36.mp3");
	volume_bgm(2);
	set_rare_rate(75, 25, 0, 0);
	size_set(9, 9, 9);
	teacher[0].get_cards()->set_card(Card(9, 0, 700, 240, 70, 120), 0);
	teacher[0].get_cards()->set_card(Card(18, 0, 0, 55, 40, 70), 1);
	teacher[0].get_cards()->set_card(Card(28, 0, 0, 60, 60, 80), 2);
	teacher[0].get_cards()->set_card(Card(18, 0, 0, 55, 40, 70), 3);
	teacher[0].get_cards()->set_card(Card(28, 0, 0, 60, 60, 80), 4);

	teacher[1].get_cards()->set_card(Card(22, 0, 700, 75, 120, 130), 0);
	teacher[1].get_cards()->set_card(Card(12, 0, 0, 25, 105, 130), 1);
	teacher[1].get_cards()->set_card(Card(5, 0, 0, 37, 70, 90), 2);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 75, 120, 130), 3);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 75, 120, 130), 4);
}
//��b�ƃo�g��������
int MainMission14::play() {
	return normal_text();
}
void MainMission14::text1() {
	if (com == NULL) {
		int x[10] = { 100,200,-100,-100,-100,-100,500,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�n���_", "�悵�A��������ˁB�s�������B", H_NORMAL, 1);
		com->text_insert("�~���g�l", "��A��낵�����肢���܂��c�c", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�n���_", "��낵���I", H_NORMAL, 1);
		com->text_insert("�I���_", "�o���[�A���[�A�T�l�A��������[�H�p", H_NORMAL, 5);
		com->text_insert("�n���_", "�ǎq�B�ǂ������H", H_NORMAL, 1);
		com->text_insert("�I���_", "���c�����Ȃ��񂾂��ǁB�����Ɛ��������񂾂�ˁH", H_NORMAL, 5);
		com->text_insert("�n���_", "���I�H�c�c���͂����Ƃ������͂������ǁc�c", H_NORMAL, 1);
		com->text_insert("�I���_", "�c�c���������Ă�����Ă܂��c�c", H_NORMAL, 5);
		com->text_insert("�n���_", "���c�c�I�I�����A�܂���l�Łc�c�c�c�I�I", H_NORMAL, 1);
		com->text_insert("�I���_", "�͂��c�c�ǂ�����A�ǂ������悤���H", H_NORMAL, 5);
		com->text_insert("�n���_", "�c�c���{�́A���Ă邩�H", H_NORMAL, 1);
		com->text_insert("�I���_", "����A�t���͂����Ɨ��Ă��B", H_NORMAL, 5);
		com->text_insert("�n���_", "���Ⴀ�c�c�ǂ������Ă���B", H_NORMAL, 1);
		com->text_insert("�n���_", "������Ȃ��Ă��A��l�Ő키�̂͂�߂č����͂�����߂悤�B", H_NORMAL, 1);
		com->text_insert("�I���_", "�킩�����B���������C��t���ĂˁB", H_NORMAL, 5);
		com->text_insert("�n���_", "�����B��낵���ȁB", H_NORMAL, 1);
		com->text_insert("�n���_", "�c�c�c�c", H_NORMAL, 1);
		com->text_insert("�~���g�l", "�c�c�c�c���A���v�����ł����c�c", H_NORMAL, 6);
		com->text_insert("�n���_", "�킩��Ȃ��c�c", H_NORMAL, 1);
		com->text_insert("�n���_", "�����͂��c�c�{���Ɂc�c�c�c�I�I", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
	}
	play_bgm(bgm[0], 7, 22);
	finish(22);
	text_play();
}
void MainMission14::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,100,-100,-100,300,-100,400,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�I�I�^", "�ӂ��c�c�c�c", H_NORMAL, 2);
		com->text_insert("�I���_", "�ӂ��A����Ȃ����I�I", H_NORMAL, 5);
		com->text_insert("�I���_", "���c�A����ȍs����������I�I", H_NORMAL, 5);
		com->text_insert("�I�I�^", "���Ă��ꂽ�̂͊��ӂ��邯�ǂ�A�������ė��Ȃ��Ă悩�����񂾂��B", H_NORMAL, 2);
		com->text_insert("�I���_", "���񂽂˂��A�Ȃ�ŗT�l�̌������Ɩ�������̂�I�I", H_NORMAL, 5);
		com->text_insert("�I�I�^", "�t�ɕ������ǂ�A�Ȃ�ŏ��a�c�͗T�l�̌������Ƃ𕷂��񂾂�B", H_NORMAL, 2);
		com->text_insert("�I���_", "�T�l�݂͂�Ȃ̂��Ƃ���ɍl���Ă�̂�B", H_NORMAL, 5);
		com->text_insert("�I���_", "���񂽂͈Ⴄ�ł���H���������B", H_NORMAL, 5);
		com->text_insert("�I�I�^", "�����́I�I", H_NORMAL, 2);
		com->text_insert("�I���_", "���I�I", H_NORMAL, 5);
		com->text_insert("�I�I�^", "�����́c�c�݂�ȂɊ댯�Ȃ��Ƃ������Ă邶��˂����c�c", H_NORMAL, 2);
		com->text_insert("�I���_", "�c�c�c�c", H_NORMAL, 5);
		com->text_insert("�I�I�^", "�|�������搶������A�������玩����l�ł��ׂ�����˂��̂��H", H_NORMAL, 2);
		com->text_insert("�I�I�^", "���݂͂�Ȃ̂��߂ɐ키�������B�T�l�́A�������ɔC���Ă������񂾂�B", H_NORMAL, 2);
		com->text_insert("�I���_", "�c�c�c�c", H_NORMAL, 5);
		
	}
	play_bgm(bgm[1], 1, 15);
	move(8, 2, 200, 7);
	finish(15);
	text_play();
}

//MM15
//�搶�̎�ނƃJ�[�h
MainMission15::MainMission15(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[2];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/piano24.mp3");
	bgm[1] = LoadSoundMem("bgm/piano22.mp3");
	volume_bgm(2);
	set_rare_rate(70, 25, 5, 0);
	size_set(9, 9, 9);
	teacher[0].get_cards()->set_card(Card(14, 0, 900, 15, 110, 140), 0);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 80, 100, 120), 1);
	teacher[0].get_cards()->set_card(Card(14, 0, 0, 15, 110, 140), 2);
	teacher[0].get_cards()->set_card(Card(10, 0, 0, 40, 70, 90), 3);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 80, 100, 120), 4);

	teacher[1].get_cards()->set_card(Card(2, 0, 900, 40, 80, 100), 0);
	teacher[1].get_cards()->set_card(Card(23, 0, 0, 95, 160, 170), 1);
	teacher[1].get_cards()->set_card(Card(23, 0, 0, 95, 160, 170), 2);
	teacher[1].get_cards()->set_card(Card(2, 0, 0, 40, 80, 100), 3);
	teacher[1].get_cards()->set_card(Card(23, 0, 0, 95, 160, 170), 4);
}
//��b�ƃo�g��������
int MainMission15::play() {
	switch (now) {
	case 0:
		text1();
		break;
	case 1:
		result = bse->battle_loop(kind);//�g�p�����L������kind�ɓ���ċ����Ă��炤
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 2:
		text2();
		break;
	case 3:
		text3();
		break;
	case 4:
		text4();
		break;
	case 5:
		text5();
		break;
	case 6:
		return result;
		break;
	}
	return 0;
}
void MainMission15::text1() {
	if (com == NULL) {
		int x[10] = { 100,300,-100,-100,-100,-100,-100,-100,400,-100 };
		com = new Text(x, -100);
		com->text_insert("�n���_", "�悵�A���Ⴀ�s�������B", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�C�o���J��", "�˂��c�c���c�́A���v���ȁc�c�H", H_NORMAL, 8);
		com->text_insert("�n���_", "���A�����c�c���ӂ͂������ǁA�ǂ����낤�ˁc�c�B", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�i����܂ł����c�͎U�X���ӂ��Ă����B�j", H_MOUSOU, 0);
		com->text_insert("�A�J�c�L", "�i���ꂾ���Ԃ����Ă��A���c�͑��c�Ƃ̊ւ���₽�Ȃ��B�j", H_MOUSOU, 0);
		com->text_insert("�A�J�c�L", "�i�ڂɌ����Ȃ��F��Ƃ������̂�����񂾂낤���B�j", H_MOUSOU, 0);
		com->text_insert("�C�o���J��", "���c�c�c�c�c", H_NORMAL, 8);
		com->text_insert("�n���_", "�i���c�c�c���O�͂������A���̌������Ƃ�ے肷���ȁB�j", H_MOUSOU, 1);
		com->text_insert("�n���_", "�i���񂾂��Ă������B����̂��Ƃ𒍈ӂ��Ă��A����l�ɔC����΂������Č������ȁB�j", H_MOUSOU, 1);
		com->text_insert("�n���_", "�i���O�͂������l�ɐS�z��������B�j", H_MOUSOU, 1);
		com->text_insert("�n���_", "�i�������Ĉ�l�œ˂�����B�j", H_MOUSOU, 1);
		com->text_insert("�n���_", "�i���Ƃ͐^�t���B�j", H_MOUSOU, 1);
		com->text_insert("�n���_", "�i�{���ɗ����ł��Ȃ��B�j", H_MOUSOU, 1);
		com->text_insert("�n���_", "�i�\�\�����j", H_MOUSOU, 1);
		com->text_insert("�n���_", "�i�\�\�\�\���O�̂��Ƃ́A��������Ȃ��B�j", H_MOUSOU, 1);
	}
	play_bgm(bgm[0], 8, 16);
	finish(16);
	text_play();
}
void MainMission15::text2() {
	if (com == NULL) {
		int x[10] = { 500,300,-100,-100,-100,-100,-100,-100,400,-100 };
		com = new Text(x, -100);
		com->text_insert("�I���_", "�o�T�l�I�I�T�l�I�I�p", H_NORMAL, 5);
		com->text_insert("�n���_", "�ǎq�I�H�ǁA�ǂ������I�H", H_NORMAL, 1);
		com->text_insert("�I���_", "���c���c�c�c�c�I�I���c���c�c�c�c�I�I", H_NORMAL, 5);
		com->text_insert("�n���_", "�\�\���A���c���c�c", H_NORMAL, 1);
		com->text_insert("�C�o���J��", "���c�c�c�c�H", H_NORMAL, 8);
		com->text_insert("�n���_", "�������A�������ɁA�s���I�I", H_NORMAL, 1);

		com->text_insert("�C�o���J��", "���c�I�I", H_NORMAL, 8);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
	}
	play_bgm(bgm[1], 1, 8);
	move(6, 1, -100, 7);
	move(7, 8, -500, 7);
	finish(8);
	text_play();
}
void MainMission15::text3() {
	if (com == NULL) {
		int x[10] = { -100,-100,400,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, 300);
		com->text_insert("�I�I�^", "�����c�c�c�c", H_NORMAL, 2);
		com->text_insert("Q�搶", "�͂����A�͂����c�c���ܔ����Ă��邩��A�����Ȃ񂾂�c�c�c�c�I�I", H_NORMAL, 10);
		com->text_insert("�I�I�^", "�c�c�c�c�c�c�c�c�c�c�c�c", H_NORMAL, 2);

		com->text_insert("�I�I�^", "�c�c�c�c��", H_NORMAL, 2);
		com->text_insert("�I�I�^", "���߁A���c�c�c�c", H_NORMAL, 2);
		com->text_insert("�I�I�^", "���c�c�c�c", H_NORMAL, 2);
		com->text_insert("�I�I�^", "�i�\�\�\�\�\�\�\�\���ʁB�j", H_MOUSOU, 2);
		com->text_insert("�I�I�^", "�c�c�c�c�T�l�c�c�c�c", H_NORMAL, 2);
		com->text_insert("�I�I�^", "���߁c�c�c�c", H_NORMAL, 2);
		com->text_insert("�I�I�^", "�i�c�c�c�c���߂�ȁc�c�j", H_MOUSOU, 2);
	}
	move(2, 10, -100, 3);
	finish(10);
	text_play();
}
void MainMission15::text4() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�I���_", "���c�����Ȃ���������O�݂����ɒǂ��������񂾂��ǁA���܂݂��Q�搶��������������c�c�c�c", H_NORMAL, 4);
		com->text_insert("�n���_", "�c�c�c�c�͂��A�͂��c�c�c�c���c�c�c�c�c�I�I", H_NORMAL, 4);
		com->text_insert("�I���_", "�������c�́\�\Q�搶�Ɂ\�\", H_NORMAL, 4);
		com->text_insert("�n���_", "�Ⴄ�I�I", H_NORMAL, 4);
		com->text_insert("�I���_", "�T�l�c�c�c�c", H_NORMAL, 4);
	}
	move(1, 1, 800, 7);
	finish(5);
	text_play();
}
void MainMission15::text5() {
	if (com == NULL) {
		int x[10] = { -100,-100,250,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�I�I�^", "�i�T�l�c�c���O�́c�c��Ȃ��������񂾂�c�c�j", H_MOUSOU, 4);
		com->text_insert("�I�I�^", "�i���a�Ȃ����ɂ�c�c���[�_�[�݂����ɐU�����Ă񂶂�˂���c�c�j", H_MOUSOU, 4);
		com->text_insert("�I�I�^", "�i�݂�Ȃ̂��Ƒ厖�ɑz���Ă���Ă͕̂����邺�B�ł��ȁc�c�j", H_MOUSOU, 4);
		com->text_insert("�I�I�^", "�i�S�����������ނ��Ƃ˂��񂾂�c�c�c�c�j", H_MOUSOU, 4);
		com->text_insert("�I�I�^", "�i�������邶��˂����c�c���������A�����Ă����c�c�j", H_MOUSOU, 4);
		com->text_insert("�I�I�^", "�i�܂��Łc�c�����ł��˂��B�j", H_MOUSOU, 4);
		com->text_insert("�I�I�^", "�i�ł���A�T�l�A�����c�c�j", H_MOUSOU, 4);
		com->text_insert("�I�I�^", "�i�\�\���O�̂��ƁA��������Ȃ����B�j", H_MOUSOU, 4);

		com->text_insert("�I�I�^", "�i���̓��\�\���̎��\�\���͐������񂾁B�j", H_MOUSOU, 4);
		com->text_insert("�I�I�^", "�i�����̋��ň�l�A���������Ă������c�c�c�c�j", H_MOUSOU, 4);
		com->text_insert("�I�I�^", "�i�F�B�̂��Ȃ����������c�c�����Ă��ꂽ�B�j", H_MOUSOU, 4);
		com->text_insert("�I�I�^", "�i���肪�Ƃ��A�T�l�\�\�j", H_MOUSOU, 4);
		com->text_insert("�I�I�^", "�i�\�\���߂�\�\�����A�j�����܂����\�\�j", H_MOUSOU, 4);
	}
	play_bgm(bgm[0], 1, 13);
	event(8, "event/�I�I�^�ߋ�.png");//�}�G
	event(11, "event/�I�I�^�ƃn���_.png");//�}�G
	finish(13);
	text_play();
}

//MM16
//�搶�̎�ނƃJ�[�h
MainMission16::MainMission16(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/piano33.mp3");
	volume_bgm(1);
	set_rare_rate(70, 25, 5, 0);
	size_set(9, 9, 9);
	teacher[1].get_cards()->set_card(Card(24, 0, 1500, 40, 80, 100), 0);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 95, 130, 120), 1);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 95, 130, 120), 2);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 95, 130, 120), 3);
	teacher[1].get_cards()->set_card(Card(24, 0, 0, 40, 80, 100), 4);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 95, 130, 120), 5);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 95, 130, 120), 6);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 90, 130, 120), 7);
}
//��b�ƃo�g��������
int MainMission16::play() {
	return normal_text();
}
void MainMission16::text1() {
	if (com == NULL) {
		int x[10] = { 500,-100,-100,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�C�o���J��", "�˂��A������ƁI�I�����Ă�́I�H", H_NORMAL, 8);
		com->text_insert("�A�J�c�L", "�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�C�o���J��", "�Ȃ�ł��Ă��Ȃ��́I�H", H_NORMAL, 8);
		com->text_insert("�A�J�c�L", "�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�C�o���J��", "���c���c�c���c���c�c�S�z����Ȃ��́I�H", H_NORMAL, 8);
		com->text_insert("�A�J�c�L", "�c�c���́c�c�c�c", H_NORMAL, 0);
		com->text_insert("�C�o���J��", "�c�c�c�c", H_NORMAL, 8);
		com->text_insert("�A�J�c�L", "���c����A�w������ĂȂ��c�c", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "������A�����őҋ@����c�c", H_NORMAL, 0);
		com->text_insert("�C�o���J��", "�ȂɁc�c����c�c�c�c", H_NORMAL, 8);
		com->text_insert("�C�o���J��", "���񂽁c�c���c������ł������́I�H", H_NORMAL, 8);
		com->text_insert("�A�J�c�L", "����Ȃ��Ƃ͎v���ĂȂ��c�c", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "�i�����A�킢���Ă͎̂��Ɨׂ荇�킹���B�j", H_MOUSOU, 0);
		com->text_insert("�A�J�c�L", "�i�l�̎����D���Ȃ悤�ɑ��삷��͖̂����Ȃ��Ƃ��B�j", H_MOUSOU, 0);
		com->text_insert("�A�J�c�L", "���́c�c���@���L���[�ɋt�炤�C�͂Ȃ��c�c", H_NORMAL, 0);
		com->text_insert("�C�o���J��", "�͂��c�c�c�c���������I�I����ɂ���I�I", H_NORMAL, 8);
	}
	play_bgm(bgm[0], 1, 16);
	move(0, 8, 300, 6);
	move(16, 8, -500, 6);
	finish(16);
	text_play();
}
void MainMission16::text2() {
	if (com == NULL) {
		int x[10] = { -100,200,-100,-100,-100,400,-100,500,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�n���_", "���c�c�c�w�́A�Ƃ������c�c�c�c", H_NORMAL, 1);
		com->text_insert("�I���_", "�T�l�c�c�c�c", H_NORMAL, 5);
		com->text_insert("�n���_", "�c�c�c�c", H_NORMAL, 1);
		com->text_insert("�n���_", "�i�\�\�����ƁA�������̂͑��c��������Ȃ��B�j", H_MOUSOU, 1);
		com->text_insert("�n���_", "�i���A���������̂Ȃ����̂��������񂾁B�j", H_MOUSOU, 1);
		com->text_insert("�n���_", "�i���ɂȂ����̂��������񎝂��Ă���A�厖�ȗF�B���A�Ȃ������񂾁B�j", H_MOUSOU, 1);
	}
	finish(6);
	text_play();
}

//MM17
//�搶�̎�ނƃJ�[�h
MainMission17::MainMission17(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/piano27.mp3");
	volume_bgm(1);
	set_rare_rate(70, 23, 7, 0);
	size_set(9, 9, 9);
	teacher[0].get_cards()->set_card(Card(22, 0, 700, 60, 120, 130), 0);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 60, 120, 130), 1);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 60, 120, 130), 2);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 60, 120, 130), 3);
	teacher[0].get_cards()->set_card(Card(22, 0, 0, 60, 120, 130), 4);

	teacher[1].get_cards()->set_card(Card(22, 0, 1000, 70, 150, 170), 0);
	teacher[1].get_cards()->set_card(Card(13, 0, 0, 300, 110, 110), 1);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 70, 150, 170), 2);
	teacher[1].get_cards()->set_card(Card(13, 0, 0, 300, 110, 110), 3);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 70, 150, 170), 4);

	teacher[2].get_cards()->set_card(Card(22, 0, 700, 70, 100, 110), 0);
	teacher[2].get_cards()->set_card(Card(22, 0, 0, 70, 100, 110), 1);
	teacher[2].get_cards()->set_card(Card(7, 0, 0, 40, 100, 110), 2);
	teacher[2].get_cards()->set_card(Card(22, 0, 0, 70, 140, 120), 3);
	teacher[2].get_cards()->set_card(Card(22, 0, 0, 70, 140, 120), 4);
}
//��b�ƃo�g��������
int MainMission17::play() {
	return normal_text();
}
void MainMission17::text1() {
	if (com == NULL) {
		int x[10] = { 800,-100,-100,-100,-100,200,300,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�I���_", "���Ԃ����A�����͂�낵���I", H_NORMAL, 5);
		com->text_insert("�~���g�l", "��A��낵���c�c�c�c", H_NORMAL, 6);
		com->text_insert("�I���_", "���A�Ԍ���������ˁB���Ⴀ�s�����B", H_NORMAL, 5);
		com->text_insert("�A�J�c�L", "�����c�c�c�c", H_NORMAL, 0);
		com->text_insert("�~���g�l", "�\�\���A���́I�I", H_NORMAL, 6);
		com->text_insert("�~���g�l", "���́c�c�������������������A���߂�Ȃ����c�c", H_NORMAL, 6);
		com->text_insert("�I���_", "���A�����̂����́A�C�ɂ��Ȃ��ŁI", H_NORMAL, 5);
		com->text_insert("�I���_", "����ɑ��v��B�����Ƃ��܂�������āB", H_NORMAL, 5);
		com->text_insert("�~���g�l", "�ł��c�c���̑O�͎��s��������āc�c", H_NORMAL, 6);
		com->text_insert("�I���_", "����Ȃ̋C�ɂ���l���Ȃ����āB", H_NORMAL, 5);
		com->text_insert("�I���_", "���ƁA�����ƋC�y�ɘb���Ă���Ă����̂�B", H_NORMAL, 5);
		com->text_insert("�I���_", "���Ԃ����A������ƍd���Ȃ肷����B", H_NORMAL, 5);
		com->text_insert("�~���g�l", "�����c�c�c�c", H_NORMAL, 6);
		com->text_insert("�I���_", "�����Ƌ����k�߂Ă���Ă������B", H_NORMAL, 5);
		com->text_insert("�A�J�c�L", "�c�c�c�c", H_NORMAL, 0);
	}
	play_bgm(bgm[0], 1, 15);
	move(2, 0, 500, 5);
	finish(15);
	text_play();
}
void MainMission17::text2() {
	if (com == NULL) {
		int x[10] = { 200,-100,-100,-100,-100,300,400,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�I���_", "�ӂ��B�����͂��肪�Ƃ��A�܂���낵���I", H_NORMAL, 5);
		com->text_insert("�~���g�l", "���A����c�c��낵���c�c�c�c", H_NORMAL, 6);
		com->text_insert("�I���_", "���Ⴀ�܂������I", H_NORMAL, 5);
		com->text_insert("�~���g�l", "�܁A�܂��c�c�c�c", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�~���g�l", "���c�c�c�c", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "��c�c�c�c", H_NORMAL, 0);
		com->text_insert("�~���g�l", "����c�c�c�c�Ȃ�ł��Ȃ��c�c", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�c�c�c�c", H_NORMAL, 0);
	}
	move(3, 5, -200, 5);
	move(5, 0, 100, 3);
	move(9, 0, -300, 3);
	finish(9);
	text_play();
}

//MM18
//�搶�̎�ނƃJ�[�h
MainMission18::MainMission18(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/piano27.mp3");
	volume_bgm(1);
	set_rare_rate(70, 23, 7, 0);
	size_set(9, 9, 9);
	teacher[0].get_cards()->set_card(Card(18, 0, 1200, 70, 30, 60), 0);
	teacher[0].get_cards()->set_card(Card(28, 0, 0, 80, 80, 90), 1);
	teacher[0].get_cards()->set_card(Card(28, 0, 0, 80, 80, 90), 2);
	teacher[0].get_cards()->set_card(Card(9, 0, 0, 300, 70, 120), 3);
	teacher[0].get_cards()->set_card(Card(9, 0, 0, 300, 70, 120), 4);

	teacher[1].get_cards()->set_card(Card(22, 0, 1200, 90, 100, 120), 0);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 90, 100, 120), 1);
	teacher[1].get_cards()->set_card(Card(6, 0, 0, 30, 120, 140), 2);
	teacher[1].get_cards()->set_card(Card(6, 0, 0, 30, 120, 140), 3);
	teacher[1].get_cards()->set_card(Card(6, 0, 0, 30, 120, 140), 4);
}
//��b�ƃo�g��������
int MainMission18::play() {
	return normal_text();
}
void MainMission18::text1() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,-100,500,200,-100,-100,-100,400 };
		com = new Text(x, -100);
		com->text_insert("�I���_", "�͂��c�c�c�c", H_NORMAL, 5);
		com->text_insert("�g���`", "�c�c�c�c", H_NORMAL, 9);
		com->text_insert("�^�C��", "�c�c�c�c", H_NORMAL, 4);
		com->text_insert("�I���_", "������Ɓc�c�u�ǂ������́H�v���炢�����Ă���Ȃ��́H", H_NORMAL, 5);
		com->text_insert("�^�C��", "�u�p�������Ă���邾�낤�A�Ǝv�����B", H_NORMAL, 4);
		com->text_insert("�g���`", "�͂�A����Ȃ����Ă킩���Ă邭���ɁB", H_NORMAL, 9);
		com->text_insert("�^�C��", "����A���ꂩ�猾�����B", H_NORMAL, 4);
		com->text_insert("�^�C��", "�u�p�͉��ƒ��ǂ��������͂�������ȁB", H_NORMAL, 4);
		com->text_insert("�g���`", "�ǂ��������ƁH�S���Ӗ���������Ȃ��񂾂��ǁB", H_NORMAL, 9);
		com->text_insert("�^�C��", "���ƒ��ǂ��Ȃ�قǁA����Ƃ����ǂ��Ȃ�邩�炾�B", H_NORMAL, 4);
		com->text_insert("�g���`", "�c�c�c�c", H_NORMAL, 9);
		com->text_insert("�^�C��", "�f�[�g�����񂾂��ĂȁB����m���Ă邱�Ƃ͑S�ĉ����m���Ă�Ǝv�������������B", H_NORMAL, 4);
		com->text_insert("�g���`", "����̂�����ׂ�߁c�c�c�c", H_NORMAL, 9);
		com->text_insert("�^�C��", "���S����B�����A�u�p����̍D�ӂɂ͋C�Â��ĂȂ��B", H_NORMAL, 4);
		com->text_insert("�g���`", "�ł��f�[�g�͂����̉��Z�ŁA�������邩����H", H_NORMAL, 9);
		com->text_insert("�g���`", "�c�c�c�c�܁A������B���a�c�A�Y�݂�ł������āH", H_NORMAL, 9);
		com->text_insert("�I���_", "���[�I�H�Ƃ��������A�Ɏu���D���Ȃ́[�I�H", H_NORMAL, 5);
		com->text_insert("�I���_", "�ق�ƈӊO�B�Ƃ�������񗝑z�������Ȃ̂ɁI", H_NORMAL, 5);
		com->text_insert("�^�C��", "����͈Ɏu�Ɏ��炾��c�c�c�c", H_NORMAL, 4);
		com->text_insert("�g���`", "�܂������A�_���_��������ˁ[�B", H_NORMAL, 9);
		com->text_insert("�^�C��", "�����c�c�c�c", H_NORMAL, 4);
		com->text_insert("�g���`", "�ŁA�Y�݂��Ă̂͂ȂɁH���������܂Ő[������Ȃ���ˁH", H_NORMAL, 9);
		com->text_insert("�I���_", "�����A����\�\���Ԃ����A�̂��ƂȂ񂾂��ǁc�c", H_NORMAL, 5);
		com->text_insert("�^�C��", "�{�����́c�c", H_NORMAL, 4);
		com->text_insert("�I���_", "�Ȃ񂩋�������񂾂�ˁB�V�q�����Ƃ͂܂��Ⴄ�����Łc�c", H_NORMAL, 5);
		com->text_insert("�I���_", "�������A�S�z�Ȃ́B���Ԃ����̂��ƁA�x���Ă������Ȃ����Ȃ��āc�c�c�c", H_NORMAL, 5);
		com->text_insert("�^�C��", "�S�̎x���H", H_NORMAL, 4);
		com->text_insert("�I���_", "�m��Ȃ��H�c�c��r�I�L���Șb�Ȃ񂾂��ǁ\�\", H_NORMAL, 5);
		com->text_insert("�I���_", "���Ԃ����A�e�����������������ĉ\�B", H_NORMAL, 5);
		com->text_insert("�g���`", "���[�A���܂ɕ�����ˁ[�B", H_NORMAL, 9);
		com->text_insert("�g���`", "�ޏ����g�A���шȊO�ڗ����ĂȂ����A����܂�b��ɂȂ�Ȃ����ǁB", H_NORMAL, 9);
		com->text_insert("�^�C��", "�������c�c���ǁA�{�l����������Ȃ��Ȃ�A�����������o�����邱�Ƃ���Ȃ��񂶂�Ȃ����H", H_NORMAL, 4);
		com->text_insert("�I���_", "���������A����Ȃ����ǁc�c", H_NORMAL, 5);
		com->text_insert("�g���`", "�܁A�������œ��������͖̂����ł���B�Ƃ肠�����~�b�V�����Еt���Ȃ��H", H_NORMAL, 9);
		com->text_insert("�^�C��", "�������ȁB", H_NORMAL, 4);
	}
	play_bgm(bgm[0], 1, 35);
	move(0, 4, 450, 3);
	move(0, 5, 150, 3);
	move(0, 9, 350, 3);
	move(1, 4, 400, 3);
	move(1, 5, 100, 3);
	move(1, 9, 300, 3);
	move(2, 4, 350, 3);
	move(2, 5, 50, 3);
	move(2, 9, 250, 3);
	move(16, 5, 100, 3);
	finish(35);
	text_play();
}
void MainMission18::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,-100,350,200,-100,-100,-100,500 };
		com = new Text(x, -100);
		com->text_insert("�I���_", "������B���Ⴀ�A�܂������c�c�B", H_NORMAL, 5);
		com->text_insert("�^�C��", "���Ⴀ�B", H_NORMAL, 4);
		com->text_insert("�g���`", "�\�\�˂��A�{��������̂��Ƃ�����", H_NORMAL, 9);
		com->text_insert("�I���_", "�\�\�\�\�\�\�\�\", H_NORMAL, 5);
		com->text_insert("�g���`", "�܂��A������Ƃ����C�ɗ��߂Ă�������B", H_NORMAL, 9);
		com->text_insert("�I���_", "�\�\�I���肪�Ƃ��I", H_NORMAL, 5);
		com->text_insert("�g���`", "�c�c�c�c", H_NORMAL, 9);
		com->text_insert("�g���`", "����ɂ͉�������Ȃ��Ă�������B�ʂɋ����Ȃ����B", H_NORMAL, 9);
		com->text_insert("�^�C��", "�����猫��Ɖ���Ĉꏏ�ɋA�邪�A�u�p���ǂ����H", H_NORMAL, 4);
		com->text_insert("�g���`", "�s���I�I", H_NORMAL, 9);
	}
	play_bgm(bgm[0], 1, 10);
	move(2, 5, 100, 4);
	move(6, 5, -300, 4);
	finish(10);
	text_play();
}

//MM19
//�搶�̎�ނƃJ�[�h
MainMission19::MainMission19(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[2];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/piano22.mp3");
	bgm[1] = LoadSoundMem("bgm/pianoMM19.mp3");
	volume_bgm(2);
	set_rare_rate(70, 23, 7, 0);
	size_set(9, 9, 9);
	teacher[1].get_cards()->set_card(Card(5, 0, 2000, 35, 90, 110), 0);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 90, 120, 100), 1);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 90, 120, 100), 2);
	teacher[1].get_cards()->set_card(Card(5, 0, 0, 35, 90, 110), 3);
	teacher[1].get_cards()->set_card(Card(12, 0, 0, 40, 110, 110), 4);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 90, 120, 100), 5);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 90, 120, 100), 6);
	teacher[1].get_cards()->set_card(Card(12, 0, 0, 40, 110, 110), 7);
}
//��b�ƃo�g��������
int MainMission19::play() {
	return normal_text();
}
void MainMission19::text1() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,-100,-100,-100,250,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�~���g�l", "�c�c�c�c", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�~���g�l", "���c�c�c�c", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�~���g�l", "�c�c�c�c���߁A��c�c�Ȃ����c�c�c�c", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�i�~���g�l�́A�W���ꏊ�ɗ��Ȃ������B�j", H_MOUSOU, 0);
		com->text_insert("�A�J�c�L", "�i�A�����Ă��Ȃ���Ȃ�����A���c�Ɍ����ĒT���ɗ������c�c�c�c�j", H_MOUSOU, 0);
		com->text_insert("�~���g�l", "���߂�Ȃ����A���߂�Ȃ����c�c�c�c���c�c�c�c", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�i�ޏ��́A���ꂢ�ȗ܂��ۂ�ۂ�Ɨ����Ă����B�j", H_MOUSOU, 0);
		com->text_insert("�~���g�l", "���߂�Ȃ������߂�Ȃ������߂�Ȃ����c�c�c�c", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�~���g�l", "���߂�c�c�Ȃ����c�c�c�c", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�~���g�l", "�c�c�c�c�c�c�c�c", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�~���g�l", "�ǂ��A���āc�c�c�c�H", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�~���g�l", "�Ȃɂ��c�c����Ȃ��́c�c�H", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�ʂɁc�c�c�c�Ȃɂ��c�c�c�c", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "�i�������Ȃ��������B�{���������āA�Ȃɂ��킩���ĂȂ��B�j", H_MOUSOU, 0);
		com->text_insert("�A�J�c�L", "�i�ǂ����N�����ɂ͂��Ă���Ȃ��B�j", H_MOUSOU, 0);
		com->text_insert("�~���g�l", "�c�c�ς���Ă�A�ˁc�c", H_NORMAL, 6);
		com->text_insert("�~���g�l", "�Ȃ񂾂낤�c�c�Ȃ񂾂������A���S�A����c�c", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "�i�Ȃ񂾁c�c�H�l�q���A�ς�����c�c�j", H_MOUSOU, 0);
		com->text_insert("�~���g�l", "���c�c���߂�c�c���������A��ˁB", H_NORMAL, 6);
		com->text_insert("�~���g�l", "�ł���������Ȃ��Ԍ��N�ɁA�Ȃ񂾂����S�����́c�c�c�c", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�~���g�l", "�Ȃ�ł��낤�c�c�c�c�݂�ȁc�c���a�c������A�D�����񂾂��ǁc�c", H_NORMAL, 6);
		com->text_insert("�~���g�l", "�S�z�⓯��Ă�����Ă��A���S�ł��Ȃ���������c�c�c�c", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�c�c�c�c����", H_NORMAL, 0);
		com->text_insert("�~���g�l", "���肪�Ƃ��A�Ԍ��N�c�c�c�c", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�i�ςȁA�����������B�j", H_MOUSOU, 0);
		com->text_insert("�~���g�l", "���߂�ˁA�����s����B�~�b�V�����B", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�c�c�c�c���", H_NORMAL, 0);
		com->text_insert("�~���g�l", "���H", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "��l�ŁA�s�����c�c�c�c�~�b�V�����B", H_NORMAL, 0);
		com->text_insert("�~���g�l", "�����I�H", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "���v���c�c���Ă�B", H_NORMAL, 0);
		com->text_insert("�~���g�l", "���A����c�c", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�i���́c�c�c�c�l���ŏ��߂Ă��낤���B�j", H_MOUSOU, 0);
		com->text_insert("�A�J�c�L", "�i���c�̎w���𖳎�����B�j", H_MOUSOU, 0);
	}
	play_bgm(bgm[0], 1, 43);
	move(1, 0, 100, 3);
	finish(43);
	text_play();
}
void MainMission19::text2() {
	if (com == NULL) {
		int x[10] = { 200,-100,-100,-100,-100,-100,400,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�~���g�l", "���肪�Ƃ��c�c�c�c�Ԍ��N�B", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�c�c�c�c�����B", H_NORMAL, 0);
		com->text_insert("�~���g�l", "�c�c�c�c", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�~���g�l", "�c�c���̉\�c�c�c�c�m���Ă�H", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�\�c�c�H����c�c�c�c", H_NORMAL, 0);
		com->text_insert("�~���g�l", "�������c�c�c�c", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�i�{�����́A�\�̓��e������Ȃ��B�j", H_MOUSOU, 0);
		com->text_insert("�~���g�l", "�˂��c�c�c�c�׋��ł�����A�K���ɂȂ��Ǝv���H", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�܂��c�c�����Ȃ񂶂�Ȃ���", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "�i�K���̒�`�ɂ����B�����E�ɏA�����Ƃ��K���Ȃ�A�̘b���B�j", H_MOUSOU, 0);
		com->text_insert("�A�J�c�L", "�i���́A�K���Ȃ񂩂ɋ����Ȃ��B�j", H_MOUSOU, 0);
		com->text_insert("�~���g�l", "����΂��ĕ׋����āA�������炢�����Z�ɍs����", H_NORMAL, 6);
		com->text_insert("�~���g�l", "�������Z�ŕ׋��撣������A������w�ɍs����", H_NORMAL, 6);
		com->text_insert("�~���g�l", "������w�ŕ׋��撣������A�����E�ɂ���", H_NORMAL, 6);
		com->text_insert("�~���g�l", "�����E�ɂ����疈���d���撣���������Ƌ���������c�c�c�c", H_NORMAL, 6);
		com->text_insert("�~���g�l", "���X�ˁA�l����́c�c�c�c", H_NORMAL, 6);
		com->text_insert("�~���g�l", "���c�c�c�c���K���ɂȂ��񂾂낤���āB", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "�i�܂��A�����������c�c�c�c�j", H_MOUSOU, 0);
		com->text_insert("�A�J�c�L", "�i������Ȃ��c�c�c�c���͂Ȃɂ��A����Ȃ��Ă����c�c�c�c�j", H_MOUSOU, 0);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�~���g�l", "�c�c�c�c�c�c�c�c", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�c�c�c�c��c�c�Ȃ�c�c", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "���Ȃ�c�c�c�c�׋��́A��߂�΂����c�c�c�c", H_NORMAL, 0);
		com->text_insert("�~���g�l", "�����A����ˁc�c�ł��A�����Ȃ́c�c�c�c", H_NORMAL, 6);
		com->text_insert("�~���g�l", "���܂Ŋ撣���Ă�������c�c�c�c�������c�c�~�܂�Ȃ��́c�c�c�c", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "���́c�c�c�c�׋��ł��Ȃ�����c�c�c�c", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "�{�����̂��Ƃ͂������Ǝv�����A�������K���ɂȂ�񂾂Ǝv���B", H_NORMAL, 0);
		com->text_insert("�~���g�l", "���X���߂�Ȃ�āc�c�c�c���������Ȃ���ˁc�c�c�c", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�����c�c�c�c�K�����Ă̂͂���ȒP������Ȃ��̂������č��v�����B", H_NORMAL, 0);
		com->text_insert("�~���g�l", "�c�c�c�c", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�c�c�s�������B�����A��Ȃ��Ƃ����Ȃ����Č����Ă���ȁB", H_NORMAL, 0);

		com->text_insert("�~���g�l", "�c�c�c�c�c�c�c�c", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�~���g�l", "�c�c�c�c���肢", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�~���g�l", "�����c�c�c�c���E�ɂȂ����Ƃ��\�\", H_NORMAL, 6);
		com->text_insert("�~���g�l", "�\�\���Ɂ\�\���߂����Ăق����́c�c�c�c", H_NORMAL, 6);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "�i�{�����Ɉ�����n���������邱�Ƃ��A���͐S���������B�j", H_MOUSOU, 0);
	}
	play_bgm(bgm[1], 1, 41);
	move(33, 6, 270, 3);
	event(35, "event/�~���g�l�ƃA�J�c�L.png");//�}�G
	finish(41);
	text_play();
}

//MM20
//�搶�̎�ނƃJ�[�h
MainMission20::MainMission20(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/pianoMM20.mp3");
	volume_bgm(1);
	ed = NULL;
	set_rare_rate(52, 33, 13, 2);
	size_set(9, 9, 9);
	teacher[1].get_cards()->set_card(Card(28, 0, 1600, 70, 70, 70), 0);//2500
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 80, 100, 100), 1);
	teacher[1].get_cards()->set_card(Card(23, 0, 0, 100, 130, 130), 2);
	teacher[1].get_cards()->set_card(Card(28, 0, 0, 70, 70, 70), 3);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 80, 100, 100), 4);
	teacher[1].get_cards()->set_card(Card(23, 0, 0, 100, 130, 130), 5);
	teacher[1].get_cards()->set_card(Card(28, 0, 0, 70, 70, 70), 6);
	teacher[1].get_cards()->set_card(Card(22, 0, 0, 80, 100, 100), 7);
	teacher[1].get_cards()->set_card(Card(23, 0, 0, 100, 130, 130), 8);
}
//��b�ƃo�g��������
extern bool mouse_flag;
int MainMission20::play() {
	switch (now) {
	case 0:
		text1();
		break;
	case 1:
		result = bse->battle_loop(kind);//�g�p�����L������kind�ɓ���ċ����Ă��炤
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 2:
		text2();
		break;
	case 3:
		if (ed == NULL) {
			ed = new Ed;
		}
		if (ed->play() == true) {
			delete ed;
			now++;
		}
		break;
	case 4:
		return result;
		break;
	}
	return 0;
}
void MainMission20::text1() {
	if (com == NULL) {
		int x[10] = { -100,300,-100,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�n���_", "�i�񑩂̎��ԂȂ̂Ɂc�c����c�c�c�c�j", H_MOUSOU, 1);
		com->text_insert("�n���_", "�c�c�c�c�c�c�c�c�Ȃ��", H_NORMAL, 1);
		com->text_insert("�n���_", "�i���炪���̌��������Ƃ𖳎�����Ȃ�Ă��肦�Ȃ��B�j", H_MOUSOU, 1);
		com->text_insert("�n���_", "�i�ł��A���Ȃ班�������^���Ă��܂��B�j", H_MOUSOU, 1);
		com->text_insert("�n���_", "�i����A����͏��߂ĉ��̎w���𖳎������s�����Ƃ����B�j", H_MOUSOU, 1);
		com->text_insert("�n���_", "�ǂ��A���āc�c�c�c", H_NORMAL, 1);
		com->text_insert("�n���_", "�i����̂��Ƃ͐��E�ň�ԐM�p���Ă�B�̂��炻���������B�j", H_MOUSOU, 1);
		com->text_insert("�n���_", "�����A��l�ōZ����|���ďI���ɂ��悤���āA�������̂Ɂc�c", H_NORMAL, 1);
		com->text_insert("�n���_", "�i�����ƃ��[�_�[�Ƃ��Ă݂�Ȃ����������Ă����肾�������ǁj", H_MOUSOU, 1);
		com->text_insert("�n���_", "�Ȃ񂾂낤�c�c�ǓƂ��A������c�c�c�c", H_NORMAL, 1);

		com->text_insert("�n���_", "�c�c�c�c�c�c�c�c", H_NORMAL, 1);

		com->text_insert("�n���_", "�c�c�c�c�c�c�c�c�I", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�n���_", "��傤�A��c�c", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "Z�Z���A�|�����A���B", H_NORMAL, 0);
		com->text_insert("�n���_", "�Ȃ�Łc�c�c�c���X�����񂾂�B", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "���̂��Ƃ��B", H_NORMAL, 0);
		com->text_insert("�n���_", "�����ԑ҂����Ǝv���Ă�񂾁B�ǂ����āc�c�x���Ȃ����񂾂�B", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "���������Ă�B�܂��[�����B", H_NORMAL, 0);
		com->text_insert("�n���_", "�c�c�c�c�́H", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�܂����Ƃ��I������΂������B", H_NORMAL, 0);
		com->text_insert("�n���_", "���A����Ȃ킯�c�c�Ȃ񂾁A�ǂ��Ȃ��Ă�񂾁I�H", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "���񂾂�AZ�Z���B�s�������B", H_NORMAL, 0);
		com->text_insert("�n���_", "����Ȃ̂��������I�I���́A�����Ԃ��҂����񂾁I�I", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "���E�Ȃ�Ă����Ƃ����������񂾁B", H_NORMAL, 0);
		com->text_insert("�n���_", "�c�c�c�c������Ƒ҂āA����A���O�A���������Ȃ����H", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�n���_", "���O�͂����A����Ȃɂ���ׂ�Ȃ�����B�������A�Ⴄ�B", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�Ȃ񂾂낤�ȁc�c�c�c������B�I�����A�����v���B", H_NORMAL, 0);
		com->text_insert("�n���_", "�c�c�c�c�c�c�c�c", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "���͂����A�����̖T�ώ҂������B", H_NORMAL, 0);
		com->text_insert("�n���_", "����c�c�c�c�H", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "���́c�c�c�c", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "���c�A���͂��O�̂��Ƃ�F�B�Ƃ͎v���ĂȂ��B", H_NORMAL, 0);
		com->text_insert("�n���_", "���c�c�c�c", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "���R�A���c�⏬�a�c���B�N���F�B�Ƃ͎v���ĂȂ��B", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "�ǂ����������A�����l���ĂȂ��񂾁B��͂��Ȃ񂾁B", H_NORMAL, 0);
		com->text_insert("�n���_", "�́c�c�c�c���c�c�c�c�H", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�S���S���A���ɂƂ��Ă͂�����Ȃ��B", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "���͂������āc�c�c�c�t�̖T�ώ҂������B", H_NORMAL, 0);
		com->text_insert("�n���_", "�c�c�c�c�c�c�c�c", H_NORMAL, 1);
		com->text_insert("�n���_", "�R�c�c�c�c�c�c�c�c�������̂��H", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�n���_", "���܂Łc�c�c�c�S���A�R�������̂��H", H_NORMAL, 1);
		com->text_insert("�n���_", "������l�ŏ���Ɂc�c�c�c����オ���Ă����������Ă̂��H", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "�����A�������B", H_NORMAL, 0);
		com->text_insert("�n���_", "�\�\�\�\�\�\�\�\�R��", H_NORMAL, 1);
		com->text_insert("�n���_", "�����������������������I�I�I", H_NORMAL, 1);

		com->text_insert("�A�J�c�L", "����̂��A�����B", H_NORMAL, 0);
		com->text_insert("�n���_", "�����A�����c�c�c�c�c�c�c�c", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�c�c�������A���c�A����Ȋ炪�ł����̂��B", H_NORMAL, 0);
		com->text_insert("�n���_", "����I�I��΁A��΁I�I�����Ȃ��I�I", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "�i�\�\�������Ă����t�̌i�F�͂������ė[���Ł\�\�j", H_MOUSOU, 0);
		com->text_insert("�A�J�c�L", "�i�S���A���ɂƂ��Ă�����Ȃ��B���ǋP���Ă�񂾁B�j", H_MOUSOU, 0);
		com->text_insert("�A�J�c�L", "�i���ꂾ���Ă������B�[�z���o�b�N�ɔ��c�����Ӗ��ə��K���Ă���B�j", H_MOUSOU, 0);
		com->text_insert("�n���_", "���炠�I�I�����A����您�����I�I", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "���I�I", H_NORMAL, 0);
		com->text_insert("�n���_", "���֌W�����Ă񂶂�˂���I�I", H_NORMAL, 1);
		com->text_insert("�n���_", "�������Ă�΁A���֐S�Ȃ�A��������ƁA�v���Ă�̂����I�I", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�\�\�\�\�\�\�\�\��", H_NORMAL, 0);
		com->text_insert("�n���_", "���O�͂��c�c�����A���؂����񂾂���c�c������񂶂�A�˂���c�c�I�I", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�\�\�\�\�\�\�\�\������", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "�i�\�\�����\�\���l��ς����񂾁B�j", H_MOUSOU, 0);
		com->text_insert("�A�J�c�L", "���ꂾ�������c�c��a���̌����́\�\", H_NORMAL, 0);
		com->text_insert("�n���_", "�c�c�c�c�c�c�c�c", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "���̎��\�\�{�����Ƃ̖񑩂Ł\�\��O�҂��Ⴂ���Ȃ��Ȃ����񂾁B", H_NORMAL, 0);
		com->text_insert("�n���_", "�ւ�ւ炵�Ă񂶂�˂���I�I�����ƁA�����ƁI�I��������Ă��������ɁI�I", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�\�\�\�\�\�\�\�\", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "�\�\�\�\�\�\�\�\����", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "�i�����A�����ƏƂ炳��Ă��񂾂����ȁB�j", H_MOUSOU, 0);
		com->text_insert("�n���_", "�킩���������c�c�킩��������A����c�c", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�����c�c�c�c", H_NORMAL, 0);
		com->text_insert("�n���_", "���O�́A���̐e�F�Ȃ񂾂�c�c", H_NORMAL, 1);
		com->text_insert("�n���_", "���O�ƈꏏ��Z�Z���|���āA����ŏI���Ȃ񂾂�c�c�c�c", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�����A�݂������ȁc�c�c�c", H_NORMAL, 0);
		com->text_insert("�n���_", "���旺��A���ƁA�ꏏ�ɁB", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�����B�s�������B", H_NORMAL, 0);
	}
	play_bgm(bgm[0], 12, 79);
	move(12, 0, 100, 3);
	move(49, 1, 180, 6);
	finish(10);
	finish(11);
	finish(79);
	text_play();
}
void MainMission20::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�A�J�c�L", "�c�c�c�c���A���B", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "�\�\�w�Z�A���邢�ȁc�c�c�c", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "�i�{���ɂy�Z�������񂾂�A���̓����炢�͊w�Z�x�݂ɂȂ����肷��̂��낤���B�j", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "�\�\�܂��A�ǂ��ł��������B", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�A�J�c�L", "�i�����A���c��҂����邱�ƂɂȂ�B�����������čs�����B�j", H_NORMAL, 0);
	}
	event(1, "event/�A�J�c�L�N��.png");//�}�G
	finish(6);
	text_play();
}

///////�f�C���[�~�b�V����//////////
int rand_te_card(int te_kind) {
	int k[5] = {0, 0, 0, 0, 0};
	int r;
	switch (te_kind) {
	case O_T:
		k[0] = 22; k[1] = 20; k[2] = 30;
		r = GetRand(2);
		return k[r];
	case P_T:
		k[0] = 7; k[1] = 22;
		r = GetRand(1);
		return k[r];
	case Q_T:
		k[0] = 24; k[1] = 22; k[2] = 22;
		r = GetRand(2);
		return k[r];
	case R_T:
		k[0] = 6; k[1] = 22;
		r = GetRand(1);
		return k[r];
	case S_T:
		k[0] = 8; k[1] = 22; k[2] = 30;
		r = GetRand(2);
		return k[r];
	case T_T:
		k[0] = 5; k[1] = 12; k[2] = 22;
		r = GetRand(2);
		return k[r];
	case U_T:
		k[0] = 13; k[1] = 27; k[2] = 27;
		r = GetRand(2);
		return k[r];
	case V_T:
		k[0] = 10; k[1] = 14; k[2] = 22;
		r = GetRand(2);
		return k[r];
	case W_T:
		k[0] = 19; k[1] = 4; k[2] = 22;
		r = GetRand(2);
		return k[r];
	case X_T:
		k[0] = 2; k[1] = 3; k[2] = 23;
		r = GetRand(2);
		return k[r];
	case Y_T:
		k[0] = 9; k[1] = 18; k[2] = 28; k[3] = 18; k[4] = 28;
		r = GetRand(4);
		return k[r];
	case Z_T:
		k[0] = 23; k[1] = 18; k[2] = 25;
		r = GetRand(2);
		return k[r];
	}
	return 0;
}
//DM1
//�搶�̎�ނƃJ�[�h
DailyMission1::DailyMission1(MissionText& detail) :
	Mission(detail)
{
	set_rare_rate(80, 20, 0, 0);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 3;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(100, 0, 0, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
//�o�g������������
int DailyMission1::play() {
	switch (now) {
	case 0:
		result = bse->battle_loop(kind);//�g�p�����L������kind�ɓ���ċ����Ă��炤
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 1:
		return result;
		break;
	}
	return 0;
}
void DailyMission1::text1() {
}
void DailyMission1::text2() {
}

DailyMission2::DailyMission2(MissionText& detail) :
	Mission(detail)
{
	set_rare_rate(70, 30, 0, 0);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(100, 0, 0, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
//�o�g������������
int DailyMission2::play() {
	switch (now) {
	case 0:
		result = bse->battle_loop(kind);//�g�p�����L������kind�ɓ���ċ����Ă��炤
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 1:
		return result;
		break;
	}
	return 0;
}
void DailyMission2::text1() {
}
void DailyMission2::text2() {
}

DailyMission3::DailyMission3(MissionText& detail) :
	Mission(detail)
{
	set_rare_rate(63, 32, 5, 0);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(70, 30, 0, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
//�o�g������������
int DailyMission3::play() {
	switch (now) {
	case 0:
		result = bse->battle_loop(kind);//�g�p�����L������kind�ɓ���ċ����Ă��炤
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 1:
		return result;
		break;
	}
	return 0;
}
void DailyMission3::text1() {
}
void DailyMission3::text2() {
}

DailyMission4::DailyMission4(MissionText& detail) :
	Mission(detail)
{
	set_rare_rate(55, 35, 10, 0);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(30, 70, 0, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
//�o�g������������
int DailyMission4::play() {
	switch (now) {
	case 0:
		result = bse->battle_loop(kind);//�g�p�����L������kind�ɓ���ċ����Ă��炤
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 1:
		return result;
		break;
	}
	return 0;
}
void DailyMission4::text1() {
}
void DailyMission4::text2() {
}

DailyMission5::DailyMission5(MissionText& detail) :
	Mission(detail)
{
	set_rare_rate(45, 40, 13, 2);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 100, 0, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
//�o�g������������
int DailyMission5::play() {
	switch (now) {
	case 0:
		result = bse->battle_loop(kind);//�g�p�����L������kind�ɓ���ċ����Ă��炤
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 1:
		return result;
		break;
	}
	return 0;
}
void DailyMission5::text1() {
}
void DailyMission5::text2() {
}

DailyMission6::DailyMission6(MissionText& detail) :
	Mission(detail)
{
	set_rare_rate(35, 45, 15, 5);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 70, 30, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
//�o�g������������
int DailyMission6::play() {
	switch (now) {
	case 0:
		result = bse->battle_loop(kind);//�g�p�����L������kind�ɓ���ċ����Ă��炤
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 1:
		return result;
		break;
	}
	return 0;
}
void DailyMission6::text1() {
}
void DailyMission6::text2() {
}

DailyMission7::DailyMission7(MissionText& detail) :
	Mission(detail)
{
	set_rare_rate(22, 60, 15, 8);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 30, 70, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
//�o�g������������
int DailyMission7::play() {
	switch (now) {
	case 0:
		result = bse->battle_loop(kind);//�g�p�����L������kind�ɓ���ċ����Ă��炤
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 1:
		return result;
		break;
	}
	return 0;
}
void DailyMission7::text1() {
}
void DailyMission7::text2() {
}

DailyMission8::DailyMission8(MissionText& detail) :
	Mission(detail)
{
	set_rare_rate(10, 65, 15, 10);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 0, 100, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
//�o�g������������
int DailyMission8::play() {
	switch (now) {
	case 0:
		result = bse->battle_loop(kind);//�g�p�����L������kind�ɓ���ċ����Ă��炤
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 1:
		return result;
		break;
	}
	return 0;
}
void DailyMission8::text1() {
}
void DailyMission8::text2() {
}

DailyMission9::DailyMission9(MissionText& detail) :
	Mission(detail)
{
	set_rare_rate(0, 58, 20, 12);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 0, 70, 30);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
//�o�g������������
int DailyMission9::play() {
	switch (now) {
	case 0:
		result = bse->battle_loop(kind);//�g�p�����L������kind�ɓ���ċ����Ă��炤
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 1:
		return result;
		break;
	}
	return 0;
}
void DailyMission9::text1() {
}
void DailyMission9::text2() {
}

DailyMission10::DailyMission10(MissionText& detail) :
	Mission(detail)
{
	set_rare_rate(0, 50, 30, 20);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 0, 30, 70);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
//�o�g������������
int DailyMission10::play() {
	switch (now) {
	case 0:
		result = bse->battle_loop(kind);//�g�p�����L������kind�ɓ���ċ����Ă��炤
		if (result == CANCEL) { return CANCEL; }
		if (result == LOSE) { return result; }
		if (result != 0) { now++; }
		break;
	case 1:
		return result;
		break;
	}
	return 0;
}
void DailyMission10::text1() {
}
void DailyMission10::text2() {
}

//�C�x���g�~�b�V����
EventMission1::EventMission1(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/EMc.ogg");
	volume_bgm(1);
	set_rare_rate(63, 32, 5, 0);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(70, 30, 0, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission1::play() {
	return after_text();
}
void EventMission1::text1() {
}
void EventMission1::text2() {
	if (com == NULL) {
		int x[10] = { 100,200,-100,-100,-100,300,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�I���_", "�c�t���̍����Ă��[�A�C�y��������ˁ`�B", H_NORMAL, 5);
		com->text_insert("�n���_", "�܂��A�׋������Ȃ��Ă悩��������ȁB", H_NORMAL, 1);
		com->text_insert("�I���_", "�T�l���Ă���Ȃɕ׋����Ă������H", H_NORMAL, 5);
		com->text_insert("�n���_", "���c�c�c�c���ĂȂ��c�c�c�c", H_NORMAL, 1);
		com->text_insert("�n���_", "�ł��A���̍��͂��������Ă邾���ł��y����������ȁB", H_NORMAL, 1);
		com->text_insert("�I���_", "�c�c�c�c�c�c�c�c�H", H_NORMAL, 5);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�n���_", "�Ȃ��^�╄�I�H�܂����̋����[���I�H", H_NORMAL, 1);
		com->text_insert("�n���_", "���≴�����I�H�����l�����ɑ����Ă邾���ł��Ȃ����y�������������I�H", H_NORMAL, 1);
		com->text_insert("�I���_", "���߂�A���������B�Ȃ�ƂȂ��킩��A����B", H_NORMAL, 5);
		com->text_insert("�n���_", "�������c�c�c�c�����A����ȁB", H_NORMAL, 1);
		com->text_insert("�I���_", "���邾�����Ă̂͋ɒ[�����ǂ��c�c�c�c�Ȃ񂾂낤�A", H_NORMAL, 5);
		com->text_insert("�I���_", "�̊y�����������ƂƂ��A�v���Ԃ��Ή����ʔ����񂾂낤�A���Ă��Ƒ�����ˁB", H_NORMAL, 5);
		com->text_insert("�n���_", "���������A�Ȃ񂩎₵���񂾂�Ȃ��B���ꂪ�B", H_NORMAL, 1);
		com->text_insert("�I���_", "�ł��A�t������񂾂�ˁ`", H_NORMAL, 5);
		com->text_insert("�n���_", "�t�H", H_NORMAL, 1);
		com->text_insert("�I���_", "�c�t���ŗT�l�������炵�����́A�v���o������ʔ������ǁA�����͂Ȃ�Ƃ��v��Ȃ�������B", H_NORMAL, 5);
		com->text_insert("�n���_", "�΂��c�c�c�c�o���Ă��̂���I�I", H_NORMAL, 1);
		com->text_insert("�I���_", "���͂͂͂́I", H_NORMAL, 5);
		com->text_insert("�n���_", "�v���o���Ă��ʔ����˂���A����Ȃ��ƁI�I", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�i�Ȃ�Ƃ��v���Ȃ��������āA��Ԓp����������c�c�c�c�j", H_MOUSOU, 0);
	}
	play_bgm(bgm[0], 1, 21);
	finish(21);
	text_play();
}

EventMission2::EventMission2(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/EMe.ogg");
	volume_bgm(1);
	set_rare_rate(63, 32, 5, 0);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(70, 30, 0, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission2::play() {
	return after_text();
}
void EventMission2::text1() {
}
void EventMission2::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,400,-100,-100,100,-100,-100,-100,300 };
		com = new Text(x, -100);
		com->text_insert("�g���`", "�˂��A���c���čD���Ȑl����H", H_NORMAL, 9);
		com->text_insert("�I�I�^", "�����B���͈�삪�D�����B", H_NORMAL, 2);
		com->text_insert("�I���_", "���������������炢�������茾�����[�I", H_NORMAL, 5);
		com->text_insert("�I�I�^", "�ւցB���͒��ԂɉB�����͂��ˁ[��B", H_NORMAL, 2);
		com->text_insert("�g���`", "������B���ɓ`���Ƃ��ˁB", H_NORMAL, 9);
		com->text_insert("�I�I�^", "���ꂾ���͊��ق��Ă���c�c�c�c", H_NORMAL, 2);
		com->text_insert("�g���`", "���v����B�ޏ������c���D��������B", H_NORMAL, 9);
		com->text_insert("�I�I�^", "���A�}�W�ŁI�H�R����I�H", H_NORMAL, 2);
		com->text_insert("�g���`", "����A�R�B", H_NORMAL, 9);
		com->text_insert("�I�I�^", "�c�c�c�c�c�c�c�c", H_NORMAL, 2);
		com->text_insert("�g���`", "�����͂͂͂͂́I", H_NORMAL, 9);
		com->text_insert("�I�I�^", "�c�c�c�c�c�c�c�c", H_NORMAL, 2);
		com->text_insert("�I���_", "���A���c�A�h���}�C�B", H_NORMAL, 5);
		com->text_insert("�I�I�^", "�c�c�c�c�c�c�c�c", H_NORMAL, 2);
		com->text_insert("�I���_", "�ˁA�ˁA���̂��R���Ă����ł��B������������ق�ƂɍD����������Ȃ���H", H_NORMAL, 5);
		com->text_insert("�I�I�^", "�c�c�c�c�c�c�c�c�����c�c�c�c", H_NORMAL, 2);
		com->text_insert("�I���_", "���A�������݂�������I�H��˂��I�I", H_NORMAL, 5);
		com->text_insert("�g���`", "�ł����̔����I�I�͂͂͂͂́I�I", H_NORMAL, 9);
		com->text_insert("�I���_", "���񂽂˂��c�c�c�c�c�c�c�c", H_NORMAL, 5);
		com->text_insert("�I�I�^", "�ւ��c�c�������B���͂ł����ɔF�߂Ă��炤���B", H_NORMAL, 2);
		com->text_insert("�I���_", "���Ƃ������藧���������c�c�c�c", H_NORMAL, 5);
		com->text_insert("�g���`", "�܁A�N�ɂ�����Ȃ���B�撣�肽�܂��A���c�N�H", H_NORMAL, 9);
		com->text_insert("�I�I�^", "�u�p�̂�남�c�c���̏����ȐS��M�т₪���āc�c�c�c", H_NORMAL, 2);
		com->text_insert("�I���_", "���[��A����ψ�a������܂���B�Ƃ��������A����ȃe���V�����̐l����Ȃ������̂Ɂc�c", H_NORMAL, 5);
		com->text_insert("�I�I�^", "�͂��I�H�܂��ŁA�����炶��˂��́A����I�I", H_NORMAL, 2);
		com->text_insert("�I���_", "����A���N�����N���X���������ǁA�߂�����܂Ƃ���������B", H_NORMAL, 5);
		com->text_insert("�I�I�^", "�R����c�c�c�c�z���ł��˂��c�c�c�c", H_NORMAL, 2);
		com->text_insert("�I���_", "��������łȂ��āA���^�Ȋ����łˁH���ƂȂ����ăN���X�̐l�C�҂������̂�H", H_NORMAL, 5);
		com->text_insert("�I�I�^", "���肦�˂��c�c�c�c���ꂪ���Z��������܂��[���ł��邩���c�c�c�c", H_NORMAL, 2);
	}
	play_bgm(bgm[0], 1, 29);
	move(22, 9, 700, 5);
	finish(29);
	text_play();
}

EventMission3::EventMission3(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/EMb.ogg");
	volume_bgm(1);
	set_rare_rate(63, 32, 5, 0);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(70, 30, 0, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission3::play() {
	return after_text();
}
void EventMission3::text1() {
}
void EventMission3::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,-100,200,-100,-100,400,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�E�`���g", "�ˁ[�ˁ[�䗅�[�A���x�׋������Ă�[�B", H_NORMAL, 7);
		com->text_insert("�^�C��", "��A���̃e�X�g�׋����H", H_NORMAL, 4);
		com->text_insert("�^�C��", "������̂͑S�R�������c�c���i���a�c�ɋ�����Ă�񂶂�Ȃ��̂��H", H_NORMAL, 4);
		com->text_insert("�E�`���g", "��[�����Ȃ񂾂��ǂˁ[�A�S�R���яオ��Ȃ��́B", H_NORMAL, 7);
		com->text_insert("�^�C��", "��������������Ɨǂ��Ȃ���Ďv���Ă�킯���H", H_NORMAL, 4);
		com->text_insert("�E�`���g", "����I�䗅�͂߂����ᐬ�їǂ����A�����Ƌ�����̂����܂���I", H_NORMAL, 7);
		com->text_insert("�^�C��", "���a�c�����їǂ����A���܂���҂��Ȃ����������Ǝv�����B", H_NORMAL, 4);
		com->text_insert("�^�C��", "���a�c���牴�ɕς��Ă��A���т��}���ɕς��Ƃ͎v���Ȃ��B", H_NORMAL, 4);
		com->text_insert("�E�`���g", "�������Ă������āA����Ă݂Ȃ��ƕ�����Ȃ����B", H_NORMAL, 7);
		com->text_insert("�^�C��", "�m���ɂ���͂������B", H_NORMAL, 4);
		com->text_insert("�^�C��", "���Ⴀ���ɂ��悤���H���͏m���Ȃ����Ȃ炢�ł��������B", H_NORMAL, 4);
		com->text_insert("�E�`���g", "���͏m���s���ĂȂ����A�e�X�g�O�͕������Ȃ�����A�䗅�̓s���ɍ��킹���B", H_NORMAL, 7);
		com->text_insert("�E�`���g", "�e�X�g�O�̂ǂ����ňꎞ�ԁA��ԓs�����������ԋ����āH", H_NORMAL, 7);
		com->text_insert("�^�C��", "��c�c�H�ꎞ�Ԃł����̂��A�������Ȃ��Ă����񂾂��B", H_NORMAL, 4);
		com->text_insert("�E�`���g", "���͂́A�����͂��ĂȂ���[�A�ǎq�ɂ�����Ȃ����ԕt�������Ă�����Ă����B", H_NORMAL, 7);
		com->text_insert("�^�C��", "�c�c�c�c�c�c�c�c�Ȃ�ق�", H_NORMAL, 4);
		com->text_insert("�^�C��", "�c�c�c�c���{�A�ꎞ�Ԃ���N�ɋ�����Ă��債�Đ��т͏オ��Ȃ����B", H_NORMAL, 4);
		com->text_insert("�E�`���g", "���c�c�c�c", H_NORMAL, 7);
		com->text_insert("�^�C��", "�i�Ȃ񂾂��̖{�C�̃L���g���Ƃ�����́c�c�j", H_MOUSOU, 4);
	}
	play_bgm(bgm[0], 1, 19);
	finish(19);
	text_play();
}

EventMission4::EventMission4(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/EMd.ogg");
	volume_bgm(1);
	set_rare_rate(63, 32, 5, 0);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(70, 30, 0, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission4::play() {
	return after_text();
}
void EventMission4::text1() {
}
void EventMission4::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,100,200,-100,350,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�~���g�l", "�͂��c�c�c�c�c�c�c�c", H_NORMAL, 6);
		com->text_insert("�C�V", "�ǁA�ǂ������́H", H_NORMAL, 3);
		com->text_insert("�~���g�l", "���A���A���߂�Ȃ����B", H_NORMAL, 6);
		com->text_insert("�~���g�l", "������Ɓc�c����̃e�X�g�̐��т����������̂���������������āc�c", H_NORMAL, 6);
		com->text_insert("�C�V", "�ցA�ւ��B�{��������ł����q�����Ƃ�����񂾁B", H_NORMAL, 3);
		com->text_insert("�C�V", "���Ȃ݂Ɂc�c���ʂ��������u���Ă����H", H_NORMAL, 3);
		com->text_insert("�~���g�l", "���A�͂��c�c�P�Q�ʁc�c�ł����B", H_NORMAL, 6);
		com->text_insert("�C�V", "�c�c�c�c�c�c�c�c", H_NORMAL, 3);
		com->text_insert("�^�C��", "������Ɗ��҂������A�R�X�ʁB", H_NORMAL, 4);
		com->text_insert("�C�V", "���A���ĂȂ��I�I�Ă����ʂŌĂԂȁI�I", H_NORMAL, 3);
		com->text_insert("�~���g�l", "�����Ɓc�c�䗅�A�N�͉��ʂł����c�c�H", H_NORMAL, 6);
		com->text_insert("�^�C��", "���͂S�ʂ������B", H_NORMAL, 4);
		com->text_insert("�~���g�l", "�����ł����c�c�c�c", H_NORMAL, 6);
		com->text_insert("�C�V", "�c�c���A�C�ɂ��Ȃ��Ă������āI�����A�ӊO�Ɛ^�ʖڂɏm�s���Ă邵�I", H_NORMAL, 3);
		com->text_insert("�~���g�l", "���A���߂�Ȃ����A�����S�Ƃ��A�����Ă�킯����Ȃ��āc�c", H_NORMAL, 6);
		com->text_insert("�~���g�l", "�׋����@�Ƃ��c�c�R�c�Ƃ��A�u�������Ȃ��āc�c", H_NORMAL, 6);
		com->text_insert("�C�V", "�i��΂��A�䗅�͋{��������قǕ׋����ĂȂ����B�j", H_MOUSOU, 3);
		com->text_insert("�C�V", "�i�����m������V���b�N���󂯂�񂶂�c�c�c�c�j", H_MOUSOU, 3);
		com->text_insert("�^�C��", "���ƒ��ɐ搶�̌��Ԃ�Ƃ�����A�e�X�g�ŏo�����\�z����̂͌��\�����ȁB", H_NORMAL, 4);
		com->text_insert("�~���g�l", "�ȁA�Ȃ�قǁc�c�c�c�I", H_NORMAL, 6);
		com->text_insert("�C�V", "�Ȃ�قǁI�I", H_NORMAL, 3);
	}
	play_bgm(bgm[0], 1, 21);
	finish(21);
	text_play();
}

EventMission5::EventMission5(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/EMb.ogg");
	volume_bgm(1);
	set_rare_rate(55, 35, 10, 0);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(30, 70, 0, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission5::play() {
	return after_text();
}
void EventMission5::text1() {
}
void EventMission5::text2() {
	if (com == NULL) {
		int x[10] = { 200,-100,-100,-100,-100,-100,-100,-100,400,-100 };
		com = new Text(x, -100);
		com->text_insert("�C�o���J��", "�܂������B�ǂ����Ĕ��c�͎��ɗ��񂾂́B", H_NORMAL, 8);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�C�o���J��", "�Ԍ��Ƃ����Ă���܈ꏏ�ɐ�������ƂȂ��̂ɁB", H_NORMAL, 8);
		com->text_insert("�C�o���J��", "�����ƕʂ̕Ґ����������ł���B�����Ƃ����ƍl���Ăق�����B", H_NORMAL, 8);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�C�o���J��", "���񂽂������v��Ȃ��H", H_NORMAL, 8);
		com->text_insert("�A�J�c�L", "����c�c�c�c�ʂɁc�c�c�c", H_NORMAL, 0);
		com->text_insert("�C�o���J��", "�c�c�܁A������B", H_NORMAL, 8);
		com->text_insert("�C�o���J��", "��̔��c�͂����l�߂��Â��Ǝv���̂�B", H_NORMAL, 8);
		com->text_insert("�C�o���J��", "���̑O�����Ă����B�����Ɛ搶�Ƃ̑������l���Ă��炠��Ȃɋ�킵�Ȃ������B", H_NORMAL, 8);
		com->text_insert("�A�J�c�L", "�c�c�c�c���c�c�c�c", H_NORMAL, 0);
		com->text_insert("�C�o���J��", "�Ȃ�", H_NORMAL, 8);
		com->text_insert("�A�J�c�L", "����c�c�c�c", H_NORMAL, 0);
		com->text_insert("�C�o���J��", "�Ȃɂ����傠��́H", H_NORMAL, 8);
		com->text_insert("�A�J�c�L", "�ʂɁc�c�c�c", H_NORMAL, 0);
		com->text_insert("�C�o���J��", "���񂽂��ˁA���܂ɂ͔��c�ɔ��_���Ă������Ǝv���B", H_NORMAL, 8);
		com->text_insert("�C�o���J��", "��ԋ߂��ɂ��邠�񂽂������ƌ����̂͑厖�ł���B", H_NORMAL, 8);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
	}
	play_bgm(bgm[0], 1, 18);
	finish(18);
	text_play();
}

EventMission6::EventMission6(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/EMe.ogg");
	volume_bgm(1);
	set_rare_rate(55, 35, 10, 0);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(30, 70, 0, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission6::play() {
	return after_text();
}
void EventMission6::text1() {
}
void EventMission6::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,200,300,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�I�I�^", "�Ȃ�����A�܂����x�V�ڂ����I", H_NORMAL, 2);
		com->text_insert("�C�V", "���A�������ǁc�c�ǂ����āH", H_NORMAL, 3);
		com->text_insert("�I�I�^", "�ǂ����āH�V�Ԃ̂ɗ��R�Ȃ�Ă���Ȃ�����[�H", H_NORMAL, 2);
		com->text_insert("�C�V", "���A���������ǁA������Ƌ������񂾁B", H_NORMAL, 3);
		com->text_insert("�C�V", "���c���l��U���Ă���Ȃ�Ďv���Ă��Ȃ������B", H_NORMAL, 3);
		com->text_insert("�I�I�^", "�͂͂́A��������������A���ʂ����āI", H_NORMAL, 2);
		com->text_insert("�C�V", "���߂�A�l���Ђ˂���Ă邾�����B", H_NORMAL, 3);
		com->text_insert("�C�V", "�l�Ȃ񂩂Ƃ����̂��ȁA�Ďv�����Ⴄ�񂾁B", H_NORMAL, 3);
		com->text_insert("�I�I�^", "���[�A�킩�邺�A���̋C�����I", H_NORMAL, 2);
		com->text_insert("�C�V", "����\�\���₢�₢��I�킩��Ȃ��ł���A�N�ɂ́I", H_NORMAL, 3);
		com->text_insert("�C�V", "�N�͖l�Ƃ͐����΂̐l�킶��񂩁I", H_NORMAL, 3);
		com->text_insert("�I�I�^", "���[�A���ł�������Ƃ͂��邺�H�����Ɏ��M���Ȃ������B", H_NORMAL, 2);
		com->text_insert("�C�V", "�͂͂́c�c����ȁA�����ɋ������Ă���Ȃ��Ă������̂Ɂc�c", H_NORMAL, 3);
		com->text_insert("�I�I�^", "����Ȃ��Ƃ˂���B�������Đ̂͂���Ȃ��������������ȁB", H_NORMAL, 2);
		com->text_insert("�I�I�^", "������Đ̂̉��Ɏ��Ă�񂾂�ȁB", H_NORMAL, 2);
		com->text_insert("�C�V", "���H�\�\�R�A����ˁH�Ȃ�ł���Ȑ^�ʖڂȌ����Ł\�\", H_NORMAL, 3);
		com->text_insert("�I�I�^", "�������ǂ������ǂ����������Ȃ�ĂȂ����H", H_NORMAL, 2);
		com->text_insert("�C�V", "���A�����Ƒ傰���Ɍ����Ă�񂾂�ˁH", H_NORMAL, 3);
		com->text_insert("�I�I�^", "����A���͂���Ȃ���ˁ[��H�R�����Ă��˂��B", H_NORMAL, 2);
		com->text_insert("�C�V", "���₢�₢�₢��I�I", H_NORMAL, 3);
	}
	play_bgm(bgm[0], 1, 20);
	finish(20);
	text_play();
}

EventMission7::EventMission7(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/EMa.ogg");
	volume_bgm(1);
	set_rare_rate(55, 35, 10, 0);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(30, 70, 0, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission7::play() {
	return after_text();
}
void EventMission7::text1() {
}
void EventMission7::text2() {
	if (com == NULL) {
		int x[10] = { -100,200,-100,-100,-100,-100,-100,500,300,-100 };
		com = new Text(x, -100);
		com->text_insert("�n���_", "���߂�ˁA�}�ɋ��͗��񂶂���āB", H_NORMAL, 1);
		com->text_insert("�C�o���J��", "�܁A���ɂ��Ȃ��Ȃ�d���Ȃ��ł���B", H_NORMAL, 8);
		com->text_insert("�E�`���g", "�S�R������[�I���ꂩ������������ɗ���ł�I", H_NORMAL, 7);
		com->text_insert("�C�o���J��", "���͂Ȃ�ׂ���߂Ăق������ǁc�c", H_NORMAL, 8);
		com->text_insert("�n���_", "���߂�A�݂�Ȏ����̓s��������̂Ɂc�c", H_NORMAL, 1);
		com->text_insert("�C�o���J��", "�����́A�Ȃɂ��Ȃ����炢�����ǁc�c���ꂩ��͂��܂���҂��Ȃ��ŁB", H_NORMAL, 8);
		com->text_insert("�E�`���g", "���A���͑����A��Ȃ��ƁI", H_NORMAL, 7);
		com->text_insert("�n���_", "�p���������񂾁B�ق�Ƃ��߂�I", H_NORMAL, 1);
		com->text_insert("�E�`���g", "������[�B�����A���ăv���e�B�[�L���A�[�Y�ςȂ��ƁI", H_NORMAL, 7);
		com->text_insert("�n���_", "��H�\�\�v���e�B�[�c�c�c�c�H", H_NORMAL, 1);
		com->text_insert("�E�`���g", "����A���T�ςĂ�񂾁[�I", H_NORMAL, 7);
		com->text_insert("�E�`���g", "���j�ɂ���Ă�񂾂��ǁA�N����Ȃ�����^�悵�Ă�́B���T�͂܂��ςĂȂ����I", H_NORMAL, 7);
		com->text_insert("�n���_", "����A���̃A�j�����̂͒m���Ă邯�ǁc�c�c�c", H_NORMAL, 1);
		com->text_insert("�n���_", "�i�˂���삳��A������ď������q���ς�A�j�����Ǝv���Ă��񂾂��ǌ��\���w���ł��ς�́H�j", H_NORMAL, 1);
		com->text_insert("�C�o���J��", "�i�m��Ȃ����B�����̊ςĂ����Ǎ��͂����ςĂȂ��B�j", H_NORMAL, 8);
		com->text_insert("�C�o���J��", "�i�\�\���A�ł��I�^�N�̃N���X���C�g���ςĂ��ȁ\�\�j", H_NORMAL, 8);
		com->text_insert("�n���_", "���{������Ă��A���ɂ��A�j���ςĂ�́H", H_NORMAL, 1);
		com->text_insert("�E�`���g", "���\�\�H���[��A�����T�ςĂ�̂̓v���L���A�[�Y�������ȁ[�B", H_NORMAL, 7);
		com->text_insert("�n���_", "�i���́A���B�j", H_NORMAL, 1);
		com->text_insert("�C�o���J��", "�i�܁A�ޏ��̂��Ƃ����A�P���ɒ��g���q���Ȃ���������B�j", H_NORMAL, 8);
		com->text_insert("�n���_", "�i�m���ɁA�I��������ȋC������B�j", H_NORMAL, 1);
		com->text_insert("�C�o���J��", "�i�ǂ����ɂ���A�F������K�v���Ȃ��ł���B�j", H_NORMAL, 8);
		com->text_insert("�n���_", "�i��������ˁB���߂�A��������ƋC�ɂȂ���������B�j", H_NORMAL, 1);
	}
	play_bgm(bgm[0], 1, 23);
	finish(23);
	text_play();
}

EventMission8::EventMission8(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/EMb.ogg");
	volume_bgm(1);
	set_rare_rate(55, 35, 10, 0);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(30, 70, 0, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission8::play() {
	return after_text();
}
void EventMission8::text1() {
}
void EventMission8::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,-100,-100,-100,400,-100,-100,500 };
		com = new Text(x, -100);
		com->text_insert("�~���g�l", "�͂��c�c�c�c�c�c�c�c", H_NORMAL, 6);
		com->text_insert("�g���`", "�ӁA�N�͉^�������B���ߑ���f�����Ƃ��ׂɎ��������̂�����B", H_NORMAL, 9);
		com->text_insert("�~���g�l", "���A�ȁA�Ȃ�ł����I�H", H_NORMAL, 6);
		com->text_insert("�g���`", "�Y�݂�����̂��낤�H���ɘb���Ƃ����B", H_NORMAL, 9);
		com->text_insert("�~���g�l", "����A�����������肶��\�\����A�Y�݂͂��邯�ǁ\�\", H_NORMAL, 6);
		com->text_insert("�g���`", "�\�\�\�\�\�\�\�\", H_NORMAL, 9);
		com->text_insert("�~���g�l", "�\�\�����ƁA���Ⴀ������c�c�c�c", H_NORMAL, 6);
		com->text_insert("�g���`", "���ł�������", H_NORMAL, 9);
		com->text_insert("�~���g�l", "�����̂����׋��̎d���������Ă��������I", H_NORMAL, 6);
		com->text_insert("�g���`", "�ꂩ���ɕK�v�ȕ׋��ʂ�����ŏI��点��΂����I�I", H_NORMAL, 9);
		com->text_insert("�g���`", "���������玟�̌��܂ŗV�т܂����I�I�ȏ�I�I", H_NORMAL, 9);
		com->text_insert("�~���g�l", "�����ɂȂ��ĂȂ����A����������I�I", H_NORMAL, 6);
		com->text_insert("�g���`", "����ňꂩ�����̕׋������΂���������B", H_NORMAL, 9);
		com->text_insert("�g���`", "����ň�C�ɂ�����ق��������I�ł���H", H_NORMAL, 9);
		com->text_insert("�~���g�l", "���́A����ŏI��点����@���m�肽���ł��c�c�c�c", H_NORMAL, 6);
		com->text_insert("�g���`", "�H�v���邱�Ƃ��I�I", H_NORMAL, 9);
		com->text_insert("�~���g�l", "���̏ڍׂ��m�肽����ł��I�I", H_NORMAL, 6);
		com->text_insert("�g���`", "���΂ł���I�I", H_NORMAL, 9);
		com->text_insert("�~���g�l", "�c�c�c�c�͂��c�c���������A���͕ʂɕ׋����Ԃ����炵�����킯����Ȃ���ł��B", H_NORMAL, 6);
		com->text_insert("�g���`", "�Ȃ�ƁB�V�Ԏ��Ԃ͂���Ȃ��̂��H", H_NORMAL, 9);
		com->text_insert("�~���g�l", "�V�΂Ȃ��ł��B�׋��������Ȃ���ŁB", H_NORMAL, 6);
		com->text_insert("�g���`", "�Ȃ�Ɓc�c�c�c�c�c�c�c�I�I", H_NORMAL, 9);
		com->text_insert("�~���g�l", "�͂��A����ς�A�˔\�Ȃ��̂��Ȃ��B", H_NORMAL, 6);
		com->text_insert("�g���`", "�c�c�c�c�ł́A���������̂����V�ѕ��������悤�B", H_NORMAL, 9);
		com->text_insert("�~���g�l", "�V�ѕ��H�\�\����A���́\�\", H_NORMAL, 6);
		com->text_insert("�g���`", "�ł͍s�����B���ɂ��Ă���̂��I�I", H_NORMAL, 9);
		com->text_insert("�~���g�l", "�׋����@����Ȃ��Ȃ炢���ł��I�I����ɑ����A���ĕ׋����Ȃ��ƁI�I", H_NORMAL, 6);
		com->text_insert("�g���`", "������I�H�ł͂������Ęb���Ă鎞�Ԃ��ɂ����B", H_NORMAL, 9);
		com->text_insert("�g���`", "�����ł����A���Ă���̂��I�I", H_NORMAL, 9);
		com->text_insert("�~���g�l", "����A�����ł����āI�����ł��I�I�\�\��������I�I", H_NORMAL, 6);
		com->text_insert("�~���g�l", "��߂�[�[�[�[�I�I�I", H_NORMAL, 6);
	}
	play_bgm(bgm[0], 1, 31);
	move(28, 9, 440, 5);
	move(30, 9, 700, 3);
	move(30, 6, 700, 3);
	finish(31);
	text_play();
}

EventMission9::EventMission9(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/EMc.ogg");
	volume_bgm(1);
	set_rare_rate(45, 40, 13, 2);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 100, 0, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission9::play() {
	return after_text();
}
void EventMission9::text1() {
}
void EventMission9::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,200,-100,300,-100,-100,400,-100 };
		com = new Text(x, -100);
		com->text_insert("�C�V", "�c�c�c�c�c�c�c�c", H_NORMAL, 3);
		com->text_insert("�I���_", "�c�c�˂��A�Ȃ�ňɎu����ׂ�Ȃ��́H", H_NORMAL, 5);
		com->text_insert("�C�V", "����A���́c�c�c�c����ׂ邱�ƂȂ����c�c", H_NORMAL, 3);
		com->text_insert("�I���_", "�Ȃ�ŁH�䗅�Ƃ��Ƃ͂�������ׂ��Ă邶���B", H_NORMAL, 5);
		com->text_insert("�C�V", "����c�c�����́A���́c�c�c�c", H_NORMAL, 3);
		com->text_insert("�I���_", "���A�ȂɁH", H_NORMAL, 5);
		com->text_insert("�C�V", "���A���c�c�c�c", H_NORMAL, 3);
		com->text_insert("�I���_", "�ْ����Ă�H�ِ��Ƙb���̋��H", H_NORMAL, 5);
		com->text_insert("�C�V", "���A�����B����c�c�c�c", H_NORMAL, 3);
		com->text_insert("�C�o���J��", "�\�\�ӂ�", H_NORMAL, 8);
		com->text_insert("�C�V", "�i�΂�ꂽ�\�\�j", H_MOUSOU, 3);
		com->text_insert("�I���_", "���v�A�ْ����邱�ƂȂ����āB�����V�q�������t�����h���[��B", H_NORMAL, 5);
		com->text_insert("�C�o���J��", "����A���͕ʂɁ\�\", H_NORMAL, 8);
		com->text_insert("�C�V", "�c�c�c�c�c�c�c�c", H_NORMAL, 3);
		com->text_insert("�I���_", "�ق�A�Ȃɂ��b��U���Ă݂Ă�I", H_NORMAL, 5);
		com->text_insert("�C�V", "�c�c�c�c�c�c�c�c", H_NORMAL, 3);
		com->text_insert("�I���_", "�ق�A�Ȃ�ł���������A�w�Z�̂��ƂƂ��I�I", H_NORMAL, 5);
		com->text_insert("�C�V", "�c�c�c�c�c�cP�搶��", H_NORMAL, 3);
		com->text_insert("�I���_", "����A����I", H_NORMAL, 5);
		com->text_insert("�C�V", "�悭�s�ǂɂ����߂��Ă邯�ǁA���킢��������ˁB", H_NORMAL, 3);
		com->text_insert("�C�o���J��", "�\�\�ӂ�", H_NORMAL, 8);
		com->text_insert("�I���_", "����I���������v���I", H_NORMAL, 5);
		com->text_insert("�I���_", "�ł����[�A�{�l�ɂ������͂���͖̂��������A�d���Ȃ��C������B�Ɏu�������v���H", H_NORMAL, 5);
		com->text_insert("�C�V", "����A�{�l�ɂ������͏��Ȃ��炸���邯�ǁA��Ԃ͉ƒ������Ȃ����ȁH", H_NORMAL, 3);
		com->text_insert("�I���_", "���H�\�\�ǂ��������ƁH", H_NORMAL, 5);
		com->text_insert("�C�V", "�s�ǂ��Ă��A�ƒ���ɖ�肪���邱�Ƒ����񂾂�B�Ⴆ�Η��e����������������B", H_NORMAL, 3);
		com->text_insert("�I���_", "���A�������\�\�H", H_NORMAL, 5);
		com->text_insert("�C�V", "�����v���ƁA�ނ���ƒ�����ǂ���΁A�l�������߂邩�킢�����Ȑl�ԂɂȂ炸�ɍς񂾂�������Ȃ���ˁB", H_NORMAL, 3);
		com->text_insert("�I���_", "���A���킢�������āA�������\�\�H", H_NORMAL, 5);
		com->text_insert("�C�o���J��", "���Ác�c�c�c�Ђ˂���Ă�c�c�c�c", H_NORMAL, 8);
		com->text_insert("�C�V", "���A�����I�H�m���ɂ��������ǁA�Ȃ����̉�b���炻���Ȃ�́I�H", H_NORMAL, 3);
		com->text_insert("�C�o���J��", "������b���������ĂȂ��������c�c", H_NORMAL, 8);
		com->text_insert("�C�V", "�����I�H�Ȃ�ŁI�H", H_NORMAL, 3);
		com->text_insert("�I���_", "�\�\�����͂͂͂͂́I�I�Ɏu���Ă������낷���I�I", H_NORMAL, 5);
		com->text_insert("�C�V", "�܂��΂�ꂽ�c�c�c�c�c�c�c�c", H_NORMAL, 3);
		com->text_insert("�C�o���J��", "�܁A����������Ǝv���΂����񂶂�Ȃ��H", H_NORMAL, 8);
		com->text_insert("�C�V", "�ӁA�D�ɗ����Ȃ��c�c", H_NORMAL, 3);
	}
	play_bgm(bgm[0], 1, 37);
	finish(37);
	text_play();
}

EventMission10::EventMission10(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/EMa.ogg");
	volume_bgm(1);
	set_rare_rate(45, 40, 13, 2);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 100, 0, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission10::play() {
	return after_text();
}
void EventMission10::text1() {
}
void EventMission10::text2() {
	if (com == NULL) {
		int x[10] = { -100,100,-100,-100,-100,-100,300,-100,-100,400 };
		com = new Text(x, -100);
		com->text_insert("�n���_", "��l�Ƃ��A�����͂��肪�Ƃ��B", H_NORMAL, 1);
		com->text_insert("�~���g�l", "�ǁA�ǂ����\�\", H_NORMAL, 6);
		com->text_insert("�g���`", "�\�\�O���猾�����Ǝv���Ă��񂾂��ǁA���c�N���Ă��[", H_NORMAL, 9);
		com->text_insert("�n���_", "���A�ȁA�ȂɁH", H_NORMAL, 1);
		com->text_insert("�g���`", "���i�݂�Ȃ̃��[�_�[�I�ȃ|�W�V�����ŐU�����Ă邯�ǂ��[", H_NORMAL, 9);
		com->text_insert("�g���`", "�{���͂���ȃL��������Ȃ������肵��", H_NORMAL, 9);
		com->text_insert("�~���g�l", "�\�\���A���A�����Ȃ�ł����H", H_NORMAL, 6);
		com->text_insert("�n���_", "���A����A����Ȃ��ƂȂ����ǁH�ǂ����Ă����v���񂾁H", H_NORMAL, 1);
		com->text_insert("�g���`", "�Ȃ񂩎��X�A����ĂȂ�����������񂾂�ˁ[�B", H_NORMAL, 9);
		com->text_insert("�n���_", "����A����ȓu�p�̒����Ō����Ă��Ȃ��B", H_NORMAL, 1);
		com->text_insert("�g���`", "�����A�m���ɁA�����܂ŗ\�z�������̂�B", H_NORMAL, 9);
		com->text_insert("�g���`", "�ł������A�x�ݎ��Ԃɂ��Ȃ��̋����̑O�ʂ����Ƃ���", H_NORMAL, 9);
		com->text_insert("�g���`", "�����̋��ň�l����������", H_NORMAL, 9);
		com->text_insert("�~���g�l", "���[�A����͈ӊO�ł��c�c�c�c", H_NORMAL, 6);
		com->text_insert("�g���`", "�����������i�̃����o�[�ȊO�Ƙb���Ă�̌������ƂȂ��񂾂�ˁ[�B", H_NORMAL, 9);
		com->text_insert("�g���`", "�ӂӂӁA�ʔ�����ˁB���\�������ă��[�_�[���Ă�񂶂�Ȃ��H", H_NORMAL, 9);
		com->text_insert("�~���g�l", "����A����ł��\�z�ł���I��l�ł����̂����܂��܂�������Ȃ��ł���ˁI�H", H_NORMAL, 6);
		com->text_insert("�n���_", "�c�c�c�c�c�c�c�c", H_NORMAL, 1);
		com->text_insert("�~���g�l", "���܂葽���̐l�Ƃ�܂Ȃ��^�C�v�Ȃ����ł���ˁI�H", H_NORMAL, 6);
		com->text_insert("�n���_", "�c�c�c�c�c�c�c�c", H_NORMAL, 1);
		com->text_insert("�~���g�l", "�����I�H�ق���������I�H", H_NORMAL, 6);
	}
	play_bgm(bgm[0], 1, 21);
	move(3, 9, 200, 5);
	finish(21);
	text_play();
}

EventMission11::EventMission11(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/EMd.ogg");
	volume_bgm(1);
	set_rare_rate(45, 40, 13, 2);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 100, 0, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission11::play() {
	return after_text();
}
void EventMission11::text1() {
}
void EventMission11::text2() {
	if (com == NULL) {
		int x[10] = { 100,-100,400,-100,-100,-100,-100,200,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�I�I�^", "������A���{�I�I", H_NORMAL, 2);
		com->text_insert("�E�`���g", "����A������[�I", H_NORMAL, 7);
		com->text_insert("�I�I�^", "��������肪�ƂȁI", H_NORMAL, 2);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c�����B", H_NORMAL, 0);
		com->text_insert("�I�I�^", "�c�c�c�c�c�c�c�c", H_NORMAL, 2);
		com->text_insert("�I�I�^", "�i����ϗ���͂������������ȁj", H_MOUSOU, 2);
		com->text_insert("�I�I�^", "�i�T�l�͊���Ă�݂Ă������A���͂܂���a�����邺�B�j", H_MOUSOU, 2);
		com->text_insert("�E�`���g", "�Ԍ��N�A�������q�悳�����������ˁI�I", H_NORMAL, 7);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�E�`���g", "����[�A���������͂Ȃ������񂾂��ǂˁA�����ǂ����Ƃ������H", H_NORMAL, 7);
		com->text_insert("�A�J�c�L", "�ʂɁc�c�c�c", H_NORMAL, 0);
		com->text_insert("�E�`���g", "�������[�I����ȓ����Ă����ˁ[�I", H_NORMAL, 7);
		com->text_insert("�A�J�c�L", "�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�E�`���g", "���A�Ȃ��H���������ȁ[�H����Ǝv���񂾂��ǂȁ[�B", H_NORMAL, 7);
		com->text_insert("�I�I�^", "�c�c�c�c�c�c�c�c", H_NORMAL, 2);
		com->text_insert("�I�I�^", "�i���{�͂������Ȃ��c�c�c�c�j", H_MOUSOU, 2);
	}
	play_bgm(bgm[0], 1, 16);
	finish(16);
	text_play();
}

EventMission12::EventMission12(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/EMc.ogg");
	volume_bgm(1);
	set_rare_rate(45, 40, 13, 2);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 100, 0, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission12::play() {
	return after_text();
}
void EventMission12::text1() {
}
void EventMission12::text2() {
	if (com == NULL) {
		int x[10] = { -100,200,-100,-100,300,450,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�n���_", "���[��A������킪�v�������񂾂��ǁA������Ɩ�肠��Ȃ��B", H_NORMAL, 1);
		com->text_insert("�I���_", "�ց[�A�ǂ�ȍ��H", H_NORMAL, 5);
		com->text_insert("�n���_", "���ƒ��ɒN�����Ȃ�������㩂�ݒu���ĕ��یセ���Ő키�񂾁B", H_NORMAL, 1);
		com->text_insert("�I���_", "�Ȃ��Ȃ����邢�ˁc�c", H_NORMAL, 5);
		com->text_insert("�^�C��", "�ƂȂ�ƁA�ǂ�����Ď��ƒ��ɔ����o�������ĂƂ����ȁB", H_NORMAL, 4);
		com->text_insert("�n���_", "�����Ȃ񂾁B�g�C�����Č����Ă��A㩎d�|����̂Ɏ��Ԃ����肻�������c�c", H_NORMAL, 1);
		com->text_insert("�I���_", "���Ⴀ�ی����ɍs�����Č��������Ȃ��񂶂�Ȃ��H", H_NORMAL, 5);
		com->text_insert("�n���_", "㩐ݒu�ɂǂꂾ�����Ԃ������邩�c�c�T�O���ȓ��ɂł��邩�ȁc�c", H_NORMAL, 1);
		com->text_insert("�I���_", "���̎��ƒ��ɂł��Ȃ��Ȃ疳���ł���B", H_NORMAL, 5);
		com->text_insert("�^�C��", "���S����΂����B����ɔ��c�A����ɉ����Ċ����łȁB", H_NORMAL, 4);
		com->text_insert("�I���_", "�m���ɁB�ł����̓p�X�ŁB���Ɣ�����̂͂���B", H_NORMAL, 5);
		com->text_insert("�n���_", "��ۂ悭��l�łł���l����Ȃ��Ƃ��߂��B����ς肱�̍��͖������B", H_NORMAL, 1);
		com->text_insert("�^�C��", "�����Ō����̂��Ȃ񂾂��A������_�����H", H_NORMAL, 4);
		com->text_insert("�n���_", "���A�����̂��H���Ǝ󂯂�Ȃ����H", H_NORMAL, 1);
		com->text_insert("�^�C��", "�S�R�������B�ނ����肽���ȁB", H_NORMAL, 4);
		com->text_insert("�I���_", "�}�W�H�ӊO�c�c�c�c", H_NORMAL, 5);
		com->text_insert("�^�C��", "���ی�O�̂T�C�U���ŒN�����Ȃ��āA�����ł��g���Ȃ������𒲂ׂ悤�B", H_NORMAL, 4);
		com->text_insert("�^�C��", "���A���c�͑��v���H�����Ȃ�u�p�ɂł����ނ��B", H_NORMAL, 4);
		com->text_insert("�n���_", "���₢��A�����邢����I�ނ��뉴����肽���I", H_NORMAL, 1);
		com->text_insert("�I���_", "�͂��c�c�~�߂Ȃ����ǂ��A���񂽂炻��ł����́H", H_NORMAL, 5);
		com->text_insert("�n���_", "����[�A���Ɣ����o���̂��Ă���Ă݂��������񂾂�Ȃ��B", H_NORMAL, 1);
		com->text_insert("�^�C��", "���̃N���X�̎��ƕ��i���̂��������Ă݂����ȁB", H_NORMAL, 4);
		com->text_insert("�n���_", "���A�����I���������Ă������I", H_NORMAL, 1);
		com->text_insert("�I���_", "����I�H�Ȃ񂩖ړI�ς���ĂȂ��I�H", H_NORMAL, 5);
	}
	play_bgm(bgm[0], 1, 24);
	finish(24);
	text_play();
}

EventMission13::EventMission13(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/EMe.ogg");
	volume_bgm(1);
	set_rare_rate(35, 45, 15, 5);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 70, 30, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission13::play() {
	return after_text();
}
void EventMission13::text1() {
}
void EventMission13::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,200,-100,-100,-100,-100,-100,400 };
		com = new Text(x, -100);
		com->text_insert("�g���`", "�Ɏu�N���Ă��[�A�ِ��ɋ����Ƃ�����́H", H_NORMAL, 9);
		com->text_insert("�C�V", "�i�܂��n�܂����c�c�c�c�j", H_MOUSOU, 3);
		com->text_insert("�C�V", "����A�ʂɂ���ȂɁB", H_NORMAL, 3);
		com->text_insert("�g���`", "���[�A��������������B", H_NORMAL, 9);
		com->text_insert("�C�V", "�Ȃ����āI", H_NORMAL, 3);
		com->text_insert("�g���`", "����͂ˁ[�A�T�^�I�ȉR�p�^�[������B�����́u�����v�Ƃ������̂�������B", H_NORMAL, 9);
		com->text_insert("�C�V", "���A�������c�c�c�c", H_NORMAL, 3);
		com->text_insert("�g���`", "���[��������[�B������čD���ȏ��q�ɈӒn���������Ⴄ��H", H_NORMAL, 9);
		com->text_insert("�C�V", "�܂����B���肦�Ȃ���B", H_NORMAL, 3);
		com->text_insert("�g���`", "���[�A�ؗ�Ɏ󂯗����ˁ[�B", H_NORMAL, 9);
		com->text_insert("�C�V", "���ꂽ�񂾂�B�N�̌����ɂ͂����f�킳��Ȃ��B", H_NORMAL, 3);
		com->text_insert("�g���`", "������Ƃ�����葁���ɂȂ��Ă邯�ǂˁB", H_NORMAL, 9);
		com->text_insert("�C�V", "���I�H�c�c�c�c��", H_NORMAL, 3);
		com->text_insert("�g���`", "�����͂��͂͂́I", H_NORMAL, 9);
		com->text_insert("�C�V", "�͂��c�c����Ⴀ���A�ِ��ɑS���������Ȃ��킯����Ȃ���B", H_NORMAL, 3);
		com->text_insert("�C�V", "���̐l�͂����ł���H", H_NORMAL, 3);
		com->text_insert("�g���`", "��������B������B���K�v�Ȃ�������ˁB�Ɏu�N������Ɏ��ł�����������ˁH", H_NORMAL, 9);
		com->text_insert("�C�V", "�����͂��́c�c��������A��������B", H_NORMAL, 3);
		com->text_insert("�C�V", "�l�����Ă��AT�搶�݂����ȑ�l�ȏ����ɓ���邵�B", H_NORMAL, 3);
		com->text_insert("�g���`", "���\�\�\�\", H_NORMAL, 9);
		com->text_insert("�g���`", "�\�\�\�\�����\�\", H_NORMAL, 9);
		com->text_insert("�C�V", "���A���߂��B�x����Ȃ����B", H_NORMAL, 3);
		com->text_insert("�g���`", "�\�\���A�͂͂́I���d�����I", H_NORMAL, 9);
		com->text_insert("�C�V", "�����[�A�������Ԃ��Ă��c�c�c�c", H_NORMAL, 3);
		com->text_insert("�g���`", "���͂͂́\�\�\�\�ӂ�", H_NORMAL, 9);
	}
	play_bgm(bgm[0], 1, 25);
	finish(25);
	text_play();
}

EventMission14::EventMission14(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/EMb.ogg");
	volume_bgm(1);
	set_rare_rate(35, 45, 15, 5);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 70, 30, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission14::play() {
	return after_text();
}
void EventMission14::text1() {
}
void EventMission14::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,-100,300,-100,200,-100,400,-100 };
		com = new Text(x, -100);
		com->text_insert("�C�o���J��", "���������A�I�I�^�̂�c�c", H_NORMAL, 8);
		com->text_insert("�~���g�l", "���c�N���ǂ���������ł����H", H_NORMAL, 6);
		com->text_insert("�C�o���J��", "���������ɂ����ł���̂�B�܂������A�����Ƃ������B", H_NORMAL, 8);
		com->text_insert("�~���g�l", "�͂́c�c�����Ȃ�ł����c�c", H_NORMAL, 6);
		com->text_insert("�C�o���J��", "�\�\�Ȃ�", H_NORMAL, 8);
		com->text_insert("�~���g�l", "���A����A�������񂾂Ȃ��Ďv���ā\�\", H_NORMAL, 6);
		com->text_insert("�C�o���J��", "�悭�Ȃ��I�I", H_NORMAL, 8);
		com->text_insert("�^�C��", "�ӁA���͂������c�̘b�΂�������ȁB", H_NORMAL, 4);
		com->text_insert("�C�o���J��", "�́A�͂��I�H����Ȃ��ƂȂ����I�I", H_NORMAL, 8);
		com->text_insert("�^�C��", "���ہA���c�̘b�����Ă鎞����ԃe���V�����������ȁB", H_NORMAL, 4);
		com->text_insert("�C�o���J��", "���A���ݕ�����Ȃ��I�΂�������Ȃ��́I�H", H_NORMAL, 8);
		com->text_insert("�^�C��", "�ق�ȁA����ς�܂񂴂�ł��Ȃ��悤�Ɍ�����B", H_NORMAL, 4);
		com->text_insert("�~���g�l", "���͂́c�c", H_NORMAL, 6);
		com->text_insert("�C�o���J��", "���������A�������������I�I���񂽁A���c�̘b�͂��Ȃ��ŁI", H_NORMAL, 8);
		com->text_insert("�^�C��", "�������c�c�������猾���o�����񂾂�c�c", H_NORMAL, 4);
		com->text_insert("�C�o���J��", "������Ƌ�s��f���������B", H_NORMAL, 8);
		com->text_insert("�^�C��", "�ӂ�", H_NORMAL, 4);
		com->text_insert("�C�o���J��", "�\�\�\�\�����A�u�ӂ��v�Č����̂��֎~�I�I", H_NORMAL, 8);
		com->text_insert("�^�C��", "�킩�����B", H_NORMAL, 4);
		com->text_insert("�C�o���J��", "�c�c�c�c�c�c�c�c", H_NORMAL, 8);
		com->text_insert("�^�C��", "�c�c�c�c�c�c�c�c", H_NORMAL, 4);
		com->text_insert("�C�o���J��", "�����A���߁I�I�ق�̂��֎~�I�I", H_NORMAL, 8);
		com->text_insert("�^�C��", "�Ȃ�����Ȑe�̋w�̂悤�ɉ����玩�R��D���\�\", H_NORMAL, 4);
	}
	play_bgm(bgm[0], 1, 23);
	finish(23);
	text_play();
}

EventMission15::EventMission15(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/EMd.ogg");
	volume_bgm(1);
	set_rare_rate(35, 45, 15, 5);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 70, 30, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission15::play() {
	return after_text();
}
void EventMission15::text1() {
}
void EventMission15::text2() {
	if (com == NULL) {
		int x[10] = { 300,400,-100,-100,-100,-100,-100,-100,-100,500 };
		com = new Text(x, -100);
		com->text_insert("�g���`", "�ˁ[�ˁ[�A�Ԍ����ĂȂ�ł���Ȃɂ���ׂ�Ȃ��́H", H_NORMAL, 9);
		com->text_insert("�n���_", "�����A�̂͂������������������������ǂ��񂾂񌸂��Ă����񂾁B", H_NORMAL, 1);
		com->text_insert("�g���`", "�ց[�����Ȃ񂾁[�A�ˁ[�ǂ����ā[�H�ǂ����Ăǂ����ā[�H", H_NORMAL, 9);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�n���_", "�ʂɂ�������A���R�Ȃ�āB����������Ȃ񂾂�B", H_NORMAL, 1);
		com->text_insert("�g���`", "�l���m��H�ł͂Ȃ���ˁA���c�Ƃ�����ׂ�Ȃ��񂾂��B", H_NORMAL, 9);
		com->text_insert("�g���`", "���������Ă���ׂ�Ȃ����J�b�P�[�I�ȓz�H����a�H", H_NORMAL, 9);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�g���`", "����[�A�Ђ���Ƃ��ē{�����H", H_NORMAL, 9);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�n���_", "����A�{��Ȃ���B�{���Ă�̌������ƂȂ����B", H_NORMAL, 1);
		com->text_insert("�g���`", "�Ӂ[��A�����Ȃ̂��ȁ[�H�����Ȃ̂��ȁ[�H", H_NORMAL, 9);
		com->text_insert("�n���_", "�ȁA�ʂɓ{���ĂȂ񂩂Ȃ���ȁH", H_NORMAL, 1);
		com->text_insert("�A�J�c�L", "�c�c�c�c�c�c�c�c", H_NORMAL, 0);
		com->text_insert("�g���`", "�c�c�c�c�c�c�c�c", H_NORMAL, 9);
		com->text_insert("�n���_", "�c�c���A����c�c�c�c�c�c�H", H_NORMAL, 1);
	}
	play_bgm(bgm[0], 1, 16);
	finish(16);
	text_play();
}

EventMission16::EventMission16(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/EMc.ogg");
	volume_bgm(1);
	set_rare_rate(35, 45, 15, 5);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 70, 30, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission16::play() {
	return after_text();
}
void EventMission16::text1() {
}
void EventMission16::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,200,-100,-100,300,450,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�I���_", "���c�I�I�����͐�Ύ����Ă��Ă�ˁI�I", H_NORMAL, 5);
		com->text_insert("�I���_", "�O�Җʒk�̊�]���ԓ͂�I�o���ĂȂ��̑��c������I�I", H_NORMAL, 5);
		com->text_insert("�I�I�^", "���[�A�킩���Ă�A�킩���Ă���āI�I", H_NORMAL, 2);
		com->text_insert("�I���_", "����Ƃ������C�̂����Ɠ���ł�I�ق獡���Б��I�I", H_NORMAL, 5);
		com->text_insert("�I�I�^", "����A���������Ƃ��Ă�Ƃ������āI�I", H_NORMAL, 2);
		com->text_insert("�I���_", "�����ƌC�R�قǂ��Ă���������ق��������񂶂�Ȃ��H", H_NORMAL, 5);
		com->text_insert("�I�I�^", "��������A���̎��R���I�I", H_NORMAL, 2);
		com->text_insert("�I���_", "���Ɓ\�\", H_NORMAL, 5);
		com->text_insert("�I�I�^", "�������ق��Ă���I�I", H_NORMAL, 2);
		com->text_insert("�I���_", "���A�҂āI�I", H_NORMAL, 5);
		com->text_insert("�I���_", "�\�\������", H_NORMAL, 5);
		com->text_insert("�E�`���g", "�͂͂́c�c�A���a�c����͑��c�N�Ɍ������ł��ˁB", H_NORMAL, 6);
		com->text_insert("�I���_", "�����͂�������������̂�B�C�ɂȂ��Ďd���Ȃ���B", H_NORMAL, 5);
		com->text_insert("�E�`���g", "�ł��Ȃ񂾂��񂾂����đ��c�N���āA�����Ƃ����ł���ˁB", H_NORMAL, 6);
		com->text_insert("�I���_", "��������Ȃ�����������ǂˁc�c", H_NORMAL, 5);
	}
	play_bgm(bgm[0], 1, 14);
	move(9, 2, -300, 7);
	move(10, 5, 200, 6);
	finish(15);
	text_play();
}

EventMission17::EventMission17(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/EMa.ogg");
	volume_bgm(1);
	set_rare_rate(22, 60, 15, 8);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 30, 70, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission17::play() {
	return after_text();
}
void EventMission17::text1() {
}
void EventMission17::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,150,-100,300,-100,-100,450,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�I�I�^", "�����ȁA���{�B��`���Ă�����āB�T�l���N���Ăׂ��Ă��邳���񂾁B", H_NORMAL, 2);
		com->text_insert("�E�`���g", "�S�R������I�I���c�̗��݂Ȃ�Ȃ񂾂��ĕ�����I�I", H_NORMAL, 7);
		com->text_insert("�I�I�^", "�ցA�Ȃ񂾂�B����ȓ��ʈ����݂Ă��ȁc�c�Ƃ�邺�B�ł����ɂ͐S�Ɍ��߂��l���\�\", H_NORMAL, 2);
		com->text_insert("�E�`���g", "������񂾂�I���c�͎��Ɠ����u���������v������ˁI", H_NORMAL, 7);
		com->text_insert("�I�I�^", "���H", H_NORMAL, 2);
		com->text_insert("�^�C��", "�ӂ�", H_NORMAL, 4);
		com->text_insert("�E�`���g", "���c�͈�Ԃ̒��Ԃ��Ǝv���Ă��I�e�X�g���P�V�O�ʂ��炢�ł���H", H_NORMAL, 7);
		com->text_insert("�I�I�^", "�����܂ň����˂���I�P�T�O�ʂ��炢���I", H_NORMAL, 2);
		com->text_insert("�E�`���g", "���[�H�����Ȃ́[�H���Ⴀ�����ǂ������B", H_NORMAL, 7);
		com->text_insert("�I�I�^", "���͌��\�׋����Ă�񂾂��B�ꏏ�ɂ��Ȃ��łق������B", H_NORMAL, 2);
		com->text_insert("�E�`���g", "�������Ă���Ă��I", H_NORMAL, 7);
		com->text_insert("�I�I�^", "�ȂɁI�H�ŁA�ł����̕�������Ă�͂����I�I", H_NORMAL, 2);
		com->text_insert("�E�`���g", "���₢��A�����݂��Ď��A���Ȃ����Ă��[�H", H_NORMAL, 7);
		com->text_insert("�^�C��", "���Ȃ݂ɓ�l�͐�T�̒��ԃe�X�g�͉��ʂ������񂾁H", H_NORMAL, 4);
		com->text_insert("�I�I�^", "�c�c�c�c�c�c�c�c", H_NORMAL, 2);
		com->text_insert("�E�`���g", "�c�c�c�c�c�c�c�c", H_NORMAL, 7);
		com->text_insert("�^�C��", "��l�Ƃ����M�Ȃ��̂���c�c", H_NORMAL, 4);
	}
	play_bgm(bgm[0], 1, 17);
	finish(17);
	text_play();
}

EventMission18::EventMission18(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/EMe.ogg");
	volume_bgm(1);
	set_rare_rate(22, 60, 15, 8);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 30, 70, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission18::play() {
	return after_text();
}
void EventMission18::text1() {
}
void EventMission18::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,200,-100,-100,-100,300,400,-100 };
		com = new Text(x, -100);
		com->text_insert("�C�V", "����[�A���Γ��{����B�������X�s�[�f�B���ˁB", H_NORMAL, 3);
		com->text_insert("�E�`���g", "���ւցA���肪�Ƃ��I", H_NORMAL, 7);
		com->text_insert("�C�V", "���{����Ȃ玞�Ԃ��z����ꂻ������B", H_NORMAL, 3);
		com->text_insert("�E�`���g", "���A�ǂ��������ƁI�H�^�C���X���b�v�����Ⴄ�́I�H", H_NORMAL, 7);
		com->text_insert("�C�V", "���ΐ����_����l����Ɨ��_��s�\����Ȃ��񂾁B", H_NORMAL, 3);
		com->text_insert("�E�`���g", "���[�A���H", H_NORMAL, 7);
		com->text_insert("�C�o���J��", "�C�ɂ��Ȃ��ŁB�ށA�������w�̎��ƂŐ搶�̎G�k�𕷂��Ă��炠�̒��q�Ȃ̂�B", H_NORMAL, 8);
		com->text_insert("�C�V", "����͐������_�Ȃ񂾁B�ʂɉ^�����������Ԉ���Ă�킯����Ȃ��񂾂��ǁA����͂܂��V�������_�Ł\�\", H_NORMAL, 3);
		com->text_insert("�E�`���g", "�Ɏu�N�͊Ԉ�������ƌ����Ă���Ă��ƁH", H_NORMAL, 7);
		com->text_insert("�C�o���J��", "��������Ȃ����ǁA���Ƃŕ����������̐󂢒m���Ō���Ă�́B", H_NORMAL, 8);
		com->text_insert("�E�`���g", "�́[��c�c���A�悭�킩��Ȃ���B", H_NORMAL, 7);
		com->text_insert("�C�o���J��", "�������Ă킩��Ȃ���B", H_NORMAL, 8);
	}
	play_bgm(bgm[0], 1, 12);
	finish(12);
	text_play();
}

EventMission19::EventMission19(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/EMa.ogg");
	volume_bgm(1);
	set_rare_rate(22, 60, 15, 8);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 30, 70, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission19::play() {
	return after_text();
}
void EventMission19::text1() {
}
void EventMission19::text2() {
	if (com == NULL) {
		int x[10] = { 100,200,400,-100,-100,-100,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�n���_", "�ǂ������񂾂�A���c�B�Ȃ񂩃e���V�����Ⴂ�ȁH", H_NORMAL, 1);
		com->text_insert("�I�I�^", "�����̋��H�̃v������������H���Ȃ��������炾�c�c", H_NORMAL, 2);
		com->text_insert("�n���_", "�����A�������̃N���X���]�����񂾁B", H_NORMAL, 1);
		com->text_insert("�I�I�^", "�C�����ł͕����Ă˂��񂾂�B����񂯂񂶂�Ȃ��ĖʐڂȂ牴�̃v�����������B", H_NORMAL, 2);
		com->text_insert("�n���_", "�ʐڂŌ��߂�Ȃ�ĕ��������ƂȂ��ȁc�c", H_NORMAL, 1);
		com->text_insert("�I�I�^", "�Ȃ񂾂�A�T�l�̓v����������ꂽ�̂��H", H_NORMAL, 2);
		com->text_insert("�n���_", "���͂��������Q�����ĂȂ��B��ŏ\�����B", H_NORMAL, 1);
		com->text_insert("�I�I�^", "�͂��A��l�Ԃ�₪���āB�{���͗~���������ɂ您�B", H_NORMAL, 2);
		com->text_insert("�n���_", "����A����Ȃ��Ɓc�c�c�c", H_NORMAL, 1);
		com->text_insert("�I�I�^", "�͂��A�҂āA���������Ă��ꂪ���ʂȂ̂��c�c�c�c�H", H_NORMAL, 2);
		com->text_insert("�I�I�^", "�l���Ă݂�΁A���w���̂Ƃ����Q������l�������Ă�C�����邺�c�c", H_NORMAL, 2);
		com->text_insert("�I�I�^", "��������ȁA�T�l��������Ȃ��B�����Ɨ��炾���ĎQ�����Ă˂���ȁH", H_NORMAL, 2);
		com->text_insert("�A�J�c�L", "�����B", H_NORMAL, 0);
		com->text_insert("�I�I�^", "�ւ��A������l�ɂȂ�˂��ƂȁB���x����͂قǂقǂɂ��邺�B", H_NORMAL, 2);
		com->text_insert("�n���_", "�ʂɋC�ɂ��Ȃ��Ă����Ǝv�����ǂȁB", H_NORMAL, 1);
		com->text_insert("�n���_", "���̏ꍇ�́A���̑O�A�ǎq�����Ƀv��������Ă��ꂽ�񂾁B", H_NORMAL, 1);
		com->text_insert("�n���_", "������A�������H�̃v���������͓I�Ɋ����Ȃ��Ȃ����񂾁B", H_NORMAL, 1);
		com->text_insert("�I�I�^", "�́A���C�Ă񂶂�˂��I�I", H_NORMAL, 2);
	}
	play_bgm(bgm[0], 1, 18);
	finish(18);
	text_play();
}

EventMission20::EventMission20(MissionText& detail) :
	Mission(detail)
{
	bgm = new int[1];//bgm�m��
	bgm[0] = LoadSoundMem("bgm/EMd.ogg");
	volume_bgm(1);
	set_rare_rate(22, 60, 15, 8);//��V�̃J�[�h���A����
	size_set(9, 9, 9);
	int card_kind;
	Card c;
	for (int i = 0; i < 3; i++) {
		int card_sum = 5;//�搶��l�����J�[�h����
		for (int j = 0; j < card_sum; j++) {
			c.set_rare(0, 30, 70, 0);//�搶�̃J�[�h�̃��A�x
			card_kind = rand_te_card(teacher[i].get_kind());
			c.set_kind(card_kind);
			normal_status(card_kind, c);
			teacher[i].get_cards()->set_card(c, j);
		}
	}
}
int EventMission20::play() {
	return after_text();
}
void EventMission20::text1() {
}
void EventMission20::text2() {
	if (com == NULL) {
		int x[10] = { -100,-100,-100,200,300,450,-100,-100,-100,-100 };
		com = new Text(x, -100);
		com->text_insert("�I���_", "�˂��A�Ɏu�͂��̔��A�����Ő؂��Ă�́H", H_NORMAL, 5);
		com->text_insert("�C�V", "���A���A���������ǁc�c�c�c", H_NORMAL, 3);
		com->text_insert("�I���_", "�Ɏu�͊炢���񂾂���A����������Ƃ����Ƃ��ŎU�����Ȃ�B", H_NORMAL, 5);
		com->text_insert("�C�V", "���A����ʂɋ����Ȃ����c�c", H_NORMAL, 3);
		com->text_insert("�I���_", "�����Ƃ�����Ȃ��Ă��A�����Ƃ����A�I�[�����o�����߂ɁH", H_NORMAL, 5);
		com->text_insert("�C�V", "����A������A���������́c�c", H_NORMAL, 3);
		com->text_insert("�I���_", "�����Ǝ����������ăV���v���ňÂ��̂΂����ł���H", H_NORMAL, 5);
		com->text_insert("�I���_", "����������Ƃ�����ꂵ�Ă݂���H���E�ς���H", H_NORMAL, 5);
		com->text_insert("�C�V", "������A�ς��Ȃ��āc�c�c�c", H_NORMAL, 3);
		com->text_insert("�I���_", "�ق�A�䗅�͊��Ƃ����Ƃ��Ă邶��Ȃ��H", H_NORMAL, 5);
		com->text_insert("�^�C��", "�������ȁc�c���ƁA�ȁB", H_NORMAL, 4);
		com->text_insert("�C�V", "�����Ƃ��Ă���ĂȂ񂾂�I�܂�Ŗl�������Ƃ��ĂȂ��݂����ɁI�I", H_NORMAL, 3);
		com->text_insert("�I���_", "���߂񂲂߂�B�����������肶��Ȃ��́B", H_NORMAL, 5);
		com->text_insert("�C�V", "���������A�s������Ȃ���΂Ȃ񂾂��Ă�������I�H�l�̎��R���I�I", H_NORMAL, 3);
		com->text_insert("�^�C��", "�܂��܂��A�����������āB", H_NORMAL, 4);
		com->text_insert("�I���_", "�������A���R����B���߂�ˁA������Ƃ����A�h�o�C�X��B", H_NORMAL, 5);
		com->text_insert("�^�C��", "�������͋C���������Ƃ��ɂ���΂����񂶂�Ȃ����B", H_NORMAL, 4);
		com->text_insert("�C�V", "�������e�ɓ{���Ă�񂶂�Ȃ��I", H_NORMAL, 3);
		com->text_insert("�C�V", "�����l�́A������ꂵ�Ă�l����̓�̏ォ��ڐ������ʂقǌ����Ȃ񂾁I�I", H_NORMAL, 3);
		com->text_insert("�^�C��", "�c�c�c�c�c�c�c�c", H_NORMAL, 4);
		com->text_insert("�I���_", "�c�c�c�c�c�c�c�c�����\�\", H_NORMAL, 5);
		com->text_insert("�I���_", "�\�\���߂�A�Ȃ񂩁B���낢��ƁA���߂�B", H_NORMAL, 5);
	}
	play_bgm(bgm[0], 1, 22);
	finish(22);
	text_play();
}