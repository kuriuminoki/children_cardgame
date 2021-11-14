#include "DxLib.h"
#include "control.h"
#include"create.h"
#include"sequence.h"

Bsequence::Bsequence(Enemy te[3], int st_kind[3], bool st_fixed[3]) {
	bf = NULL;
	for (int i = 0; i < 3; i++) {//�󂯎������������
		teacher[i] = te[i];//�搶������
		kind[i] = st_kind[i];//�b��̃L����
		fixed[i] = st_fixed[i];//�Œ�L����
	}
	focus = 0;
	load_store(store);//�q�ɂ̃f�[�^�����[�h(�Z�[�u�f�[�^����)
	load_deck(st_deck);//�f�b�L�̃J�[�h�����[�h����(�Z�[�u�f�[�^����)
	load_item(item);//�A�C�e���q�ɂ̃f�[�^�����[�h(�Z�[�u�f�[�^����)
	item.load_cards();//�A�C�e���q�ɂ̃J�[�h�����[�h����
	for (int i = 0; i < 10; i++) {
		store[i].load_cards();//�q�ɂ̃J�[�h�̉摜�Ȃǂ����[�h����
		st_deck[i].load_cards();
	}
	//�摜�≹�����[�h
	s_kettei = LoadSoundMem("sound/����.wav");
	s_modoru = LoadSoundMem("sound/�L�����Z��.wav");
	//ChangeVolumeSoundMem(255 * 60 / 100, s_kettei);
	//ChangeVolumeSoundMem(255 * 60 / 100, s_modoru);
	load_stand(stand_handle);

	for (int i = 0; i < 3; i++) { //�L�����ƃf�b�L�̑傫���b��
		student[i].kind_set(kind[i]);
		student[i].get_cards()->set_size(9);
		*student[i].get_cards() = st_deck[kind[i]];//�Z�[�u���Ă������f�b�L���R�s�[
	}
	for (int i = 0; i < 3; i++) {//�f�b�L�̊e�J�[�h�̉摜�Ȃǂ����[�h
		student[i].get_cards()->load_cards();
	}
}

Bsequence::~Bsequence() {
	if (bf != NULL) {
		delete bf;
	}
	DeleteSoundMem(s_kettei);
	DeleteSoundMem(s_modoru);
}

//�ύX���ꂽ�f�b�L��K�p����
void Bsequence::st_deck_set() {
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (student[i].get_kind() != 0)
			*student[i].get_cards() = st_deck[student[i].get_kind() - 1];
	}
}

//�o�g����ɃA�C�e�������K�p����
void Bsequence::dec_item() {
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (student[i].get_kind() != 0)
			st_deck[student[i].get_kind() - 1] = after_deck[i];
	}
}

void Bsequence::st_kind_set(int another_kind[]) {
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		student[i].kind_set(kind[i]);
		another_kind[i] = kind[i];//�g���L������mission�ɂ������Ă�����
	}
}

const int CHARA_SELECT = 0;
const int DECK_CREATE = 1;
const int DECK_SELECT = 2;
const int BATTLE = 3;
const int CHANGE = 4;
int Bsequence::battle_loop(int another_kind[]) {
	static int now = CHARA_SELECT;
	int result;
	switch (now) {
	case CHARA_SELECT://�L�����N�^�[�I�����
		result = character_select(kind, stand_handle, fixed, s_kettei, s_modoru, focus);
		if (result == 1) {
			SetUseASyncLoadFlag(TRUE);//�񓯊������J�n
			st_kind_set(another_kind);
			st_deck_set();
			bf = new BattleField(student, teacher);//�o�g���t�B�[���h��p��
			now = BATTLE;
			save_store(store);//�Z�[�u
			save_deck(st_deck);//�Z�[�u
			save_item(item);//�Z�[�u
		}
		else if (result == 3) {
			now = DECK_CREATE;
		}
		else if (result == -1) {
			save_store(store);//�Z�[�u
			save_deck(st_deck);//�Z�[�u
			save_item(item);//�Z�[�u
			return CANCEL;
		}
		break;
	case DECK_CREATE://�f�b�L�ҏW���
		result = deck_create(st_deck[focus - 1], store[focus - 1], item, s_kettei);
		if (result == 1) {
			now = CHARA_SELECT;
		}
		break;
	case BATTLE://�o�g�����
		result = bf->test(after_deck);
		if (result == INTERVAL) {
			dec_item();//�A�C�e������
			now = DECK_SELECT;
			bf->get_remain_hp(remain_hp);
			SetUseASyncLoadFlag(FALSE);//�񓯊������I��
		}
		else if (result == WIN || result == LOSE) {
			dec_item();//�A�C�e������
			now = CHARA_SELECT;
			SetUseASyncLoadFlag(FALSE);//�񓯊������I��
			return result;//����
		}
		break;
	case DECK_SELECT://�C���^�[�o���ōĕҏW����f�b�L�̑I�����
		bf->play_music(true);
		result = deck_select(kind, stand_handle, s_kettei, focus);
		for (int i = 0; i < 3; i++) {
			if (remain_hp[i] <= 0) { continue; }
			DrawFormatString(85 + i*180, 380, GetColor(0, 0, 0), "�c��HP�F%d", remain_hp[i]);
		}
		if (result == 1) {
			st_deck_set();
			for (int i = 0; i < MAX_WITH_BATTLE; i++) {
				bf->deck_reflesh(*student[i].get_cards(), i);
			}
			now = BATTLE;
			save_store(store);//�Z�[�u
			save_deck(st_deck);//�Z�[�u
			save_item(item);//�Z�[�u
		}
		else if (result == 2) {
			now = CHANGE;
		}
		break;
	case CHANGE://�C���^�[�o���ł̃f�b�L�ҏW���
		result = deck_create(st_deck[focus - 1], store[focus - 1], item, s_kettei);
		if (result == 1) {
			now = DECK_SELECT;
		}
		break;
	}

	return 0;
}

void Bsequence::battle_end(int kind, Card& c) {
	int last;
	store[kind - 1].get_last(last);//�q�ɂ̖����𒲂ׂ�
	store[kind - 1].set_card(c, last);//�q�ɂ̖����֑}��
	save_store(store);//�Z�[�u
	save_deck(st_deck);//�Z�[�u
}