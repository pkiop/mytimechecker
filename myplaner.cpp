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
	//���� �� �͵� ����Ʈ
	vector < pair<pair<int, char>, pair<int, string>>> record; // index, color, time, contents
	vector < pair<pair<int, int>, pair<int,char>>> for_table_data; // ���۽ð��� h,m �� ���� �ð� 

	//results
	int O_time[4], B_time[4], X_time[4]; // Orange, Blue, Xtime (X time�� lab�� �����ų�) 0 : �ѽð� 1 : ���� 2 : ���� 3 : ����
	int todaypercent[4];
	map<string, pair<int,char> > contents_time; // key�� �� �� ���� ������ �ð��� �װ��� color
	char graph_table[13][61];

public:
	// ������ �Է� �κ� 
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
	//������ �Էºκ� ��



	//text �Է¹��� ������ ó���κ�
	void first_line_check(string &s, int cnt) { // ���ǵ���, ���۽ð�, ��ħ� üũ��
		switch (cnt) {
		case lab_arrive:
			//12���� �����ϴ� ����. �ѱ��� char�� �ٲٸ� ?�� �Ǵµ� �ѱ� �ѱ��ڴ� ?? �� �Ǿ 2ĭ�� �����ϴ� �ɷ� ��� 
			mytime::lab_arrive_func(int(s[12] - '0'), int(s[14] - '0') * 10 + int(s[15] - '0')); // �պκ� ���� �ð��� �ٲ㼭 �ֱ�
			break;
		case start_time:
			mytime::start_time_func(int(s[12] - '0'), int(s[14] - '0') * 10 + int(s[15] - '0')); // �պκ� ���� �ð��� �ٲ㼭 �ֱ�
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
	//text �Է¹��� ������ ó���κ� ��
	

	//������ ó���ߴ� �����ͷ� ����ϰ� ��賻�� �κ�
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

		X_time[0] = 60 * 12 - O_time[0] - B_time[0]; // X�� Ÿ���� 12�ð� * 60�� O�� B ����
		for (int i = 1; i < 4; ++i) {
			X_time[i] = 240 - O_time[i] - B_time[i];
		}
	}

	void contents_time_cal() {
		for (auto x : record) {
			contents_time[x.second.second].first += x.second.first;
			contents_time[x.second.second].second = x.first.second; // O�� B �����ϰ��ؼ� ����� �� ���� ������ �Ϸ��� 
		}
	}
	//������ ó���ߴ� �����ͷ� ����ϰ� ��賻�� �κ� ��

	//table ó�� �κ�
	void table_cal() {
		for (int i = 1; i < 61; ++i) {
			graph_table[0][i] = char((i-1)%10 + '0');
		}

		for (int i = 1; i < 13; ++i) {
			graph_table[i][0] = char(i % 10 + '0');
		}

		for (auto x : for_table_data) { // ���۽ð��� h�� m, �� �ð�
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

	//table ó�� �κ� �� 



	//����� �κ�
	void debug_print() {
		cout << "----------�����----------	" << endl;
		cout << "lab_h : " << lab_h << '\t' << "lab_m : " << lab_m << endl;
		cout << "st_h : " << st_h << '\t' << "st_m : " << st_m << endl;
		cout << "morning : " << morning_exercise <<  endl;

		for (auto x : record) {
			cout << x.first.first << ' ' << x.first.second << ' ' << x.second.first << ' ' << x.second.second << endl;
		}

		for (int i = 0; i < 4; ++i) {
			if (i == 0) {
				cout << "--- �� �ð�(O_time[0]) --- " << endl;
			}
			else {
				cout << "--- �� �ð�(O_time[" << i << "] ---" << endl;
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

		cout << "������ �ۼ�Ʈ" << endl;
		for (int i = 0; i < 4; ++i) {
			cout << i << " : " << todaypercent[i] << "%\n";
		}
	}
	//����� �κ� ��

	void print_data() {
		cout << "----------����Ʈ----------	" << endl;
		cout << "lab_h : " << lab_h << '\t' << "lab_m : " << lab_m << endl;
		cout << "st_h : " << st_h << '\t' << "st_m : " << st_m << endl;
		cout << "morning : " << morning_exercise << endl;

		/*for (auto x : record) {
			cout << x.first.first << ' ' << x.first.second << ' ' << x.second.first << ' ' << x.second.second << endl;
		}*/

		for (int i = 0; i < 4; ++i) {
			if (i == 0) {
				cout << "--- �� �ð�(O_time[0]) --- " << endl;
			}
			else {
				cout << "--- �� �ð�(O_time[" << i << "] ---" << endl;
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

		cout << "������ �ۼ�Ʈ" << endl;
		for (int i = 0; i < 4; ++i) {
			cout << i << " : " << todaypercent[i] << "%\n";
		}
	}
};



int main()
{
	string filePath = "time.txt";
	mytime go;

	int for_first_line_cnt = 0; // ���ǽð�, ���۽ð�, ��ħ�ð� üũ��
	int time_index = 0;
	ifstream openFile(filePath.data());
	if (openFile.is_open()) {
		string line;
		while (getline(openFile, line)) {
			if (line[0] == '-') {
				time_index++;
				continue;
			}

			//ù�� ó����
			if (for_first_line_cnt < 3) {
				go.first_line_check(line, for_first_line_cnt);
			}
			else {
				//�ð�ǥ�� �������� �� ������
				go.time_classification(time_index, line);
			}

			for_first_line_cnt++; // first_line ���� cnt
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


