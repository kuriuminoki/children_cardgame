#ifndef INCLUDED_SEQUENCE_H
#define INCLUDED_SEQUENCE_H

#include"battle.h"

class Bsequence {
private:
	int stand_handle[10];//10人分の立ち絵 設定した衣装を格納
	int s_kettei, s_modoru;//決定、戻る音
private:
	int focus;//操作しようとしているキャラのkind
	int kind[3];//生徒三人の種類
	bool fixed[3];//生徒の種類が固定ならtrue
private:
	Friend student[3];//BattleFieldへ渡す生徒の情報
	Enemy teacher[3];//missionクラスが用意する敵
	Deck st_deck[10];//生徒10人分のデッキ
	Deck store[10];//10人分のカード倉庫
	Deck item;//アイテム倉庫
	Deck after_deck[3];//バトルでアイテムなどを消費した後
	int remain_hp[3];//生徒の残りHP
private:
	BattleField* bf;//student teacherを渡して生成
private:
	void st_deck_set();//st_deckをstudentへコピー
	void st_kind_set(int another_kind[]);//kindをstudentへコピー 使ったキャラを知るためにanotherにも
	void dec_item();
public:
	Bsequence(Enemy te[3], int st_kind[3], bool st_fixed[3]);
	~Bsequence();
	//WINまたはLOSEを返す終わってないなら0 使ったキャラを知るためにkindも渡す
	int battle_loop(int another_kind[]);
	//報酬を受け取って終了処理
	void battle_end(int kind, Card& c);
};

#endif
