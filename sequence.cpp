#include "DxLib.h"
#include "control.h"
#include"create.h"
#include"sequence.h"

Bsequence::Bsequence(Enemy te[3], int st_kind[3], bool st_fixed[3]) {
	bf = NULL;
	for (int i = 0; i < 3; i++) {//受け取った引数から
		teacher[i] = te[i];//先生を決定
		kind[i] = st_kind[i];//暫定のキャラ
		fixed[i] = st_fixed[i];//固定キャラ
	}
	focus = 0;
	load_store(store);//倉庫のデータをロード(セーブデータから)
	load_deck(st_deck);//デッキのカードをロードする(セーブデータから)
	load_item(item);//アイテム倉庫のデータをロード(セーブデータから)
	item.load_cards();//アイテム倉庫のカードをロードする
	for (int i = 0; i < 10; i++) {
		store[i].load_cards();//倉庫のカードの画像などをロードする
		st_deck[i].load_cards();
	}
	//画像や音をロード
	s_kettei = LoadSoundMem("sound/決定.wav");
	s_modoru = LoadSoundMem("sound/キャンセル.wav");
	//ChangeVolumeSoundMem(255 * 60 / 100, s_kettei);
	//ChangeVolumeSoundMem(255 * 60 / 100, s_modoru);
	load_stand(stand_handle);

	for (int i = 0; i < 3; i++) { //キャラとデッキの大きさ暫定
		student[i].kind_set(kind[i]);
		student[i].get_cards()->set_size(9);
		*student[i].get_cards() = st_deck[kind[i]];//セーブしてあったデッキをコピー
	}
	for (int i = 0; i < 3; i++) {//デッキの各カードの画像などをロード
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

//変更されたデッキを適用する
void Bsequence::st_deck_set() {
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (student[i].get_kind() != 0)
			*student[i].get_cards() = st_deck[student[i].get_kind() - 1];
	}
}

//バトル後にアイテム消費を適用する
void Bsequence::dec_item() {
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		if (student[i].get_kind() != 0)
			st_deck[student[i].get_kind() - 1] = after_deck[i];
	}
}

void Bsequence::st_kind_set(int another_kind[]) {
	for (int i = 0; i < MAX_WITH_BATTLE; i++) {
		student[i].kind_set(kind[i]);
		another_kind[i] = kind[i];//使うキャラをmissionにも教えてあげる
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
	case CHARA_SELECT://キャラクター選択画面
		result = character_select(kind, stand_handle, fixed, s_kettei, s_modoru, focus);
		if (result == 1) {
			SetUseASyncLoadFlag(TRUE);//非同期処理開始
			st_kind_set(another_kind);
			st_deck_set();
			bf = new BattleField(student, teacher);//バトルフィールドを用意
			now = BATTLE;
			save_store(store);//セーブ
			save_deck(st_deck);//セーブ
			save_item(item);//セーブ
		}
		else if (result == 3) {
			now = DECK_CREATE;
		}
		else if (result == -1) {
			save_store(store);//セーブ
			save_deck(st_deck);//セーブ
			save_item(item);//セーブ
			return CANCEL;
		}
		break;
	case DECK_CREATE://デッキ編集画面
		result = deck_create(st_deck[focus - 1], store[focus - 1], item, s_kettei);
		if (result == 1) {
			now = CHARA_SELECT;
		}
		break;
	case BATTLE://バトル画面
		result = bf->test(after_deck);
		if (result == INTERVAL) {
			dec_item();//アイテム消費
			now = DECK_SELECT;
			bf->get_remain_hp(remain_hp);
			SetUseASyncLoadFlag(FALSE);//非同期処理終了
		}
		else if (result == WIN || result == LOSE) {
			dec_item();//アイテム消費
			now = CHARA_SELECT;
			SetUseASyncLoadFlag(FALSE);//非同期処理終了
			return result;//決着
		}
		break;
	case DECK_SELECT://インターバルで再編集するデッキの選択画面
		bf->play_music(true);
		result = deck_select(kind, stand_handle, s_kettei, focus);
		for (int i = 0; i < 3; i++) {
			if (remain_hp[i] <= 0) { continue; }
			DrawFormatString(85 + i*180, 380, GetColor(0, 0, 0), "残りHP：%d", remain_hp[i]);
		}
		if (result == 1) {
			st_deck_set();
			for (int i = 0; i < MAX_WITH_BATTLE; i++) {
				bf->deck_reflesh(*student[i].get_cards(), i);
			}
			now = BATTLE;
			save_store(store);//セーブ
			save_deck(st_deck);//セーブ
			save_item(item);//セーブ
		}
		else if (result == 2) {
			now = CHANGE;
		}
		break;
	case CHANGE://インターバルでのデッキ編集画面
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
	store[kind - 1].get_last(last);//倉庫の末尾を調べる
	store[kind - 1].set_card(c, last);//倉庫の末尾へ挿入
	save_store(store);//セーブ
	save_deck(st_deck);//セーブ
}