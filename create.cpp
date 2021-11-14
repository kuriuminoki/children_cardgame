#include"create.h"
#include"menu.h"
#include"control.h"
#include"DxLib.h"
#include<stdio.h>

#define MAX_PICTURE_CARD 10//一画面で縦に表示できるカードの最大数

void set_swap(int indicate, Card*& c, Deck* deck, Deck* store, int sound);
void swap_card(Card* first, Card* second);
bool swap_able(bool first_deck, bool second_deck, bool item_flag, Card& first, Card& second);
void card_text(char c[], const char* str);
void up_down(int hand_x, int hand_y, int& indicate, bool up, bool down);
void insert_drop(int hand_x, int hand_y, int& indicate, int num, Deck& deck, Deck& store, int deck_last, int store_last);
void cards_bring(Deck& cards, int erase_index);
int deck_item_create(Deck& deck, Deck& store, int sound, bool item_flag);

bool save_other(int* day, bool clear[], int today_mission[]) {
	FILE *fp = fopen("セーブデータ/other.dat", "wb");
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
	FILE *fp = fopen("セーブデータ/money.dat", "wb");
	if (fp == NULL) {
		return false;
	}
	fwrite(money, sizeof(int), 1, fp);
	fclose(fp);
	return true;
}

bool save_deck(Deck* deck) {
	FILE *fp = fopen("セーブデータ/deck.dat", "wb");
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
	FILE *fp = fopen("セーブデータ/store.dat", "wb");
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
	FILE *fp = fopen("セーブデータ/item.dat", "wb");
	if (fp == NULL) {
		return false;
	}
	fwrite(item.get_cards(), sizeof(Card), STORE_MAX, fp);
	fclose(fp);
	return true;
}

//お金とかミッションロード
bool load_money(int* money) {
	FILE *fp = fopen("セーブデータ/money.dat", "rb");
	if (fp == NULL) {
		return false;
	}
	fread(money, sizeof(int), 1, fp);
	fclose(fp);
	return true;
}

bool load_other(int* day, bool clear[], int today_mission[]) {
	FILE *fp = fopen("セーブデータ/other.dat", "rb");
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

//アイテム倉庫ロード
bool load_item(Deck& item) {
	item.set_size(STORE_MAX);
	FILE *fp = fopen("セーブデータ/item.dat", "rb");
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
	//item.set_card(Card(-1, 0, 0, 20, 30, 30), 0);//テスト用
	//item.set_card(Card(-2, 0, 0, 22, 30, 30), 1);//テスト用
	//item.set_card(Card(-3, 0, 0, 25, 30, 30), 2);//テスト用
	return true;
}
//デッキのカードロード
bool load_deck(Deck* deck) {
	for (int i = 0; i < 10; i++) {
		deck[i].set_size(DECK_MAX);
	}
	FILE *fp = fopen("セーブデータ/deck.dat", "rb");
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
	////空白を敷き詰める
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
//倉庫のカードを10人分ロードする
bool load_store(Deck* store) {
	for (int i = 0; i < 10; i++) {
		store[i].set_size(STORE_MAX);
	}
	FILE *fp = fopen("セーブデータ/store.dat", "rb");
	if (fp == NULL) {
		return false;
	}
	for (int i = 0; i < 10; i++) {
		Card* c = new Card[STORE_MAX];
		fread(c, sizeof(Card), STORE_MAX, fp);
		for (int k = 0; k < STORE_MAX; k++) {
			c[k].update();
			store[i].set_card(c[k], k);
			store[i].get_cards()[k].null_ability();//null_abilityを呼ぶのはここだけ
			c[k].null_ability();////null_abilityを呼ぶのはここだけ
		}
		delete[] c;
	}
	fclose(fp);
	return true;
	//以下テスト
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

//立ち絵をロード(デフォルトの画像)
void load_stand(int stand_handle[10]) {
	stand_handle[0] = LoadGraph("stand/アカツキ.png");
	stand_handle[1] = LoadGraph("stand/ハンダ.png");
	stand_handle[2] = LoadGraph("stand/オオタ.png");
	stand_handle[3] = LoadGraph("stand/イシ.png");
	stand_handle[4] = LoadGraph("stand/タイラ.png");
	stand_handle[5] = LoadGraph("stand/オワダ.png");
	stand_handle[6] = LoadGraph("stand/ミヤトネ.png");
	stand_handle[7] = LoadGraph("stand/ウチモト.png");
	stand_handle[8] = LoadGraph("stand/イバラカワ.png");
	stand_handle[9] = LoadGraph("stand/トモチ.png");
	//テスト
	//for (int i = 0; i < 10; i++) { stand_handle[i] = LoadGraph("stand/トモチicon.png"); }
}

//デッキ、倉庫、立ち絵を一括ロードする
void load_cards_stand(Deck*& store, Deck*& deck, Deck*& item, int*& stand_handle) {
	store = new Deck[10];
	deck = new Deck[10];
	item = new Deck;
	stand_handle = new int[10];
	load_stand(stand_handle);
	load_store(store);//倉庫のデータをロード(セーブデータから)
	load_deck(deck);//デッキのカードをロードする(セーブデータから)
	load_item(*item);
	for (int i = 0; i < 10; i++) {
		store[i].load_cards();//倉庫のカードの画像などをロードする
		deck[i].load_cards();
	}
	item->load_cards();//アイテム倉庫のカードをロードする
}

//左上に戻るアイコンを表示 マウスの重なりも確認する 重なっているならindicate = -1;
void modoru_icon(int hand_x, int hand_y, int& indicate) {
	if (hand_x >= 5 && hand_x <= 60 && hand_y >= 5 && hand_y <= 35) {
		indicate = -1;
		DrawBox(2, 2, 63, 38, GetColor(0, 255, 0), TRUE);
	}
	DrawBox(5, 5, 60, 35, GetColor(100, 100, 255), TRUE);
	DrawFormatString(10, 15, GetColor(0, 0, 0), "戻る");
}

//右上に倉庫とアイテム変換アイコンを表示 マウスの重なりも確認する 重なっているならindicate = -6;
void item_switch_icon(int hand_x, int hand_y, int& indicate) {
	if (hand_x >= 450 && hand_x <= 620 && hand_y >= 5 && hand_y <= 35) {
		indicate = -6;
		DrawBox(447, 2, 623, 38, GetColor(0, 255, 0), TRUE);
	}
	DrawBox(450, 5, 620, 35, GetColor(100, 100, 255), TRUE);
	DrawFormatString(450, 15, GetColor(0, 0, 0), "アイテム倉庫切り替え");
}

//右上に倉庫のソートアイコンを表示 マウスの重なりも確認する 重なっているならindicate = -7;
void sort_icon(int hand_x, int hand_y, int& indicate) {
	if (hand_x >= 320 && hand_x <= 420 && hand_y >= 15 && hand_y <= 35) {
		indicate = -7;
		DrawBox(317, 12, 423, 37, GetColor(0, 255, 0), TRUE);
	}
	DrawBox(320, 15, 420, 35, GetColor(100, 100, 255), TRUE);
	DrawFormatString(340, 15, GetColor(0, 0, 0), "ソート");
}

//デッキを編集するキャラを決める（戦闘中）決まってないなら0 決まったなら1 デッキ編集へ以降は2
int deck_select(int kind[3], int stand_handle[10], int sound, int& focus) {
	int indicate = 0;//マウスカーソルの重なっている部分は？
	int hand_x, hand_y;//マウスカーソルの座標
	GetMousePoint(&hand_x, &hand_y);
	int size_x, size_y;//画像のサイズ
	GetGraphSize(stand_handle[0], &size_x, &size_y);
	size_x = (int)(size_x * 0.75);	size_y = (int)(size_y * 0.75);
	DrawBox(0, 0, 640, 480, GetColor(100, 100, 100), TRUE);//背景
	DrawFormatString(180, 5, GetColor(255, 0, 0), "インターバル　デッキを編集可能");
	DrawFormatString(200, 25, GetColor(0, 0, 0), "注：HPの変化は適用されない");
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
		DrawFormatString(x + 25, y + size_x + 100, GetColor(0, 0, 0), "デッキを編集");
	}

	int c = left_click();
	if (c == 1) {//右クリックが押された瞬間
		if (indicate == -1) {
			PlaySoundMem(sound, DX_PLAYTYPE_BACK);//音
			return 1;
		}
		if (indicate > 0) {
			focus = kind[indicate - 1];
			PlaySoundMem(sound, DX_PLAYTYPE_BACK);//音
			return 2;
		}
	}

	return 0;
}

//キャラクターを決める 決まってないなら0 決まったなら1 服選びに移行なら2 デッキ構築なら3 キャンセルは-1
int character_select(int kind[3], int stand_handle[10], bool fixed[3], int s_kettei, int s_modoru, int& focus) {
	static int member = -1;//変更しようとしているメンバー
	static int wait = -1;//変更しようとしている待機生徒
	int indicate = 0;//マウスカーソルの重なっている部分は？服:-4 デッキ:-3 戻る:-2 確定:-1 選択1-3 待機4-14
	int hand_x, hand_y;//マウスカーソルの座標
	GetMousePoint(&hand_x, &hand_y);
	int size_x, size_y;//画像のサイズ
	GetGraphSize(stand_handle[0], &size_x, &size_y);
	DrawBox(0, 0, 640, 480, GetColor(100, 100, 100), TRUE);//背景
	DrawFormatString(70, 5, GetColor(255, 0, 0), "キャラクター選択");
	DrawFormatString(250, 5, GetColor(0, 0, 0), "右クリックでキャラを外す");
	modoru_icon(hand_x, hand_y, indicate);
	if (hand_x >= 580 && hand_x <= 635 && hand_y >= 5 && hand_y <= 35) {
		indicate = -2;//確定に重なっている
		DrawBox(577, 2, 638, 38, GetColor(0, 255, 0), TRUE);
	}
	DrawBox(580, 5, 635, 35, GetColor(100, 100, 255), TRUE);
	DrawFormatString(585, 15, GetColor(0, 0, 0), "確定");
	//決める3人
	size_x = (int)(size_x * 0.75);	size_y = (int)(size_y * 0.75);
	for (int i = 0; i < 3; i++) {
		int x, y;
		x = 60 + 180 * i; y = 50;
		bool overlap = hand_x >= x && hand_x <= x + size_x && hand_y >= y && hand_y <= y + size_y;
		//重なっているかどうか
		if (!fixed[i] && overlap) {
			indicate = i + 1;
			DrawBox(x - 3, y - 3, x + 3 + size_x, y + 3 + size_y, GetColor(0, 255, 0), TRUE);
		}
		if (member == i) {//選択中
			DrawBox(x - 3, y - 3, x + 3 + size_x, y + 3 + size_y, GetColor(255, 0, 0), TRUE);
		}
		if (fixed[i]) { DrawFormatString(x+30, y - 15, GetColor(255,0,0), "固定キャラ"); }
		DrawBox(x, y, x + size_x, y + size_y, GetColor(0,0,0), TRUE);
		if (kind[i] > 0 && kind[i] <= 10) {
			DrawRotaGraph2(x, y, 0, 0, 0.75, 0, stand_handle[kind[i] - 1], TRUE, FALSE);
		}
		if (overlap && kind[i] != 0) {//服装やデッキ編集
			if (hand_x >= x + 10 && hand_x <= x - 10 + size_x && hand_y >= y + 100 && hand_y <= y + 120) {
				indicate = -3;//デッキ編集
				focus = kind[i];//焦点を当てられているキャラは？
				DrawBox(x + 8, y + 98, x - 8 + size_x, y + 122, GetColor(255, 0, 0), TRUE);
			}
			DrawBox(x + 10, y + 100, x - 10 + size_x, y + 120, GetColor(100, 100, 100), TRUE);
			DrawFormatString(x + 15, y + 102, GetColor(0, 0, 0), "デッキ編集");
			if (hand_x >= x + 10 && hand_x <= x - 10 + size_x && hand_y >= y + 130 && hand_y <= y + 150) {
				indicate = -4;//服装変更
				focus = kind[i];//焦点を当てられているキャラは？
				DrawBox(x + 8, y + 128, x - 8 + size_x, y + 152, GetColor(255, 0, 0), TRUE);
			}
			DrawBox(x + 10, y + 130, x - 10 + size_x, y + 150, GetColor(100, 100, 100), TRUE);
			DrawFormatString(x + 15, y + 132, GetColor(0, 0, 0), "服装変更");
		}
	}
	//選択肢の10人
	GetGraphSize(stand_handle[0], &size_x, &size_y);
	size_x = (int)(size_x * 0.4);	size_y = (int)(size_y * 0.35);
	for (int i = 0; i < 10; i++) {
		int x, y;
		if (i < 5) {//上の行に配置
			x = 80 + 100 * i;
			y = 270;
		}
		else {//下の行に配置
			x = 80 + 100 * (i - 5);
			y = 370;
		}
		bool used = false;
		for (int j = 0; j < 3; j++) {
			if (kind[j] == i + 1) {
				used = true;//パーティーに入っているメンバー
			}
		}
		//重なっているかどうか
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
			PlaySoundMem(s_modoru, DX_PLAYTYPE_BACK);//音
		}
	}
	if (c == 1) {//右クリックが押された瞬間
		if (indicate < 0) {
			member = -1;
			wait = -1;
		}
		if (indicate == -2) { //確定
			if (kind[0] == 0 && kind[1] == 0 && kind[2] == 0) { return 0; }
			PlaySoundMem(s_kettei, DX_PLAYTYPE_BACK);//音
			return 1;
		}
		if (indicate == -1) {//戻る
			//PlaySoundMem(s_modoru, DX_PLAYTYPE_BACK);//音
			return -1;
		}
		if (indicate == -3) {//デッキ編集
			PlaySoundMem(s_kettei, DX_PLAYTYPE_BACK);//音
			return 3;
		}
		if (indicate == -4) {//服装変更
			PlaySoundMem(s_kettei, DX_PLAYTYPE_BACK);//音
			return 2;
		}
		if (indicate > 0 && indicate < 4) {//入れ替えようとする生徒の変更
			if (indicate - 1 == member) {
				member = -1;
				PlaySoundMem(s_modoru, DX_PLAYTYPE_BACK);//音
			}
			else {
				member = indicate - 1;
				PlaySoundMem(s_kettei, DX_PLAYTYPE_BACK);//音
			}
		}
		if (indicate >= 4 && indicate <= 13) {//入れ替えようとする生徒の変更
			if (indicate - 4 == wait) {//同じ、解除
				wait = -1;
				PlaySoundMem(s_modoru, DX_PLAYTYPE_BACK);//音
			}
			else {
				wait = indicate - 4;//別の待機生徒へ
				PlaySoundMem(s_kettei, DX_PLAYTYPE_BACK);//音
			}
		}
	}
	if (member != -1 && wait != -1) {//入れ替え
		kind[member] = wait + 1;
		member = -1;
		wait = -1;
	}
	return 0;
}

//キャラ1体の服を決める 決まってないなら0 決まったなら1
int huku_select(int* huku, int stand_handle[]) {

	return 0;
}

//カードのステータスを表示する座標
const int TEXT_X = 10;
const int TEXT_Y = 400;
//カードのステータスを上または下へ表示
void picture_card_state(int h, int p, int s, int r, bool up) {
	int d = 0;
	if (up) { 
		d = 80;
		DrawFormatString(10, TEXT_Y - d - 20, GetColor(0, 0, 0), "選択中のカード");
		DrawBox(0, TEXT_Y - d, 640, 480 - d, GetColor(255, 0, 0), TRUE);
	}
	DrawBox(0+1, TEXT_Y - d+1, 640-1, TEXT_Y + 20 - d, GetColor(255, 255, 255), TRUE);//カードステータス
	DrawBox(0+1, TEXT_Y + 20 - d, 640-1, 480 - d-1, GetColor(255, 255, 100), TRUE);//カード効果

	DrawFormatString(TEXT_X, TEXT_Y + 2 - d, GetColor(0, 0, 0), "HP:");
	DrawFormatString(TEXT_X + 160, TEXT_Y + 2 - d, GetColor(0, 0, 0), "POWER:");
	DrawFormatString(TEXT_X + 320, TEXT_Y + 2 - d, GetColor(0, 0, 0), "SPEED:");
	DrawFormatString(TEXT_X + 480, TEXT_Y + 2 - d, GetColor(0, 0, 0), "RIGID:");

	DrawFormatString(TEXT_X + 30, TEXT_Y + 2 - d, GetColor(255, 0, 0), "%d", h);
	DrawFormatString(TEXT_X + 60 + 160, TEXT_Y + 2 - d, GetColor(255, 0, 0), "%d", p);
	DrawFormatString(TEXT_X + 60 + 320, TEXT_Y + 2 - d, GetColor(255, 0, 0), "%d", s);
	DrawFormatString(TEXT_X + 60 + 480, TEXT_Y + 2 - d, GetColor(255, 0, 0), "%d", r);
}

//交換予定のカードにセットする
void set_swap(int indicate, Card*& c, Deck* deck, Deck* store, int sound) {
	if (indicate < 10) {
		if (indicate - 1 < deck->get_size() && deck->get_cards()[indicate - 1].get_kind() != 0) {
			c = &deck->get_cards()[indicate - 1];
			PlaySoundMem(sound, DX_PLAYTYPE_BACK);//音
		}
	}
	else {
		if (indicate - 10 < store->get_size() && store->get_cards()[indicate - 10].get_kind() != 0) {
			c = &store->get_cards()[indicate - 10];
			PlaySoundMem(sound, DX_PLAYTYPE_BACK);//音
		}
	}
}

//カードを交換できるかどうか
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

//カードfirst secondを入れ替える
void swap_card(Card* first, Card* second) {
	Card tmp;
	tmp = *first;
	*first = *second;
	*second = tmp;
}

//カードの効果テキストを適当な長さで切る
void card_text(char c[], const char* str) {
	for (int j = 0; j < 22; j++) {//コピーしていく
		c[j] = str[j];
		if (c[j] == '\0')break;
	}
	c[22] = '\0';//入りきらないので途中で切る
}

//倉庫のカードスクロールバー
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
	DrawFormatString(415, 80, GetColor(0, 0, 0), "↑");
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
	DrawFormatString(415, 210, GetColor(0, 0, 0), "↓");
}

//挿入or排除のボタン
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
		DrawFormatString(10,270,GetColor(0,0,0),"倉庫へ移動");
	}
	else {
		DrawBox(218, 268, 412, 292, GetColor(0, 0, 0), TRUE);
		if (deck_last < deck.get_size()) {//挿入できる空がある
			if (hand_x >= 220 && hand_x <= 410 && hand_y >= 270 && hand_y <= 290) {
				DrawBox(218, 268, 412, 292, GetColor(0, 255, 0), TRUE);
				indicate = -5;
			}
			DrawBox(220, 270, 410, 290, GetColor(150, 150, 150), TRUE);
		}
		else {
			DrawBox(220, 270, 410, 290, GetColor(50, 50, 50), TRUE);
		}
		DrawFormatString(220, 270, GetColor(0, 0, 0), "デッキの最後へ挿入");
	}
}

//カードを一枚削除して寄せる
void cards_bring(Deck& cards, int erase_index) {//erase_indexのカードを消すように後のカードを寄せる
	for (int i = erase_index + 1; i < cards.get_size(); i++) {
		if (cards.get_cards()[i].get_kind() == 0) {
			cards.get_cards()[i - 1].set_kind(0);//ひとつ前のカードは消えて、終わり
			return;
		}
		cards.get_cards()[i - 1] = cards.get_cards()[i];//前のカードへコピー
	}
	cards.get_cards()[cards.get_size() - 1].set_kind(0);//末尾のカードは消えて、終わり
}

//デッキとストアでカードの入れ替え 決まってないなら0 決まったなら1
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
//デッキとストアでカードの入れ替え 決まってないなら0 決まったなら1 アイテム倉庫切り替えは-1
int deck_item_create(Deck& deck, Deck& store, int sound, bool item_flag) {
	static int deck_last = -1;//空になっている最初のインデックスは？
	static int store_last = -1;//空になっている最初のインデックスは？
	if (deck_last == -1) {
		deck.get_last(deck_last);
		store.get_last(store_last);
	}
	const int CARD_X = 550;
	const int CARD_Y = 150;
	static Card* first = NULL; static Card* second = NULL;//交換するカードのポインタ
	static bool first_deck = false, second_deck = false;//交換するカードがデッキか倉庫か?
	int first_index;
	static int store_i = 0;	//一番上に表示されるカードのインデックス
	int indicate = 0;//指し示しているのものの番号0はなし
	int hand_x, hand_y;//マウスカーソルの座標
	GetMousePoint(&hand_x, &hand_y);
	DrawBox(0, 0, 640, 480, GetColor(100, 100, 100), TRUE);//背景
	DrawFormatString(470, 270, GetColor(0, 0, 0), "デッキの合計HP");
	DrawFormatString(470, 290, GetColor(0, 0, 0), "%d", deck.get_hp());
	//戻るアイコン
	modoru_icon(hand_x, hand_y, indicate);
	item_switch_icon(hand_x, hand_y, indicate);
	sort_icon(hand_x, hand_y, indicate);
	up_down(hand_x, hand_y, indicate, store_i > 0, store_i < store_last - STORE_PAGE);
	DrawFormatString(150, 20, GetColor(0, 0, 0), "デッキ");//カードの効果
	DrawFormatString(220, 20, GetColor(0, 0, 0), "倉庫:%d枚", store_last);//カードの効果
	DrawBox(5, 37, 208, 263 - (STORE_PAGE - DECK_MAX)*25, GetColor(150, 150, 150), TRUE);
	//デッキと倉庫のリストを9枚ずつ表示
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
		//カーソルが重なっているとき
		if (hand_x >= x && hand_x <= x + 190 && hand_y >= y && hand_y <= y + 20) {
			indicate = now + index + init;//指し示しているカード Deck:1～9 Store:10～
			DrawBox(x - 3, y - 3, x + 193, y + 23, GetColor(0, 255, 0), TRUE);
		}
		//リスト表示
		if (&d->get_cards()[now + init] == first) {//選択中のカードは赤く囲んで強調
			DrawBox(x - 3, y - 3, x + 193, y + 23, GetColor(255, 0, 0), TRUE);
			//挿入or排除のボタン表示
			if (i >= DECK_MAX || ((!item_flag && first->get_kind() > 0) || (item_flag && first->get_kind() < 0))) {
				insert_drop(hand_x, hand_y, indicate, i, deck, store, deck_last, store_last);
			}
			first_index = now + init;//first のインデックス
		}
		DrawBox(x, y, x + 190, y + 20, GetColor(100,100,255), TRUE);
		Card* card = &d->get_cards()[now + init];
		if (now < d->get_size() && card->get_kind() != 0) {
			const char* str;
			str = card->get_ability()->get_text();//カードのテキスト
			char c[23];
			card_text(c, str);//cにstrを入りきる部分だけコピー
			DrawFormatString(x+2, y+2, GetColor(0, 0, 0), c);
			if (indicate == now + index + init) {//カーソルが重なっているときカードの情報を表示
				int h, p, s, r;
				h = card->get_hp(); p = card->get_power(); s = card->get_speed(); r = card->get_rigid();
				//SetDrawMode(DX_DRAWMODE_NEAREST);
				DrawRotaGraph(CARD_X, CARD_Y, 1.5, 0, card->get_handle(), TRUE, FALSE);//カードの画像
				//SetDrawMode(DX_DRAWMODE_BILINEAR);
				picture_card_state(h, p, s, r);
				DrawFormatString(TEXT_X, TEXT_Y + 40, GetColor(0, 0, 0), str);//カードの効果
			}
		}
	}
	if (first != NULL) {//選択中のカード表示
		int h, p, s, r;
		h = first->get_hp(); p = first->get_power(); s = first->get_speed(); r = first->get_rigid();
		const char* str;
		str = first->get_ability()->get_text();
		picture_card_state(h, p, s, r, true);
		DrawFormatString(TEXT_X, TEXT_Y - 40, GetColor(0, 0, 0), str);//カードの効果
	}

	int c = left_click();
	if (c == 1) {//左クリックが押された瞬間
		if (indicate < 0) {
			PlaySoundMem(sound, DX_PLAYTYPE_BACK);//音
		}
		if (indicate == -1 && !deck.empty()) {//戻るボタン
			store_i = 0; 
			first = NULL; second = NULL;
			first_deck = false; second_deck = false;
			deck_last = -1; store_last = -1;
			return 1;
		}
		else if (indicate == -2) {//上へスクロール
			store_i--;
		}
		else if (indicate == -3) {//下へスクロール
			store_i++;
		}
		else if (indicate == -4) {//デッキから倉庫へカードを移動
			store.get_cards()[store_last] = *first;//挿入（コピー）
			cards_bring(deck, first_index);
			deck_last = -1; store_last = -1; first = NULL;
		}
		else if (indicate == -5) {//倉庫からデッキへ挿入
			deck.get_cards()[deck_last] = *first;//挿入（コピー）
			cards_bring(store, first_index);
			deck_last = -1; store_last = -1; first = NULL;
		}
		else if (indicate == -6) {
			store_i = 0; 
			first = NULL; second = NULL;
			first_deck = false; second_deck = false;
			deck_last = -1; store_last = -1;
			return -1;//いったんリセットしてアイテム倉庫切り替え
		}
		else if (indicate == -7) {
			store_i = 0;
			first = NULL; second = NULL;
			first_deck = false; second_deck = false;
			deck_last = -1; store_last = -1;
			store.sort();
		}
		else if (indicate > 0) {//何かと重なっているなら
			if (first == NULL) {//一つ目のカードを選ぶ
				set_swap(indicate, first, &deck, &store, sound);//firstをセット
				if (indicate <= DECK_MAX) { first_deck = true; }
			}
			else {//二つ目のカードを選んで交換
				set_swap(indicate, second, &deck, &store, sound);//secondをセット
				if (indicate <= DECK_MAX) { second_deck = true; }
				if (second != NULL) {//セットに成功しているなら
					if (swap_able(first_deck, second_deck, item_flag, *first, *second)) {
						swap_card(first, second);//交換
					}
					first = NULL; second = NULL;
					first_deck = false; second_deck = false;
				}
			}
		}
	}
	if (deck.empty()) {
		DrawFormatString(5, 200, GetColor(0, 0, 0), "デッキに一枚以上");//カードの効果
		DrawFormatString(5, 220, GetColor(0, 0, 0), "カードを入れてください");//カードの効果
	}
	//DrawFormatString(50, 300, GetColor(0, 0, 0), "first = %d, second = %d", first_deck, second_deck);//カードの効果
	return 0;
}

//カードの売却 -1で終了 1以上で売却によるお金
int card_sell(Deck& store, int& last, int sound, int money_sound) {
	const int CARD_X = 550;
	const int CARD_Y = 150;
	static int store_i = 0;	//一番上に表示されるカードのインデックス
	static Card* sell = NULL;//売却するカードのポインタ
	int sell_index;
	int indicate = 0;//指し示しているのものの番号0はなし
	int hand_x, hand_y;//マウスカーソルの座標
	GetMousePoint(&hand_x, &hand_y);
	DrawBox(0, 0, 640, 480, GetColor(120, 150, 120), TRUE);//背景
	static int store_last = -1;//空になっている最初のインデックスは？
	if (store_last == -1) {
		store.get_last(store_last);
	}

	modoru_icon(hand_x, hand_y, indicate);
	sort_icon(hand_x, hand_y, indicate);
	up_down(hand_x, hand_y, indicate, store_i > 0, store_i < last - STORE_PAGE);
	DrawFormatString(220, 20, GetColor(0, 0, 0), "倉庫:%d枚", store_last);//カードの効果
	//倉庫のリストを9枚ずつ表示
	for (int i = 0; i < STORE_PAGE; i++) {//i<DECK_MAX
		int x, y; int index, init;
		Deck* d;
		x = 220; y = 40 + 25 * i; index = 1; init = store_i;
		d = &store;
		//カーソルが重なっているとき
		if (hand_x >= x && hand_x <= x + 190 && hand_y >= y && hand_y <= y + 20) {
			indicate = i + index + init;//指し示しているカード Deck:1～9 Store:10～
			DrawBox(x - 3, y - 3, x + 193, y + 23, GetColor(0, 255, 0), TRUE);
		}
		//リスト表示
		if (&d->get_cards()[i + init] == sell) {//選択中のカードは赤く囲んで強調
			DrawBox(x - 3, y - 3, x + 193, y + 23, GetColor(255, 0, 0), TRUE);
			//売却ボタン表示
			if (hand_x >= 20 && hand_x <= 120 && hand_y >= 100 && hand_y <= 150) {
				DrawBox(18, 98, 122, 152, GetColor(0, 255, 0), TRUE);
				indicate = -4;
			}
			DrawBox(20, 100, 120, 150, GetColor(100,100,255), TRUE);
			DrawFormatString(50, 120, GetColor(0, 0, 0), "売却");//カードの効果
			sell_index = i + init;//first のインデックス
		}
		DrawBox(x, y, x + 190, y + 20, GetColor(100, 100, 255), TRUE);
		Card* card = &d->get_cards()[i + init];
		if (i < d->get_size() && card->get_kind() != 0) {
			const char* str;
			str = card->get_ability()->get_text();//カードのテキスト
			char c[23];
			card_text(c, str);//cにstrを入りきる部分だけコピー
			DrawFormatString(x+2, y+2, GetColor(0, 0, 0), c);
			if (indicate == i + index + init) {//カーソルが重なっているときカードの情報を表示
				int h, p, s, r;
				h = card->get_hp(); p = card->get_power(); s = card->get_speed(); r = card->get_rigid();
				DrawRotaGraph(CARD_X, CARD_Y, 1.5, 0, card->get_handle(), TRUE, FALSE);//カードの画像
				picture_card_state(h, p, s, r);
				DrawFormatString(TEXT_X, TEXT_Y + 40, GetColor(0, 0, 0), str);//カードの効果
			}
		}
	}

	int money = 0;//売却額
	if (sell != NULL) {//選択中のカード表示
		int h, p, s, r;
		h = sell->get_hp(); p = sell->get_power(); s = sell->get_speed(); r = sell->get_rigid();
		const char* str;
		str = sell->get_ability()->get_text();
		picture_card_state(h, p, s, r, true);
		DrawFormatString(TEXT_X, TEXT_Y - 40, GetColor(0, 0, 0), str);//カードの効果
		money = sell_money(sell->get_kind(), sell->get_rare());
		DrawFormatString(50, 170, GetColor(0, 0, 0), "売却額: %d", money);//カードの効果
	}

	if (left_click() == 1) {
		if (indicate == -1) { 
			sell = NULL;
			store_i = 0;
			PlaySoundMem(sound, DX_PLAYTYPE_BACK);//音
			store_last = -1;
			return -1;
		}
		if (indicate == -2) {//上へスクロール
			store_i--;
			PlaySoundMem(sound, DX_PLAYTYPE_BACK);//音
		}
		if (indicate == -3) {//下へスクロール
			store_i++;
			PlaySoundMem(sound, DX_PLAYTYPE_BACK);//音
		}
		if (indicate == -4) {//売却
			cards_bring(store, sell_index);
			last--; sell = NULL;
			PlaySoundMem(money_sound, DX_PLAYTYPE_BACK);//音
			store_last = -1;
			return money;
		}
		if (indicate == -7) {
			PlaySoundMem(sound, DX_PLAYTYPE_BACK);//音
			sell = NULL;
			store_i = 0;
			store.sort();
		}
		if (indicate > 0 && store.get_cards()[indicate - 1].get_kind() != 0) {//何かと重なっているなら
			sell = &store.get_cards()[indicate - 1];
			PlaySoundMem(sound, DX_PLAYTYPE_BACK);//音
		}

	}
	return 0;
}

//キャラクターを決める 決まってないなら0 決まったなら1 服選びに移行なら2 デッキ構築なら3
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
			PlaySoundMem(s_modoru, DX_PLAYTYPE_BACK);//音
			return 1;
		}
		if (indicate > 0) {
			PlaySoundMem(s_kettei, DX_PLAYTYPE_BACK);//音
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
	DrawFormatString(130, 200, GetColor(0, 0, 0), "カード売却");
	if (hand_x >= 340 && hand_x <= 590 && hand_y >= 100 && hand_y <= 300) {
		DrawBox(338, 98, 592, 302, GetColor(0, 255, 0), TRUE);
		indicate = 2;
	}
	DrawBox(340, 100, 590, 300, GetColor(150, 150, 150), TRUE);
	DrawFormatString(410, 200, GetColor(0, 0, 0), "アイテム売却");
	if (left_click() == 1) {
		if (indicate == 1) {
			PlaySoundMem(s_kettei, DX_PLAYTYPE_BACK);//音
			focus = -1;
			return 0;
		}
		if (indicate == 2) {
			PlaySoundMem(s_kettei, DX_PLAYTYPE_BACK);//音
			focus = -2;
			return 0;
		}
		if (indicate == -1) {
			PlaySoundMem(s_modoru, DX_PLAYTYPE_BACK);//音
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