#include"data.h"
#include"DxLib.h"
#include"control.h"
#include"create.h"


DataBase::DataBase(bool clear_data[]) {
	student_handle[0] = LoadGraph("stand/アカツキ.png");
	student_handle[1] = LoadGraph("stand/ハンダ.png");
	student_handle[2] = LoadGraph("stand/オオタ.png");
	student_handle[3] = LoadGraph("stand/イシ.png");
	student_handle[4] = LoadGraph("stand/タイラ.png");
	student_handle[5] = LoadGraph("stand/オワダ.png");
	student_handle[6] = LoadGraph("stand/ミヤトネ.png");
	student_handle[7] = LoadGraph("stand/ウチモト.png");
	student_handle[8] = LoadGraph("stand/イバラカワ.png");
	student_handle[9] = LoadGraph("stand/トモチ.png");
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
	kettei_sound = LoadSoundMem("sound/決定.wav");
	em_sum = 0;
	for (int i = 0; i < M_TOTAL; i++) {
		clear[i] = clear_data[i];
		if (i >= EM1 && i <= EM20 && clear[i]) {
			em_sum++;
		}
	}
	now = DATA_MAIN;
	font_handle = CreateFontToHandle(NULL, 35, 4);//フォント
}

DataBase::~DataBase() {

}

int DataBase::get_now() {
	return now;
}

//trueを返して終了
bool DataBase::play() {
	int indicate = 0;
	DrawBox(0, 0, 640, 480, GetColor(190, 120, 100), TRUE);//背景
	GetMousePoint(&hand_x, &hand_y);
	if (now == DATA_MAIN) {//メイン画面
		if (hand_x >= 10 && hand_x <= 270 && hand_y >= 150 && hand_y <= 250) {
			DrawBox(8, 148, 272, 252, GetColor(200, 200, 0), TRUE);
			if (left_click() == 1) { 
				now = DATA_CHARA;
				PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//音
			}
		}
		DrawBox(10, 150, 270, 250, GetColor(100, 150, 100), TRUE);
		DrawStringToHandle(20, 190, "キャラクター", GetColor(0, 0, 0), font_handle);
		if (hand_x >= 420 && hand_x <= 620 && hand_y >= 150 && hand_y <= 250) {
			DrawBox(418, 148, 622, 252, GetColor(200, 200, 0), TRUE);
			if (left_click() == 1) { 
				now = DATA_CLEAR;
				PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//音
			}
		}
		DrawBox(420, 150, 620, 250, GetColor(100, 150, 100), TRUE);
		DrawStringToHandle(430, 190, "クリア状況", GetColor(0, 0, 0), font_handle);
		//OP
		if (hand_x >= 20 && hand_x <= 220 && hand_y >= 350 && hand_y <= 450) {
			DrawBox(18, 348, 222, 452, GetColor(200, 200, 0), TRUE);
			if (left_click() == 1) {
				now = DATA_OP;
				op = new Op;
				PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//音
			}
		}
		DrawBox(20, 350, 220, 450, GetColor(100, 150, 100), TRUE);
		DrawStringToHandle(50, 390, "OPを観る", GetColor(0, 0, 0), font_handle);
		//ED
		if (clear[MM20] && hand_x >= 420 && hand_x <= 620 && hand_y >= 350 && hand_y <= 450) {
			DrawBox(418, 348, 622, 452, GetColor(200, 200, 0), TRUE);
			if (left_click() == 1) {
				now = DATA_ED;
				ed = new Ed;
				PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//音
			}
		}
		if(clear[MM20]) {
			DrawBox(420, 350, 620, 450, GetColor(100, 150, 100), TRUE);
		}
		else {
			DrawBox(420, 350, 620, 450, GetColor(50, 100, 50), TRUE);
		}
		DrawStringToHandle(450, 390, "EDを観る", GetColor(0, 0, 0), font_handle);
	}
	else if (now == DATA_CHARA) {//キャラ詳細画面
		watch_character();
	}
	else if (now == DATA_CLEAR) {//クリア状況画面
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

	//戻るアイコン
	modoru_icon(hand_x, hand_y, indicate);
	if (left_click() == 1 && indicate == -1) {
		if (now == DATA_MAIN) {
			return true;//終了
		}
		else {
			PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//音
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
			DrawFormatString(x + 2, y + 2, red, "クリア");
		}
		else {
			DrawBox(x, y, x + WIDTH, y + HEIGHT, color2, TRUE);
		}
		DrawFormatString(x + 80, y + 2, black, "%d日目", i + 1);
		
		if (i <= MM3) {
			DrawFormatString(x + 150, y + 2, black, "1章 %d", i + 1);
		}
		else if (i <= MM6) {
			DrawFormatString(x + 150, y + 2, black, "2章 %d", i - MM3);
		}
		else if (i <= MM9) {
			DrawFormatString(x + 150, y + 2, black, "3章 %d", i - MM6);
		}
		else if (i <= MM13) {
			DrawFormatString(x + 150, y + 2, black, "4章 %d", i - MM9);
		}
		else if (i <= MM16) {
			DrawFormatString(x + 150, y + 2, black, "5章 %d", i - MM13);
		}
		else if (i <= MM19) {
			DrawFormatString(x + 150, y + 2, black, "6章 %d", i - MM16);
		}
		else if (i <= MM20) {
			DrawFormatString(x + 150, y + 2, black, "7章", i + 1);
		}
	}
	DrawFormatString(10, 450, black, "クリアしたイベントミッション：%d / %d", em_sum, (EM20 - EM1 + 1));
	return false;
}


void load_name(std::string name[]) {
	name[0] = "アカツキ　リョウヤ";
	name[1] = "ハンダ　ユウト";
	name[2] = "オオタ　カツヤ";
	name[3] = "イシ　ケンスケ";
	name[4] = "タイラ　コウキ";
	name[5] = "オワダ　アンコ";
	name[6] = "ミヤトネ　モモカ";
	name[7] = "ウチモト　ハルカ";
	name[8] = "イバラカワ　テンコ";
	name[9] = "トモチ　スエ";
	name[10] = "Ｏ先生";
	name[11] = "Ｐ先生";
	name[12] = "Ｑ先生";
	name[13] = "Ｒ先生";
	name[14] = "Ｓ先生";
	name[15] = "Ｔ先生";
	name[16] = "Ｕ先生";
	name[17] = "Ｖ先生";
	name[18] = "Ｗ先生";
	name[19] = "Ｘ先生";
	name[20] = "Ｙ先生";
	name[21] = "Ｚ校長";
	name[22] = "シロナ　フタエ";
	name[23] = "クロエ　ヒロタ";
}
void load_detail(int num, int& handle, std::string& text) {
	text.erase();
	if (handle != -1) { DeleteGraph(handle); }
	switch (num) {
	case 0:
		handle = LoadGraph("stand/アカツキ.png");
		text = "赤月亮也：本作の主人公。非常に根暗であり、めったに声を発さない。";
		text += "半田と小和田とは幼馴染であり、親友の半田が言うことは決して否定しない。";
		text += "しかし半田でさえ赤月の思考を知らない。ただ、半田は赤月のことを非常に信頼している。";
		text += "戦闘においては、複数回の連続攻撃や防御力を無視した攻撃をする。";
		break;
	case 1:
		handle = LoadGraph("stand/ハンダ.png");
		text = "半田裕斗：仲間の中心に立って行動をするリーダー。赤月とは幼馴染であり、親友である。";
		text += "小和田とも幼馴染であり、数年前から付き合っているが特にカップルらしいことはしていない。";
		text += "実はリーダーの素質はなく、クラスでも目立たない生徒である。";
		text += "戦闘においては、仲間の攻撃力を上げたり、回復をしたりと、仲間のサポートをおこなう。";
		break;
	case 2:
		handle = LoadGraph("stand/オオタ.png");
		text = "太田勝也：常に先頭に立って仲間を助けようとする熱い男。";
		text += "半田とはよく考えが衝突して喧嘩するが、互いに認め合う仲である。";
		text += "運動神経が非常に良いが、勉強の方は芳しくない。";
		text += "戦闘においては、強力な攻撃をおこなう。また、我慢による単独での攻撃も得意とする。";
		break;
	case 3:
		handle = LoadGraph("stand/イシ.png");
		text = "伊志賢介：明るく元気で、遊びが大好きな少年。";
		text += "台羅とは付き合いが長く、親友である。";
		text += "発想力に優れ、勉強もそこそこできるが、運動は大の苦手である。";
		text += "また、仲が良い人以外が相手だと消極的になる人見知りである。";
		text += "気弱な面もあり、いつもなにかと悩みを抱えている。";
		text += "戦闘においては、相手に状態異常を付与して様々な戦い方ができる。";
		text += "また、仲間のスピードを上げることもできる。";
		break;
	case 4:
		handle = LoadGraph("stand/タイラ.png");
		text = "台羅功紀：非常に頭がいい天才。";
		text += "自然と周りの人と仲良くなれるが、台羅にとって一番の友人は伊志である。";
		text += "あまり勉強をしないが、定期テストで学年一桁の順位以外になったことがない。";
		text += "とてもクールな性格だが、口数が少ないわけではなく、思ったことを正直に言うことが多い。";
		text += "戦闘においては、相手の攻撃力を下げることができる。";
		text += "また、身を隠して安全に攻撃をおこなうこともできる。";
		break;
	case 5:
		handle = LoadGraph("stand/オワダ.png");
		text = "小和田杏子：人望があり、友人の多い生徒。";
		text += "赤月と半田とは幼馴染であり、半田とは数年前から付き合っている。";
		text += "勉強と運動ともに平均以上で、人付き合いもうまい。";
		text += "また、流行のものに目がなく、特にファッションには常に気を配っている。";
		text += "戦闘においては、相手全体への攻撃で相手を翻弄する。";
		text += "また、味方の状態異常を治すこともできる。";
		break;
	case 6:
		handle = LoadGraph("stand/ミヤトネ.png");
		text = "宮利根桃花：友人はほとんどおらず、いつも勉強しているが、";
		text += "定期テストの順位は一桁になれるかどうか、といったところ。";
		text += "勉強ばかりしているのは親が厳しいからと噂されているが、真相は定かではない。";
		text += "学年屈指の美人と言われるが、他人と関わることがないためあまり意味のない話。";
		text += "戦闘において、高いＨＰを活かして敵の注意を引き付ける。";
		text += "味方や自身の防御力を上げることもできる。";
		break;
	case 7:
		handle = LoadGraph("stand/ウチモト.png");
		text = "内本春香：元気いっぱいの天然少女。";
		text += "運動が得意だが、勉強は苦手。いつも小和田に勉強を教えてもらうが、";
		text += "あまり結果には繋がっていない。";
		text += "細かいことを気にせず、純粋な性格ゆえにクラスでは比較的人気者。";
		text += "戦闘においては素早い攻撃を繰り出して相手の動きを制限する。";
		text += "さらに、相手のスピードを下げることもできる。";
		break;
	case 8:
		handle = LoadGraph("stand/イバラカワ.png");
		text = "茨川天子：約一年前に遠い町から引っ越してきた転校生。";
		text += "一年も経つと、転校生であることを知らない人との関わりも多い。";
		text += "クールな性格で、無愛想だと思われがちだが本人はあまり気にしていない様子。";
		text += "運動も成績も平均程度で、異性との関わりはほとんどない。";
		text += "戦闘においては、敵の攻撃に対して回避やカウンターをおこなう。";
		break;
	case 9:
		handle = LoadGraph("stand/トモチ.png");
		text = "砥用須恵：運動、勉強、顔立ち、人付き合いすべてが高スペックな完璧人間。";
		text += "現在は奇行が目立ち、周りの人から避けられているが入学して最初のころは";
		text += "非常にまともで学年屈指の人気者だった。";
		text += "現在の彼女に首をかしげる人は生徒だけではない。";
		text += "戦闘におけるスペックも高く、安定したダメージが期待できる。";
		text += "さらに相手の防御力を下げることもできる。";
		break;
	case 10:
		handle = LoadGraph("enemy/O.png");
		text = "国語の担当教員。この学校での勤務は数年ほど。";
		text += "生徒からの評判は良くも悪くもない。そもそも話題にならない。";
		text += "授業中にクラスが騒がしくなっても特に注意はしない。";
		text += "戦闘においては特にこれといった特徴的な行動はとらない。";
		text += "シンプルな攻撃をしてくる。";
		break;
	case 11:
		handle = LoadGraph("enemy/P.png");
		text = "数学の担当教員。この学校での勤務はまだ月日が浅い。";
		text += "生徒からの評判は賛否両論である。それは彼の気弱な性格が原因である。";
		text += "授業中にクラスが騒がしくなっても躊躇いながら情けない声で注意をする。";
		text += "頼りがないと嘆く生徒もいれば、気楽だと喜ぶ生徒もいる。";
		text += "よく不良のいじめのターゲットになる。";
		text += "戦闘においては防御力の上昇を得意とする。";
		break;
	case 12:
		handle = LoadGraph("enemy/Q.png");
		text = "体育の担当教員。この学校での勤務はまだ月日が浅い。";
		text += "生徒からの評判はあまりよくない。";
		text += "体育教員特有の理不尽な言動が原因と思われる。";
		text += "陸上部の顧問だが、陸上部員からの評判はまだ少しマシな様子。";
		text += "不良に対して優しい。そして真面目な生徒には無関心。";
		text += "戦闘においてはスピードの上昇を得意とする。";
		break;
	case 13:
		handle = LoadGraph("enemy/R.png");
		text = "社会の担当教員。この学校での勤務は数年ほど。";
		text += "生徒からの評判は非常に良い。";
		text += "授業の進め方や彼のジョークが非常に多くの生徒から支持されている。";
		text += "そして彼が放つ優しい人柄のオーラが自然と信頼を獲得するのだろう。";
		text += "戦闘においては全体攻撃を得意とする。";
		break;
	case 14:
		handle = LoadGraph("enemy/S.png");
		text = "技術の担当教員。この学校での勤務は数年ほど。";
		text += "生徒からの評判はあまりよくない。しかし、彼が嫌いというより、";
		text += "うざいと感じる生徒が大半といった様子。";
		text += "非常にまじめな性格で、校則に違反した生徒を見つけると、不良であろうと注意する。";
		text += "戦闘においては生徒のスピードを下げることが多い。";
		break;
	case 15:
		handle = LoadGraph("enemy/T.png");
		text = "保健の先生。この学校に今年から勤務している。";
		text += "生徒との付き合いは非常に短いにもかかわらず、生徒からの人気は高い。";
		text += "生徒によってはほとんど関わることがないが、逆によくお世話になる生徒もいる。";
		text += "非常に優しく、常に穏やかな口調で話す。";
		text += "戦闘においては生徒の攻撃力を下げる。また、仲間や自身の体力を回復することもできる。";
		break;
	case 16:
		handle = LoadGraph("enemy/U.png");
		text = "国語の担当教員。この学校での勤務は長い。";
		text += "生徒からの評判は賛否両論。女子生徒からの評判が悪い。";
		text += "授業は非常にわかりやすく、授業自体の評判はいい。";
		text += "ある生徒が職員室へ行ったとき、昼食の弁当が大きくて先生の顔が見えなかったという。";
		text += "戦闘においては我慢によって攻撃がキャンセルされるのを防いだりできる。また、体力が多い。";
		break;
	case 17:
		handle = LoadGraph("enemy/V.png");
		text = "理科の担当教員。この学校での勤務は数年ほど。";
		text += "生徒からの評判は比較的良い。";
		text += "口数が少なく、気だるげな様子で授業を行う。";
		text += "しかし、実験の際は口数が増え、テンションが少し高くなる。";
		text += "その様子がおもしろいと言う生徒は少なくない。";
		text += "戦闘においては毒を付与するのを得意とし、防御力を下げることもできる。";
		break;
	case 18:
		handle = LoadGraph("enemy/W.png");
		text = "数学の担当教員。この学校での勤務は長い。";
		text += "生徒からの評判は非常に悪い。";
		text += "それも当然で、彼はいつも生徒に対して嫌味を吐いたり問題発言をする。";
		text += "授業も非常にわかりにくい。黒板に数式を書きながら淡々と説明をする。";
		text += "ただ、彼は有名大学を卒業しており、数学の能力自体は他の数学教員の比ではない。";
		text += "戦闘においては自身への攻撃に対するカウンターや相手を麻痺させたりする。";
		break;
	case 19:
		handle = LoadGraph("enemy/X.png");
		text = "体育の担当教員。この学校に今年から勤務している。";
		text += "生徒からの評判は良くも悪くもない。";
		text += "今年から勤務しているにも関わらず、教員の中でも比較的偉い地位にいる様子。";
		text += "おそらく年配者であることが理由である。";
		text += "授業ではよく自身も生徒の中に混じって運動している。";
		text += "戦闘で繰り出す攻撃は非常に攻撃力が高く、そのうえ攻撃力を上げることもできる。";
		break;
	case 20:
		handle = LoadGraph("enemy/Y.png");
		text = "家庭科の担当教員。この学校での勤務は数年ほど。";
		text += "生徒からの評判は比較的良い。";
		text += "最初は怖いと思う生徒が多いが、授業を受けるうちに印象が変わる。";
		text += "授業は面白いと評判である。";
		text += "見た目通りの口調で話すが、話す内容自体は非常にまともで優しい。";
		text += "特に得意なのは料理で、調理実習ではそのスキルの片鱗を見せる。";
		text += "戦闘においては素早い攻撃を得意とする。また、攻撃を回避することもある。";
		break;
	case 21:
		handle = LoadGraph("enemy/Z.png");
		text = "この学校の校長。勤務歴は長いが、校長になったのは数年前。";
		text += "生徒からの評判は良くも悪くもない。";
		text += "彼と関わる生徒はほとんどいないと思われる。";
		text += "そもそも普段何をしているのかすら生徒は知らない。";
		text += "たまに、授業中廊下を歩いて授業風景を確認する姿を目撃することができる。";
		text += "戦闘におけるステータスは未知数だが、おそらく全てにおいて高いスペックを持っていると考えられる。";
		break;
	case 22:
		handle = -1;
		text = "白那二恵：この学校の生徒であり、赤月たちの同級生。";
		text += "黒枝とは昔から非常に仲が良い。無口な彼を引っ張ってあげようと思っている。";
		text += "クラスでは目立たないが、友人はそこそこいる様子。";
		text += "よくドジを踏むが、本人はその頻度の高さに気づいていない。";
		break;
	case 23:
		handle = -1;
		text = "黒枝広太：この学校の生徒であり、赤月たちの同級生。";
		text += "白那とは昔から非常に仲が良い。危なっかしい彼女を守ろうと思っている。";
		text += "クラスでは目立たないが、友人はそこそこいる様子。";
		text += "無口だが、表情は豊かでしゃべらなくても意思疎通はしやすい。";
		break;
	}
}
const int DATA_SUM = 24;//データの数
const int DISP_MAX = 10;
const int C_WIDTH = 200;
const int C_HEIGHT = 20;
const int BLUE = GetColor(150, 150, 250);
const int BLUE2 = GetColor(50, 50, 200);
const int GREEN = GetColor(0, 255, 0);
const int GRAY = GetColor(100, 100, 100);
const int BLACK = GetColor(0, 0, 0);
bool DataBase::watch_character() {

	static std::string name[DATA_SUM];//キャラの名前全員分

	if (name[0].size() == 0) {
		load_name(name);
	}
	int x = 20;
	int y = 100;
	//↑
	if (c_num != 0) {
		if (hand_x >= x && hand_x <= x + C_WIDTH && hand_y >= y && hand_y <= y + C_HEIGHT) {
			DrawBox(x - 2, y - 2, x + C_WIDTH + 2, y + C_HEIGHT + 2, GREEN, TRUE);
			if (left_click() == 1) {
				c_num--;
				PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//音
			}
		}
		DrawBox(x, y, x + C_WIDTH, y + C_HEIGHT, GRAY, TRUE);
		DrawFormatString(x + 90, y + 2, BLACK, "↑");
	}
	y += (10 + C_HEIGHT) * (DISP_MAX + 1);
	//↓
	if (c_num < DATA_SUM - DISP_MAX) {
		if (hand_x >= x && hand_x <= x + C_WIDTH && hand_y >= y && hand_y <= y + C_HEIGHT) {
			DrawBox(x - 2, y - 2, x + C_WIDTH + 2, y + C_HEIGHT + 2, GREEN, TRUE);
			if (left_click() == 1) {
				c_num++;
				PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//音
			}
		}
		DrawBox(x, y, x + C_WIDTH, y + C_HEIGHT, GRAY, TRUE);
		DrawFormatString(x + 90, y + 2, BLACK, "↓");
	}
	y = 100;
	//各データからDISP_MAX個表示
	for (int i = 0; i < DISP_MAX; i++) {
		y += 10 + C_HEIGHT;
		if (hand_x >= x && hand_x <= x + C_WIDTH && hand_y >= y && hand_y <= y + C_HEIGHT) {
			DrawBox(x - 2, y - 2, x + C_WIDTH + 2, y + C_HEIGHT + 2, GREEN, TRUE);
			if (left_click() == 1) {
				c_now = i + c_num;
				load_detail(c_now, handle, text);
				PlaySoundMem(kettei_sound, DX_PLAYTYPE_BACK);//音
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