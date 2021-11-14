#include "DxLib.h"
#include "control.h"


//////////�L�[�{�[�h�̓��͏�Ԃ��i�[����////////////////

int Key[256]; // �L�[��������Ă���t���[�������i�[����

			  // �L�[�̓��͏�Ԃ��X�V����
int UpdateKey() {
	char tmpKey[256]; // ���݂̃L�[�̓��͏�Ԃ��i�[����
	GetHitKeyStateAll(tmpKey); // �S�ẴL�[�̓��͏�Ԃ𓾂�
	for (int i = 0; i<256; i++) {
		if (tmpKey[i] != 0) { // i�Ԃ̃L�[�R�[�h�ɑΉ�����L�[��������Ă�����
			Key[i]++;     // ���Z
		}
		else {              // ������Ă��Ȃ����
			Key[i] = 0;   // 0�ɂ���
		}
	}
	return 0;
}

void mouse_limit(int& hand_x, const int hand_y) {
	if (hand_x < 0 && hand_y > 0 && hand_y < 480) {
		hand_x = 0;
		SetMousePoint(0, hand_y);//�}�E�X�J�[�\���̈ʒu
	}
	else if (hand_x > 640 && hand_y > 0 && hand_y < 480) {
		hand_x = 640;
		SetMousePoint(640, hand_y);//�}�E�X�J�[�\���̈ʒu
	}
}

///////////�}�E�X�̓��͏�Ԃ�Ԃ�//////////////
static int left_cnt = 0;
static int right_cnt = 0;
void mouse_click() {
	if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0) { left_cnt++; }
	else { left_cnt = 0; }
	if (GetMouseInput() == MOUSE_INPUT_RIGHT) { right_cnt++; }
	else { right_cnt = 0; }
}
int left_click() {
	return left_cnt;
}
int right_click() {
	return right_cnt;
}


//���E�̈ړ�
int control_stick()
{
	if (Key[KEY_INPUT_RIGHT] >= 1) { // �E�L�[��������Ă�����
		return RIGHT_KEY;
	}
	if (Key[KEY_INPUT_LEFT] >= 1) { // ���L�[��������Ă�����
		return LEFT_KEY;
	}
	if (Key[KEY_INPUT_DOWN] >= 1) { // ���L�[��������Ă�����
		return DOWN_KEY;
	}
	if (Key[KEY_INPUT_UP] >= 1) { // ���L�[��������Ă�����
		return UP_KEY;
	}
	else {
		return FALSE;
	}
}

//�W�����v
int control_space()
{
	if (Key[KEY_INPUT_SPACE] == 1) { // �X�y�[�X�L�[�������ꂽ��W�����v
		return SPACE_KEY;
	}
	if (Key[KEY_INPUT_SPACE] == 60) {//�X�y�[�X�L�[������
		return SPACE_KEY_LONG;
	}
	else {
		return FALSE;
	}
}

//�U��
int control_attack()
{
	// �X�y�[�X�L�[��������A�L�[�������ꂽ��a��
	if (Key[KEY_INPUT_A] == 1 || Key[KEY_INPUT_SPACE] == 1) {
		return ZANGEKI_KEY;
	}
	else {
		return FALSE;
	}
}

//�I����ʂɎg��
int control_focus()
{
	if (Key[KEY_INPUT_RIGHT] == 1) { // �E�L�[�������ꂽ��
		return RIGHT_KEY;
	}
	if (Key[KEY_INPUT_LEFT] == 1) { // ���L�[�������ꂽ��
		return LEFT_KEY;
	}
	if (Key[KEY_INPUT_UP] == 1) { // ��L�[�������ꂽ��
		return UP_KEY;
	}
	if (Key[KEY_INPUT_DOWN] == 1) { // ���L�[�������ꂽ��
		return DOWN_KEY;
	}

	else {
		return FALSE;
	}
}

//�f�o�b�O���[�h�N���p
int control_debug() {
	if (Key[KEY_INPUT_F] == 1) { // F�L�[��������Ă�����
		return TRUE;
	}
	return FALSE;
}

//�Q�[���I���p
int control_esc() {
	if (Key[KEY_INPUT_ESCAPE] == 60) { //ESC�L�[��60�J�E���g������Ă�����
		return TRUE;
	}
	return FALSE;
}