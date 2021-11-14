#include "menu.h"
#include"control.h"
#include"create.h"
#include"DxLib.h"

static bool window_flag = FALSE;

CardGame::CardGame() {
	cnt = 0;
	deck = NULL;
	store = NULL;
	item = NULL;
	task = NULL;
	stand_handle = NULL;
	kettei_sound = LoadSoundMem("sound/����.wav");
	modoru_sound = LoadSoundMem("sound/�L�����Z��.wav");;
	bgm = LoadSoundMem("music/town14.ogg");
	ChangeVolumeSoundMem(255 * 60 / 100, bgm);
	font_handle = CreateFontToHandle(NULL, 35, 4);//�t�H���g
	mouse_handle = LoadGraph("picture/hand.png");
	task_detail = NULL;
	for (int i = 0; i < DAYLY_MISSION; i++) { daily_mission[i] = NULL; }
	//�Z�[�u�f�[�^���烍�[�h����
	money = 0; day = 1;
	for (int i = 0; i < M_TOTAL; i++) { clear[i] = false; }
	for (int i = 0; i < TODAY_MAX; i++) { today_mission[i] = NO_MISSION; }
	//save_other(&day, clear, today_mission);//�e�X�g
	//save_money(&money);//�e�X�g
	load_money(&money);//������
	load_other(&day, clear, today_mission);//������
	//�����܂ł̓Z�[�u�f�[�^���烍�[�h����
}

CardGame::~CardGame() {
	save_money(&money);
	save_other(&day, clear, today_mission);
	if (deck != NULL)delete[] deck;
	if (store != NULL)delete[] store;
	if (item != NULL)delete item;
	if (task != NULL)delete task;
	if (stand_handle != NULL)delete[] stand_handle;
}

//CardGame�ȊO�Ŏg���摜�ƃt�H���g�����ׂč폜����
void CardGame::picture_reflesh() {
	InitGraph();
	InitFontToHandle();
	font_handle = CreateFontToHandle(NULL, 35, 4);//�t�H���g
	mouse_handle = LoadGraph("picture/hand.png");
}
//CardGame�ȊO�Ŏg���T�E���h�����ׂč폜����
void CardGame::sound_reflesh() {
	InitSoundMem();
	kettei_sound = LoadSoundMem("sound/����.wav");
	modoru_sound = LoadSoundMem("sound/�L�����Z��.wav");
	bgm = LoadSoundMem("music/town14.ogg");
	ChangeVolumeSoundMem(255 * 60 / 100, bgm);
}

//�}�E�X�J�[�\����\��
void CardGame::picture_mouse() {
	GetMousePoint(&mouse_x, &mouse_y);
	//�}�E�X�J�[�\���`��
	DrawGraph(mouse_x, mouse_y, mouse_handle, TRUE);
}

//���j���[��ʂ̃A�C�R�� �~�b�V����:1 �f�b�L:2 �V���b�v:3
void CardGame::menu_key(int hand_x, int hand_y, int& indicate) {
	if (hand_x >= 10 && hand_x <= 210 && hand_y >= 350 && hand_y <= 450) {
		DrawBox(8, 348, 212, 452, GetColor(200, 200, 0), TRUE);
		indicate = 1;
	}
	DrawBox(10, 350, 210, 450, GetColor(100, 150, 100), TRUE);
	DrawStringToHandle(20, 390, "�~�b�V����", GetColor(255, 0, 0), font_handle);
	if (hand_x >= 220 && hand_x <= 420 && hand_y >= 350 && hand_y <= 450) {
		DrawBox(218, 348, 422, 452, GetColor(200, 200, 0), TRUE);
		indicate = 2;
	}
	DrawBox(220, 350, 420, 450, GetColor(100, 150, 100), TRUE);
	DrawStringToHandle(260, 390, "�f�b�L", GetColor(255, 0, 255), font_handle);//260, 370
	//DrawStringToHandle(270, 405, "�ߑ�", GetColor(255, 0, 255), font_handle);
	if (hand_x >= 430 && hand_x <= 630 && hand_y >= 350 && hand_y <= 450) {
		DrawBox(428, 348, 632, 452, GetColor(200, 200, 0), TRUE);
		indicate = 3;
	}
	DrawBox(430, 350, 630, 450, GetColor(100, 150, 100), TRUE);
	DrawStringToHandle(450, 390, "�V���b�v", GetColor(0, 0, 255), font_handle);
}

//�����̃~�b�V�����\��
void CardGame::mission_key(int hand_x, int hand_y, int& indicate) {
	static bool daily_flag = false;
	static int daily_difficult = 1;
	if (hand_x >= 10 && hand_x <= 210 && hand_y >= 310 && hand_y <= 340) {
		DrawBox(8, 308, 212, 342, GetColor(200, 200, 0), TRUE);
		if (left_click() == 1) { 
			daily_flag = !daily_flag;
			PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//��
		}
	}
	DrawBox(10, 310, 210, 340, GetColor(100, 150, 100), TRUE);
	DrawFormatString(20, 320, GetColor(255, 0, 0), "�~�b�V�����؂�ւ�");
	if (!daily_flag) {
		for (int i = 0; i < 3; i++) {
			if (today_mission[i] == NO_MISSION) { continue; }
			if (task_detail[i].picture_mission(10 + (i * 210), 50, hand_x, hand_y, clear[today_mission[i]])) {
				indicate = i + 4;
			}
		}
	}
	else {
		if (daily_difficult == 1) {
			DrawBox(100, 80, 150, 280, GetColor(50, 50, 50), TRUE);
		}
		else {
			if (hand_x >= 100 && hand_x <= 150 && hand_y >= 80 && hand_y <= 280) {
				DrawBox(100 - 2, 80 - 2, 150 + 2, 280 + 2, GetColor(0, 255, 0), TRUE);
				if (left_click() == 1) { 
					PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//��
					daily_difficult--;
				}
			}
			DrawBox(100, 80, 150, 280, GetColor(150, 150, 150), TRUE);
		}
		DrawFormatString(110, 180, GetColor(0, 0, 0), "��");
		if (daily_difficult == 10) {
			DrawBox(490, 80, 540, 280, GetColor(50, 50, 50), TRUE);
		}
		else {
			if (hand_x >= 490 && hand_x <= 540 && hand_y >= 80 && hand_y <= 280) {
				DrawBox(490 - 2, 80 - 2, 540 + 2, 280 + 2, GetColor(0, 255, 0), TRUE);
				if (left_click() == 1) {
					PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//��
					daily_difficult++;
				}
			}
			DrawBox(490, 80, 540, 280, GetColor(150, 150, 150), TRUE);
		}
		DrawFormatString(500, 180, GetColor(0, 0, 0), "��");
		if (daily_mission[daily_difficult - 1]->picture_mission(10 + (1 * 210), 50, hand_x, hand_y, false)) {
			indicate = daily_difficult + 6;
		}
	}
}

//�����̃~�b�V���������߂�
void CardGame::mission_decide() {
	if (today_mission[0] != NO_MISSION && daily_mission[0] != NULL)return;//���Ɍ��܂��Ă���
	if (today_mission[0] == NO_MISSION) {
		mission_rule(today_mission, clear, day);//���[���ɉ����ă~�b�V���������߂�
	}
	//today_mission[0] = MM1;//�e�X�g
	//today_mission[1] = EM18;//�e�X�g
	//today_mission[2] = EM19;//�e�X�g
	task_detail = new MissionText[3]{ today_mission[0], today_mission[1], today_mission[2] };
	for (int i = 0; i < DAYLY_MISSION; i++) {
		daily_mission[i] = new MissionText(DM1 + i);
	}
	save_other(&day, clear, today_mission);
}

//�����G�ƑS�Ẵf�b�L�Ƒq�ɂ����[�h����
void CardGame::init_cards() {
	load_cards_stand(store, deck, item, stand_handle);
}
//�J�[�h���Z�[�u�A��������Ԃ�
void CardGame::end_cards() {
	save_store(store);//�Z�[�u
	save_deck(deck);//�Z�[�u
	save_item(*item);
	save_money(&money);
	save_other(&day, clear, today_mission);
	delete[] store;
	delete[] deck;
	delete item;
	delete[] stand_handle;
	deck = NULL;
	store = NULL;
	item = NULL;
	stand_handle = NULL;
}

//�N���b�N���ꂽ�Ƃ��̏���
void CardGame::menu_click(int indicate, int& now) {
	if (left_click() != 1)return;//���N���b�N����Ă��Ȃ�
	if (indicate == 1) {
		if (now == M_SELECT) {
			now = FALSE;//�~�b�V�����\���L�����Z��
			PlaySoundMem(modoru_sound, DX_PLAYTYPE_BACK);//��
		}
		else {
			now = M_SELECT;//�~�b�V�����\��
			PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//��
		}
	}
	else if (indicate == 2) {
		init_cards();
		now = CUS;//�f�b�L�ҏW��
		PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//��
	}
	else if (indicate == 3) {
		shop = new Shop;
		now = SHOP;//�V���b�v��
		PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//��
	}
	else if (indicate >= 4 && (indicate <= 6 || indicate <= 6+10)) {
		//3�̃~�b�V������1���v���C
		sound_reflesh();
		now = M_PLAY;
		m_kind = indicate - 4;
		PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//��
		init_mission();
	}
}

//�L�����N�^�[�I�� focus�ύX,��������now��0�ɂ��ďI��
void CardGame::chara_focus(int& focus, int& now) {
	int result = 0;
	result = character_custom(stand_handle, kettei_sound, modoru_sound, focus);
	if (result == 1) {//�߂�{�^���ŏI��
		now = 0;
		picture_reflesh();
		end_cards();
	}
}

//���j���[��� �~�b�V������A�C�R���͏���
void CardGame::picture_menu() {
	DrawBox(0, 0, 640, 480, GetColor(50, 50, 100), TRUE);//�w�i
	DrawFormatString(10, 10, GetColor(0, 0, 0), "���j���[���  %d����", day);
	DrawFormatString(300, 10, GetColor(255, 255, 0), "������: %d", money);
}

//���C���֐�
void CardGame::CMain() {
	static int now = FALSE;//1:�~�b�V�����I�� 2:�~�b�V�����v���C�� 3:�f�b�L�ҏW�� 4:�V���b�v��
	int hand_x, hand_y, indicate = 0;
	if (now == DATA && (db->get_now() == DATA_OP || db->get_now() == DATA_ED)) {//OP��ED���{����
		StopSoundMem(bgm);
	}
	if (now != M_PLAY && CheckSoundMem(bgm) == 0) {
		PlaySoundMem(bgm, DX_PLAYTYPE_BACK);//���j���[��ʂ�BGM
	}
	//�S��ʁE�E�B���h�E�؂�ւ�
	if ((now == FALSE || now == M_SELECT) && control_space() == SPACE_KEY) {
		window_flag = !window_flag;
		ChangeWindowMode(window_flag);
		picture_reflesh(); sound_reflesh();
		SetMouseDispFlag(FALSE);//�}�E�X��\��
		SetMousePoint(320, 240);//�}�E�X�J�[�\���̏����ʒu
		SetDrawScreen(DX_SCREEN_BACK);
		SetDrawMode(DX_DRAWMODE_BILINEAR);
	}
	if (cnt > 0 && now == FALSE) {
		cnt--;
		int color = 480 - cnt * 2;
		if (color > 255) { color = 255;  }
		if (color < 0) { color = 0; }
		DrawFormatStringToHandle(260, 200, GetColor(color, color, color), font_handle, "%d����", day);
		return;
	}
	GetMousePoint(&hand_x, &hand_y);
	if (now == M_PLAY) {//�~�b�V�����v���C��
		if (play_mission() == true) {
			now = FALSE;//�~�b�V�����I��
			picture_reflesh();
		}
	}
	else if (now == CUS) {//�J�X�^�}�C�Y���
		static int focus = 0;//�ҏW����L����1�`10
		if (focus == 0) {//�ҏW����L���������߂�
			chara_focus(focus, now);
		}
		else {//�ҏW����L���������܂��Ă���Ƃ�
			int result = deck_create(deck[focus - 1], store[focus - 1], *item, kettei_sound);
			if (result == 1) { focus = 0; }//�߂�{�^���ŃL�����I���֖߂�
		}
	}
	else if (now == SHOP) {//�V���b�v���
		if (shop->shop_main(money) == true) {
			PlaySoundMem(modoru_sound, DX_PLAYTYPE_BACK);//��
			now = FALSE;
			picture_reflesh();
			delete shop;
		}
	}
	else if (now == DATA) {//�f�[�^�m�F���
		if (db->play()) {
			now = FALSE;
			delete db;
			picture_reflesh();
			sound_reflesh();
			PlaySoundMem(modoru_sound, DX_PLAYTYPE_BACK);//��
		}
	}
	else {//���j���[���
		picture_menu();
		mission_decide();
		menu_key(hand_x, hand_y, indicate);//�A�C�R��
		if (hand_x >= 550 && hand_x <= 630 && hand_y >= 5 && hand_y <= 35) {
			DrawBox(547, 2, 633, 38, GetColor(0, 255, 0), TRUE);
			if (left_click() == 1) { 
				now = DATA;
				db = new DataBase(clear);
				PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//��
			}//�f�[�^�m�F��ʂ�
		}
		DrawBox(550, 5, 630, 35, GetColor(100, 100, 255), TRUE);
		DrawFormatString(565, 15, GetColor(0, 0, 0), "�f�[�^");
		//�����̃~�b�V�����\��
		if (now == M_SELECT) { 
			mission_key(hand_x, hand_y, indicate);
		}
		menu_click(indicate, now);//�N���b�N�ɂ����now�ω�
		DrawFormatString(20, 460, GetColor(0, 0, 0), "�G�X�P�[�v�L�[�������ŏI���A�X�y�[�X�L�[�ŉ�ʃ��[�h�؂�ւ�");
	}
}

/////�~�b�V�����v���C�֘A�O��////////////////////////////////////////////////
//���� �v���C�̑O�Ɉ�x�ĂԂ���
void CardGame::init_mission() {
	if (m_kind <= 2) {
		get_mission(task, today_mission[m_kind], task_detail[m_kind]);//��肽���~�b�V�����̃I�u�W�F�N�g���m�ۂ���
	}
	else {
		get_mission(task, DM1 + m_kind - 3, *daily_mission[m_kind - 3]);//��肽���~�b�V�����̃I�u�W�F�N�g���m�ۂ���
	}
	task->init();//���[�h����
}
//�v���C
bool CardGame::play_mission() {
	static Card c;
	static int m = 0;
	static bool f_flag = false;
	if (f_flag) {
		picture_result(m, c);
		if (left_click() == 1) {//��V��ʏI��
			f_flag = false;
			m = 0; c.set_kind(0);
			end_mission();//�I������
			PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//��
			return true;//����
		}
		return false;
	}
	int result;
	result = task->play();//WIN or LOSE or 0
	if (result == WIN && m_kind < 3) {//�N���A
		clear[today_mission[m_kind]] = true;
	}
	if (result == WIN || result == LOSE) {
		f_flag = true;
		task->reward(result, m, c);//��V���󂯎��
		day++;//���t�ύX
		if (day == 21) { day = 1; }
		cnt = DAY_COUNT;
		for (int i = 0; i < 3; i++) { today_mission[i] = NO_MISSION; }//���Z�b�g
		money += m;
		save_money(&money);
		save_other(&day, clear, today_mission);
	}
	else if (result == CANCEL) {
		if (task != NULL)delete task;//�~�b�V�����j��
		task = NULL;
		//end_mission();//�I������
		PlaySoundMem(modoru_sound, DX_PLAYTYPE_BACK);//��
		return true;
	}
	return false;
}
//�I������
void CardGame::end_mission() {
	if (task != NULL)delete task;//�~�b�V�����j��
	task = NULL;
	delete[] task_detail;
	task_detail = NULL;
	for (int i = 0; i < DAYLY_MISSION; i++) { delete daily_mission[i]; }
	for (int i = 0; i < DAYLY_MISSION; i++) { daily_mission[i] = NULL; }
}
//////////////////////////////////////////////////////////////////////////////

//��V�l�����
void CardGame::picture_result(int money, Card& c) {
	DrawBox(0, 0, 640, 480, GetColor(100, 100, 100), TRUE);
	DrawFormatString(20, 100, GetColor(255, 0, 0), "��V�l��");
	DrawFormatString(20, 150, GetColor(255, 255, 0), "������: +%d", money);
	DrawFormatString(10, 10, GetColor(0,0,0), "���N���b�N�ŏI��");
	if (c.get_kind() != 0) {
		picture_card_state(c.get_hp(), c.get_power(), c.get_speed(), c.get_rigid(), false);
		DrawFormatString(10, 440, GetColor(0, 0, 0), c.get_ability()->get_text());
		DrawFormatString(300, 50, GetColor(0, 0, 255), "�l�������J�[�h");
		SetDrawMode(DX_DRAWMODE_NEAREST);
		DrawRotaGraph2(300, 70, 0, 0, 2.0, 0, c.get_handle(), TRUE, FALSE);
		SetDrawMode(DX_DRAWMODE_BILINEAR);
	}
}