// myplaner.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

class mytime {
	int lab_h, lab_m; // for lab arrive
	int st_h, st_m; // for start tiem
	string morning_exercise;

	enum for_firstline {
		lab_arrive, start_time, M_exercise
	};
	//공부 한 것들 리스트
	vector < pair<pair<int, char>, pair<int, string>>> record; // index, color, time, contents
	vector < pair<pair<int, int>, pair<int,char>>> for_table_data; // 시작시간의 h,m 과 뒤의 시간 

	//results
	int O_time[4], B_time[4], X_time[4]; // Orange, Blue, Xtime (X time은 lab에 없었거나) 0 : 총시간 1 : 오전 2 : 오후 3 : 저녁
	int todaypercent[4];
	map<string, pair<int,char> > contents_time; // key로 한 일 적고 값으로 시간과 그것의 color
	char graph_table[13][61];

public:
	// 데이터 입력 부분 
	void lab_arrive_func(int h, int m) {
		lab_h = h;
		lab_m = m;
	}
	void start_time_func(int h, int m) {
		st_h = h;
		st_m = m;
	}
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
	void first_line_check(string &s, int cnt) { // 랩실도착, 시작시간, 아침운동 체크용
		switch (cnt) {
		case lab_arrive:
			//12부터 시작하는 이유. 한글을 char로 바꾸면 ?가 되는데 한글 한글자당 ?? 로 되어서 2칸씩 차지하는 걸로 계산 
			mytime::lab_arrive_func(int(s[12] - '0'), int(s[14] - '0') * 10 + int(s[15] - '0')); // 앞부분 빼고 시간만 바꿔서 넣기
			break;
		case start_time:
			mytime::start_time_func(int(s[12] - '0'), int(s[14] - '0') * 10 + int(s[15] - '0')); // 앞부분 빼고 시간만 바꿔서 넣기
		case M_exercise:
			vector<char> input;
			for (int i = 12; i < s.size(); ++i) {
				input.push_back(s[i]);
			}
			mytime::M_exercise_func(input);
		}
	}
	void time_classification(int index, string& s) {
		int start_h, start_m, finish_h, finish_m;
		string contents = s.substr(14, s.size());
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
		for_table_data.push_back({ {start_h, start_m}, {push_m,s[0]} });
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
			if (x.first.second == 'O') {
				O_time[x.first.first] += x.second.first;
			}
			if (x.first.second == 'B') {
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
			graph_table[0][i] = char((i-1)%10 + '0');
		}

		for (int i = 1; i < 13; ++i) {
			graph_table[i][0] = char(i % 10 + '0');
		}

		for (auto x : for_table_data) { // 시작시간의 h와 m, 그 시간
			int row = x.first.first;
			int col = x.first.second;
			int cnt = x.second.first;

			if (row <= 12) {
				row -= 7;
			}
			else if (row <= 17) {
				row -= 8;
			}
			else if (row <= 23) {
				row -= 10;
			}

			bool OorB;
			if (x.second.second == 'O') {
				OorB = true;
			}
			else {
				OorB = false;
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
					else if(cnt == 0){
						graph_table[row][z] = ']';
					}
					else {
						graph_table[row][z] = '@';
					}
				}
				else {
					graph_table[row][z] = '-';
				}
			}
			if (cnt != 0) {
				cnt++;
				for (int z = 1; z <= cnt; ++z) {

					if (OorB) {
						if (z == cnt) {
							graph_table[row + 1][z-1] = ']';
						}
						else {
							graph_table[row + 1][z] = '@';
						}
					}
					else {
						graph_table[row + 1][z] = '_';
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



	//디버그 부분
	void debug_print() {
		cout << "----------디버그----------	" << endl;
		cout << "lab_h : " << lab_h << '\t' << "lab_m : " << lab_m << endl;
		cout << "st_h : " << st_h << '\t' << "st_m : " << st_m << endl;
		cout << "morning : " << morning_exercise <<  endl;

		for (auto x : record) {
			cout << x.first.first << ' ' << x.first.second << ' ' << x.second.first << ' ' << x.second.second << endl;
		}

		for (int i = 0; i < 4; ++i) {
			if (i == 0) {
				cout << "--- 총 시간(O_time[0]) --- " << endl;
			}
			else {
				cout << "--- 각 시간(O_time[" << i << "] ---" << endl;
			}
			cout << "O : " << O_time[i] << "\tB : " << B_time[i] << "\tX : " << X_time[i] << endl;
		}

		cout << "--- O ---" << endl;
		for (auto x : contents_time) {
			if (x.second.second == 'B') continue;
			cout << x.first << ' ' << x.second.first << endl;
		}

		cout << "--- B --- " << endl;
		for (auto x : contents_time) {
			if (x.second.second == 'O') continue;
			cout << x.first << ' ' << x.second.first << endl;
		} 

		int hour = 7;
		for (int i = 0; i < 13; ++i) {
			if (hour < 10 && hour != 7) {
				cout << '0';
			}
			if (hour == 12) {
				hour++;
			}
			if (hour == 17) {
				hour += 2;
			}
			if (hour == 7) {
				cout << "  ";
				hour++;
			}
			else {
				cout << hour++;
			}
			for (int j = 1; j < 61; ++j) {
				if ((j-1)% 10 == 0) {
					cout << '|';
				}
				cout << graph_table[i][j];
			}
			cout << endl;
		}

		cout << "오늘의 퍼센트" << endl;
		for (int i = 0; i < 4; ++i) {
			cout << i << " : " << todaypercent[i] << "%\n";
		}
	}
	//디버그 부분 끝

	void print_data() {
		cout << "----------프린트----------	" << endl;
		cout << "lab_h : " << lab_h << '\t' << "lab_m : " << lab_m << endl;
		cout << "st_h : " << st_h << '\t' << "st_m : " << st_m << endl;
		cout << "morning : " << morning_exercise << endl;

		/*for (auto x : record) {
			cout << x.first.first << ' ' << x.first.second << ' ' << x.second.first << ' ' << x.second.second << endl;
		}*/

		for (int i = 0; i < 4; ++i) {
			if (i == 0) {
				cout << "--- 총 시간(O_time[0]) --- " << endl;
			}
			else {
				cout << "--- 각 시간(O_time[" << i << "] ---" << endl;
			}
			cout << "O : " << O_time[i] << "\tB : " << B_time[i] << "\tX : " << X_time[i] << endl;
		}

		cout << "--- O ---" << endl;
		for (auto x : contents_time) {
			if (x.second.second == 'B') continue;
			cout << x.first << ' ' << x.second.first << endl;
		}

		cout << "--- B --- " << endl;
		for (auto x : contents_time) {
			if (x.second.second == 'O') continue;
			cout << x.first << ' ' << x.second.first << endl;
		}

		int hour = 7;
		for (int i = 0; i < 13; ++i) {
			if (hour < 10 && hour != 7) {
				cout << '0';
			}
			if (hour == 12) {
				hour++;
			}
			if (hour == 17) {
				hour += 2;
			}
			if (hour == 7) {
				cout << "  ";
				hour++;
			}
			else {
				cout << hour++;
			}
			for (int j = 1; j < 61; ++j) {
				if ((j - 1) % 10 == 0) {
					cout << '|';
				}
				cout << graph_table[i][j];
			}
			cout << endl;
		}

		cout << "오늘의 퍼센트" << endl;
		for (int i = 0; i < 4; ++i) {
			cout << i << " : " << todaypercent[i] << "%\n";
		}
	}
};



int main()
{
	string filePath = "time.txt";
	mytime go;

	int for_first_line_cnt = 0; // 랩실시간, 시작시간, 아침시간 체크용
	int time_index = 0;
	ifstream openFile(filePath.data());
	if (openFile.is_open()) {
		string line;
		while (getline(openFile, line)) {
			if (line[0] == '-') {
				time_index++;
				continue;
			}

			//첫줄 처리용
			if (for_first_line_cnt < 3) {
				go.first_line_check(line, for_first_line_cnt);
			}
			else {
				//시간표로 나머지는 다 보내기
				go.time_classification(time_index, line);
			}

			for_first_line_cnt++; // first_line 위한 cnt
		}
		openFile.close();
	}
	go.color_time_cal();
	go.contents_time_cal();
	go.table_cal();
	go.todaypercent_cal();
	//go.debug_print();
	go.print_data();

	return 0;
}


