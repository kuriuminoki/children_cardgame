#include"battle.h"
#include"DxLib.h"
#include"control.h"

#define FINISH_COUNT 180
#define MUSIC_START 3//�����E���h�ڂ��特�y�n�܂邩

int BattleField::test(Deck after_deck[3]) {
	if (!load_flag && GetASyncLoadNum() == 0) {
		ChangeVolumeSoundMem(255 * 55 / 100, trigger_handle1);
		ChangeVolumeSoundMem(255 * 60 / 100, trigger_handle2);
		ChangeVolumeSoundMem(255 * 35 / 100, target_sound);
		ChangeVolumeSoundMem(255 * 55 / 100, target_cancel);
		for (int i = 0; i < 18; i++) { ChangeVolumeSoundMem(255 * 60 / 100, skill_sound_handle[i]); }
		ChangeVolumeSoundMem(255 * 45 / 100, skill_sound_handle[0]);
		ChangeVolumeSoundMem(255 * 50 / 100, skill_sound_handle[7]);
		ChangeVolumeSoundMem(255 * 100 / 100, skill_sound_handle[14]);
		ChangeVolumeSoundMem(255 * 60 / 100, music);
		ChangeVolumeSoundMem(255 * 65 / 100, bgm);
		load_flag = true;
	}
	play_music(false);
	target_decide();//�^�[�Q�b�e�B���O
	target_hpzero();//HP���[���ɂȂ�����^�[�Q�b�g����
	queue_trigger();//�J�[�h�����ɂ��ꎞ��~�ւ̐؂�ւ�
	if (flag) {//�o�g���i�s��
		for (int i = 0; i < MAX_WITH_BATTLE; i++) {
			if (student[i].get_kind()) {
				student[i].skill_trigger(student, teacher, target, skill_sound_handle);//�X�L���̌��ʓK�p
			}
			if (teacher[i].get_kind()) {
				teacher[i].skill_trigger(student, teacher, -1, skill_sound_handle);//�X�L���̌��ʓK�p
			}
			target_hpzero();//HP���[���ɂȂ�����^�[�Q�b�g����
		}
		for (int i = 0; i < MAX_WITH_BATTLE; i++) {
			if (student[i].get_kind()) {
				student[i].seek_hp();//php��hp�ɋ߂Â���
				student[i].effect_counting();//�G�t�F�N�g
				student[i].state_cnt();//��Ԉُ�Ȃǂ̃J�E���g
				student[i].counting(wait_num);//�ҋ@
			}
			if (teacher[i].get_kind()) {
				teacher[i].seek_hp();
				teacher[i].effect_counting();
				teacher[i].state_cnt();
				teacher[i].counting(wait_num);//�ҋ@
			}
		}
	}
	else {//�ꎞ��~��

	}
	//�`��t�F�[�Y
	DrawRotaGraph(0, 0, 1, 0, haikei, TRUE);
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		int k;
		if (student[i].get_kind()) {
			student[i].picture();
			k = student[i].get_effect();
			student[i].picture_effect(effect_handle[0][k], effect_handle[1][k], effect_handle[2][k], effect_handle[3][k]);
			student[i].picture_damage(flag);
		}
		if (teacher[i].get_kind()) {
			teacher[i].picture();
			k = teacher[i].get_effect();
			teacher[i].picture_effect(effect_handle[0][k], effect_handle[1][k], effect_handle[2][k], effect_handle[3][k]);
			teacher[i].picture_damage(flag);
		}
	}
	picture_target();
	picture_bar();
	//DrawFormatString(0, 0, GetColor(255, 255, 255), "next = %d", student[0].next);
	//DrawFormatString(100, 0, GetColor(255, 255, 255), "now = %d", student[0].now);
	DrawLine(0, 232, 640, 232, GetColor(255, 255, 0));
	DrawLine(0, 233, 640, 233, GetColor(0, 0, 0));
	DrawLine(0, 234, 640, 234, GetColor(255, 255, 0));
	picture_card_trigger();//�J�[�h������� �I��������flag��true��
	return result(after_deck);
}

const int SKILL_SOUND_SUM = 18;
//�R���X�g���N�^
BattleField::BattleField(Friend* st, Enemy* te) {
	font_handle = CreateFontToHandle(NULL, 75, 7);//���������Ƃ��̃t�H���g
	flag = true;//�f�o�b�O�p��true�X�^�[�g
	target = -1;
	hand = -1;
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		student[i] = st[i];
		teacher[i] = te[i];
	}
	round = 1;//���E���h1����X�^�[�g
	//�w�i�̃��[�h
	haikei = LoadGraph("battle_picture/�w�i.png");
	//�J�[�h������ʂ̉�
	trigger_handle1 = LoadSoundMem("sound/�J�[�h����1.wav");
	//ChangeVolumeSoundMem(255 * 40 / 100, trigger_handle1);
	trigger_handle2 = LoadSoundMem("sound/�J�[�h����2.wav");
	//ChangeVolumeSoundMem(255 * 40 / 100, trigger_handle2);
	target_sound = LoadSoundMem("sound/�^�[�Q�b�e�B���O.wav");
	//ChangeVolumeSoundMem(255 * 20 / 100, target_sound);
	target_cancel = LoadSoundMem("sound/�^�[�Q�b�e�B���O�L�����Z��.wav");
	//ChangeVolumeSoundMem(255 * 50 / 100, target_cancel);
	//�o�t�f�o�t��Ԉُ�̃A�C�R��
	icon[0] = LoadGraph("battle_picture/�U���͏㏸.png");
	icon[1] = LoadGraph("battle_picture/�U���͉��~.png");
	icon[2] = LoadGraph("battle_picture/�h��͏㏸.png");
	icon[3] = LoadGraph("battle_picture/�h��͉��~.png");
	icon[4] = LoadGraph("battle_picture/�X�s�[�h�㏸.png");
	icon[5] = LoadGraph("battle_picture/�X�s�[�h���~.png");
	icon[6] = LoadGraph("battle_picture/��.png");
	icon[7] = LoadGraph("battle_picture/��.png");
	icon[8] = LoadGraph("battle_picture/���.png");
	icon[9] = LoadGraph("battle_picture/���.png");
	icon[10] = LoadGraph("battle_picture/����.png");
	icon[11] = LoadGraph("battle_picture/�X�e���X.png");
	icon[12] = LoadGraph("battle_picture/�J�E���^�[.png");
	icon[13] = LoadGraph("battle_picture/�䖝.png");
	icon[14] = LoadGraph("battle_picture/�\�h�ڎ�.png");
	//�X�L�������̌��ʉ�
	skill_sound_handle = new int[SKILL_SOUND_SUM];
	skill_sound_handle[0] = LoadSoundMem("sound/�a��.wav");
	skill_sound_handle[1] = LoadSoundMem("sound/�U���̓A�b�v.wav");
	skill_sound_handle[2] = LoadSoundMem("sound/�U���̓_�E��.wav");
	skill_sound_handle[3] = LoadSoundMem("sound/�h��̓A�b�v.wav");
	skill_sound_handle[4] = LoadSoundMem("sound/�h��̓_�E��.wav");
	skill_sound_handle[5] = LoadSoundMem("sound/�X�s�[�h�A�b�v.wav");
	skill_sound_handle[6] = LoadSoundMem("sound/�X�s�[�h�_�E��.wav");
	skill_sound_handle[7] = LoadSoundMem("sound/���.wav");
	skill_sound_handle[8] = LoadSoundMem("sound/��.wav");
	skill_sound_handle[9] = LoadSoundMem("sound/���.wav");
	skill_sound_handle[10] = LoadSoundMem("sound/���t�^.wav");
	skill_sound_handle[11] = LoadSoundMem("sound/�J�E���^�[.wav");
	skill_sound_handle[12] = LoadSoundMem("sound/�J�E���^�[�t�^.wav");
	skill_sound_handle[13] = LoadSoundMem("sound/�����t�^.wav");
	skill_sound_handle[14] = LoadSoundMem("sound/�X�e���X�t�^.wav");
	skill_sound_handle[15] = LoadSoundMem("sound/��Ԉُ��.wav");
	skill_sound_handle[16] = LoadSoundMem("sound/��.wav");
	skill_sound_handle[17] = LoadSoundMem("sound/�䖝�t�^.wav");
	for (int i = 0; i < 18; i++) { ChangeVolumeSoundMem(255 * 50 / 100, skill_sound_handle[i]); }
	/*ChangeVolumeSoundMem(255 * 40 / 100, skill_sound_handle[0]);
	ChangeVolumeSoundMem(255 * 45 / 100, skill_sound_handle[7]);
	ChangeVolumeSoundMem(255 * 100 / 100, skill_sound_handle[14]);*/
	//�G�t�F�N�g�̃A�j���[�V����
	for (int i = 0; i < EFFECT_NUM; i++)effect_handle[i] = new int[17];
	effect_handle[0][0] = LoadGraph("battle_picture/�I�����W1.png");
	effect_handle[1][0] = LoadGraph("battle_picture/�I�����W2.png");
	effect_handle[2][0] = LoadGraph("battle_picture/�I�����W3.png");
	effect_handle[3][0] = LoadGraph("battle_picture/�I�����W4.png");
	effect_handle[0][1] = LoadGraph("battle_picture/�U���͏㏸1.png");
	effect_handle[1][1] = LoadGraph("battle_picture/�U���͏㏸2.png");
	effect_handle[2][1] = LoadGraph("battle_picture/�U���͏㏸3.png");
	effect_handle[3][1] = LoadGraph("battle_picture/�U���͏㏸4.png");
	effect_handle[0][2] = LoadGraph("battle_picture/�U���͌���1.png");
	effect_handle[1][2] = LoadGraph("battle_picture/�U���͌���2.png");
	effect_handle[2][2] = LoadGraph("battle_picture/�U���͌���3.png");
	effect_handle[3][2] = LoadGraph("battle_picture/�U���͌���4.png");
	effect_handle[0][3] = LoadGraph("battle_picture/�h��͏㏸1.png");
	effect_handle[1][3] = LoadGraph("battle_picture/�h��͏㏸2.png");
	effect_handle[2][3] = LoadGraph("battle_picture/�h��͏㏸3.png");
	effect_handle[3][3] = LoadGraph("battle_picture/�h��͏㏸4.png");
	effect_handle[0][4] = LoadGraph("battle_picture/�h��͌���1.png");
	effect_handle[1][4] = LoadGraph("battle_picture/�h��͌���2.png");
	effect_handle[2][4] = LoadGraph("battle_picture/�h��͌���3.png");
	effect_handle[3][4] = LoadGraph("battle_picture/�h��͌���4.png");
	effect_handle[0][5] = LoadGraph("battle_picture/�X�s�[�h�㏸1.png");
	effect_handle[1][5] = LoadGraph("battle_picture/�X�s�[�h�㏸2.png");
	effect_handle[2][5] = LoadGraph("battle_picture/�X�s�[�h�㏸3.png");
	effect_handle[3][5] = LoadGraph("battle_picture/�X�s�[�h�㏸4.png");
	effect_handle[0][6] = LoadGraph("battle_picture/�X�s�[�h����1.png");
	effect_handle[1][6] = LoadGraph("battle_picture/�X�s�[�h����2.png");
	effect_handle[2][6] = LoadGraph("battle_picture/�X�s�[�h����3.png");
	effect_handle[3][6] = LoadGraph("battle_picture/�X�s�[�h����4.png");
	effect_handle[0][7] = LoadGraph("battle_picture/��1.png");
	effect_handle[1][7] = LoadGraph("battle_picture/��2.png");
	effect_handle[2][7] = LoadGraph("battle_picture/��3.png");
	effect_handle[3][7] = LoadGraph("battle_picture/��4.png");
	effect_handle[0][8] = LoadGraph("battle_picture/���1.png");
	effect_handle[1][8] = LoadGraph("battle_picture/���2.png");
	effect_handle[2][8] = LoadGraph("battle_picture/���3.png");
	effect_handle[3][8] = LoadGraph("battle_picture/���4.png");
	effect_handle[0][9] = LoadGraph("battle_picture/�J�E���^�[�q�b�g1.png");
	effect_handle[1][9] = LoadGraph("battle_picture/�J�E���^�[�q�b�g2.png");
	effect_handle[2][9] = LoadGraph("battle_picture/�J�E���^�[�q�b�g3.png");
	effect_handle[3][9] = LoadGraph("battle_picture/�J�E���^�[�q�b�g4.png");
	effect_handle[0][10] = LoadGraph("battle_picture/�J�E���^�[1.png");
	effect_handle[1][10] = LoadGraph("battle_picture/�J�E���^�[2.png");
	effect_handle[2][10] = LoadGraph("battle_picture/�J�E���^�[3.png");
	effect_handle[3][10] = LoadGraph("battle_picture/�J�E���^�[4.png");
	effect_handle[0][11] = LoadGraph("battle_picture/����1.png");
	effect_handle[1][11] = LoadGraph("battle_picture/����2.png");
	effect_handle[2][11] = LoadGraph("battle_picture/����3.png");
	effect_handle[3][11] = LoadGraph("battle_picture/����4.png");
	effect_handle[0][12] = LoadGraph("battle_picture/�X�e���X1.png");
	effect_handle[1][12] = LoadGraph("battle_picture/�X�e���X2.png");
	effect_handle[2][12] = LoadGraph("battle_picture/�X�e���X3.png");
	effect_handle[3][12] = LoadGraph("battle_picture/�X�e���X4.png");
	effect_handle[0][13] = LoadGraph("battle_picture/�䖝1.png");
	effect_handle[1][13] = LoadGraph("battle_picture/�䖝2.png");
	effect_handle[2][13] = LoadGraph("battle_picture/�䖝3.png");
	effect_handle[3][13] = LoadGraph("battle_picture/�䖝4.png");
	effect_handle[0][14] = LoadGraph("battle_picture/���1.png");
	effect_handle[1][14] = LoadGraph("battle_picture/���2.png");
	effect_handle[2][14] = effect_handle[0][14];
	effect_handle[3][14] = effect_handle[1][14];
	effect_handle[0][15] = LoadGraph("battle_picture/��𐬌�1.png");
	effect_handle[1][15] = LoadGraph("battle_picture/��𐬌�2.png");
	effect_handle[2][15] = LoadGraph("battle_picture/��𐬌�3.png");
	effect_handle[3][15] = LoadGraph("battle_picture/��𐬌�4.png");
	effect_handle[0][16] = LoadGraph("battle_picture/��Ԉُ��1.png");
	effect_handle[1][16] = LoadGraph("battle_picture/��Ԉُ��2.png");
	effect_handle[2][16] = LoadGraph("battle_picture/��Ԉُ��3.png");
	effect_handle[3][16] = LoadGraph("battle_picture/��Ԉُ��4.png");
	switch (GetRand(4)) {
	case 0:
		music = LoadSoundMem("music/12345_full.mp3");
		break;
	case 1:
		music = LoadSoundMem("music/kobune_full.mp3");
		break;
	case 2:
		music = LoadSoundMem("music/noapusa_full.mp3");
		break;
	case 3:
		music = LoadSoundMem("music/star_full.mp3");
		break;
	case 4:
		music = LoadSoundMem("music/zinnia_full.mp3");
		break;
	}
	ChangeVolumeSoundMem(255 * 60 / 100, music);
	switch (GetRand(4)) {
	case 0:
		bgm = LoadSoundMem("music/event33.ogg");
		break;
	case 1:
		bgm = LoadSoundMem("music/event34.ogg");
		break;
	case 2:
		bgm = LoadSoundMem("music/event42.ogg");
		break;
	case 3:
		bgm = LoadSoundMem("music/event46.ogg");
		break;
	case 4:
		bgm = LoadSoundMem("music/event7.ogg");
		break;
	}
	ChangeVolumeSoundMem(255 * 65 / 100, bgm);
}

BattleField::~BattleField() {
	DeleteFontToHandle(font_handle);//�t�H���g�f�[�^�̍폜
	for (int i = 0; i < SKILL_SOUND_SUM; i++) {
		DeleteSoundMem(skill_sound_handle[i]);
	}
	delete[] skill_sound_handle;
	for (int i = 0; i < EFFECT_NUM; i++)delete[] effect_handle[i];
	DeleteSoundMem(trigger_handle1);
	DeleteSoundMem(trigger_handle2);
	DeleteSoundMem(target_sound);
	DeleteSoundMem(target_cancel);
}

//�C���^�[�o�������̃f�b�L�X�V
void BattleField::deck_reflesh(Deck& new_deck, int index) {
	*student[index].get_cards() = new_deck;
}

//�C���^�[�o���ɓ���O
void BattleField::reset() {
	flag = true;
	finish_cnt = 0;
	target = -1;
	round++;
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		student[i].reset();
		teacher[i].reset();
	}
	if (round == MUSIC_START) {
		StopSoundMem(bgm);
	}
}

//���k�̎c��hp��������
void BattleField::get_remain_hp(int remain_hp[]) {
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		remain_hp[i] = student[i].get_hp();
	}
}

//���y�𗬂�
void BattleField::play_music(bool interval_flag) {
	if (round < MUSIC_START) {
		if (CheckSoundMem(bgm) == 0) {
			PlaySoundMem(bgm, DX_PLAYTYPE_BACK);
		}
	}
	else if(!interval_flag || round > MUSIC_START){
		if (CheckSoundMem(music) == 0) {
			PlaySoundMem(music, DX_PLAYTYPE_BACK);
		}
	}
}

//�I���E�C���^�[�o���ֈڍs���Ă��������ǂ���
bool BattleField::finish_ok() {
	bool ok = true;
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (student[i].get_wait() > 0 || teacher[i].get_wait() > 0) {
			ok = false; break;
		}
	}
	return (ok && wait_num.empty() && hand == -1);
}

int BattleField::result(Deck after_deck[3]) {
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (student[i].get_hp() > 0) {
			break;
		}
		if (i == MAX_WITH_BATTLE - 1) {
			flag = false; finish_cnt++;
			DrawBox(0, 130, 640, 350, GetColor(100, 100, 100), TRUE);
			if(finish_cnt <= 60)DrawStringToHandle(640 - finish_cnt * 7, 200, "LOSE", GetColor(0, 0, 255), font_handle);
			else DrawStringToHandle(220, 200, "LOSE", GetColor(0, 0, 255), font_handle);
			if (finish_cnt >= FINISH_COUNT) {
				for (int i = 0; i < 3; i++) { after_deck[i] = *student[i].get_cards(); }
				StopSoundMem(bgm);
				StopSoundMem(music);
				return LOSE;//�s�k
			}
		}
	}
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (teacher[i].get_hp() > 0) {
			break;
		}
		if (i == MAX_WITH_BATTLE - 1) {
			flag = false; finish_cnt++;
			DrawBox(0, 130, 640, 350, GetColor(100, 100, 100), TRUE);
			if (finish_cnt <= 60)DrawStringToHandle(640 - finish_cnt * 7, 200, "WIN", GetColor(255, 0, 0), font_handle);
			else DrawStringToHandle(220, 200, "WIN", GetColor(255, 0, 0), font_handle);
			if (finish_cnt >= FINISH_COUNT) {
				for (int i = 0; i < 3; i++) { after_deck[i] = *student[i].get_cards(); }
				StopSoundMem(bgm);
				StopSoundMem(music);
				return WIN;//����
			}
		}
	}
	if (!finish_ok())return CONTINUE;
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		int n = student[i].get_now();
		int s = student[i].get_cards()->get_size();
		int k = student[i].get_kind();
		if (n <= s && k != 0) {
			if (student[i].get_hp() > 0) {
				return CONTINUE;
			}
		}
		int n2 = teacher[i].get_now();
		int s2 = teacher[i].get_cards()->get_size();
		int k2 = teacher[i].get_kind();
		if (n2 <= s2 && k2 != 0) {
			if (teacher[i].get_hp() > 0) {
				return CONTINUE;
			}
		}
	}
	flag = false; finish_cnt++;
	DrawBox(0, 130, 640, 350, GetColor(100,100,100),TRUE);
	if (finish_cnt <= 60)DrawStringToHandle(640 - finish_cnt * 8, 200, "INTERVAL", GetColor(255, 0, 255), font_handle);
	else DrawStringToHandle(160, 200, "INTERVAL", GetColor(255, 0, 255), font_handle);
	if (finish_cnt >= FINISH_COUNT) {
		reset();
		for (int i = 0; i < 3; i++) { after_deck[i] = *student[i].get_cards(); }
		return INTERVAL;//�C���^�[�o����
	}
	return CONTINUE;
}

//�̗̓o�[�Ƒҋ@���ԃo�[�A�A�C�R����\��
void BattleField::picture_bar() {
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (student[i].get_kind()) {
			student[i].picture_hp();
			student[i].picture_wait();
			student[i].picture_state();
			student[i].picture_icon(icon);
		}
		if (teacher[i].get_kind()) {
			teacher[i].picture_hp();
			teacher[i].picture_wait();
			teacher[i].picture_state();
			teacher[i].picture_icon(icon);
		}
	}
}

void BattleField::picture_card_trigger() {
	if (!flag) {
		if (hand != -1) {//�L�������J�[�h�𔭓�
			if (hand < MAX_WITH_BATTLE) {//student������
				if (student[hand].picture_trigger(trigger_handle2)) {
					flag = true;
					hand = -1;
				}
			}
			else {//�搶������
				if (teacher[hand - MAX_WITH_BATTLE].picture_trigger(trigger_handle2)) {
					flag = true;
					hand = -1;
				}
			}
		}
	}
}

void BattleField::queue_trigger() {
	if(!wait_num.empty() && flag) {//�L���[������ɂȂ�܂ŃJ�[�h����
		flag = false;//�ꎞ��~
		hand = wait_num.front();//�L���[������o��
		wait_num.pop();
		PlaySoundMem(trigger_handle1, DX_PLAYTYPE_BACK);
	}
}

//�N���b�N�Ń^�[�Q�b�e�B���O
void BattleField::target_decide() {
	int m = left_click();
	int a = -1;
	//�����G�ƃJ�[�\�����d�Ȃ��Ă���Ȃ�a == num�ƂȂ�͂�
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		int x = teacher[i].mouse_hit();
		if (a == -1)a = x;
	}
	//���N���b�N�������ꂽ�u��
	if (m == 1) { 
		if (a == target || a == -1) {
			if (target == -1)return;
			target = -1;//���݃^�[�Q�b�e�B���O���Ă���G�Ȃ����
			PlaySoundMem(target_cancel, DX_PLAYTYPE_BACK);//��
		}
		else {
			target = a;//�V���ɓG���^�[�Q�b�e�B���O
			PlaySoundMem(target_sound, DX_PLAYTYPE_BACK);//��
		}
	}
}

void BattleField::target_hpzero() {
	if (target != -1 && teacher[target].get_hp() <= 0)target = -1;//����ł���Ȃ����
}

void BattleField::picture_target() {
	if (target == -1)return;
	teacher[target].picture_red_circle();
}