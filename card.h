#ifndef INCLUDED_CARD_H
#define INCLUDED_CARD_H

#include"skill.h"


#define DECK_MAX 5		//デッキにセットできるカードの最大数9
#define STORE_MAX 1000	//保管できるカードの最大数100

//実装済みのカード数
const int CARD_SUM[11] = { 3,3,3,3,3,3,3,3,3,3,0 };

enum {
	C_RARE,
	B_RARE,
	A_RARE,
	S_RARE
};

///////カード///////////
class Card {
protected:
	int kind;//カードの種類
	int rare;//カードのレアリティ
	int hp;//ステータス
	int power;//カードの効果の絶対値(攻撃力やデバフの威力)
	int speed;//カードを出してから発動までの時間
	int rigid;//硬直時間
	int handle;//画像ハンドル
	Skill* ability;//カードの効果kindに応じて派生クラスを入手
public:
	Card(int k = 0, int ra = 0, int h = 0, int p = 0, int s = 0, int r = 0);
	~Card();
	void null_ability();//セーブデータをロードしたときにのみ使う
	void operator=(const Card& other);//コピー用
	void set_kind(int x);//kindをセット
	void set_handle(int h);
	int get_kind();
	int get_hp();
	int get_power();
	int get_speed();
	int get_rigid();
	int get_rare();
	void load_card(bool load_flag = true);//画像のロード kindに応じてスキル取得
	int get_handle();
	Skill* get_ability();
	void trigger(Character* self, Character* enemy, int num, int target, int* handle);
	void Card::set_state(int h, int p, int s, int r);
	void set_rare(int c_rate, int b_rate, int a_rate, int s_rate);
public:
	bool update();//カードのアップデート
};

///////デッキはカードの集合体にすぎない//////////
class Deck {
private:
	int size;//デッキのカード枚数
	Card* cards;//デッキ本体
public:
	Deck();//コンストラクタでカードは一度空にする
	~Deck();
	void operator=(const Deck& other);//コピー用
	Card* get_cards();
	bool empty();
	void get_last(int& last);
	void sort();
	int partition(int left, int right);
	void quick_sort(int left, int right);
	int get_size();
	int get_hp();
	void set_size(int n);
	void set_card(Card& card, int i);
	void load_cards();
};

void reward_card(int kind, Card& c, int c_rate = 25, int b_rate = 25, int a_rate = 25, int s_rate = 25, int card_kind = -1);
int sell_money(int kind, int rare = C_RARE);
int buy_money(int kind);
void normal_status(int kind, Card& c);

#endif