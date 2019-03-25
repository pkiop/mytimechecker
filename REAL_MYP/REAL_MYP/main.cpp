// myplaner.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set> // 자료 정렬하기 위함
#include <algorithm> // O랑 B 정렬하기 위함
#include <direct.h> //폴더 만들기 위함
#include <windows.h> // 폴더 만들기 위함

#include "mysql.h"

#pragma comment(lib, "libmysql.lib")
using namespace std;

class mytime {
	int lab_h, lab_m; // for lab arrive
	int st_h, st_m; // for start tiem
	int sleep_h, sleep_m;
	string morning_exercise;

	enum for_firstline {
		lab_arrive, start_time, M_exercise
	};
	//공부 한 것들 리스트
	vector < pair<pair<int, char>, pair<int, string> > > record; // index, color, time, contents
	vector < pair<pair<int, int>, pair<int, char> > > for_table_data; // 시작시간의 h,m 과 뒤의 시간 

	//results
	int O_time[4], B_time[4], X_time[4]; // Orange, Blue, Xtime (X time은 lab에 없었거나) 0 : 총시간 1 : 오전 2 : 오후 3 : 저녁
	int todaypercent[4];
	char graph_table[19][61]; // 얘가 contents_time의 메모리공간 침범하는 듯한데..
	map<string, pair<int, char> > contents_time; // key로 한 일 적고 값으로 시간과 그것의 color

public:
	string ret_todaypercent() {
		return to_string(todaypercent[0]);
	}
	string ret_sleeptime() {
		return (to_string(sleep_h) + '.' + to_string(sleep_m));
	}
	//debugging 용. 지워도 댐
	void contents_time_size_print() {
		cout << "contents_time.size : " << contents_time.size() << endl;
	}
	// 데이터 입력 부분 
	//저장만 해두고 추후에 사용하려고. 현재는 어디서 사용하는지 모르겠.
	void lab_arrive_func(int h, int m) {
		lab_h = h;
		lab_m = m;
	}
	void start_time_func(int h, int m) {
		st_h = h;
		st_m = m;
	}
	//저장만 해두고 추후에 사용하려고. 현재는 어디서 사용하는지 모르겠.
	void M_exercise_func(vector<char>& char_vec) {
		string s;
		for (int i = 0; i < char_vec.size(); ++i) {
			s += char_vec[i];
		}
		morning_exercise = s;
	}
	void push_record(int index, char color, int time, string& contents) {
		record.push_back({ {index,color},{time,contents} });
	}
	//데이터 입력부분 끝

	//text 입력받은 데이터 처리부분
	void sleeping_time_cal(string &s) {
		int h, m;
		for (int i = 0; i < s.size(); ++i) {
			cout << i << ' ' << s[i] << endl;
		}
		if (int(s[12] - '0') > 0) {
			h = int(s[18] - '0') * 10 + int(s[19] - '0');
			m = int(s[21] - '0') * 10 + int(s[22] - '0');
			h += (23 - (int(s[12] - '0') * 10 + int(s[13] - '0')));
			m += (60 - (int(s[18] - '0') * 10 + int(s[19] - '0')));
			while (m >= 60) {
				m -= 60;
				h++;
			}
		}
		else {
			h = (int(s[18] - '0') * 10 + int(s[19] - '0')) - (int(s[12] - '0') * 10 + int(s[13] - '0'));
			m = (int(s[21] - '0') * 10 + int(s[22] - '0')) - (int(s[15] - '0') * 10 + int(s[16] - '0'));
		}

		if (m < 0) {
			h--;
			m += 60;
		}
		mytime::sleep_h = h;
		mytime::sleep_m = m;
	};

	void first_line_check(string &s, int cnt) { // 랩실도착, 시작시간, 아침운동 체크용
		switch (cnt) {
		case lab_arrive:
			//12부터 시작하는 이유. 한글을 char로 바꾸면 ?가 되는데 한글 한글자당 ?? 로 되어서 2칸씩 차지하는 걸로 계산 
			mytime::lab_arrive_func((int(s[12] - '0') * 10 + int(s[13] - '0')), int(s[15] - '0') * 10 + int(s[16] - '0')); // 앞부분 빼고 시간만 바꿔서 넣기
			break;
		case start_time:
			mytime::start_time_func((int(s[12] - '0') * 10 + int(s[13] - '0')), int(s[15] - '0') * 10 + int(s[16] - '0')); // 앞부분 빼고 시간만 바꿔서 넣기
			break;
		case M_exercise:
			vector<char> input;
			for (int i = 12; i < s.size(); ++i) {
				input.push_back(s[i]);
			}
			mytime::M_exercise_func(input);
		}
	}
	string contents;
	void time_classification(int index, string& s) {
		int start_h, start_m, finish_h, finish_m;
		contents = s.substr(14, s.size());
		start_h = int(s[2] - '0') * 10 + int(s[3] - '0');
		start_m = int(s[5] - '0') * 10 + int(s[6] - '0');
		finish_h = int(s[8] - '0') * 10 + int(s[9] - '0');
		finish_m = int(s[11] - '0') * 10 + int(s[12] - '0');

		int h = finish_h - start_h;
		int push_m;
		if (start_m > finish_m) {
			h -= 1;
			finish_m += 60;
		}
		push_m = finish_m - start_m + 60 * h;

		push_record(index, s[0], push_m, contents);
		for_table_data.push_back({ {start_h, start_m}, {push_m, s[0]} });
	}
	//일단 시간만 입력받고 마지막 문자는 recode_sort_and_alphabet_chance에서 record 정렬된 다음 for_data_table_cal 돌리면 
	//시간 많이 쓴 순서대로 A~Z, a~z로 정렬되어 출력된다.

	void recode_sort_and_alphabet_change() { //입력받은 것에 따라 분이 가장 높은 순서대로 ABCD, abcd로 분류할 것 
		vector<pair<int, string>> for_O, for_B;
		for (auto x : contents_time) {

			if (x.second.second >= 'a') {
				for_B.push_back({ x.second.first,x.first });
			}
			else {
				for_O.push_back({ x.second.first,x.first });
			}
		}
		sort(for_O.begin(), for_O.end(), greater<pair<int, string>>()); //활용한 시간, 활용못한시간 sort
		sort(for_B.begin(), for_B.end(), greater<pair<int, string>>());

		char Alpha = 'A';
		for (auto x : for_O) {
			for (int i = 0; i < record.size(); ++i) { //여기 auto로 하면  값 안바뀐다. 
				if (record[i].second.second == x.second) {
					record[i].first.second = Alpha;
				}
			}
			Alpha++;
		}
		Alpha = 'a';
		for (auto x : for_B) {
			for (int i = 0; i < record.size(); ++i) {
				if (record[i].second.second == x.second) {
					record[i].first.second = Alpha;
				}
			}
			Alpha++;
		}
	}

	void for_table_data_cal() {
		for (int i = 0; i < record.size(); ++i) {
			if (for_table_data[i].second.second != 'Z' && for_table_data[i].second.second != 'z') // Z남겨놓는 이유는 수업은 따로처리하기위함
				for_table_data[i].second.second = record[i].first.second;
		}
	}
	//text 입력받은 데이터 처리부분 끝


	//데이터 처리했던 데이터로 계산하고 통계내는 부분
	void color_time_cal() {
		for (int i = 0; i < 4; ++i) {
			O_time[i] = 0;
			B_time[i] = 0;
			X_time[i] = 0;
		}

		for (auto x : record) {
			if (x.first.second < 'a') {
				O_time[x.first.first] += x.second.first;
			}
			else {
				B_time[x.first.first] += x.second.first;
			}
		}

		for (int i = 1; i < 4; ++i) {
			O_time[0] += O_time[i];
			B_time[0] += B_time[i];
		}

		X_time[0] = 60 * 12 - O_time[0] - B_time[0]; // X총 타임은 12시간 * 60에 O랑 B 뺀것
		for (int i = 1; i < 4; ++i) {
			X_time[i] = 240 - O_time[i] - B_time[i];
		}
	}

	void contents_time_cal() {
		for (auto x : record) {
			contents_time[x.second.second].first += x.second.first;
			contents_time[x.second.second].second = x.first.second; // O랑 B 구분하게해서 출력할 때 따로 나오게 하려고 

		}
	}
	//데이터 처리했던 데이터로 계산하고 통계내는 부분 끝

	//table 처리 부분
	void table_cal() {
		for (int i = 1; i < 61; ++i) {
			graph_table[0][i] = char((i - 1) % 10 + '0');
		}

		for (int i = 1; i < 18; ++i) {
			graph_table[i][0] = char(i % 10 + '0');
		}

		for (auto x : for_table_data) { // 시작시간의 h와 m, 그 시간
			int row = x.first.first;
			int col = x.first.second;
			int cnt = x.second.first;
			char summary_char = x.second.second;
			//7시부터 시작하기 위해서. 예전에 17시는 한칸 땡겨서 뭐 이런식으로 하기 위해 row 값이 8보다 컸음 
			if (row <= 12) {
				row -= 6;
			}
			else if (row <= 17) {
				row -= 6;
			}
			else if (row <= 24) {
				row -= 6;
			}

			bool OorB;
			if (summary_char >= 'a') { //대문자이면 활용시간 소문자이면 활용못한것 
				OorB = false;
			}
			else {
				OorB = true;
			}
			for (int z = col + 1; z < 61; ++z) {
				if (cnt == 0) {
					break;
				}
				cnt--;
				if (OorB) {
					if (z == col + 1) {
						graph_table[row][z] = '[';
					}
					else if (cnt == 0) {
						graph_table[row][z] = ']';
					}
					else {
						graph_table[row][z] = summary_char;
					}
				}
				else {
					graph_table[row][z] = summary_char;
				}
			}
			if (cnt != 0) {
				cnt++;
				for (int z = 1; z <= cnt; ++z) {

					if (OorB) {
						if (z == cnt) {
							graph_table[row + 1][z - 1] = ']';
						}
						else {
							graph_table[row + 1][z] = summary_char;
						}
					}
					else {
						graph_table[row + 1][z] = summary_char;
					}
				}
			}

		}
	}

	void todaypercent_cal() {
		int resttime = 30;
		todaypercent[0] = (O_time[0] * 100) / (720 - resttime * 3);
		for (int i = 1; i < 4; ++i) {
			todaypercent[i] = (O_time[i] * 100) / (240 - resttime);
		}
	}
	//table 처리 부분 끝 

	template<typename T>
	void print_data(T& os, string filename) {
		os << "\n\n    --------------------------- 20" << filename.substr(0, 2) << "년 " << filename.substr(2, 2) << "월 " << filename.substr(4, 2) << "일" << " -------------------------" << endl;
		/*os << "\t\t\t\t도착 시간 : " << lab_h << ':' << lab_m << endl;
		os << "\t\t\t\t시작 시간 : " << st_h << ':' << st_m << endl;
		os << "\t\t\t\t아침 운동 : " << morning_exercise << endl;*/

		/*for (auto x : record) {
			os << x.first.first << ' ' << x.first.second << ' ' << x.second.first << ' ' << x.second.second << endl;
		}*/
		os << "\n    --------------------------- [   기록한 표   ] ------------------------" << endl;
		int hour = 6;
		for (int i = 0; i < 18; ++i) {
			os << "    ";
			if (hour < 10 && hour != 6) {
				os << '0';
			}
			//if (hour == 12) {
			//	hour++;
			//}
			//if (hour == 17) {
			//	hour += 2;
			//}
			if (hour == 6) {
				os << "  ";
				hour++;
			}
			else {
				os << hour++;
			}
			for (int j = 1; j < 61; ++j) {
				if ((j - 1) % 10 == 0) {
					os << '|';
				}
				os << graph_table[i][j];
			}
			os << endl;
		}


		vector<pair<int, string>> for_O, for_B;
		for (auto x : contents_time) {
			if (x.second.second >= 'a') {
				for_B.push_back({ x.second.first,x.first });
			}
			else {
				for_O.push_back({ x.second.first,x.first });
			}
		}
		sort(for_O.begin(), for_O.end(), greater<pair<int, string>>()); //활용한 시간, 활용못한시간 sort
		sort(for_B.begin(), for_B.end(), greater<pair<int, string>>());

		os << "\n    --------------------------   [활용한 시간]   --------------------------" << endl;
		for (auto x : for_O) {
			os << "\t\t\t";
			int len = x.second.size();
			for (auto y : record) {
				if (y.second.second == x.second) {
					os << y.first.second << ". ";
					break;
				}
			}
			os << x.second;
			for (int i = 0; i < 20 - len; ++i) { // 글자수 20 이내이면 공백 만들어준다.
				os << ' ';
			}
			os << "\t| " << x.first << endl;
		}
		os << "    ---------------------------  [활용못한 시간]  -------------------------" << endl;
		for (auto x : for_B) {
			os << "\t\t\t";
			for (auto y : record) {
				if (y.second.second == x.second) {
					os << y.first.second << ". ";
					break;
				}
			}
			int len = x.second.size();
			os << x.second;
			for (int i = 0; i < 20 - len; ++i) { // 글자수 20 이내이면 공백 만들어준다.
				os << ' ';
			}
			os << "\t| " << x.first << endl; // 마지막 tab으로 자잘한 자리 차이 나는거 채워준다. 
		}

		for (int i = 0; i < 1; ++i) {
			if (i == 0) {
				os << "\n    --------------------------- [  총 활용한 시간] ------------------------" << endl;
			}
			else {
				os << "    --------------------------- [";
				switch (i) {

				case 1:
					os << "오전";
					break;
				case 2:
					os << "오후";
					break;
				case 3:
					os << "저녁";
					break;
				default:
					os << "총활용시간 오류";
				}

				os << " 활용한 시간] -------------------------" << endl;
			}
			os << "			O : " << O_time[i] << " |    B : " << B_time[i] << "\t | X : " << (X_time[i] >= 0 ? X_time[i] : 0) << endl;
		}

		os << "\n    --------------------------- [오늘의 퍼센트] ---------------------------" << endl;
		for (int i = 0; i < 1; ++i) {
			switch (i) {
			case 0:
				os << "\t\t\t\t 하루 : ";
				break;
			case 1:
				os << "\t\t\t\t 오전 : ";
				break;
			case 2:
				os << "\t\t\t\t 오후 : ";
				break;
			case 3:
				os << "\t\t\t\t 저녁 : ";
				break;
			default:
				os << "퍼센트에러" << endl;
				break;
			}
			os << todaypercent[i] << "%\n";
		}
		os << "잔 시간 : " << sleep_h << ':' << sleep_m;
	}
};

int main()
{


	/*	if (mysql_real_connect(conn, "localhost", "root", "!", "pkiop_planner", 3306, NULL, 0) == NULL) {
			cerr << mysql_error(conn);
			mysql_close(conn);
			return 1;
		}
	*/

	//if (mysql_query(conn, "show databases")) {
	//	cerr << mysql_error(conn);
	//	mysql_close(conn);
	//	return 1;
	//}


	mytime go;
	string filePath = "time.txt";
	cout << "년도 끝 두자리, 월, 일 을 입력하세요 (ex 180123) : ";
	string filename, filename_out;
	cin >> filename;
	filename_out = filename;
	filename += ".txt";
	string dirname = filename.substr(0, 4);
	ofstream writeFile(filename.data());

	//요일 구하기
	int monthday[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
	string week[] = { "Mo","Tu", "We", "Th", "Fr", "Sa", "Su" };
	int date = stoi(filename);
	int whatweek;
	string yoil_st;
	if (date / 10000 == 19) {
		int month = (date / 100) % 100;
		int day = date % 100;
		int whatweekday = day;
		for (int i = 0; i < month - 1; ++i) {
			day += monthday[i];
		}
		yoil_st = week[(day) % 7];

		//그 달의 첫날의 요일 알기
		int day2 = 1;
		for (int i = 0; i < month - 1; ++i) {
			day2 += monthday[i];
		}
		int yoil = day % 7; // 첫날의 요일
		if ((whatweekday % 7) < yoil) {
			whatweek = whatweekday / 7 + 1;
		}
		else {
			whatweek = whatweekday / 7 + 2;
		}

		system("pause");
	}
	else {
		cerr << "아직 19년도만" << endl;
		system("pause");
		return -1;
	}

	int for_first_line_cnt = 0; // 랩실시간, 시작시간, 아침시간 체크용
	int time_index = 0;
	ifstream openFile(filePath.data());
	string today_exercise;
	if (openFile.is_open()) {
		string line;
		writeFile << "\t\t\t\t---- 데이터 ----\n";
		while (getline(openFile, line) && line != "end") {
			if (line[0] == '-') {
				time_index++;
				writeFile << "\t\t  " << line << '\n';
				continue;
			}

			//첫줄 처리용
			if (for_first_line_cnt == 0) {
				go.sleeping_time_cal(line);
			}
			if (for_first_line_cnt < 4) {
				writeFile << "\t\t\t\t" << line << '\n';
				go.first_line_check(line, for_first_line_cnt);
			}
			else if (for_first_line_cnt == 4) {
				today_exercise = line;
			}
			else {
				//맨 앞에 입력되는 문자는 후에 순서대로 다른 알파벳으로 변경된다. 
				//시간표로 나머지는 다 보내기
				go.time_classification(time_index, line);
				writeFile << "\t\t\t       " << line << '\n';
			}

			for_first_line_cnt++; // first_line 위한 cnt
		}
		openFile.close();
	}
	go.color_time_cal();
	go.contents_time_cal();
	go.recode_sort_and_alphabet_change();
	go.for_table_data_cal();
	go.table_cal();
	go.todaypercent_cal();
	//go.debug_print();

	go.print_data(cout, filename);
	go.print_data(writeFile, filename);

	cout << mysql_get_client_info() << endl;

	MYSQL *conn = mysql_init(NULL);

	if (mysql_real_connect(conn, "localhost", "root", "1111", "pkiop_planner", 3306, NULL, 0) == NULL) {
		cerr << mysql_error(conn);
		mysql_close(conn);
		system("pause");
		return 1;
	}
	cout << " do << " << endl;
	string query_string = "INSERT INTO result VALUES('" + filename_out + "' , " + (go.ret_todaypercent()) + ", '" + go.ret_sleeptime() + "', '" + today_exercise + "' , " + to_string(whatweek) + ",'" + yoil_st + "')";
	char* query = new char[query_string.length()];
	for (int i = 0; i < query_string.length(); ++i) {
		query[i] = query_string[i];
	}
	query[query_string.length()] = ')';
	const char* query_out = query;
	cout << query_string << endl;
	if (mysql_query(conn, query_string.c_str()))
	{
		cerr << mysql_error(conn);
		cout << "false" << endl;
		system("pause");
	}


	return 0;
}


