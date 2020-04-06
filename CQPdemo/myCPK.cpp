#include "myCPK.h"

chess* chess::running = nullptr;
chess::chess(size_t _size) :size(RANGE_IN(7, 20, _size))
{
	if (running)
	{
		delete running;
		running = nullptr;
	}

	running = this;
	init_table();
}
chess::~chess()
{
	if (running)
	{
		delete running;
		running = nullptr;
	}
}

void chess::init_table()
{
	for (auto it_i = table.begin(); it_i != table.end(); )
	{
		for (auto it_j = (*it_i).begin(); it_j != (*it_i).end(); )
		{
			it_j = (*it_i).erase(it_j);
		}
		it_i = table.erase(it_i);
	}

	for (int i = 0; i <= size + 1; ++i)
	{
		vector<short >temp;
		for (int j = 0; j <= size + 1; ++j)
		{
			temp.push_back(0);
		}
		table.push_back(temp);
	}
}

size_t chess::msg_size()
{
	return msgque.size();
}
string chess::pop_msg()
{
	if (!msgque.empty())
	{
		string mid = msgque.front();
		msgque.pop_front();
		return mid;
	}
	else
	{
		if (now == Mode::sleep)
		{
			groupid = 0;
		}
		return "";
	}
}

void chess::control(string msg, pair<long long, string> user, long long group)
{
	if (groupid && groupid != group)
	{
		return;
	}

	switch (now)
	{
	case Mode::sleep:
		if (msg == "chess")
		{
			now = Mode::waiting;
			playerid[0] = user.first;
			groupid = group;

			stringstream ss;
			ss << user.second << "发起游戏";
			init_table();
			msgque.push_back(ss.str());
		}
		break;
	case Mode::waiting:
		if (playerid[0] != user.first)
		{
			if (msg == "join")
			{
				now = Mode::start;
				playerid[1] = user.first;

				stringstream ss;
				ss << user.second << "加入了游戏";
				msgque.push_back(ss.str());

				ss.str("");
				ss << "游戏开始\n先手为:[CQ:at,qq=" << playerid[role] << "]\n" << map_to_str();
				msgque.push_back(ss.str());
			}
		}
		else
		{
			if (msg == "cancle")
			{
				stringstream ss;
				ss << user.second << "取消了游戏";
				msgque.push_back(ss.str());
				clear_table();
			}
		}
		break;
	case Mode::start:
		if (playerid[role] == user.first)
		{
			if (msg == "back")
			{
				if (history.size() > 2)
				{
					history.pop_back();
					history.pop_back();
					table = history.back();
					msgque.push_back(map_to_str());
				}
				else
				{
					msgque.push_back("目前无法悔棋");
				}
				break;
			}
			if (msg == "end")
			{
				clear_table();
				stringstream ss;
				ss << user.second;
				ss << "投降认输";
				msgque.push_back(ss.str());
				break;
			}

			stringstream ss(msg);
			int x = 0, y = 0;
			ss >> x >> y;
			if (action(x, y))
			{
				if (game_over())
				{
					stringstream ss;
					ss << map_to_str();
					ss << "\n[CQ:at,qq=" << playerid[role] << "]";
					ss << " 玩家胜利\n";
					msgque.push_back(ss.str());
					clear_table();
					break;
				}
				else
				{
					stringstream ss;
					role = !role;
					ss << "轮到 [CQ:at,qq=" << playerid[role] << "]" << "落子" << endl;
					ss << map_to_str();

					msgque.push_back(ss.str());
				}

				history.push_back(table);
				if (history.size() > 6)
				{
					history.pop_front();
				}
			}
		}
		break;
	default:
		break;
	}
}
void chess::clear_table()
{
	now = Mode::sleep;
	history.clear();
	playerid[0] = playerid[1] = 0;
}
bool chess::action(int x, int y)
{
	if (x <= 0 || x > size || y <= 0 || y > size)
	{
		return false;
	}
	else
	{
		if (table[x][y])
		{
			msgque.push_back("此处不能落子");
		}
		else
		{
			table[x][y] = role + 1;
			return true;
		}
	}
	return false;
}
string chess::map_to_str()
{
	stringstream rstr;
	rstr << "x\\y 1 2  3  4 5  6  7 8  9\n";
	string allstr[] = { " O"," X"," Y" };
	for (int i = 1; i <= size; ++i)
	{
		rstr << ' ' << i << ' ';
		for (int j = 1; j <= size; ++j)
		{
			rstr << allstr[table[i][j]];
		}
		rstr << endl;
	}
	rstr << "x\\y 1 2  3  4 5  6  7 8  9";

	return rstr.str();
}
bool chess::game_over()
{
	const int check = role + 1;

	for (int i = 1; i <= size; ++i)
	{
		for (int j = 1; j <= size; ++j)
		{
			for (int z = 0, count[3] = { 0 }; z + j <= size && z + i <= size && z < 5; ++z)
			{
				if (table[i][z + j] == check)
				{
					count[0]++;//横
				}
				if (table[z + i][j] == check)
				{
					count[1]++;//纵
				}
				if (table[z + j][z + j] == check)
				{
					count[2]++;//斜
				}

				if (count[0] == 5 || count[1] == 5 || count[2] == 5)
				{
					return true;
				}
			}
		}
	}

	return false;
}