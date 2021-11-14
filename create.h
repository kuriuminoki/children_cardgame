#ifndef INCLUDED_CREATE_H
#define INCLUDED_CREATE_H

#include"card.h"

//セーブする
bool save_money(int* money);
bool save_other(int* day, bool clear[], int today_mission[]);
bool save_deck(Deck* deck);
bool save_store(Deck* store);
bool save_item(Deck& item);
//ロードする
bool load_money(int* money);
bool load_other(int* day, bool clear[], int today_mission[]);
bool load_deck(Deck* deck);
bool load_store(Deck* store);
bool load_item(Deck& item);
void load_stand(int stand_handle[10]);
void load_cards_stand(Deck*& store, Deck*& deck, Deck*& item, int*& stand_handle);


//キャラや服を決める、デッキを作る、カードを管理する

//左上に戻るアイコンを表示 重なっているならindicate = -1;
void modoru_icon(int hand_x, int hand_y, int& indicate);
void sort_icon(int hand_x, int hand_y, int& indicate);
void item_switch_icon(int hand_x, int hand_y, int& indicate);

//インターバル中、デッキを編集するキャラを3人から決める
int deck_select(int kind[3], int stand_handle[10], int sound, int& focus);

//戦闘するキャラクター3人を決める
int character_select(int kind[3], int stand_handle[10], bool fixed[3], int s_kettei, int s_modoru, int& focus);

//衣装を決める
int huku_select(int* huku, int stand_handle[]);

//デッキと倉庫間でのカードのやり取り
int deck_create(Deck& deck, Deck& store, Deck& item, int sound);

//カードの売却
int card_sell(Deck& store, int& last, int sound, int money_sound);

//10人の生徒から1人を選ぶ
int character_custom(int stand_handle[10], int s_kettei, int s_modoru, int& focus);

//カードのステータスを表示する(上か下)
void picture_card_state(int h, int p, int s, int r, bool up = false);

int item_or_card(int s_kettei, int s_modoru, int& focus);

int sell_item_load(Card*& sell_item);

#endif