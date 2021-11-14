#ifndef INCLUDED_MISSION_H
#define INCLUDED_MISSION_H

#include"sequence.h"
#include"text.h"
#include"movie.h"

//各ミッションの数値割り当て
enum {
	NO_MISSION = -1,
	//MainMission
	MM1,
	MM2,
	MM3,
	MM4,
	MM5,
	MM6,
	MM7,
	MM8,
	MM9,
	MM10,
	MM11,
	MM12,
	MM13,
	MM14,
	MM15,
	MM16,
	MM17,
	MM18,
	MM19,
	MM20,
	//SubMission
	SM1,
	SM2,
	SM3,
	SM4,
	SM5,
	SM6,
	SM7,
	SM8,
	SM9,
	SM10,
	SM11,
	SM12,
	SM13,
	SM14,
	SM15,
	SM16,
	SM17,
	SM18,
	SM19,
	SM20,
	//DailyMission
	DM1,
	DM2,
	DM3,
	DM4,
	DM5,
	DM6,
	DM7,
	DM8,
	DM9,
	DM10,
	DM11,//後々追加するかもしれないDM11 ～ DM20
	DM12,
	DM13,
	DM14,
	DM15,
	DM16,
	DM17,
	DM18,
	DM19,
	DM20,
	//EventMission
	EM1,
	EM2,
	EM3,
	EM4,
	EM5,
	EM6,
	EM7,
	EM8,
	EM9,
	EM10,
	EM11,
	EM12,
	EM13,
	EM14,
	EM15,
	EM16,
	EM17,
	EM18,
	EM19,
	EM20,
	EM21,
	EM22,
	EM23,
	EM24,
	EM25,
	EM26,
	EM27,
	EM28,
	EM29,
	EM30,
	EM31,
	EM32,
	EM33,
	EM34,
	EM35,
	EM36,
	EM37,
	EM38,
	EM39,
	EM40,
	//ミッション総数
	M_TOTAL
};
void mission_rule(int today_mission[], bool clear[], int day);

//ミッションの概要
class MissionText {
private:
	std::string name;//ミッション名
	std::string text;//ミッション概要
	int kind[3] = { 0,0,0 };//固定キャラ
	int teacher[3] = { 0,0,0 };//敵
	int difficult;//難易度
	int reward;//報酬の金額
public:
	MissionText(int m_kind);
	bool picture_mission(int x, int y, int hand_x, int hand_y, bool clear);
	void set_st(int first, int second, int third);
	void set_te(int first, int second, int third);
	void set_te_random();
	int* get_student();
	int* get_teacher();
	int get_reward();
};

class Mission {
protected:
	Text* com;
	Bsequence* bse;
	int kind[3] = { 0,0,0 };//生徒の種類
	bool fixed[3] = { false, false, false };//生徒の種類が固定ならtrue
	Enemy teacher[3];//ミッションの種類に応じて作る
	int now;//会話、バトル、会話の流れ0->1->2...
	int result;//バトルの結果
	int reward_money;//報酬金額
	int rare[4];//報酬のカードのレアの確率
	int* bgm;//BGM
protected:
	void kind_set(int first, int second, int third);//先生の種類を一気にセットする
	void size_set(int first, int second, int third);//先生のデッキサイズを一気にセットする
	void st_set(bool f, bool s, bool t, int first, int second, int third);//生徒のfixedとkindを一気にセットする
	void text_play();//会話パート終了
	void set_rare_rate(int c_rate, int b_rate, int a_rate, int s_rate);//報酬のレア割合をセット
public:
	Mission(MissionText& detail);
	~Mission();
	void init();//カードのロードを先生3人分とBsequence用意
	virtual int play() = 0;
public:
	//報酬を受け取る 受け取ったらtrueを返す
	bool reward(int result, int& money, Card& c);
protected:
	void move(int num, int kind, int gx, int speed);
	void event(int num, const char* path);
	void finish(int num);
	void play_bgm(int handle, int num1, int num2);
	void volume_bgm(int sum);
protected:
	int normal_text();
	int after_text();
	virtual void text1() = 0;
	virtual void text2() = 0;
};

//kindに応じて動的にミッションを確保する
void get_mission(Mission*& m, int kind, MissionText& detail);
int student_sum(int kind[]);

class MainMission1 :
	public Mission {
public:
	MainMission1(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class MainMission2 :
	public Mission {
public:
	MainMission2(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class MainMission3 :
	public Mission {
public:
	MainMission3(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class MainMission4 :
	public Mission {
public:
	MainMission4(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class MainMission5 :
	public Mission {
public:
	MainMission5(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class MainMission6 :
	public Mission {
public:
	MainMission6(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class MainMission7 :
	public Mission {
public:
	MainMission7(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class MainMission8 :
	public Mission {
public:
	MainMission8(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class MainMission9 :
	public Mission {
public:
	MainMission9(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class MainMission10 :
	public Mission {
public:
	MainMission10(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class MainMission11 :
	public Mission {
public:
	MainMission11(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class MainMission12 :
	public Mission {
public:
	MainMission12(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class MainMission13 :
	public Mission {
public:
	MainMission13(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class MainMission14 :
	public Mission {
public:
	MainMission14(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class MainMission15 :
	public Mission {
public:
	MainMission15(MissionText& detail);
	int play();
	void text1();
	void text2();
	void text3();
	void text4();
	void text5();
};

class MainMission16 :
	public Mission {
public:
	MainMission16(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class MainMission17 :
	public Mission {
public:
	MainMission17(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class MainMission18 :
	public Mission {
public:
	MainMission18(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class MainMission19 :
	public Mission {
public:
	MainMission19(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class MainMission20 :
	public Mission {
public:
	MainMission20(MissionText& detail);
	int play();
	void text1();
	void text2();
	Ed* ed;
};

//デイリーミッション
class DailyMission1 :
	public Mission {
public:
	DailyMission1(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class DailyMission2 :
	public Mission {
public:
	DailyMission2(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class DailyMission3 :
	public Mission {
public:
	DailyMission3(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class DailyMission4 :
	public Mission {
public:
	DailyMission4(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class DailyMission5 :
	public Mission {
public:
	DailyMission5(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class DailyMission6 :
	public Mission {
public:
	DailyMission6(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class DailyMission7 :
	public Mission {
public:
	DailyMission7(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class DailyMission8 :
	public Mission {
public:
	DailyMission8(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class DailyMission9 :
	public Mission {
public:
	DailyMission9(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class DailyMission10 :
	public Mission {
public:
	DailyMission10(MissionText& detail);
	int play();
	void text1();
	void text2();
};

//イベントミッション
class EventMission1 :
	public Mission {
public:
	EventMission1(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission2 :
	public Mission {
public:
	EventMission2(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission3 :
	public Mission {
public:
	EventMission3(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission4 :
	public Mission {
public:
	EventMission4(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission5 :
	public Mission {
public:
	EventMission5(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission6 :
	public Mission {
public:
	EventMission6(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission7 :
	public Mission {
public:
	EventMission7(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission8 :
	public Mission {
public:
	EventMission8(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission9 :
	public Mission {
public:
	EventMission9(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission10 :
	public Mission {
public:
	EventMission10(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission11 :
	public Mission {
public:
	EventMission11(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission12 :
	public Mission {
public:
	EventMission12(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission13 :
	public Mission {
public:
	EventMission13(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission14 :
	public Mission {
public:
	EventMission14(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission15 :
	public Mission {
public:
	EventMission15(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission16 :
	public Mission {
public:
	EventMission16(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission17 :
	public Mission {
public:
	EventMission17(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission18 :
	public Mission {
public:
	EventMission18(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission19 :
	public Mission {
public:
	EventMission19(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission20 :
	public Mission {
public:
	EventMission20(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission21 :
	public Mission {
public:
	EventMission21(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission22 :
	public Mission {
public:
	EventMission22(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission23 :
	public Mission {
public:
	EventMission23(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission24 :
	public Mission {
public:
	EventMission24(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission25 :
	public Mission {
public:
	EventMission25(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission26 :
	public Mission {
public:
	EventMission26(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission27 :
	public Mission {
public:
	EventMission27(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission28 :
	public Mission {
public:
	EventMission28(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission29 :
	public Mission {
public:
	EventMission29(MissionText& detail);
	int play();
	void text1();
	void text2();
};

class EventMission30 :
	public Mission {
public:
	EventMission30(MissionText& detail);
	int play();
	void text1();
	void text2();
};
#endif