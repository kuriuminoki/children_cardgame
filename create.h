#ifndef INCLUDED_CREATE_H
#define INCLUDED_CREATE_H

#include"card.h"

//�Z�[�u����
bool save_money(int* money);
bool save_other(int* day, bool clear[], int today_mission[]);
bool save_deck(Deck* deck);
bool save_store(Deck* store);
bool save_item(Deck& item);
//���[�h����
bool load_money(int* money);
bool load_other(int* day, bool clear[], int today_mission[]);
bool load_deck(Deck* deck);
bool load_store(Deck* store);
bool load_item(Deck& item);
void load_stand(int stand_handle[10]);
void load_cards_stand(Deck*& store, Deck*& deck, Deck*& item, int*& stand_handle);


//�L�����╞�����߂�A�f�b�L�����A�J�[�h���Ǘ�����

//����ɖ߂�A�C�R����\�� �d�Ȃ��Ă���Ȃ�indicate = -1;
void modoru_icon(int hand_x, int hand_y, int& indicate);
void sort_icon(int hand_x, int hand_y, int& indicate);
void item_switch_icon(int hand_x, int hand_y, int& indicate);

//�C���^�[�o�����A�f�b�L��ҏW����L������3�l���猈�߂�
int deck_select(int kind[3], int stand_handle[10], int sound, int& focus);

//�퓬����L�����N�^�[3�l�����߂�
int character_select(int kind[3], int stand_handle[10], bool fixed[3], int s_kettei, int s_modoru, int& focus);

//�ߑ������߂�
int huku_select(int* huku, int stand_handle[]);

//�f�b�L�Ƒq�ɊԂł̃J�[�h�̂����
int deck_create(Deck& deck, Deck& store, Deck& item, int sound);

//�J�[�h�̔��p
int card_sell(Deck& store, int& last, int sound, int money_sound);

//10�l�̐��k����1�l��I��
int character_custom(int stand_handle[10], int s_kettei, int s_modoru, int& focus);

//�J�[�h�̃X�e�[�^�X��\������(�ォ��)
void picture_card_state(int h, int p, int s, int r, bool up = false);

int item_or_card(int s_kettei, int s_modoru, int& focus);

int sell_item_load(Card*& sell_item);

#endif