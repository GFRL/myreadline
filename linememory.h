#pragma once
#define HISTORY_MAX 100
#include<string.h>
#include<string>
#include<iostream>
#include<vector>
#include <termio.h>
#include <stdio.h>

char scanKeyboard()
{
	char input;
	struct termios new_settings;
	struct termios stored_settings;
	tcgetattr(0, &stored_settings);
	new_settings = stored_settings;
	new_settings.c_lflag &= (~ICANON);
	new_settings.c_cc[VTIME] = 0;
	tcgetattr(0, &stored_settings);
	new_settings.c_cc[VMIN] = 1;
	tcsetattr(0, TCSANOW, &new_settings);

	input = getchar();

	tcsetattr(0, TCSANOW, &stored_settings);
	return input;
}
class lineclass {
private:
	std::string Line;//record the text
	int history_index=0;	
public:
	lineclass(std::string Line="", int history_index = 0) :Line(Line), history_index(history_index) {};
	void show(int mode=0) {//1 red
		switch (mode) {
		case 0:std::cout << Line; break;
		case 1:std::cout << "\033[1;31m" << Line << "\033[39;39m,"; break;
		}
	}
	void set_history_index(int u) { history_index = u; }
	void set_Line(std::string S) { Line = S; }
	int get_history_index() { return history_index; }
	std::string get_Line() { return Line; }
	void save() { std::cout << history_index << std::endl << Line; }
	void read() { std::cin >> history_index >> Line; }
};
class linememoryclass {
private:
	int history_num = 0;
	int now_index = 0;
	std::vector<lineclass*>history;
	std::vector<lineclass*>complete_data;
public:
	void set_history_limit(int history_limit) {//no more than HISTORY_MAX;
		if (history_limit <= 0)return;
		else if (history_limit > HISTORY_MAX)history_num = HISTORY_MAX;
		else history_num = HISTORY_MAX;
	}
	int get_history_limit() { return history_num; }
	int get_now_index() { return now_index; }
	void reset(int history_limit) {
		history_num = history_limit;
		now_index = 0;
		for (int i = 0; i < history.size(); i++) {
			if (history[i])delete history[i];
		}
		history.clear();
		for (int i = 0; i < complete_data.size(); i++) {
			if (complete_data[i])delete complete_data[i];
		}
		complete_data.clear();
	}
	bool GO_ON() {
		char t;
		int special = 0;
		std::string tmp="";
		lineclass* A = new lineclass();
		while (t = scanKeyboard()) {
			if (t == '\r') {
				special = 0;
				continue; 
			}
			if (t == '\n') {
				special = 0;
				A->set_history_index(now_index); now_index++;
				history.push_back(A);
				A = new lineclass;
				std::cout << "\033[1E";
			}
			else if (t == 27) {
				t = scanKeyboard();
				if (t == 91)t = scanKeyboard();
				if (t == 68) {
					if (A->get_Line().size() > 0) {
						tmp = A->get_Line()[A->get_Line().size() - 1] + tmp;
						A->set_Line((A->get_Line().substr(0, A->get_Line().size() - 1)));
					}
				}
				else if (t == 67) {
					if (tmp.size() > 0) {
						A->set_Line(A->get_Line() + tmp[0]);
						tmp = tmp.substr(1);
					}
				}
				else if (t == 66||t==65) {//ÏòÉÏ
					if (t == 65)special++; else special--;
					int now = history.size() - special - 1;
					if (now > history.size() - 1)now = history.size() - 1,special++;
					else if (now <= 0)now = 0,special--;
					if (now == 0)A = new lineclass();
					else A = history[now];
					tmp = "";
				}
				
			}
			else {
				special = 0;
				A->set_Line((A->get_Line())+t);
			}
			std::cout << "\033[1000D" << "\033[K";
			std::cout << "\033[32;41m" << A->get_Line() << tmp << "\033[0m";
			for (int i = tmp.size() - 1; i >= 0; i--)std::cout << "\033[1D";
		}
	}
};

