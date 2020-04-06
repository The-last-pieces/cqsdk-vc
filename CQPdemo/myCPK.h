#pragma once

#include <string>
#include <vector>
#include <deque>
#include <sstream>
#include <iomanip>

#define MY_MIN(x,y) ((x)<(y)?(x):(y))
#define MY_MAX(x,y) ((x)>(y)?(x):(y))
#define RANGE_IN(min,max,num) (MY_MAX(MY_MIN((num), (max)), (min)))

using namespace std;
class chess
{
	enum class Mode
	{
		sleep,
		waiting,
		start
	};
	Mode now = Mode::sleep;

	deque<string> msgque;//消息队列
	deque<vector<vector<short>>> history;//记录
	vector<vector<short>> table;//棋盘
	const int size;
	bool isend = false;
	int role = 0;//轮次
	string errlog;
	long long playerid[2] = { 0 };//玩家id

	static chess* running;
	chess(size_t _size = 9);
public:

	static chess* getinstance()
	{
		if (running)
		{
			return running;
		}
		return new chess;
	}

	size_t msg_size();

	string pop_msg();
	void control(string msg, long long userid);

	~chess();

private:
	void init_table();
	bool action(int x, int y);
	void map_to_str();
	bool game_over();
};
