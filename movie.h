#ifndef INCLUDED_MOVIE_H
#define INCLUDED_MOVIE_H

class Op {
private:
	int handle1;
	int handle2;
	int music;
	int font;
	const int INIT_X = -500;
	const int INIT_X2 = 100;
	const int MUSIC_START = 100;
	const int MUSIC_RESTART = 7000;
	int cnt = 0;
	int x = INIT_X;
	int x2 = INIT_X2;
	int finish_flag = false;
public:
	Op();
	~Op();
	bool play();
};

class Ed {
private:
	int akatsuki_handle;
	int all_handle[2];
	int class_room_handle[3];
	int cry_handle[3];
	int dark_handle[6];
	int eye_handle[3];
	int final_handle[3];
	int miyatone_handle[3];
	int miyatone_n_handle[3];
	int miyatone_46_handle[3];
	int note_handle[2];
	int music;
	int font;
private:
	const int INIT_X = -100;
	const int MUSIC_START = 100;
	const int MUSIC_RESTART = 7000;
	int cnt = 0;
	int bright = 255;
	int x = INIT_X;
	int y = 0;
private:
	const int MIYATONE_FLAME = 10;
private:
	const int MIYATONE_FINISH = 2150;
	const int CLASS_ROOM_FINISH = 2850;
	const int NOTE_FINISH = 3500;
	const int DARK_CHANGE = 4150;
	const int DARK_FINISH = 4300;
	const int CRY_FINISH = 4450;
	const int MIYATONE_N_FINISH = 5500;
	const int EYE_FINISH = 5600;
	const int ALL_1_FINISH = 6050;
	const int ALL_2_FINISH = 6500;
	const int AKATSUKI_FINISH = 6850;
	const int FINAL_FINISH = 7000;

public:
	Ed();
	~Ed();
	void black_out(const int TIME);
	int get_num(const int START, const int FINISH, const int sum);
	bool play();
};


#endif