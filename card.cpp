#include"card.h"
#include"character.h"
#include"DxLib.h"
#include<map>
#include<utility>

//カードのステータスにアップデートを適用
bool Card::update() {
	switch (kind) {
	case 16:
		if (power < 10) {
			int h = hp, s = speed, r = rigid;
			set_state(1, 240, 1, 1);
			hp = h; speed = s; rigid = r;
			return true;
		}
		break;
	}
	return false;
}

void swap(Card* x, Card* y) {
	Card tmp;
	tmp = *x;
	*x = *y;
	*y = tmp;
}

int Deck::partition(int left, int right) {
	int i, j, pivot;
	i = left;
	j = right + 1;
	pivot = left;   // 先頭要素をpivotとする

	do {
		int p = cards[pivot].get_kind() * 10 - cards[pivot].get_rare();
		do { i++; } while (cards[i].get_kind() * 10 - cards[i].get_rare() < p);
		do { j--; } while (p < cards[j].get_kind() * 10 - cards[j].get_rare());
		// pivotより小さいものを左へ、大きいものを右へ
		if (i < j) { swap(&cards[i], &cards[j]); }
	} while (i < j);

	swap(&cards[pivot], &cards[j]);   //pivotを更新

	return j;
}

/* クイックソート */
void Deck::quick_sort(int left, int right) {
	int pivot;

	if (left < right) {
		pivot = partition(left, right);
		quick_sort(left, pivot - 1);   // pivotを境に再帰的にクイックソート
		quick_sort(pivot + 1, right);
	}
}

////////デッキ関連////////////

Deck::Deck() {
	size = 10;
	cards = new Card[size];
}

Deck::~Deck() {
	delete[] cards;
}

void Deck::operator=(const Deck& other) {
	size = other.size;
	Card* c = new Card[size];
	delete[] cards;
	cards = c;
	for (int i = 0; i < size; i++) {
		cards[i] = other.cards[i];//カードをコピー
	}
}

Card* Deck::get_cards() {
	return cards;
}

int Deck::get_size() {
	return size;
}

//デッキに一枚以上のカードがなければtrue アイテムはカードではない
bool Deck::empty() {
	for (int i = 0; i < size; i++) {
		if (cards[i].get_kind() > 0) {
			return false;
		}
	}
	return true;
}

//デッキのカードの枚数を決める
void Deck::set_size(int n) {
	size = n;
	if (size > 0) {
		Card* c = new Card[size];
		delete[] cards;
		cards = c;
	}
	for (int i = 0; i < size; i++) {
		cards[i].set_kind(0);//最初はカードが空の状態
	}
}

//カードを一枚、指定のインデックスのカードへコピー
void Deck::set_card(Card& card, int i) {
	if (i >= size || i < 0) { return; }
	cards[i] = card;
	//cards[i].load_card();
}

//デッキのカードをすべてロード　ただし画像は各種類一回ずつ
void Deck::load_cards() {
	int n=0;
	int sum;
	std::map<double, int> s;
	for (int i = 0; i < size; i++) {
		std::map<double, int>::iterator ite;
		double key = cards[i].get_kind() + (cards[i].get_rare() / 10.0);
		ite = s.find(key);
		if (ite == s.end()) {//新たに出現したカード
			cards[i].load_card();
			s.insert(std::make_pair(key, cards[i].get_handle()));
		}
		else {//既にロード済みのカード
			n++;
			cards[i].set_handle(ite->second);
			cards[i].load_card(false);
		}
		sum = i;
	}
	sum = 0;
	n = 0;
}

//デッキの合計HPを返す
int Deck::get_hp() {
	int hp = 0;
	for (int i = 0; i < size; i++) {
		if (cards[i].get_kind() != 0) {
			hp += cards[i].get_hp();
		}
	}
	return hp;
}

//デッキの末尾を調べる
void Deck::get_last(int& last) {
	last = size;//カードは満タン
	for (int i = 0; i < size; i++) {
		if (cards[i].get_kind() == 0) {
			last = i;//カードはインデックスlastが最初の空
			break;
		}
	}
}

void Deck::sort() {
	int last;
	get_last(last);
	quick_sort(0, last - 1);
}

//////////////////////////////////////
/////////各カード関連ここから/////////

Card::Card(int k, int ra, int h, int p, int s, int r) {
	kind = k;
	rare = ra;
	hp = h;
	power = p;
	speed = s;
	rigid = r;
	ability = NULL;
}

Card::~Card() {
	if (ability != NULL) {
		delete ability;
	}
}

//セーブデータをロードしたときにのみ使う
void Card::null_ability() {
	ability = NULL;
}

//カードのコピー
//これで初めてデッキのカードが種類付けされる
void Card::operator=(const Card& other) {
	hp = other.hp;
	kind = other.kind;
	rare = other.rare;
	power = other.power;
	speed = other.speed;
	rigid = other.rigid;
	if (ability == NULL) {
		load_card(false);
	}
	else {
		*ability = *other.ability;
	}
	handle = other.handle;
}

void Card::set_kind(int x) {
	kind = x;
}

void Card::set_handle(int h) {
	handle = h;
}

int Card::get_hp() {
	return hp;
}

int Card::get_power() {
	return power;
}

int Card::get_kind() {
	return kind;
}

int Card::get_speed() {
	return speed;
}

int Card::get_rigid() {
	return rigid;
}

int Card::get_handle() {
	return handle;
}

int Card::get_rare() {
	return rare;
}

Skill* Card::get_ability() {
	return ability;
}

void Card::trigger(Character* self, Character* enemy, int num, int target, int* handle) {
	ability->skill_trigger(self, enemy, num, target, handle);
	if (kind < 0) {
		kind = 0;//アイテム消費
	}
}

const int SPEED_MIN = 10;
//カードのステータスを乱数も含めて一括セットする
void Card::set_state(int h, int p, int s, int r) {
	double ra;
	double add;
	switch (rare) {
	case C_RARE:
		add = 1.0;
		break;
	case B_RARE:
		add = 1.4;
		break;
	case A_RARE:
		add = 1.4 * 1.4;
		break;
	case S_RARE:
		add = 1.4 * 1.4 * 1.4;
		break;
	}

	ra = 1.0 + (GetRand(49) + 1) / 100.0;//1.0～1.5倍
	hp = (int)(h * ra * add);

	ra = 1.0 + (GetRand(49) + 1) / 100.0;//1.0～1.5倍
	power = (int)(p * ra * add);

	ra = 1.0 + (GetRand(49) + 1) / 100.0;//1.0～1.5倍
	speed = (int)(s * ra - (add-1.0)*10);
	if (speed < SPEED_MIN) { speed = SPEED_MIN; }//負の値にはならない

	ra = 1.0 + (GetRand(49) + 1) / 100.0;//1.0～1.5倍
	rigid = (int)(r * ra - (add-1.0)*10);
	if (rigid < SPEED_MIN) { rigid = SPEED_MIN; }
}

//与えられた確率をレアリティに適用してセットする
void Card::set_rare(int c_rate, int b_rate, int a_rate, int s_rate) {
	int sum = c_rate + b_rate + a_rate + s_rate;
	int r = GetRand(sum-1);
	if (r < c_rate) {
		rare = C_RARE;
	}
	else if (r < c_rate + b_rate) {
		rare = B_RARE;
	}
	else if (r < c_rate + b_rate + a_rate) {
		rare = A_RARE;
	}
	else {
		rare = S_RARE;
	}
}

//キャラの種類に応じてランダムもしくはcard_kindでカードの種類を返す
void reward_card(int kind, Card& c, int c_rate, int b_rate, int a_rate, int s_rate, int card_kind) {
	c.set_rare(c_rate, b_rate, a_rate, s_rate);
	if (card_kind > 0) {
		normal_status(card_kind, c);
	}
	else {
		//例えばアカツキ(kind == 1)ならカードの種類は 1, 11, 21, 31...
		int r = GetRand(CARD_SUM[kind - 1] - 1) * 10 + kind;
		c.set_kind(r);
		normal_status(r, c);
	}
}

//各カードの売却金額
int sell_money(int kind, int rare) {
	if (kind == 0)return 0;
	switch (kind) {
	case -1:
		return 10;
	case -2:
	case -3:
		return 50;
	}
	if (kind >= 1 && kind <= 30) {
		return 10 * (rare+1) * (rare+1);
	}
	return 0;
}

//ショップで買うのに必要な金額（アイテムのみ）
int buy_money(int kind) {
	switch (kind) {
	case -1:
		return 200;
		break;
	case -2:
	case -3:
		return 500;
		break;
	}
	return 0;
}

//カードの基本ステータス
void normal_status(int kind, Card& c) {
	if (kind == 0)return;
	switch (kind) {
	//アイテム
	case -1://回復薬
		c.set_state(0, 80, 30, 30);
		break;
	case -2://シロナ
		c.set_state(0, 80, 50, 10);
		break;
	case -3://クロエ
		c.set_state(0, 80, 50, 10);
		break;
	//アカツキ
	case 1:
		c.set_state(100, 30, 100, 115);
		break;
	case 11:
		c.set_state(100, 55, 95, 110);
		break;
	case 21:
		c.set_state(100, 27, 90, 110);
		break;
	//ハンダ
	case 2:
		c.set_state(100, 50, 60, 70);
		break;
	case 12:
		c.set_state(100, 20, 100, 120);
		break;
	case 22:
		c.set_state(100, 50, 100, 125);
		break;
	//オオタ
	case 3:
		c.set_state(110, 70, 100, 120);
		break;
	case 13:
		c.set_state(110, 240, 50, 30);
		break;
	case 23:
		c.set_state(110, 90, 125, 140);
		break;
	//イシ
	case 4:
		c.set_state(100, 240, 100, 150);
		break;
	case 14:
		c.set_state(100, 15, 90, 120);
		break;
	case 24:
		c.set_state(100, 25, 85, 90);
		break;
	//タイラ
	case 5:
		c.set_state(97, 50, 65, 85);
		break;
	case 15:
		c.set_state(100, 240, 40, 90);
		break;
	case 25:
		c.set_state(115, 65, 110, 130);
		break;
	//オワダ
	case 6:
		c.set_state(100, 30, 120, 150);
		break;
	case 16:
		c.set_state(100, 240, 60, 60);//c.set_state(100, 1, 60, 60);
		break;
	case 26:
		c.set_state(100, 60, 90, 90);
		break;
	//ミヤトネ
	case 7:
		c.set_state(120, 40, 60, 70);
		break;
	case 17:
		c.set_state(120, 180, 70, 90);
		break;
	case 27:
		c.set_state(120, 50, 113, 125);
		break;
	//ウチモト
	case 8:
		c.set_state(100, 33, 50, 60);
		break;
	case 18:
		c.set_state(100, 45, 25, 80);
		break;
	case 28:
		c.set_state(100, 50, 50, 83);
		break;
	//イバラカワ
	case 9:
		c.set_state(95, 180, 50, 32);
		break;
	case 19:
		c.set_state(100, 240, 63, 70);
		break;
	case 29:
		c.set_state(90, 60, 70, 100);
		break;
	//トモチ
	case 10:
		c.set_state(105, 50, 63, 85);
		break;
	case 20:
		c.set_state(105, 65, 95, 105);
		break;
	case 30:
		c.set_state(105, 55, 85, 95);
		break;
	}
}

//カードのロード kindの一の位でキャラを判断すること
void Card::load_card(bool load_flag) {
	std::string path;
	switch (rare) {
	case C_RARE:
		path = "card/C/";
		break;
	case B_RARE:
		path = "card/B/";
		break;
	case A_RARE:
		path = "card/A/";
		break;
	case S_RARE:
		path = "card/S/";
		break;
	}
	if (kind == 0)return;
	switch (kind) {
	//アイテム
	case -1:
		ability = new HpRepairItem;
		ability->set_text("使用者の体力を回復");
		if (load_flag)handle = LoadGraph("card/item/回復薬.png");
		break;
	case -2:
		ability = new Shirona;
		ability->set_text("シロナが攻撃をおこない、使用者の防御力が60上昇する");
		if (load_flag)handle = LoadGraph("card/item/シロナ.png");
		break;
	case -3:
		ability = new Kuroe;
		ability->set_text("クロエが攻撃をおこない、使用者の攻撃力が60上昇する");
		if (load_flag)handle = LoadGraph("card/item/クロエ.png");
		break;
	/////アカツキ/////
	case 1:
		ability = new DamageDouble1;
		ability->set_text("敵1体への通常攻撃(2回)");
		path += "アカツキ1.png";
		break;
	case 11:
		ability = new Damage2;
		ability->set_text("敵1体への防御力を無視した攻撃");
		path += "アカツキ2.png";
		break;
	case 21:
		ability = new DamageDouble2;
		ability->set_text("敵1体への防御力を無視した攻撃(2回)");
		path += "アカツキ3.png";
		break;
	/////ハンダ/////
	case 2:
		ability = new AttackUpAll_5sec;
		ability->set_text("味方全体の攻撃力上昇(5秒間)");
		path += "ハンダ1.png";
		break;
	case 12:
		ability = new HpRepair;
		ability->set_text("味方全体の体力を回復");
		path += "ハンダ2.png";
		break;
	case 22:
		ability = new Damage1;
		ability->set_text("敵1体への通常攻撃");
		path += "ハンダ3.png";
		break;
	/////オオタ/////
	case 3:
		ability = new Damage1;
		ability->set_text("敵1体へ強めの通常攻撃");
		path += "オオタ1.png";
		break;
	case 13:
		ability = new EndureSelf;
		ability->set_text("攻撃をキャンセルされず、防御力とスピードが３０％アップ");
		path += "オオタ2.png";
		break;
	case 23:
		ability = new Damage1;
		ability->set_text("敵1体へかなり強い通常攻撃");
		path += "オオタ3.png";
		break;
	/////イシ/////
	case 4:
		ability = new Paralysis;
		ability->set_text("敵1体にマヒを付与");
		path += "イシ1.png";
		break;
	case 14:
		ability = new Poison_5sec;
		ability->set_text("敵1体に5秒間の毒を付与");
		path += "イシ2.png";
		break;
	case 24:
		ability = new SpeedUpAll_5sec;
		ability->set_text("味方全体のスピード上昇(5秒間)");
		path += "イシ3.png";
		break;
	/////タイラ/////
	case 5:
		ability = new AttackDown_5sec;
		ability->set_text("敵1体の攻撃力減少(5秒間)");
		path += "タイラ1.png";
		break;
	case 15:
		ability = new HideSelf;
		ability->set_text("敵に狙われにくくなり、攻撃力とスピードが３０％アップ");
		path += "タイラ2.png";
		break;
	case 25:
		ability = new Damage1;
		ability->set_text("敵1体への通常攻撃");
		path += "タイラ3.png";
		break;
	/////オワダ/////
	case 6:
		ability = new DamageAll;
		ability->set_text("敵全体への通常攻撃");
		path += "オワダ2.png";
		break;
	case 16:
		ability = new StateRepair;
		ability->set_text("有益な状態異常を持たない味方全員に予防接種を付与");
		path += "オワダ1.png";
		break;
	case 26:
		ability = new Damage1;
		ability->set_text("敵1体への通常攻撃");
		path += "オワダ3.png";
		break;
	/////ミヤトネ
	case 7:
		ability = new DeffenseUpAll_5sec;
		ability->set_text("味方全体の防御力上昇(5秒間)");
		path += "ミヤトネ1.png";
		break;
	case 17:
		ability = new ProvocationSelf_30;
		ability->set_text("敵に狙われやすくなり、ダメージは３０％カットされる");
		path += "ミヤトネ2.png";
		break;
	case 27:
		ability = new Damage1;
		ability->set_text("敵1体への通常攻撃");
		path += "ミヤトネ3.png";
		break;
	/////ウチモト/////
	case 8:
		ability = new SpeedDown_5sec;
		ability->set_text("敵1体のスピード減少(5秒間)");
		path += "ウチモト1.png";
		break;
	case 18:
		ability = new Damage1;
		ability->set_text("敵1体へのかなり素早い通常攻撃");
		path += "ウチモト2.png";
		break;
	case 28:
		ability = new Damage1;
		ability->set_text("敵1体への素早い通常攻撃");
		path += "ウチモト3.png";
		break;
	/////イバラカワ/////
	case 9:
		ability = new AvoidSelf;
		ability->set_text("一定時間攻撃を70%%の確率で回避");
		path += "イバラカワ3.png";
		break;
	case 19:
		ability = new CounterSelf;
		ability->set_text("一定時間敵からの攻撃に1.2倍で反撃する");
		path += "イバラカワ2.png";
		break;
	case 29:
		ability = new Damage1;
		ability->set_text("敵1体への通常攻撃");
		path += "イバラカワ1.png";
		break;
	/////トモチ/////
	case 10:
		ability = new DeffenseDown_5sec;
		ability->set_text("敵1体の防御力減少(5秒間)");
		path += "トモチ1.png";
		break;
	case 20:
		ability = new Damage1;
		ability->set_text("敵1体へ少し強めの通常攻撃");
		path += "トモチ2.png";
		break;
	case 30:
		ability = new Damage1;
		ability->set_text("敵1体へ少し素早い通常攻撃");
		path += "トモチ3.png";
		break;
	}
	if (load_flag && kind > 0)handle = LoadGraph(path.c_str());
	ability->set_power(power);
}