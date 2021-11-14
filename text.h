#ifndef INCLUDED_TEXT_H
#define INCLUDED_TEXT_H

#include<string>
#include<queue>
#include<utility>

#define STAND_Y 230

enum {
	H_NORMAL,
	H_MOUSOU
};

class Text {
private:
	int dot_handle[10];
	int teacher_handle;
	std::pair<int, int> xy[10];
	std::pair<int, int> te_xy;
	int haikei;
	int event_handle = -1;
	int hukidasi_handle[2];
	int kettei_sound;
	int max_size;
	int event_cnt;
	bool play_flag;
	bool finish_flag;
private:
	std::string display;
	std::string display_first;
	std::string display_second;
	std::queue<std::string> texts;
	std::string name;
	std::queue<std::string> names;
	int hukidasi_kind;//フキダシの種類 通常or妄想
	int speaker;//フキダシが誰から出てるか
	std::queue<std::pair<int, int> > hukidasi_set;
public:
	Text(int x[10], int te_x);
	~Text();
	void text_insert(const char* name_str, const char* text_str, int h_kind, int s_kind);
	int get_num();
	bool get_flag();
	void set_event(int handle);
	void event_cancel();
	int play();
	int move(int kind, int gx, int speed);
	int event();
	int finish();
private:
	void next_text();
};


#endif