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
	int now = 0;//今どの画面か？
	int focus = 0;//どのキャラのカードを売るか？
private:
	void picture_main();
	void picture_buy(int& money);
	void picture_sell(int& money);
	void picture_gacha(int& money);
private:
	Card* c;//ガチャで手に入れたカード
public:
	Shop();
	~Shop();
	bool shop_main(int& money);//Shopのmain関数
};

#endif