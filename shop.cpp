#include"DxLib.h"
#include"shop.h"
#include"control.h"

Shop::Shop() {
	now = SHOP_MAIN;
	focus = 0;
	load_cards_stand(store, deck, item, stand_handle);
	kettei_sound = LoadSoundMem("sound/����.wav");
	modoru_sound = LoadSoundMem("sound/�L�����Z��.wav");
	money_sound = LoadSoundMem("sound/money.mp3");
	font_handle = CreateFontToHandle(NULL, 35, 4);//�t�H���g
}

Shop::~Shop() {
	save_store(store);//�Z�[�u
	save_deck(deck);//�Z�[�u
	DeleteSoundMem(kettei_sound);
	DeleteSoundMem(modoru_sound);
	DeleteSoundMem(money_sound);
	delete[] store;
	delete[] deck;
	delete item;
	delete[] stand_handle;
}

bool Shop::shop_main(int& money) {
	indicate = 0;
	GetMousePoint(&hand_x, &hand_y);
	switch (now) {
	case SHOP_MAIN:
		picture_main();
		break;
	case SHOP_BUY:
		picture_buy(money);
		break;
	case SHOP_SELL:
		picture_sell(money);
		break;
	case SHOP_GACHA:
		picture_gacha(money);
		break;
	}
	if (now == -1)return true;
	return false;
}

//�V���b�v���C�����
void Shop::picture_main() {
	DrawBox(0, 0, 640, 480, GetColor(150, 150, 150), TRUE);//�w�i
	DrawStringToHandle(245, 50, "�V���b�v", GetColor(0, 0, 0), font_handle);
	modoru_icon(hand_x, hand_y, indicate);
	if (hand_x >= 10 && hand_x <= 210 && hand_y >= 150 && hand_y <= 250) {
		DrawBox(8, 148, 212, 252, GetColor(200, 200, 0), TRUE);
		indicate = SHOP_BUY;
	}
	DrawBox(10, 150, 210, 250, GetColor(100, 150, 100), TRUE);
	DrawStringToHandle(40, 160, "�A�C�e��", GetColor(255, 0, 0), font_handle);
	DrawStringToHandle(75, 210, "�w��", GetColor(255, 0, 0), font_handle);
	if (hand_x >= 220 && hand_x <= 420 && hand_y >= 150 && hand_y <= 250) {
		DrawBox(218, 148, 422, 252, GetColor(200, 200, 0), TRUE);
		indicate = SHOP_SELL;
	}
	DrawBox(220, 150, 420, 250, GetColor(100, 150, 100), TRUE);
	DrawStringToHandle(280, 190, "���p", GetColor(255, 0, 255), font_handle);
	if (hand_x >= 430 && hand_x <= 630 && hand_y >= 150 && hand_y <= 250) {
		DrawBox(428, 148, 632, 252, GetColor(200, 200, 0), TRUE);
		indicate = SHOP_GACHA;
	}
	DrawBox(430, 150, 630, 250, GetColor(100, 150, 100), TRUE);
	DrawStringToHandle(470, 190, "�K�`��", GetColor(0, 0, 255), font_handle);
	if (left_click() == 1) {
		if (indicate == -1) {
			now = indicate;
		}
		else if (indicate != 0) {
			PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//��
			item->get_last(item_last);//�A�C�e���̒[�𒲂ׂĂ���
			now = indicate;
		}
	}
}

//�V���b�v�w�����
void Shop::picture_buy(int& money) {
	static int num = 0;//���Ԗڂ̕i����\�����Ă��邩�H
	static Card* sell_item = NULL;
	static int sum = 0;
	if (sell_item == NULL) { sum = sell_item_load(sell_item); }
	DrawBox(0, 0, 640, 480, GetColor(150, 120, 120), TRUE);//�w�i
	DrawFormatString(300, 20, GetColor(255, 255, 0), "������: %d", money);
	modoru_icon(hand_x, hand_y, indicate);

	if (num == 0) {
		DrawBox(100, 100, 150, 300, GetColor(50, 50, 50), TRUE);
	}
	else {
		if (hand_x >= 100 && hand_x <= 150 && hand_y >= 100 && hand_y <= 300) {
			DrawBox(100-2, 100-2, 150+2, 300+2, GetColor(0, 255, 0), TRUE);
			indicate = -2;
		}
		DrawBox(100, 100, 150, 300, GetColor(150, 150, 150), TRUE);
	}
	DrawFormatString(110, 200, GetColor(0, 0, 0), "��");
	if (num == sum - 1) {
		DrawBox(490, 100, 540, 300, GetColor(50, 50, 50), TRUE);
	}
	else {
		if (hand_x >= 490 && hand_x <= 540 && hand_y >= 100 && hand_y <= 300) {
			DrawBox(490-2, 100-2, 540+2, 300+2, GetColor(0, 255, 0), TRUE);
			indicate = -3;
		}
		DrawBox(490, 100, 540, 300, GetColor(150, 150, 150), TRUE);
	}
	DrawFormatString(500, 200, GetColor(0, 0, 0), "��");
	SetDrawMode(DX_DRAWMODE_NEAREST);
	DrawRotaGraph(300, 200, 2, 0, sell_item[num].get_handle(), TRUE, FALSE);//�J�[�h�̉摜
	SetDrawMode(DX_DRAWMODE_BILINEAR);
	DrawBox(0, 400, 640, 480, GetColor(100, 100, 255), TRUE);
	DrawFormatString(10, 420, GetColor(0, 0, 0), sell_item[num].get_ability()->get_text());
	int need_money = buy_money(sell_item[num].get_kind());
	DrawFormatString(10, 380, GetColor(0, 0, 0), "���l: %d", need_money);
	if (hand_x >= 500 && hand_x <= 600 && hand_y >= 350 && hand_y <= 390) {
		DrawBox(500 - 2, 350 - 2, 600 + 2, 390 + 2, GetColor(0, 255, 0), TRUE);
		indicate = num + 1;
	}
	DrawBox(500, 350, 600, 390, GetColor(50, 50, 200), TRUE);
	DrawFormatString(510, 360, GetColor(0, 0, 0), "�w������");

	if (left_click() == 1) {
		if (indicate == -1) {
			PlaySoundMem(modoru_sound, DX_PLAYTYPE_BACK);//��
			num = 0;
			delete[] sell_item; sell_item = NULL;
			now = SHOP_MAIN;
		}
		if (indicate == -2) {
			num--;
			PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//��
		}
		if (indicate == -3) {
			num++;
			PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//��
		}
		if (indicate > 0) {
			if(money >= need_money){
				PlaySoundMem(money_sound, DX_PLAYTYPE_BACK);//��
				Card c;
				c.set_kind(sell_item[indicate - 1].get_kind());
				normal_status(sell_item[indicate - 1].get_kind(), c);
				c.load_card();
				item->set_card(c, item_last);
				item_last++;
				money -= need_money;
				save_item(*item);
			}
			else {
				PlaySoundMem(modoru_sound, DX_PLAYTYPE_BACK);//��
			}
		}
	}
}

//�V���b�v���p���
void Shop::picture_sell(int& money) {
	if (focus == 0) {//�A�C�e��or�J�[�h
		int result = 0;
		result = item_or_card(kettei_sound, modoru_sound, focus);
		if (focus == -2) {
			item->get_last(item_last);//�A�C�e�����p��
		}
		if (result == 1) {//�߂�{�^���������ꂽ
			now = SHOP_MAIN;
		}
	}
	else if (focus == -1) {//�ǂ̃L�����̃J�[�h�𔄂邩���܂��Ă��Ȃ�
		int result = 0;
		result = character_custom(stand_handle, kettei_sound, modoru_sound, focus);
		if (focus != -1) {//�L���������܂���
			store[focus-1].get_last(store_last);
		}
		if (result == 1) {//�߂�{�^���������ꂽ
			focus = 0;
		}
	}
	else if(focus > 0){//�ǂ̃L�����̃J�[�h�𔄂邩���܂��Ă���
		int result = card_sell(store[focus - 1], store_last, kettei_sound, money_sound);
		if (result == -1) {
			save_store(store);//�Z�[�u
			focus = -1;
		}
		else {
			if (result > 0) {
				money += result;
				save_store(store);//�Z�[�u
			}
		}
	}
	else if(focus == -2){//�A�C�e�����p
		int result = card_sell(*item, item_last, kettei_sound, money_sound);
		if (result == -1) {
			save_item(*item);//�Z�[�u
			focus = 0;
		}
		else {
			if(result > 0){
				money += result;
				save_item(*item);//�Z�[�u
				save_money(&money);
			}
		}
	}
	DrawFormatString(450, 10, GetColor(255, 255, 0), "������: %d", money);
}

//�V���b�v�K�`�����
void Shop::picture_gacha(int& money) {
	const int COST = 1000;//���ɂ����邨��
	static int buy_flag = false;
	DrawBox(0, 0, 640, 480, GetColor(120, 120, 150), TRUE);//�w�i
	if (!buy_flag) {
		modoru_icon(hand_x, hand_y, indicate);
		DrawBox(100, 50, 540, 450, GetColor(200, 200, 200), TRUE);
		DrawFormatString(450, 10, GetColor(255, 255, 0), "������: %d", money);

		//money >= COST && 
		if (money >= COST && hand_x >= 200 && hand_x <= 440 && hand_y >= 300 && hand_y <= 400) {
			DrawBox(200 - 2, 300 - 2, 440 + 2, 400 + 2, GetColor(0, 255, 0), TRUE);
			if (left_click() == 1) {//�K�`����
				buy_flag = true;//�l���J�[�h�\����ʂ�
				int chara_kind = GetRand(9) + 1;//�ǂ̃L�����̃J�[�h�H
				PlaySoundMem(money_sound, DX_PLAYTYPE_BACK);//��
				c = new Card;
				reward_card(chara_kind, *c, 50, 30, 15, 5);//�J�[�h����ɓ����
				c->load_card();
				int last = 0;
				store[chara_kind - 1].get_last(last);//�q�ɂ̖����𒲂ׂ�
				store[chara_kind - 1].set_card(*c, last);//�q�ɂ̖����֑}��
				save_store(store);//�Z�[�u
				money -= COST;
			}
		}
		DrawBox(200, 300, 440, 400, GetColor(50, 50, 200), TRUE);
		DrawFormatString(230, 340, GetColor(0, 0, 0), "�t���P��̊�����I");

		DrawStringToHandle(250, 100, "�K�`��", GetColor(0, 0, 255), font_handle);
		DrawFormatString(220, 200, GetColor(255, 100, 0), "�₹�A�t�̃J�[�h�I�I");
		DrawFormatString(235, 420, GetColor(0, 0, 0), "�P��F�P�O�O�O����");
	}
	else {//�l���J�[�h�\�����
		DrawFormatString(10, 10, GetColor(0, 0, 0), "���N���b�N�ŏI��");
		picture_card_state(c->get_hp(), c->get_power(), c->get_speed(), c->get_rigid(), false);
		DrawFormatString(10, 440, GetColor(0, 0, 0), c->get_ability()->get_text());
		DrawFormatString(300, 50, GetColor(0, 0, 255), "�l�������J�[�h");
		SetDrawMode(DX_DRAWMODE_NEAREST);
		DrawRotaGraph2(300, 70, 0, 0, 2.0, 0, c->get_handle(), TRUE, FALSE);
		SetDrawMode(DX_DRAWMODE_BILINEAR);
		if (left_click() == 1) {
			delete c;
			buy_flag = false;
		}
	}
	

	if (left_click() == 1) {
		if (indicate == -1) {
			PlaySoundMem(modoru_sound, DX_PLAYTYPE_BACK);//��
			now = SHOP_MAIN;
		}
	}
}