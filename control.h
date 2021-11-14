#ifndef CONTROL_H_INCLUDED
#define CONTROL_H_INCLUDED
//////�L�[�{�[�h����̓��͂��i�[���A�Ԃ��֐��Q///////////

/////////////���͏��/////////////////////
//�L�[�̓��͏��
enum {//�e�s����control�֐�����̖߂�l ���ɁAA,S.D,W
	ZANGEKI_KEY = 1,
	SPACE_KEY,
	SPACE_KEY_LONG
};

//�X�e�B�b�N�̓��͏��
enum {
	RIGHT_KEY = 1,
	LEFT_KEY = 180,
	UP_KEY = 3,
	DOWN_KEY = 4
};
////////////////////////////////////////////

int UpdateKey();

void mouse_limit(int& hand_x, const int hand_y);
//���N���b�N�̏��
void mouse_click();
int left_click();
int right_click();

//�X�e�B�b�N�̓��͏��
int control_stick();

//�X�y�[�X�L�[��������Ă��邩�ǂ���
int control_space();

//�L�[�̓��͏��
int control_attack();

//�I�����
int control_focus();

//FPS�\���̃I���I�t
int control_debug();

//ESC�L�[�F�Q�[���I��
int control_esc();

#endif