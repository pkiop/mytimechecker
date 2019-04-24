// myplaner.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set> // 자료 정렬하기 위함
#include <algorithm> // O랑 B 정렬하기 위함
#include <direct.h> //폴더 만들기 위함
#include <windows.h> // 폴더 만들기 위함

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
			for (int i = 0; i < record.size();++i) { //여기 auto로 해버리면 값 안바뀐다. 
				if (record[i].second.second == x.second) {
					record[i].first.second = Alpha;
				}
			}
			Alpha++;
		}
		Alpha = 'a';
		for (auto x : for_B) {
			for (int i = 0;i<record.size();++i) {
				if (record[i].second.second == x.second) {
					record[i].first.second = Alpha;
				}
			}
			Alpha++;
		}
	}

	void for_table_data_cal() {
		for (int i = 0; i < record.size(); ++i) {
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
			graph_table[0][i] = char((i-1)%10 + '0');
		}

		for (int i = 1; i < 13; ++i) {
			graph_table[i][0] = char(i % 10 + '0');
		}

		for (auto x : for_table_data) { // 시작시간의 h와 m, 그 시간
			int row = x.first.first;
			int col = x.first.second;
			int cnt = x.second.first;
			char summary_char = x.second.second;

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
			if (summary_char >= 'a' ) { //대문자이면 활용시간 소문자이면 활용못한것 
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
					else if(cnt == 0){
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
							graph_table[row + 1][z-1] = ']';
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
		os << "\n\n    --------------------------- 20" << filename.substr(0,2) << "년 " << filename.substr(2,2) << "월 " << filename.substr(4,2) << "일" << " -------------------------" << endl;
		/*os << "\t\t\t\t도착 시간 : " << lab_h << ':' << lab_m << endl;
		os << "\t\t\t\t시작 시간 : " << st_h << ':' << st_m << endl;
		os << "\t\t\t\t아침 운동 : " << morning_exercise << endl;*/

		/*for (auto x : record) {
			os << x.first.first << ' ' << x.first.second << ' ' << x.second.first << ' ' << x.second.second << endl;
		}*/

		os << "\n    --------------------------- [   기록한 표   ] ------------------------" << endl;
		int hour = 7;
		for (int i = 0; i < 13; ++i) {
			os << "    ";
			if (hour < 10 && hour != 7) {
				os << '0';
			}
			if (hour == 12) {
				hour++;
			}
			if (hour == 17) {
				hour += 2;
			}
			if (hour == 7) {
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
		sort(for_O.begin(), for_O.end(), greater<pair<int,string>>()); //활용한 시간, 활용못한시간 sort
		sort(for_B.begin(), for_B.end(), greater<pair<int,string>>());

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
		
		for (int i = 0; i < 4; ++i) {
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
		for (int i = 0; i < 4; ++i) {
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
	}
};

int main()
{
	mytime go;
	string filePath = "time.txt";

	cout << "년도 끝 두자리, 월, 일 을 입력하세요 (ex 180123) : ";
	string filename;
	cin >> filename;
	filename += ".txt";
	string dirname = filename.substr(0, 4);
	ofstream writeFile(filename.data());

	int for_first_line_cnt = 0; // 랩실시간, 시작시간, 아침시간 체크용
	int time_index = 0;
	ifstream openFile(filePath.data());
	if (openFile.is_open()) {
		string line;
		writeFile << "\t\t\t\t---- 데이터 ----\n";
		while (getline(openFile, line)) {
			
			if (line[0] == '-') {
				time_index++;
				writeFile << "\t\t  " << line << '\n';
				continue;
			}

			//첫줄 처리용
			if (for_first_line_cnt < 3) {
				writeFile << "\t\t\t\t" << line << '\n';
				go.first_line_check(line, for_first_line_cnt);
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

	return 0;
}


