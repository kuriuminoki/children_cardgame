#include"create.h"
#include"menu.h"
#include"control.h"
#include"DxLib.h"
#include<stdio.h>

#define MAX_PICTURE_CARD 10//���ʂŏc�ɕ\���ł���J�[�h�̍ő吔

void set_swap(int indicate, Card*& c, Deck* deck, Deck* store, int sound);
void swap_card(Card* first, Card* second);
bool swap_able(bool first_deck, bool second_deck, bool item_flag, Card& first, Card& second);
void card_text(char c[], const char* str);
void up_down(int hand_x, int hand_y, int& indicate, bool up, bool down);
void insert_drop(int hand_x, int hand_y, int& indicate, int num, Deck& deck, Deck& store, int deck_last, int store_last);
void cards_bring(Deck& cards, int erase_index);
int deck_item_create(Deck& deck, Deck& store, int sound, bool item_flag);

bool save_other(int* day, bool clear[], int today_mission[]) {
	FILE *fp = fopen("�Z�[�u�f�[�^/other.dat", "wb");
	if (fp == NULL) {
		return false;
	}
	int* m_total = new int;
	*m_total = M_TOTAL;
	fwrite(day, sizeof(int), 1, fp);
	fwrite(m_total, sizeof(int), 1, fp);
	fwrite(clear, sizeof(bool), M_TOTAL, fp);
	fwrite(today_mission, sizeof(int), TODAY_MAX, fp);
	fclose(fp);
	delete m_total;
	return true;
}

bool save_money(int* money) {
	FILE *fp = fopen("�Z�[�u�f�[�^/money.dat", "wb");
	if (fp == NULL) {
		return false;
	}
	fwrite(money, sizeof(int), 1, fp);
	fclose(fp);
	return true;
}

bool save_deck(Deck* deck) {
	FILE *fp = fopen("�Z�[�u�f�[�^/deck.dat", "wb");
	if (fp == NULL) {
		return false;
	}
	for (int i = 0; i < 10; i++) {
		fwrite(deck[i].get_cards(), sizeof(Card), DECK_MAX, fp);
	}
	fclose(fp);
	return true;
}

bool save_store(Deck* store) {
	FILE *fp = fopen("�Z�[�u�f�[�^/store.dat", "wb");
	if (fp == NULL) {
		return false;
	}
	for (int i = 0; i < 10; i++) {
		fwrite(store[i].get_cards(), sizeof(Card), STORE_MAX, fp);
	}
	fclose(fp);
	return true;
}

bool save_item(Deck& item) {
	FILE *fp = fopen("�Z�[�u�f�[�^/item.dat", "wb");
	if (fp == NULL) {
		return false;
	}
	fwrite(item.get_cards(), sizeof(Card), STORE_MAX, fp);
	fclose(fp);
	return true;
}

//�����Ƃ��~�b�V�������[�h
bool load_money(int* money) {
	FILE *fp = fopen("�Z�[�u�f�[�^/money.dat", "rb");
	if (fp == NULL) {
		return false;
	}
	fread(money, sizeof(int), 1, fp);
	fclose(fp);
	return true;
}

bool load_other(int* day, bool clear[], int today_mission[]) {
	FILE *fp = fopen("�Z�[�u�f�[�^/other.dat", "rb");
	if (fp == NULL) {
		return false;
	}
	int* m_total = new int;
	*m_total = M_TOTAL;
	fread(day, sizeof(int), 1, fp);
	fread(m_total, sizeof(int), 1, fp);
	fread(clear, sizeof(bool), *m_total, fp);
	fread(today_mission, sizeof(int), TODAY_MAX, fp);
	fclose(fp);
	for (int i = *m_total; i < M_TOTAL; i++) {
		clear[i] = false;
	}
	delete m_total;
	return true;
}

//�A�C�e���q�Ƀ��[�h
bool load_item(Deck& item) {
	item.set_size(STORE_MAX);
	FILE *fp = fopen("�Z�[�u�f�[�^/item.dat", "rb");
	if (fp == NULL) {
		return false;
	}
	Card* c = new Card[STORE_MAX];
	fread(c, sizeof(Card), STORE_MAX, fp);
	for (int k = 0; k < STORE_MAX; k++) {
		item.set_card(c[k], k);
		item.get_cards()[k].null_ability();//
		c[k].null_ability();//
	}
	delete[] c;
	fclose(fp);
	//item.set_card(Card(-1, 0, 0, 20, 30, 30), 0);//�e�X�g�p
	//item.set_card(Card(-2, 0, 0, 22, 30, 30), 1);//�e�X�g�p
	//item.set_card(Card(-3, 0, 0, 25, 30, 30), 2);//�e�X�g�p
	return true;
}
//�f�b�L�̃J�[�h���[�h
bool load_deck(Deck* deck) {
	for (int i = 0; i < 10; i++) {
		deck[i].set_size(DECK_MAX);
	}
	FILE *fp = fopen("�Z�[�u�f�[�^/deck.dat", "rb");
	if (fp == NULL) {
		return false;
	}
	for (int i = 0; i < 10; i++) {
		Card* c = new Card[DECK_MAX];
		fread(c, sizeof(Card), DECK_MAX, fp);
		for (int k = 0; k < DECK_MAX; k++) {
			c[k].update();
			deck[i].set_card(c[k], k);
			deck[i].get_cards()[k].null_ability();//
			c[k].null_ability();//
		}
		delete[] c;
	}
	fclose(fp);
	////�󔒂�~���l�߂�
	//for (int i = 0; i < 10; i++) {
	//	for (int k = 0; k < DECK_MAX - 1; k++) {
	//		if (deck[i].get_cards()[k].get_kind() != 0) {
	//			continue;
	//		}
	//		for (int j = k + 1; j < DECK_MAX; j++) {
	//			deck[i].get_cards()[j - 1] = deck[i].get_cards()[j];
	//			deck[i].get_cards()[j].set_kind(0);
	//		}
	//	}
	//}
	return true;
}
//�q�ɂ̃J�[�h��10�l�����[�h����
bool load_store(Deck* store) {
	for (int i = 0; i < 10; i++) {
		store[i].set_size(STORE_MAX);
	}
	FILE *fp = fopen("�Z�[�u�f�[�^/store.dat", "rb");
	if (fp == NULL) {
		return false;
	}
	for (int i = 0; i < 10; i++) {
		Card* c = new Card[STORE_MAX];
		fread(c, sizeof(Card), STORE_MAX, fp);
		for (int k = 0; k < STORE_MAX; k++) {
			c[k].update();
			store[i].set_card(c[k], k);
			store[i].get_cards()[k].null_ability();//null_ability���ĂԂ̂͂�������
			c[k].null_ability();////null_ability���ĂԂ̂͂�������
		}
		delete[] c;
	}
	fclose(fp);
	return true;
	//�ȉ��e�X�g
	for (int i = 0; i < 10; i++) {
		store[i].set_size(STORE_MAX);
	}
	store[0].set_card(Card(1, 0, 100, 30, 150, 173), 0);
	store[0].set_card(Card(1, 0, 100, 30, 150, 173), 1);
	store[0].set_card(Card(11, 0, 100, 55, 143, 165), 2);
	store[0].set_card(Card(11, 0, 100, 55, 143, 165), 3);
	store[0].set_card(Card(21, 0, 100, 27, 135, 165), 4);
	store[0].set_card(Card(21, 0, 100, 27, 135, 165), 5);

	store[1].set_card(Card(2, 0, 100, 50, 90, 105), 0);
	store[1].set_card(Card(2, 0, 100, 50, 90, 105), 1);
	store[1].set_card(Card(12, 0, 100, 20, 150, 180), 2);
	store[1].set_card(Card(22, 0, 100, 50, 150, 188), 3);
	store[1].set_card(Card(22, 0, 100, 50, 150, 188), 4);
	store[1].set_card(Card(22, 0, 100, 50, 150, 188), 5);

	store[2].set_card(Card(3, 0, 110, 70, 150, 180), 0);
	store[2].set_card(Card(3, 0, 110, 70, 150, 180), 1);
	store[2].set_card(Card(13, 0, 110, 240, 75, 45), 2);
	store[2].set_card(Card(13, 0, 110, 240, 75, 45), 3);
	store[2].set_card(Card(23, 0, 110, 90, 188, 210), 4);
	store[2].set_card(Card(23, 0, 110, 90, 188, 210), 5);

	store[3].set_card(Card(4, 0, 100, 240, 150, 225), 0);
	store[3].set_card(Card(4, 0, 100, 240, 150, 225), 1);
	store[3].set_card(Card(14, 0, 100, 15, 135, 180), 2);
	store[3].set_card(Card(14, 0, 100, 15, 135, 180), 3);
	store[3].set_card(Card(24, 0, 100, 25, 128, 135), 4);
	store[3].set_card(Card(24, 0, 100, 25, 128, 135), 5);

	store[4].set_card(Card(5, 0, 97, 50, 98, 128), 0);
	store[4].set_card(Card(5, 0, 97, 50, 98, 128), 1);
	store[4].set_card(Card(15, 0, 100, 240, 60, 135), 2);
	store[4].set_card(Card(25, 0, 115, 65, 165, 195), 3);
	store[4].set_card(Card(25, 0, 115, 65, 165, 195), 4);
	store[4].set_card(Card(25, 0, 115, 65, 165, 195), 5);

	store[5].set_card(Card(6, 0, 100, 30, 180, 225), 0);
	store[5].set_card(Card(6, 0, 100, 30, 180, 225), 1);
	store[5].set_card(Card(16, 0, 100, 240, 90, 90), 2);
	store[5].set_card(Card(26, 0, 100, 60, 135, 135), 3);
	store[5].set_card(Card(26, 0, 100, 60, 135, 135), 4);
	store[5].set_card(Card(26, 0, 100, 60, 135, 135), 5);

	store[6].set_card(Card(7, 0, 120, 40, 90, 105), 0);
	store[6].set_card(Card(7, 0, 120, 40, 90, 105), 1);
	store[6].set_card(Card(17, 0, 120, 180, 105, 135), 2);
	store[6].set_card(Card(27, 0, 120, 50, 170, 188), 3);
	store[6].set_card(Card(27, 0, 120, 50, 170, 188), 4);
	store[6].set_card(Card(27, 0, 120, 50, 170, 188), 5);

	store[7].set_card(Card(8, 0, 100, 33, 75, 90), 0);
	store[7].set_card(Card(8, 0, 100, 33, 75, 90), 1);
	store[7].set_card(Card(18, 0, 100, 45, 38, 120), 2);
	store[7].set_card(Card(18, 0, 100, 45, 38, 120), 3);
	store[7].set_card(Card(28, 0, 100, 50, 75, 125), 4);
	store[7].set_card(Card(28, 0, 100, 50, 75, 125), 5);

	store[8].set_card(Card(9, 0, 95, 180, 75, 48), 0);
	store[8].set_card(Card(19, 0,100, 240, 95, 105), 1);
	store[8].set_card(Card(29, 0, 90, 60, 105, 150), 2);
	store[8].set_card(Card(29, 0, 90, 60, 105, 150), 3);
	store[8].set_card(Card(29, 0, 90, 60, 105, 150), 4);
	store[8].set_card(Card(29, 0, 90, 60, 105, 150), 5);

	store[9].set_card(Card(10, 0, 105, 50, 95, 128), 0);
	store[9].set_card(Card(10, 0, 105, 50, 95, 128), 1);
	store[9].set_card(Card(20, 0, 105, 65, 143, 158), 2);
	store[9].set_card(Card(20, 0, 105, 65, 143, 158), 3);
	store[9].set_card(Card(30, 0, 105, 55, 128, 143), 4);
	store[9].set_card(Card(30, 0, 105, 55, 128, 143), 5);
	return true;
}

//�����G�����[�h(�f�t�H���g�̉摜)
void load_stand(int stand_handle[10]) {
	stand_handle[0] = LoadGraph("stand/�A�J�c�L.png");
	stand_handle[1] = LoadGraph("stand/�n���_.png");
	stand_handle[2] = LoadGraph("stand/�I�I�^.png");
	stand_handle[3] = LoadGraph("stand/�C�V.png");
	stand_handle[4] = LoadGraph("stand/�^�C��.png");
	stand_handle[5] = LoadGraph("stand/�I���_.png");
	stand_handle[6] = LoadGraph("stand/�~���g�l.png");
	stand_handle[7] = LoadGraph("stand/�E�`���g.png");
	stand_handle[8] = LoadGraph("stand/�C�o���J��.png");
	stand_handle[9] = LoadGraph("stand/�g���`.png");
	//�e�X�g
	//for (int i = 0; i < 10; i++) { stand_handle[i] = LoadGraph("stand/�g���`icon.png"); }
}

//�f�b�L�A�q�ɁA�����G���ꊇ���[�h����
void load_cards_stand(Deck*& store, Deck*& deck, Deck*& item, int*& stand_handle) {
	store = new Deck[10];
	deck = new Deck[10];
	item = new Deck;
	stand_handle = new int[10];
	load_stand(stand_handle);
	load_store(store);//�q�ɂ̃f�[�^�����[�h(�Z�[�u�f�[�^����)
	load_deck(deck);//�f�b�L�̃J�[�h�����[�h����(�Z�[�u�f�[�^����)
	load_item(*item);
	for (int i = 0; i < 10; i++) {
		store[i].load_cards();//�q�ɂ̃J�[�h�̉摜�Ȃǂ����[�h����
		deck[i].load_cards();
	}
	item->load_cards();//�A�C�e���q�ɂ̃J�[�h�����[�h����
}

//����ɖ߂�A�C�R����\�� �}�E�X�̏d�Ȃ���m�F���� �d�Ȃ��Ă���Ȃ�indicate = -1;
void modoru_icon(int hand_x, int hand_y, int& indicate) {
	if (hand_x >= 5 && hand_x <= 60 && hand_y >= 5 && hand_y <= 35) {
		indicate = -1;
		DrawBox(2, 2, 63, 38, GetColor(0, 255, 0), TRUE);
	}
	DrawBox(5, 5, 60, 35, GetColor(100, 100, 255), TRUE);
	DrawFormatString(10, 15, GetColor(0, 0, 0), "�߂�");
}

//�E��ɑq�ɂƃA�C�e���ϊ��A�C�R����\�� �}�E�X�̏d�Ȃ���m�F���� �d�Ȃ��Ă���Ȃ�indicate = -6;
void item_switch_icon(int hand_x, int hand_y, int& indicate) {
	if (hand_x >= 450 && hand_x <= 620 && hand_y >= 5 && hand_y <= 35) {
		indicate = -6;
		DrawBox(447, 2, 623, 38, GetColor(0, 255, 0), TRUE);
	}
	DrawBox(450, 5, 620, 35, GetColor(100, 100, 255), TRUE);
	DrawFormatString(450, 15, GetColor(0, 0, 0), "�A�C�e���q�ɐ؂�ւ�");
}

//�E��ɑq�ɂ̃\�[�g�A�C�R����\�� �}�E�X�̏d�Ȃ���m�F���� �d�Ȃ��Ă���Ȃ�indicate = -7;
void sort_icon(int hand_x, int hand_y, int& indicate) {
	if (hand_x >= 320 && hand_x <= 420 && hand_y >= 15 && hand_y <= 35) {
		indicate = -7;
		DrawBox(317, 12, 423, 37, GetColor(0, 255, 0), TRUE);
	}
	DrawBox(320, 15, 420, 35, GetColor(100, 100, 255), TRUE);
	DrawFormatString(340, 15, GetColor(0, 0, 0), "�\�[�g");
}

//�f�b�L��ҏW����L���������߂�i�퓬���j���܂��ĂȂ��Ȃ�0 ���܂����Ȃ�1 �f�b�L�ҏW�ֈȍ~��2
int deck_select(int kind[3], int stand_handle[10], int sound, int& focus) {
	int indicate = 0;//�}�E�X�J�[�\���̏d�Ȃ��Ă��镔���́H
	int hand_x, hand_y;//�}�E�X�J�[�\���̍��W
	GetMousePoint(&hand_x, &hand_y);
	int size_x, size_y;//�摜�̃T�C�Y
	GetGraphSize(stand_handle[0], &size_x, &size_y);
	size_x = (int)(size_x * 0.75);	size_y = (int)(size_y * 0.75);
	DrawBox(0, 0, 640, 480, GetColor(100, 100, 100), TRUE);//�w�i
	DrawFormatString(180, 5, GetColor(255, 0, 0), "�C���^�[�o���@�f�b�L��ҏW�\");
	DrawFormatString(200, 25, GetColor(0, 0, 0), "���FHP�̕ω��͓K�p����Ȃ�");
	modoru_icon(hand_x, hand_y, indicate);
	for (int i = 0; i < 3; i++) {
		int x, y;
		x = 60 + 180 * i; y = 50;
		DrawBox(x, y, x + size_x, y + size_y, GetColor(0, 0, 0), TRUE);
		if (kind[i] > 0 && kind[i] <= 10) {
			DrawRotaGraph2(x, y, 0, 0, 0.75, 0, stand_handle[kind[i] - 1], TRUE, FALSE);
		}
		else { continue; }
		DrawBox(x - 2, y + size_y + 18, x + size_x + 2, y + size_y + 102, GetColor(0, 0, 0), TRUE);
		if (hand_x >= x && hand_x <= x + size_x && hand_y >= y + size_y + 20 && hand_y <= y + size_y + 100) {
			DrawBox(x - 2, y + size_y + 18, x + size_x + 2, y + size_y + 102, GetColor(0, 255, 0), TRUE);
			indicate = i + 1;
		}
		DrawBox(x, y + size_y + 20, x + size_x, y + size_y + 100, GetColor(150, 150, 150), TRUE);
		DrawFormatString(x + 25, y + size_x + 100, GetColor(0, 0, 0), "�f�b�L��ҏW");
	}

	int c = left_click();
	if (c == 1) {//�E�N���b�N�������ꂽ�u��
		if (indicate == -1) {
			PlaySoundMem(sound, DX_PLAYTYPE_BACK);//��
			return 1;
		}
		if (indicate > 0) {
			focus = kind[indicate - 1];
			PlaySoundMem(sound, DX_PLAYTYPE_BACK);//��
			return 2;
		}
	}

	return 0;
}

//�L�����N�^�[�����߂� ���܂��ĂȂ��Ȃ�0 ���܂����Ȃ�1 ���I�тɈڍs�Ȃ�2 �f�b�L�\�z�Ȃ�3 �L�����Z����-1
int character_select(int kind[3], int stand_handle[10], bool fixed[3], int s_kettei, int s_modoru, int& focus) {
	static int member = -1;//�ύX���悤�Ƃ��Ă��郁���o�[
	static int wait = -1;//�ύX���悤�Ƃ��Ă���ҋ@���k
	int indicate = 0;//�}�E�X�J�[�\���̏d�Ȃ��Ă��镔���́H��:-4 �f�b�L:-3 �߂�:-2 �m��:-1 �I��1-3 �ҋ@4-14
	int hand_x, hand_y;//�}�E�X�J�[�\���̍��W
	GetMousePoint(&hand_x, &hand_y);
	int size_x, size_y;//�摜�̃T�C�Y
	GetGraphSize(stand_handle[0], &size_x, &size_y);
	DrawBox(0, 0, 640, 480, GetColor(100, 100, 100), TRUE);//�w�i
	DrawFormatString(70, 5, GetColor(255, 0, 0), "�L�����N�^�[�I��");
	DrawFormatString(250, 5, GetColor(0, 0, 0), "�E�N���b�N�ŃL�������O��");
	modoru_icon(hand_x, hand_y, indicate);
	if (hand_x >= 580 && hand_x <= 635 && hand_y >= 5 && hand_y <= 35) {
		indicate = -2;//�m��ɏd�Ȃ��Ă���
		DrawBox(577, 2, 638, 38, GetColor(0, 255, 0), TRUE);
	}
	DrawBox(580, 5, 635, 35, GetColor(100, 100, 255), TRUE);
	DrawFormatString(585, 15, GetColor(0, 0, 0), "�m��");
	//���߂�3�l
	size_x = (int)(size_x * 0.75);	size_y = (int)(size_y * 0.75);
	for (int i = 0; i < 3; i++) {
		int x, y;
		x = 60 + 180 * i; y = 50;
		bool overlap = hand_x >= x && hand_x <= x + size_x && hand_y >= y && hand_y <= y + size_y;
		//�d�Ȃ��Ă��邩�ǂ���
		if (!fixed[i] && overlap) {
			indicate = i + 1;
			DrawBox(x - 3, y - 3, x + 3 + size_x, y + 3 + size_y, GetColor(0, 255, 0), TRUE);
		}
		if (member == i) {//�I��
			DrawBox(x - 3, y - 3, x + 3 + size_x, y + 3 + size_y, GetColor(255, 0, 0), TRUE);
		}
		if (fixed[i]) { DrawFormatString(x+30, y - 15, GetColor(255,0,0), "�Œ�L����"); }
		DrawBox(x, y, x + size_x, y + size_y, GetColor(0,0,0), TRUE);
		if (kind[i] > 0 && kind[i] <= 10) {
			DrawRotaGraph2(x, y, 0, 0, 0.75, 0, stand_handle[kind[i] - 1], TRUE, FALSE);
		}
		if (overlap && kind[i] != 0) {//������f�b�L�ҏW
			if (hand_x >= x + 10 && hand_x <= x - 10 + size_x && hand_y >= y + 100 && hand_y <= y + 120) {
				indicate = -3;//�f�b�L�ҏW
				focus = kind[i];//�œ_�𓖂Ă��Ă���L�����́H
				DrawBox(x + 8, y + 98, x - 8 + size_x, y + 122, GetColor(255, 0, 0), TRUE);
			}
			DrawBox(x + 10, y + 100, x - 10 + size_x, y + 120, GetColor(100, 100, 100), TRUE);
			DrawFormatString(x + 15, y + 102, GetColor(0, 0, 0), "�f�b�L�ҏW");
			if (hand_x >= x + 10 && hand_x <= x - 10 + size_x && hand_y >= y + 130 && hand_y <= y + 150) {
				indicate = -4;//�����ύX
				focus = kind[i];//�œ_�𓖂Ă��Ă���L�����́H
				DrawBox(x + 8, y + 128, x - 8 + size_x, y + 152, GetColor(255, 0, 0), TRUE);
			}
			DrawBox(x + 10, y + 130, x - 10 + size_x, y + 150, GetColor(100, 100, 100), TRUE);
			DrawFormatString(x + 15, y + 132, GetColor(0, 0, 0), "�����ύX");
		}
	}
	//�I������10�l
	GetGraphSize(stand_handle[0], &size_x, &size_y);
	size_x = (int)(size_x * 0.4);	size_y = (int)(size_y * 0.35);
	for (int i = 0; i < 10; i++) {
		int x, y;
		if (i < 5) {//��̍s�ɔz�u
			x = 80 + 100 * i;
			y = 270;
		}
		else {//���̍s�ɔz�u
			x = 80 + 100 * (i - 5);
			y = 370;
		}
		bool used = false;
		for (int j = 0; j < 3; j++) {
			if (kind[j] == i + 1) {
				used = true;//�p�[�e�B�[�ɓ����Ă��郁���o�[
			}
		}
		//�d�Ȃ��Ă��邩�ǂ���
		if (!used && hand_x >= x && hand_x <= x + size_x && hand_y >= y && hand_y <= y + size_y) {
			indicate = i + 4;
			DrawBox(x - 3, y - 3, x + 3 + size_x, y + 3 + size_y, GetColor(0, 255, 0), TRUE);
		}
		if (wait == i) {
			DrawBox(x - 3, y - 3, x + 3 + size_x, y + 3 + size_y, GetColor(255, 0, 0), TRUE);
		}
		DrawBox(x, y, x + size_x, y + size_y, GetColor(100, 100, 255), TRUE);
		if (used) { SetDrawBright(100, 100, 100); }
		DrawRotaGraph2(x + 3, y-1, 0, 0, 0.35, 0, stand_handle[i], TRUE, FALSE);
		SetDrawBright(255, 255, 255);
	}
	int c = left_click();
	int c2 = right_click();
	if (c2 == 1) {
		if (indicate > 0 && indicate < 4) {
			member = -1;
			wait = -1;
			kind[indicate - 1] = 0;
			PlaySoundMem(s_modoru, DX_PLAYTYPE_BACK);//��
		}
	}
	if (c == 1) {//�E�N���b�N�������ꂽ�u��
		if (indicate < 0) {
			member = -1;
			wait = -1;
		}
		if (indicate == -2) { //�m��
			if (kind[0] == 0 && kind[1] == 0 && kind[2] == 0) { return 0; }
			PlaySoundMem(s_kettei, DX_PLAYTYPE_BACK);//��
			return 1;
		}
		if (indicate == -1) {//�߂�
			//PlaySoundMem(s_modoru, DX_PLAYTYPE_BACK);//��
			return -1;
		}
		if (indicate == -3) {//�f�b�L�ҏW
			PlaySoundMem(s_kettei, DX_PLAYTYPE_BACK);//��
			return 3;
		}
		if (indicate == -4) {//�����ύX
			PlaySoundMem(s_kettei, DX_PLAYTYPE_BACK);//��
			return 2;
		}
		if (indicate > 0 && indicate < 4) {//����ւ��悤�Ƃ��鐶�k�̕ύX
			if (indicate - 1 == member) {
				member = -1;
				PlaySoundMem(s_modoru, DX_PLAYTYPE_BACK);//��
			}
			else {
				member = indicate - 1;
				PlaySoundMem(s_kettei, DX_PLAYTYPE_BACK);//��
			}
		}
		if (indicate >= 4 && indicate <= 13) {//����ւ��悤�Ƃ��鐶�k�̕ύX
			if (indicate - 4 == wait) {//�����A����
				wait = -1;
				PlaySoundMem(s_modoru, DX_PLAYTYPE_BACK);//��
			}
			else {
				wait = indicate - 4;//�ʂ̑ҋ@���k��
				PlaySoundMem(s_kettei, DX_PLAYTYPE_BACK);//��
			}
		}
	}
	if (member != -1 && wait != -1) {//����ւ�
		kind[member] = wait + 1;
		member = -1;
		wait = -1;
	}
	return 0;
}

//�L����1�̂̕������߂� ���܂��ĂȂ��Ȃ�0 ���܂����Ȃ�1
int huku_select(int* huku, int stand_handle[]) {

	return 0;
}

//�J�[�h�̃X�e�[�^�X��\��������W
const int TEXT_X = 10;
const int TEXT_Y = 400;
//�J�[�h�̃X�e�[�^�X����܂��͉��֕\��
void picture_card_state(int h, int p, int s, int r, bool up) {
	int d = 0;
	if (up) { 
		d = 80;
		DrawFormatString(10, TEXT_Y - d - 20, GetColor(0, 0, 0), "�I�𒆂̃J�[�h");
		DrawBox(0, TEXT_Y - d, 640, 480 - d, GetColor(255, 0, 0), TRUE);
	}
	DrawBox(0+1, TEXT_Y - d+1, 640-1, TEXT_Y + 20 - d, GetColor(255, 255, 255), TRUE);//�J�[�h�X�e�[�^�X
	DrawBox(0+1, TEXT_Y + 20 - d, 640-1, 480 - d-1, GetColor(255, 255, 100), TRUE);//�J�[�h����

	DrawFormatString(TEXT_X, TEXT_Y + 2 - d, GetColor(0, 0, 0), "HP:");
	DrawFormatString(TEXT_X + 160, TEXT_Y + 2 - d, GetColor(0, 0, 0), "POWER:");
	DrawFormatString(TEXT_X + 320, TEXT_Y + 2 - d, GetColor(0, 0, 0), "SPEED:");
	DrawFormatString(TEXT_X + 480, TEXT_Y + 2 - d, GetColor(0, 0, 0), "RIGID:");

	DrawFormatString(TEXT_X + 30, TEXT_Y + 2 - d, GetColor(255, 0, 0), "%d", h);
	DrawFormatString(TEXT_X + 60 + 160, TEXT_Y + 2 - d, GetColor(255, 0, 0), "%d", p);
	DrawFormatString(TEXT_X + 60 + 320, TEXT_Y + 2 - d, GetColor(255, 0, 0), "%d", s);
	DrawFormatString(TEXT_X + 60 + 480, TEXT_Y + 2 - d, GetColor(255, 0, 0), "%d", r);
}

//�����\��̃J�[�h�ɃZ�b�g����
void set_swap(int indicate, Card*& c, Deck* deck, Deck* store, int sound) {
	if (indicate < 10) {
		if (indicate - 1 < deck->get_size() && deck->get_cards()[indicate - 1].get_kind() != 0) {
			c = &deck->get_cards()[indicate - 1];
			PlaySoundMem(sound, DX_PLAYTYPE_BACK);//��
		}
	}
	else {
		if (indicate - 10 < store->get_size() && store->get_cards()[indicate - 10].get_kind() != 0) {
			c = &store->get_cards()[indicate - 10];
			PlaySoundMem(sound, DX_PLAYTYPE_BACK);//��
		}
	}
}

//�J�[�h�������ł��邩�ǂ���
bool swap_able(bool first_deck, bool second_deck, bool item_flag, Card& first, Card& second) {
	if (first_deck == second_deck) {
		return true;
	}
	if (item_flag) {
		if ((first_deck && first.get_kind() > 0) || (second_deck && second.get_kind() > 0)) {
			return false;
		}
	}
	else {
		if ((first_deck && first.get_kind() < 0) || (second_deck && second.get_kind() < 0)) {
			return false;
		}
	}
	return true;
}

//�J�[�hfirst second�����ւ���
void swap_card(Card* first, Card* second) {
	Card tmp;
	tmp = *first;
	*first = *second;
	*second = tmp;
}

//�J�[�h�̌��ʃe�L�X�g��K���Ȓ����Ő؂�
void card_text(char c[], const char* str) {
	for (int j = 0; j < 22; j++) {//�R�s�[���Ă���
		c[j] = str[j];
		if (c[j] == '\0')break;
	}
	c[22] = '\0';//���肫��Ȃ��̂œr���Ő؂�
}

//�q�ɂ̃J�[�h�X�N���[���o�[
void up_down(int hand_x, int hand_y, int& indicate, bool up, bool down) {
	if (up && hand_x >= 415 && hand_x <= 430 && hand_y >= 40 && hand_y <= 140) {
		indicate = -2;
		DrawBox(413, 38, 432, 142, GetColor(0, 255, 0), TRUE);
	}
	else {
		DrawBox(413, 38, 432, 142, GetColor(0, 0, 0), TRUE);
	}
	if (up) {
		DrawBox(415, 40, 430, 140, GetColor(150, 150, 150), TRUE);
	}
	else {
		DrawBox(415, 40, 430, 140, GetColor(50, 50, 50), TRUE);
	}
	DrawFormatString(415, 80, GetColor(0, 0, 0), "��");
	if (down && hand_x >= 415 && hand_x <= 430 && hand_y >= 165 && hand_y <= 260) {
		indicate = -3;
		DrawBox(413, 163, 432, 262, GetColor(0, 255, 0), TRUE);
	}
	else {
		DrawBox(413, 163, 432, 262, GetColor(0, 0, 0), TRUE);
	}
	if (down) {
		DrawBox(415, 165, 430, 260, GetColor(150, 150, 150), TRUE);
	}
	else {
		DrawBox(415, 165, 430, 260, GetColor(50, 50, 50), TRUE);
	}
	DrawFormatString(415, 210, GetColor(0, 0, 0), "��");
}

//�}��or�r���̃{�^��
void insert_drop(int hand_x, int hand_y,int& indicate, int num, Deck& deck, Deck& store, int deck_last, int store_last) {
	if (num < DECK_MAX) {
		DrawBox(8, 268, 202, 292, GetColor(0, 0, 0), TRUE);
		if (store_last < store.get_size()) {
			if (hand_x >= 10 && hand_x <= 200 && hand_y >= 270 && hand_y <= 290) {
				DrawBox(8, 268, 202, 292, GetColor(0, 255, 0), TRUE);
				indicate = -4;
			}
			DrawBox(10, 270, 200, 290, GetColor(150, 150, 150), TRUE);
		}
		else {
			DrawBox(10, 270, 200, 290, GetColor(50, 50, 50), TRUE);
		}
		DrawFormatString(10,270,GetColor(0,0,0),"�q�ɂֈړ�");
	}
	else {
		DrawBox(218, 268, 412, 292, GetColor(0, 0, 0), TRUE);
		if (deck_last < deck.get_size()) {//�}���ł���󂪂���
			if (hand_x >= 220 && hand_x <= 410 && hand_y >= 270 && hand_y <= 290) {
				DrawBox(218, 268, 412, 292, GetColor(0, 255, 0), TRUE);
				indicate = -5;
			}
			DrawBox(220, 270, 410, 290, GetColor(150, 150, 150), TRUE);
		}
		else {
			DrawBox(220, 270, 410, 290, GetColor(50, 50, 50), TRUE);
		}
		DrawFormatString(220, 270, GetColor(0, 0, 0), "�f�b�L�̍Ō�֑}��");
	}
}

//�J�[�h���ꖇ�폜���Ċ񂹂�
void cards_bring(Deck& cards, int erase_index) {//erase_index�̃J�[�h�������悤�Ɍ�̃J�[�h���񂹂�
	for (int i = erase_index + 1; i < cards.get_size(); i++) {
		if (cards.get_cards()[i].get_kind() == 0) {
			cards.get_cards()[i - 1].set_kind(0);//�ЂƂO�̃J�[�h�͏����āA�I���
			return;
		}
		cards.get_cards()[i - 1] = cards.get_cards()[i];//�O�̃J�[�h�փR�s�[
	}
	cards.get_cards()[cards.get_size() - 1].set_kind(0);//�����̃J�[�h�͏����āA�I���
}

//�f�b�L�ƃX�g�A�ŃJ�[�h�̓���ւ� ���܂��ĂȂ��Ȃ�0 ���܂����Ȃ�1
int deck_create(Deck& deck, Deck& store, Deck& item, int sound) {
	static bool item_flag = false;
	int result = 0;
	if (!item_flag) {
		result = deck_item_create(deck, store, sound, item_flag);
	}
	else {
		result = deck_item_create(deck, item, sound, item_flag);
	}
	if (result == -1) {
		item_flag = !item_flag;
		result = 0;
	}
	if (result == 1) {
		item_flag = false;
	}
	return result;
}

const int STORE_PAGE = 9;
//�f�b�L�ƃX�g�A�ŃJ�[�h�̓���ւ� ���܂��ĂȂ��Ȃ�0 ���܂����Ȃ�1 �A�C�e���q�ɐ؂�ւ���-1
int deck_item_create(Deck& deck, Deck& store, int sound, bool item_flag) {
	static int deck_last = -1;//��ɂȂ��Ă���ŏ��̃C���f�b�N�X�́H
	static int store_last = -1;//��ɂȂ��Ă���ŏ��̃C���f�b�N�X�́H
	if (deck_last == -1) {
		deck.get_last(deck_last);
		store.get_last(store_last);
	}
	const int CARD_X = 550;
	const int CARD_Y = 150;
	static Card* first = NULL; static Card* second = NULL;//��������J�[�h�̃|�C���^
	static bool first_deck = false, second_deck = false;//��������J�[�h���f�b�L���q�ɂ�?
	int first_index;
	static int store_i = 0;	//��ԏ�ɕ\�������J�[�h�̃C���f�b�N�X
	int indicate = 0;//�w�������Ă���̂��̂̔ԍ�0�͂Ȃ�
	int hand_x, hand_y;//�}�E�X�J�[�\���̍��W
	GetMousePoint(&hand_x, &hand_y);
	DrawBox(0, 0, 640, 480, GetColor(100, 100, 100), TRUE);//�w�i
	DrawFormatString(470, 270, GetColor(0, 0, 0), "�f�b�L�̍��vHP");
	DrawFormatString(470, 290, GetColor(0, 0, 0), "%d", deck.get_hp());
	//�߂�A�C�R��
	modoru_icon(hand_x, hand_y, indicate);
	item_switch_icon(hand_x, hand_y, indicate);
	sort_icon(hand_x, hand_y, indicate);
	up_down(hand_x, hand_y, indicate, store_i > 0, store_i < store_last - STORE_PAGE);
	DrawFormatString(150, 20, GetColor(0, 0, 0), "�f�b�L");//�J�[�h�̌���
	DrawFormatString(220, 20, GetColor(0, 0, 0), "�q��:%d��", store_last);//�J�[�h�̌���
	DrawBox(5, 37, 208, 263 - (STORE_PAGE - DECK_MAX)*25, GetColor(150, 150, 150), TRUE);
	//�f�b�L�Ƒq�ɂ̃��X�g��9�����\��
	for (int i = 0; i < DECK_MAX + STORE_PAGE; i++) {//i<DECK_MAX * 2
		int x, y; int index, init, now;
		Deck* d;
		if (i < DECK_MAX) {
			now = i;
			x = 10; y = 40 + 25 * now; index = 1; init = 0;
			d = &deck;
		}
		else {
			now = i - DECK_MAX;
			x = 220; y = 40 + 25 * now; index = 10; init = store_i; 
			d = &store;
		}
		//�J�[�\�����d�Ȃ��Ă���Ƃ�
		if (hand_x >= x && hand_x <= x + 190 && hand_y >= y && hand_y <= y + 20) {
			indicate = now + index + init;//�w�������Ă���J�[�h Deck:1�`9 Store:10�`
			DrawBox(x - 3, y - 3, x + 193, y + 23, GetColor(0, 255, 0), TRUE);
		}
		//���X�g�\��
		if (&d->get_cards()[now + init] == first) {//�I�𒆂̃J�[�h�͐Ԃ��͂�ŋ���
			DrawBox(x - 3, y - 3, x + 193, y + 23, GetColor(255, 0, 0), TRUE);
			//�}��or�r���̃{�^���\��
			if (i >= DECK_MAX || ((!item_flag && first->get_kind() > 0) || (item_flag && first->get_kind() < 0))) {
				insert_drop(hand_x, hand_y, indicate, i, deck, store, deck_last, store_last);
			}
			first_index = now + init;//first �̃C���f�b�N�X
		}
		DrawBox(x, y, x + 190, y + 20, GetColor(100,100,255), TRUE);
		Card* card = &d->get_cards()[now + init];
		if (now < d->get_size() && card->get_kind() != 0) {
			const char* str;
			str = card->get_ability()->get_text();//�J�[�h�̃e�L�X�g
			char c[23];
			card_text(c, str);//c��str����肫�镔�������R�s�[
			DrawFormatString(x+2, y+2, GetColor(0, 0, 0), c);
			if (indicate == now + index + init) {//�J�[�\�����d�Ȃ��Ă���Ƃ��J�[�h�̏���\��
				int h, p, s, r;
				h = card->get_hp(); p = card->get_power(); s = card->get_speed(); r = card->get_rigid();
				//SetDrawMode(DX_DRAWMODE_NEAREST);
				DrawRotaGraph(CARD_X, CARD_Y, 1.5, 0, card->get_handle(), TRUE, FALSE);//�J�[�h�̉摜
				//SetDrawMode(DX_DRAWMODE_BILINEAR);
				picture_card_state(h, p, s, r);
				DrawFormatString(TEXT_X, TEXT_Y + 40, GetColor(0, 0, 0), str);//�J�[�h�̌���
			}
		}
	}
	if (first != NULL) {//�I�𒆂̃J�[�h�\��
		int h, p, s, r;
		h = first->get_hp(); p = first->get_power(); s = first->get_speed(); r = first->get_rigid();
		const char* str;
		str = first->get_ability()->get_text();
		picture_card_state(h, p, s, r, true);
		DrawFormatString(TEXT_X, TEXT_Y - 40, GetColor(0, 0, 0), str);//�J�[�h�̌���
	}

	int c = left_click();
	if (c == 1) {//���N���b�N�������ꂽ�u��
		if (indicate < 0) {
			PlaySoundMem(sound, DX_PLAYTYPE_BACK);//��
		}
		if (indicate == -1 && !deck.empty()) {//�߂�{�^��
			store_i = 0; 
			first = NULL; second = NULL;
			first_deck = false; second_deck = false;
			deck_last = -1; store_last = -1;
			return 1;
		}
		else if (indicate == -2) {//��փX�N���[��
			store_i--;
		}
		else if (indicate == -3) {//���փX�N���[��
			store_i++;
		}
		else if (indicate == -4) {//�f�b�L����q�ɂփJ�[�h���ړ�
			store.get_cards()[store_last] = *first;//�}���i�R�s�[�j
			cards_bring(deck, first_index);
			deck_last = -1; store_last = -1; first = NULL;
		}
		else if (indicate == -5) {//�q�ɂ���f�b�L�֑}��
			deck.get_cards()[deck_last] = *first;//�}���i�R�s�[�j
			cards_bring(store, first_index);
			deck_last = -1; store_last = -1; first = NULL;
		}
		else if (indicate == -6) {
			store_i = 0; 
			first = NULL; second = NULL;
			first_deck = false; second_deck = false;
			deck_last = -1; store_last = -1;
			return -1;//�������񃊃Z�b�g���ăA�C�e���q�ɐ؂�ւ�
		}
		else if (indicate == -7) {
			store_i = 0;
			first = NULL; second = NULL;
			first_deck = false; second_deck = false;
			deck_last = -1; store_last = -1;
			store.sort();
		}
		else if (indicate > 0) {//�����Əd�Ȃ��Ă���Ȃ�
			if (first == NULL) {//��ڂ̃J�[�h��I��
				set_swap(indicate, first, &deck, &store, sound);//first���Z�b�g
				if (indicate <= DECK_MAX) { first_deck = true; }
			}
			else {//��ڂ̃J�[�h��I��Ō���
				set_swap(indicate, second, &deck, &store, sound);//second���Z�b�g
				if (indicate <= DECK_MAX) { second_deck = true; }
				if (second != NULL) {//�Z�b�g�ɐ������Ă���Ȃ�
					if (swap_able(first_deck, second_deck, item_flag, *first, *second)) {
						swap_card(first, second);//����
					}
					first = NULL; second = NULL;
					first_deck = false; second_deck = false;
				}
			}
		}
	}
	if (deck.empty()) {
		DrawFormatString(5, 200, GetColor(0, 0, 0), "�f�b�L�Ɉꖇ�ȏ�");//�J�[�h�̌���
		DrawFormatString(5, 220, GetColor(0, 0, 0), "�J�[�h�����Ă�������");//�J�[�h�̌���
	}
	//DrawFormatString(50, 300, GetColor(0, 0, 0), "first = %d, second = %d", first_deck, second_deck);//�J�[�h�̌���
	return 0;
}

//�J�[�h�̔��p -1�ŏI�� 1�ȏ�Ŕ��p�ɂ�邨��
int card_sell(Deck& store, int& last, int sound, int money_sound) {
	const int CARD_X = 550;
	const int CARD_Y = 150;
	static int store_i = 0;	//��ԏ�ɕ\�������J�[�h�̃C���f�b�N�X
	static Card* sell = NULL;//���p����J�[�h�̃|�C���^
	int sell_index;
	int indicate = 0;//�w�������Ă���̂��̂̔ԍ�0�͂Ȃ�
	int hand_x, hand_y;//�}�E�X�J�[�\���̍��W
	GetMousePoint(&hand_x, &hand_y);
	DrawBox(0, 0, 640, 480, GetColor(120, 150, 120), TRUE);//�w�i
	static int store_last = -1;//��ɂȂ��Ă���ŏ��̃C���f�b�N�X�́H
	if (store_last == -1) {
		store.get_last(store_last);
	}

	modoru_icon(hand_x, hand_y, indicate);
	sort_icon(hand_x, hand_y, indicate);
	up_down(hand_x, hand_y, indicate, store_i > 0, store_i < last - STORE_PAGE);
	DrawFormatString(220, 20, GetColor(0, 0, 0), "�q��:%d��", store_last);//�J�[�h�̌���
	//�q�ɂ̃��X�g��9�����\��
	for (int i = 0; i < STORE_PAGE; i++) {//i<DECK_MAX
		int x, y; int index, init;
		Deck* d;
		x = 220; y = 40 + 25 * i; index = 1; init = store_i;
		d = &store;
		//�J�[�\�����d�Ȃ��Ă���Ƃ�
		if (hand_x >= x && hand_x <= x + 190 && hand_y >= y && hand_y <= y + 20) {
			indicate = i + index + init;//�w�������Ă���J�[�h Deck:1�`9 Store:10�`
			DrawBox(x - 3, y - 3, x + 193, y + 23, GetColor(0, 255, 0), TRUE);
		}
		//���X�g�\��
		if (&d->get_cards()[i + init] == sell) {//�I�𒆂̃J�[�h�͐Ԃ��͂�ŋ���
			DrawBox(x - 3, y - 3, x + 193, y + 23, GetColor(255, 0, 0), TRUE);
			//���p�{�^���\��
			if (hand_x >= 20 && hand_x <= 120 && hand_y >= 100 && hand_y <= 150) {
				DrawBox(18, 98, 122, 152, GetColor(0, 255, 0), TRUE);
				indicate = -4;
			}
			DrawBox(20, 100, 120, 150, GetColor(100,100,255), TRUE);
			DrawFormatString(50, 120, GetColor(0, 0, 0), "���p");//�J�[�h�̌���
			sell_index = i + init;//first �̃C���f�b�N�X
		}
		DrawBox(x, y, x + 190, y + 20, GetColor(100, 100, 255), TRUE);
		Card* card = &d->get_cards()[i + init];
		if (i < d->get_size() && card->get_kind() != 0) {
			const char* str;
			str = card->get_ability()->get_text();//�J�[�h�̃e�L�X�g
			char c[23];
			card_text(c, str);//c��str����肫�镔�������R�s�[
			DrawFormatString(x+2, y+2, GetColor(0, 0, 0), c);
			if (indicate == i + index + init) {//�J�[�\�����d�Ȃ��Ă���Ƃ��J�[�h�̏���\��
				int h, p, s, r;
				h = card->get_hp(); p = card->get_power(); s = card->get_speed(); r = card->get_rigid();
				DrawRotaGraph(CARD_X, CARD_Y, 1.5, 0, card->get_handle(), TRUE, FALSE);//�J�[�h�̉摜
				picture_card_state(h, p, s, r);
				DrawFormatString(TEXT_X, TEXT_Y + 40, GetColor(0, 0, 0), str);//�J�[�h�̌���
			}
		}
	}

	int money = 0;//���p�z
	if (sell != NULL) {//�I�𒆂̃J�[�h�\��
		int h, p, s, r;
		h = sell->get_hp(); p = sell->get_power(); s = sell->get_speed(); r = sell->get_rigid();
		const char* str;
		str = sell->get_ability()->get_text();
		picture_card_state(h, p, s, r, true);
		DrawFormatString(TEXT_X, TEXT_Y - 40, GetColor(0, 0, 0), str);//�J�[�h�̌���
		money = sell_money(sell->get_kind(), sell->get_rare());
		DrawFormatString(50, 170, GetColor(0, 0, 0), "���p�z: %d", money);//�J�[�h�̌���
	}

	if (left_click() == 1) {
		if (indicate == -1) { 
			sell = NULL;
			store_i = 0;
			PlaySoundMem(sound, DX_PLAYTYPE_BACK);//��
			store_last = -1;
			return -1;
		}
		if (indicate == -2) {//��փX�N���[��
			store_i--;
			PlaySoundMem(sound, DX_PLAYTYPE_BACK);//��
		}
		if (indicate == -3) {//���փX�N���[��
			store_i++;
			PlaySoundMem(sound, DX_PLAYTYPE_BACK);//��
		}
		if (indicate == -4) {//���p
			cards_bring(store, sell_index);
			last--; sell = NULL;
			PlaySoundMem(money_sound, DX_PLAYTYPE_BACK);//��
			store_last = -1;
			return money;
		}
		if (indicate == -7) {
			PlaySoundMem(sound, DX_PLAYTYPE_BACK);//��
			sell = NULL;
			store_i = 0;
			store.sort();
		}
		if (indicate > 0 && store.get_cards()[indicate - 1].get_kind() != 0) {//�����Əd�Ȃ��Ă���Ȃ�
			sell = &store.get_cards()[indicate - 1];
			PlaySoundMem(sound, DX_PLAYTYPE_BACK);//��
		}

	}
	return 0;
}

//�L�����N�^�[�����߂� ���܂��ĂȂ��Ȃ�0 ���܂����Ȃ�1 ���I�тɈڍs�Ȃ�2 �f�b�L�\�z�Ȃ�3
int character_custom(int stand_handle[10], int s_kettei, int s_modoru, int& focus) {
	DrawBox(0, 0, 640, 480, GetColor(100, 100, 100), TRUE);
	const int WIDE = 122, HEIGHT = 166, DISTANCE = 5;
	int hand_x, hand_y, indicate = 0;
	GetMousePoint(&hand_x, &hand_y);
	modoru_icon(hand_x, hand_y, indicate);
	for (int i = 0; i < 10; i++) {
		const int num = i % 5;
		int x = 5 + num * (WIDE + DISTANCE);
		int y = 80 + (i / 5)* (HEIGHT + DISTANCE);
		if (hand_x >= x && hand_x <= x + WIDE && hand_y >= y && hand_y <= y + HEIGHT) {
			DrawBox(x - 2, y - 2, x + WIDE + 2, y + HEIGHT + 2, GetColor(0, 255, 0), TRUE);
			indicate = i + 1;
		}
		DrawBox(x, y, x+WIDE, y+HEIGHT, GetColor(150,150,150), TRUE);
		DrawRotaGraph2(x, y, 0, 0, 0.60, 0, stand_handle[i], TRUE, FALSE);//0.60
	}
	if (left_click() == 1) {
		if (indicate == -1) {
			PlaySoundMem(s_modoru, DX_PLAYTYPE_BACK);//��
			return 1;
		}
		if (indicate > 0) {
			PlaySoundMem(s_kettei, DX_PLAYTYPE_BACK);//��
			focus = indicate;
			return 3;
		}
	}
	return 0;
}

int item_or_card(int s_kettei, int s_modoru, int& focus) {
	DrawBox(0, 0, 640, 480, GetColor(100, 100, 100), TRUE);
	int hand_x, hand_y, indicate = 0;
	GetMousePoint(&hand_x, &hand_y);
	modoru_icon(hand_x, hand_y, indicate);
	if (hand_x >= 50 && hand_x <= 300 && hand_y >= 100 && hand_y <= 300) {
		DrawBox(48, 98, 302, 302, GetColor(0, 255, 0), TRUE);
		indicate = 1;
	}
	DrawBox(50, 100, 300, 300, GetColor(150, 150, 150), TRUE);
	DrawFormatString(130, 200, GetColor(0, 0, 0), "�J�[�h���p");
	if (hand_x >= 340 && hand_x <= 590 && hand_y >= 100 && hand_y <= 300) {
		DrawBox(338, 98, 592, 302, GetColor(0, 255, 0), TRUE);
		indicate = 2;
	}
	DrawBox(340, 100, 590, 300, GetColor(150, 150, 150), TRUE);
	DrawFormatString(410, 200, GetColor(0, 0, 0), "�A�C�e�����p");
	if (left_click() == 1) {
		if (indicate == 1) {
			PlaySoundMem(s_kettei, DX_PLAYTYPE_BACK);//��
			focus = -1;
			return 0;
		}
		if (indicate == 2) {
			PlaySoundMem(s_kettei, DX_PLAYTYPE_BACK);//��
			focus = -2;
			return 0;
		}
		if (indicate == -1) {
			PlaySoundMem(s_modoru, DX_PLAYTYPE_BACK);//��
			return 1;
		}
	}
	return 0;
}

int sell_item_load(Card*& sell_item) {
	const int SUM = 3;
	sell_item = new Card[SUM];
	for (int i = 0; i < SUM; i++ ) {
		sell_item[i] = -1 * (i + 1);
		sell_item[i].load_card();
	}
	return SUM;
}