#include"data.h"
#include"DxLib.h"
#include"control.h"
#include"create.h"


DataBase::DataBase(bool clear_data[]) {
	student_handle[0] = LoadGraph("stand/�A�J�c�L.png");
	student_handle[1] = LoadGraph("stand/�n���_.png");
	student_handle[2] = LoadGraph("stand/�I�I�^.png");
	student_handle[3] = LoadGraph("stand/�C�V.png");
	student_handle[4] = LoadGraph("stand/�^�C��.png");
	student_handle[5] = LoadGraph("stand/�I���_.png");
	student_handle[6] = LoadGraph("stand/�~���g�l.png");
	student_handle[7] = LoadGraph("stand/�E�`���g.png");
	student_handle[8] = LoadGraph("stand/�C�o���J��.png");
	student_handle[9] = LoadGraph("stand/�g���`.png");
	teacher_handle[0] = LoadGraph("enemy/O.png");
	teacher_handle[1] = LoadGraph("enemy/P.png");
	teacher_handle[2] = LoadGraph("enemy/Q.png");
	teacher_handle[3] = LoadGraph("enemy/R.png");
	teacher_handle[4] = LoadGraph("enemy/S.png");
	teacher_handle[5] = LoadGraph("enemy/T.png");
	teacher_handle[6] = LoadGraph("enemy/U.png");
	teacher_handle[7] = LoadGraph("enemy/V.png");
	teacher_handle[8] = LoadGraph("enemy/W.png");
	teacher_handle[9] = LoadGraph("enemy/X.png");
	teacher_handle[10] = LoadGraph("enemy/Y.png");
	teacher_handle[11] = LoadGraph("enemy/Z.png");
	kettei_sound = LoadSoundMem("sound/����.wav");
	em_sum = 0;
	for (int i = 0; i < M_TOTAL; i++) {
		clear[i] = clear_data[i];
		if (i >= EM1 && i <= EM20 && clear[i]) {
			em_sum++;
		}
	}
	now = DATA_MAIN;
	font_handle = CreateFontToHandle(NULL, 35, 4);//�t�H���g
}

DataBase::~DataBase() {

}

int DataBase::get_now() {
	return now;
}

//true��Ԃ��ďI��
bool DataBase::play() {
	int indicate = 0;
	DrawBox(0, 0, 640, 480, GetColor(190, 120, 100), TRUE);//�w�i
	GetMousePoint(&hand_x, &hand_y);
	if (now == DATA_MAIN) {//���C�����
		if (hand_x >= 10 && hand_x <= 270 && hand_y >= 150 && hand_y <= 250) {
			DrawBox(8, 148, 272, 252, GetColor(200, 200, 0), TRUE);
			if (left_click() == 1) { 
				now = DATA_CHARA;
				PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//��
			}
		}
		DrawBox(10, 150, 270, 250, GetColor(100, 150, 100), TRUE);
		DrawStringToHandle(20, 190, "�L�����N�^�[", GetColor(0, 0, 0), font_handle);
		if (hand_x >= 420 && hand_x <= 620 && hand_y >= 150 && hand_y <= 250) {
			DrawBox(418, 148, 622, 252, GetColor(200, 200, 0), TRUE);
			if (left_click() == 1) { 
				now = DATA_CLEAR;
				PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//��
			}
		}
		DrawBox(420, 150, 620, 250, GetColor(100, 150, 100), TRUE);
		DrawStringToHandle(430, 190, "�N���A��", GetColor(0, 0, 0), font_handle);
		//OP
		if (hand_x >= 20 && hand_x <= 220 && hand_y >= 350 && hand_y <= 450) {
			DrawBox(18, 348, 222, 452, GetColor(200, 200, 0), TRUE);
			if (left_click() == 1) {
				now = DATA_OP;
				op = new Op;
				PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//��
			}
		}
		DrawBox(20, 350, 220, 450, GetColor(100, 150, 100), TRUE);
		DrawStringToHandle(50, 390, "OP���ς�", GetColor(0, 0, 0), font_handle);
		//ED
		if (clear[MM20] && hand_x >= 420 && hand_x <= 620 && hand_y >= 350 && hand_y <= 450) {
			DrawBox(418, 348, 622, 452, GetColor(200, 200, 0), TRUE);
			if (left_click() == 1) {
				now = DATA_ED;
				ed = new Ed;
				PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//��
			}
		}
		if(clear[MM20]) {
			DrawBox(420, 350, 620, 450, GetColor(100, 150, 100), TRUE);
		}
		else {
			DrawBox(420, 350, 620, 450, GetColor(50, 100, 50), TRUE);
		}
		DrawStringToHandle(450, 390, "ED���ς�", GetColor(0, 0, 0), font_handle);
	}
	else if (now == DATA_CHARA) {//�L�����ڍ׉��
		watch_character();
	}
	else if (now == DATA_CLEAR) {//�N���A�󋵉��
		watch_clear();
	}
	else if (now == DATA_OP) {
		if (op->play()) {
			delete op;
			now = DATA_MAIN;
		}
		return false;
	}
	else if (now == DATA_ED) {
		if (ed->play()) {
			delete ed;
			now = DATA_MAIN;
		}
		return false;
	}

	//�߂�A�C�R��
	modoru_icon(hand_x, hand_y, indicate);
	if (left_click() == 1 && indicate == -1) {
		if (now == DATA_MAIN) {
			return true;//�I��
		}
		else {
			PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//��
			now = DATA_MAIN;
		}
	}
	return false;
}

bool DataBase::watch_clear() {
	int color1 = GetColor(150, 150, 250);
	int color2 = GetColor(100, 100, 150);
	int black = GetColor(0, 0, 0);
	int red = GetColor(255, 0, 0);
	const int HEIGHT = 20;
	const int WIDTH = 300;
	for (int i = MM1; i <= MM20; i++) {
		int x = 10 + ((i / 10) * (WIDTH + 10));
		int y = 50 + ((i % 10) * (HEIGHT + 2));
		if(clear[i]){ 
			DrawBox(x, y, x + WIDTH, y + HEIGHT, color1, TRUE);
			DrawFormatString(x + 2, y + 2, red, "�N���A");
		}
		else {
			DrawBox(x, y, x + WIDTH, y + HEIGHT, color2, TRUE);
		}
		DrawFormatString(x + 80, y + 2, black, "%d����", i + 1);
		
		if (i <= MM3) {
			DrawFormatString(x + 150, y + 2, black, "1�� %d", i + 1);
		}
		else if (i <= MM6) {
			DrawFormatString(x + 150, y + 2, black, "2�� %d", i - MM3);
		}
		else if (i <= MM9) {
			DrawFormatString(x + 150, y + 2, black, "3�� %d", i - MM6);
		}
		else if (i <= MM13) {
			DrawFormatString(x + 150, y + 2, black, "4�� %d", i - MM9);
		}
		else if (i <= MM16) {
			DrawFormatString(x + 150, y + 2, black, "5�� %d", i - MM13);
		}
		else if (i <= MM19) {
			DrawFormatString(x + 150, y + 2, black, "6�� %d", i - MM16);
		}
		else if (i <= MM20) {
			DrawFormatString(x + 150, y + 2, black, "7��", i + 1);
		}
	}
	DrawFormatString(10, 450, black, "�N���A�����C�x���g�~�b�V�����F%d / %d", em_sum, (EM20 - EM1 + 1));
	return false;
}


void load_name(std::string name[]) {
	name[0] = "�A�J�c�L�@�����E��";
	name[1] = "�n���_�@���E�g";
	name[2] = "�I�I�^�@�J�c��";
	name[3] = "�C�V�@�P���X�P";
	name[4] = "�^�C���@�R�E�L";
	name[5] = "�I���_�@�A���R";
	name[6] = "�~���g�l�@�����J";
	name[7] = "�E�`���g�@�n���J";
	name[8] = "�C�o���J���@�e���R";
	name[9] = "�g���`�@�X�G";
	name[10] = "�n�搶";
	name[11] = "�o�搶";
	name[12] = "�p�搶";
	name[13] = "�q�搶";
	name[14] = "�r�搶";
	name[15] = "�s�搶";
	name[16] = "�t�搶";
	name[17] = "�u�搶";
	name[18] = "�v�搶";
	name[19] = "�w�搶";
	name[20] = "�x�搶";
	name[21] = "�y�Z��";
	name[22] = "�V���i�@�t�^�G";
	name[23] = "�N���G�@�q���^";
}
void load_detail(int num, int& handle, std::string& text) {
	text.erase();
	if (handle != -1) { DeleteGraph(handle); }
	switch (num) {
	case 0:
		handle = LoadGraph("stand/�A�J�c�L.png");
		text = "�Ԍ�����F�{��̎�l���B���ɍ��Âł���A�߂����ɐ��𔭂��Ȃ��B";
		text += "���c�Ə��a�c�Ƃ͗c����ł���A�e�F�̔��c���������Ƃ͌����Ĕے肵�Ȃ��B";
		text += "���������c�ł����Ԍ��̎v�l��m��Ȃ��B�����A���c�͐Ԍ��̂��Ƃ���ɐM�����Ă���B";
		text += "�퓬�ɂ����ẮA������̘A���U����h��͂𖳎������U��������B";
		break;
	case 1:
		handle = LoadGraph("stand/�n���_.png");
		text = "���c�T�l�F���Ԃ̒��S�ɗ����čs�������郊�[�_�[�B�Ԍ��Ƃ͗c����ł���A�e�F�ł���B";
		text += "���a�c�Ƃ��c����ł���A���N�O����t�������Ă��邪���ɃJ�b�v���炵�����Ƃ͂��Ă��Ȃ��B";
		text += "���̓��[�_�[�̑f���͂Ȃ��A�N���X�ł��ڗ����Ȃ����k�ł���B";
		text += "�퓬�ɂ����ẮA���Ԃ̍U���͂��グ����A�񕜂�������ƁA���Ԃ̃T�|�[�g�������Ȃ��B";
		break;
	case 2:
		handle = LoadGraph("stand/�I�I�^.png");
		text = "���c����F��ɐ擪�ɗ����Ē��Ԃ������悤�Ƃ���M���j�B";
		text += "���c�Ƃ͂悭�l�����Փ˂��Č��܂��邪�A�݂��ɔF�ߍ������ł���B";
		text += "�^���_�o�����ɗǂ����A�׋��̕��͖F�����Ȃ��B";
		text += "�퓬�ɂ����ẮA���͂ȍU���������Ȃ��B�܂��A�䖝�ɂ��P�Ƃł̍U�������ӂƂ���B";
		break;
	case 3:
		handle = LoadGraph("stand/�C�V.png");
		text = "�Ɏu����F���邭���C�ŁA�V�т���D���ȏ��N�B";
		text += "�䗅�Ƃ͕t�������������A�e�F�ł���B";
		text += "���z�͂ɗD��A�׋������������ł��邪�A�^���͑�̋��ł���B";
		text += "�܂��A�����ǂ��l�ȊO�����肾�Ə��ɓI�ɂȂ�l���m��ł���B";
		text += "�C��Ȗʂ�����A�����Ȃɂ��ƔY�݂�����Ă���B";
		text += "�퓬�ɂ����ẮA����ɏ�Ԉُ��t�^���ėl�X�Ȑ킢�����ł���B";
		text += "�܂��A���Ԃ̃X�s�[�h���グ�邱�Ƃ��ł���B";
		break;
	case 4:
		handle = LoadGraph("stand/�^�C��.png");
		text = "�䗅���I�F���ɓ��������V�ˁB";
		text += "���R�Ǝ���̐l�ƒ��ǂ��Ȃ�邪�A�䗅�ɂƂ��Ĉ�Ԃ̗F�l�͈Ɏu�ł���B";
		text += "���܂�׋������Ȃ����A����e�X�g�Ŋw�N�ꌅ�̏��ʈȊO�ɂȂ������Ƃ��Ȃ��B";
		text += "�ƂĂ��N�[���Ȑ��i�����A���������Ȃ��킯�ł͂Ȃ��A�v�������Ƃ𐳒��Ɍ������Ƃ������B";
		text += "�퓬�ɂ����ẮA����̍U���͂������邱�Ƃ��ł���B";
		text += "�܂��A�g���B���Ĉ��S�ɍU���������Ȃ����Ƃ��ł���B";
		break;
	case 5:
		handle = LoadGraph("stand/�I���_.png");
		text = "���a�c�ǎq�F�l�]������A�F�l�̑������k�B";
		text += "�Ԍ��Ɣ��c�Ƃ͗c����ł���A���c�Ƃ͐��N�O����t�������Ă���B";
		text += "�׋��Ɖ^���Ƃ��ɕ��ψȏ�ŁA�l�t�����������܂��B";
		text += "�܂��A���s�̂��̂ɖڂ��Ȃ��A���Ƀt�@�b�V�����ɂ͏�ɋC��z���Ă���B";
		text += "�퓬�ɂ����ẮA����S�̂ւ̍U���ő����|�M����B";
		text += "�܂��A�����̏�Ԉُ���������Ƃ��ł���B";
		break;
	case 6:
		handle = LoadGraph("stand/�~���g�l.png");
		text = "�{�������ԁF�F�l�͂قƂ�ǂ��炸�A�����׋����Ă��邪�A";
		text += "����e�X�g�̏��ʂ͈ꌅ�ɂȂ�邩�ǂ����A�Ƃ������Ƃ���B";
		text += "�׋��΂��肵�Ă���̂͐e������������Ɖ\����Ă��邪�A�^���͒肩�ł͂Ȃ��B";
		text += "�w�N���w�̔��l�ƌ����邪�A���l�Ɗւ�邱�Ƃ��Ȃ����߂��܂�Ӗ��̂Ȃ��b�B";
		text += "�퓬�ɂ����āA�����g�o���������ēG�̒��ӂ������t����B";
		text += "�����⎩�g�̖h��͂��グ�邱�Ƃ��ł���B";
		break;
	case 7:
		handle = LoadGraph("stand/�E�`���g.png");
		text = "���{�t���F���C�����ς��̓V�R�����B";
		text += "�^�������ӂ����A�׋��͋��B�������a�c�ɕ׋��������Ă��炤���A";
		text += "���܂茋�ʂɂ͌q�����Ă��Ȃ��B";
		text += "�ׂ������Ƃ��C�ɂ����A�����Ȑ��i�䂦�ɃN���X�ł͔�r�I�l�C�ҁB";
		text += "�퓬�ɂ����Ă͑f�����U�����J��o���đ���̓����𐧌�����B";
		text += "����ɁA����̃X�s�[�h�������邱�Ƃ��ł���B";
		break;
	case 8:
		handle = LoadGraph("stand/�C�o���J��.png");
		text = "���V�q�F���N�O�ɉ�������������z���Ă����]�Z���B";
		text += "��N���o�ƁA�]�Z���ł��邱�Ƃ�m��Ȃ��l�Ƃ̊ւ��������B";
		text += "�N�[���Ȑ��i�ŁA�����z���Ǝv��ꂪ�������{�l�͂��܂�C�ɂ��Ă��Ȃ��l�q�B";
		text += "�^�������т����ϒ��x�ŁA�ِ��Ƃ̊ւ��͂قƂ�ǂȂ��B";
		text += "�퓬�ɂ����ẮA�G�̍U���ɑ΂��ĉ����J�E���^�[�������Ȃ��B";
		break;
	case 9:
		handle = LoadGraph("stand/�g���`.png");
		text = "�u�p�{�b�F�^���A�׋��A�痧���A�l�t���������ׂĂ����X�y�b�N�Ȋ����l�ԁB";
		text += "���݂͊�s���ڗ����A����̐l����������Ă��邪���w���čŏ��̂����";
		text += "���ɂ܂Ƃ��Ŋw�N���w�̐l�C�҂������B";
		text += "���݂̔ޏ��Ɏ����������l�͐��k�����ł͂Ȃ��B";
		text += "�퓬�ɂ�����X�y�b�N�������A���肵���_���[�W�����҂ł���B";
		text += "����ɑ���̖h��͂������邱�Ƃ��ł���B";
		break;
	case 10:
		handle = LoadGraph("enemy/O.png");
		text = "����̒S�������B���̊w�Z�ł̋Ζ��͐��N�قǁB";
		text += "���k����̕]���͗ǂ����������Ȃ��B���������b��ɂȂ�Ȃ��B";
		text += "���ƒ��ɃN���X�����������Ȃ��Ă����ɒ��ӂ͂��Ȃ��B";
		text += "�퓬�ɂ����Ă͓��ɂ���Ƃ����������I�ȍs���͂Ƃ�Ȃ��B";
		text += "�V���v���ȍU�������Ă���B";
		break;
	case 11:
		handle = LoadGraph("enemy/P.png");
		text = "���w�̒S�������B���̊w�Z�ł̋Ζ��͂܂��������󂢁B";
		text += "���k����̕]���͎^�ۗ��_�ł���B����͔ނ̋C��Ȑ��i�������ł���B";
		text += "���ƒ��ɃN���X�����������Ȃ��Ă��S�O���Ȃ����Ȃ����Œ��ӂ�����B";
		text += "���肪�Ȃ��ƒQ�����k������΁A�C�y���Ɗ�Ԑ��k������B";
		text += "�悭�s�ǂ̂����߂̃^�[�Q�b�g�ɂȂ�B";
		text += "�퓬�ɂ����Ă͖h��͂̏㏸�𓾈ӂƂ���B";
		break;
	case 12:
		handle = LoadGraph("enemy/Q.png");
		text = "�̈�̒S�������B���̊w�Z�ł̋Ζ��͂܂��������󂢁B";
		text += "���k����̕]���͂��܂�悭�Ȃ��B";
		text += "�̈狳�����L�̗��s�s�Ȍ����������Ǝv����B";
		text += "���㕔�̌ږ₾���A���㕔������̕]���͂܂������}�V�ȗl�q�B";
		text += "�s�ǂɑ΂��ėD�����B�����Đ^�ʖڂȐ��k�ɂ͖��֐S�B";
		text += "�퓬�ɂ����Ă̓X�s�[�h�̏㏸�𓾈ӂƂ���B";
		break;
	case 13:
		handle = LoadGraph("enemy/R.png");
		text = "�Љ�̒S�������B���̊w�Z�ł̋Ζ��͐��N�قǁB";
		text += "���k����̕]���͔��ɗǂ��B";
		text += "���Ƃ̐i�ߕ���ނ̃W���[�N�����ɑ����̐��k����x������Ă���B";
		text += "�����Ĕނ����D�����l���̃I�[�������R�ƐM�����l������̂��낤�B";
		text += "�퓬�ɂ����Ă͑S�̍U���𓾈ӂƂ���B";
		break;
	case 14:
		handle = LoadGraph("enemy/S.png");
		text = "�Z�p�̒S�������B���̊w�Z�ł̋Ζ��͐��N�قǁB";
		text += "���k����̕]���͂��܂�悭�Ȃ��B�������A�ނ������Ƃ������A";
		text += "�������Ɗ����鐶�k���唼�Ƃ������l�q�B";
		text += "���ɂ܂��߂Ȑ��i�ŁA�Z���Ɉᔽ�������k��������ƁA�s�ǂł��낤�ƒ��ӂ���B";
		text += "�퓬�ɂ����Ă͐��k�̃X�s�[�h�������邱�Ƃ������B";
		break;
	case 15:
		handle = LoadGraph("enemy/T.png");
		text = "�ی��̐搶�B���̊w�Z�ɍ��N����Ζ����Ă���B";
		text += "���k�Ƃ̕t�������͔��ɒZ���ɂ�������炸�A���k����̐l�C�͍����B";
		text += "���k�ɂ���Ă͂قƂ�Ǌւ�邱�Ƃ��Ȃ����A�t�ɂ悭�����b�ɂȂ鐶�k������B";
		text += "���ɗD�����A��ɉ��₩�Ȍ����Řb���B";
		text += "�퓬�ɂ����Ă͐��k�̍U���͂�������B�܂��A���Ԃ⎩�g�̗̑͂��񕜂��邱�Ƃ��ł���B";
		break;
	case 16:
		handle = LoadGraph("enemy/U.png");
		text = "����̒S�������B���̊w�Z�ł̋Ζ��͒����B";
		text += "���k����̕]���͎^�ۗ��_�B���q���k����̕]���������B";
		text += "���Ƃ͔��ɂ킩��₷���A���Ǝ��̂̕]���͂����B";
		text += "���鐶�k���E�����֍s�����Ƃ��A���H�ٓ̕����傫���Đ搶�̊炪�����Ȃ������Ƃ����B";
		text += "�퓬�ɂ����Ă͉䖝�ɂ���čU�����L�����Z�������̂�h������ł���B�܂��A�̗͂������B";
		break;
	case 17:
		handle = LoadGraph("enemy/V.png");
		text = "���Ȃ̒S�������B���̊w�Z�ł̋Ζ��͐��N�قǁB";
		text += "���k����̕]���͔�r�I�ǂ��B";
		text += "���������Ȃ��A�C���邰�ȗl�q�Ŏ��Ƃ��s���B";
		text += "�������A�����̍ۂ͌����������A�e���V���������������Ȃ�B";
		text += "���̗l�q���������낢�ƌ������k�͏��Ȃ��Ȃ��B";
		text += "�퓬�ɂ����Ă͓ł�t�^����̂𓾈ӂƂ��A�h��͂������邱�Ƃ��ł���B";
		break;
	case 18:
		handle = LoadGraph("enemy/W.png");
		text = "���w�̒S�������B���̊w�Z�ł̋Ζ��͒����B";
		text += "���k����̕]���͔��Ɉ����B";
		text += "��������R�ŁA�ނ͂������k�ɑ΂��Č�����f�������蔭��������B";
		text += "���Ƃ����ɂ킩��ɂ����B���ɐ����������Ȃ���W�X�Ɛ���������B";
		text += "�����A�ނ͗L����w�𑲋Ƃ��Ă���A���w�̔\�͎��̂͑��̐��w�����̔�ł͂Ȃ��B";
		text += "�퓬�ɂ����Ă͎��g�ւ̍U���ɑ΂���J�E���^�[�⑊���Ⴢ������肷��B";
		break;
	case 19:
		handle = LoadGraph("enemy/X.png");
		text = "�̈�̒S�������B���̊w�Z�ɍ��N����Ζ����Ă���B";
		text += "���k����̕]���͗ǂ����������Ȃ��B";
		text += "���N����Ζ����Ă���ɂ��ւ�炸�A�����̒��ł���r�I�̂��n�ʂɂ���l�q�B";
		text += "�����炭�N�z�҂ł��邱�Ƃ����R�ł���B";
		text += "���Ƃł͂悭���g�����k�̒��ɍ������ĉ^�����Ă���B";
		text += "�퓬�ŌJ��o���U���͔��ɍU���͂������A���̂����U���͂��グ�邱�Ƃ��ł���B";
		break;
	case 20:
		handle = LoadGraph("enemy/Y.png");
		text = "�ƒ�Ȃ̒S�������B���̊w�Z�ł̋Ζ��͐��N�قǁB";
		text += "���k����̕]���͔�r�I�ǂ��B";
		text += "�ŏ��͕|���Ǝv�����k���������A���Ƃ��󂯂邤���Ɉ�ۂ��ς��B";
		text += "���Ƃ͖ʔ����ƕ]���ł���B";
		text += "�����ڒʂ�̌����Řb�����A�b�����e���͔̂��ɂ܂Ƃ��ŗD�����B";
		text += "���ɓ��ӂȂ̂͗����ŁA�������K�ł͂��̃X�L���̕З؂�������B";
		text += "�퓬�ɂ����Ă͑f�����U���𓾈ӂƂ���B�܂��A�U����������邱�Ƃ�����B";
		break;
	case 21:
		handle = LoadGraph("enemy/Z.png");
		text = "���̊w�Z�̍Z���B�Ζ����͒������A�Z���ɂȂ����̂͐��N�O�B";
		text += "���k����̕]���͗ǂ����������Ȃ��B";
		text += "�ނƊւ�鐶�k�͂قƂ�ǂ��Ȃ��Ǝv����B";
		text += "�����������i�������Ă���̂����琶�k�͒m��Ȃ��B";
		text += "���܂ɁA���ƒ��L��������Ď��ƕ��i���m�F����p��ڌ����邱�Ƃ��ł���B";
		text += "�퓬�ɂ�����X�e�[�^�X�͖��m�������A�����炭�S�Ăɂ����č����X�y�b�N�������Ă���ƍl������B";
		break;
	case 22:
		handle = -1;
		text = "���ߓ�b�F���̊w�Z�̐��k�ł���A�Ԍ������̓������B";
		text += "���}�Ƃ͐̂�����ɒ����ǂ��B�����Ȕނ����������Ă����悤�Ǝv���Ă���B";
		text += "�N���X�ł͖ڗ����Ȃ����A�F�l�͂�����������l�q�B";
		text += "�悭�h�W�𓥂ނ��A�{�l�͂��̕p�x�̍����ɋC�Â��Ă��Ȃ��B";
		break;
	case 23:
		handle = -1;
		text = "���}�L���F���̊w�Z�̐��k�ł���A�Ԍ������̓������B";
		text += "���߂Ƃ͐̂�����ɒ����ǂ��B��Ȃ��������ޏ�����낤�Ǝv���Ă���B";
		text += "�N���X�ł͖ڗ����Ȃ����A�F�l�͂�����������l�q�B";
		text += "���������A�\��͖L���ł���ׂ�Ȃ��Ă��ӎv�a�ʂ͂��₷���B";
		break;
	}
}
const int DATA_SUM = 24;//�f�[�^�̐�
const int DISP_MAX = 10;
const int C_WIDTH = 200;
const int C_HEIGHT = 20;
const int BLUE = GetColor(150, 150, 250);
const int BLUE2 = GetColor(50, 50, 200);
const int GREEN = GetColor(0, 255, 0);
const int GRAY = GetColor(100, 100, 100);
const int BLACK = GetColor(0, 0, 0);
bool DataBase::watch_character() {

	static std::string name[DATA_SUM];//�L�����̖��O�S����

	if (name[0].size() == 0) {
		load_name(name);
	}
	int x = 20;
	int y = 100;
	//��
	if (c_num != 0) {
		if (hand_x >= x && hand_x <= x + C_WIDTH && hand_y >= y && hand_y <= y + C_HEIGHT) {
			DrawBox(x - 2, y - 2, x + C_WIDTH + 2, y + C_HEIGHT + 2, GREEN, TRUE);
			if (left_click() == 1) {
				c_num--;
				PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//��
			}
		}
		DrawBox(x, y, x + C_WIDTH, y + C_HEIGHT, GRAY, TRUE);
		DrawFormatString(x + 90, y + 2, BLACK, "��");
	}
	y += (10 + C_HEIGHT) * (DISP_MAX + 1);
	//��
	if (c_num < DATA_SUM - DISP_MAX) {
		if (hand_x >= x && hand_x <= x + C_WIDTH && hand_y >= y && hand_y <= y + C_HEIGHT) {
			DrawBox(x - 2, y - 2, x + C_WIDTH + 2, y + C_HEIGHT + 2, GREEN, TRUE);
			if (left_click() == 1) {
				c_num++;
				PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//��
			}
		}
		DrawBox(x, y, x + C_WIDTH, y + C_HEIGHT, GRAY, TRUE);
		DrawFormatString(x + 90, y + 2, BLACK, "��");
	}
	y = 100;
	//�e�f�[�^����DISP_MAX�\��
	for (int i = 0; i < DISP_MAX; i++) {
		y += 10 + C_HEIGHT;
		if (hand_x >= x && hand_x <= x + C_WIDTH && hand_y >= y && hand_y <= y + C_HEIGHT) {
			DrawBox(x - 2, y - 2, x + C_WIDTH + 2, y + C_HEIGHT + 2, GREEN, TRUE);
			if (left_click() == 1) {
				c_now = i + c_num;
				load_detail(c_now, handle, text);
				PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//��
			}
		}
		if (i + c_num == c_now) {
			DrawBox(x, y, x + C_WIDTH, y + C_HEIGHT, BLUE2, TRUE);
		}
		else {
			DrawBox(x, y, x + C_WIDTH, y + C_HEIGHT, BLUE, TRUE);
		}
		DrawFormatString(x + 2, y + 2, BLACK, name[i + c_num].c_str());
	}
	if (c_now != -1) {
		DrawGraph(350, 10, handle, FALSE);
		unsigned int i = 0;
		const unsigned int SINGLE_MAX = 40;
		while (i < text.size()) {
			std::string single;
			int j = 0;
			while (j < SINGLE_MAX) {
				if (i == text.size()) { 
					i += SINGLE_MAX;
					break;
				}
				single += text[i];
				j++;
				i++;
			}
			DrawFormatString(270, 280 + (i / SINGLE_MAX * 20), BLACK, single.c_str());
		}
	}
	return false;
}