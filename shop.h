#ifndef INCLUDED_SHOP_H
#define INCLUDED_SHOP_H

#include"create.h"

enum {
	SHOP_MAIN=0,
	SHOP_BUY,
	SHOP_SELL,
	SHOP_GACHA
};

class Shop {
private:
	Deck* deck;
	Deck* store;
	Deck* item;
	int store_last = 0;
	int item_last = 0;
	int* stand_handle;
	int kettei_sound, modoru_sound, money_sound;
	int font_handle;
private:
	int indicate = 0;
	int hand_x, hand_y;
	int now = 0;//���ǂ̉�ʂ��H
	int focus = 0;//�ǂ̃L�����̃J�[�h�𔄂邩�H
private:
	void picture_main();
	void picture_buy(int& money);
	void picture_sell(int& money);
	void picture_gacha(int& money);
private:
	Card* c;//�K�`���Ŏ�ɓ��ꂽ�J�[�h
public:
	Shop();
	~Shop();
	bool shop_main(int& money);//Shop��main�֐�
};

#endif