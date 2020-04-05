#include "myCPK.h"

chess::chess(size_t _size) :size(RANGE_IN(7, 20, _size))
{
	init_table();
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
		return "";
	}
}

void chess::control(string msg, long long userid)
{
	switch (now)
	{
	case Mode::sleep:
		if (msg == "chess")
		{
			now = Mode::waiting;
			playerid[0] = userid;

			stringstream ss;
			ss << userid << "发起游戏";
			init_table();
			msgque.push_back(ss.str());
		}
		break;
	case Mode::waiting:
		if (playerid[0] != userid)
		{
			if (msg == "chess")
			{
				now = Mode::start;
				playerid[1] = userid;

				stringstream ss;
				ss << userid << "接受游戏";
				msgque.push_back(ss.str());
				msgque.push_back(map_to_str());
			}
		}
		break;
	case Mode::start:
		if (playerid[role] == userid)
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
				now = Mode::sleep;
				this->history.clear();
				playerid[0] = playerid[1] = 0;
				msgque.push_back("游戏中止");
				break;
			}

			stringstream ss(msg);
			int x = 0, y = 0;
			ss >> x >> y;
			if (action(x, y))
			{
				role = !role;
				msgque.push_back(map_to_str());

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
bool chess::action(int x, int y)
{
	if (x <= 0 || x > size || y <= 0 || y > size)
	{
		msgque.push_back("坐标错误");
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

	if (!game_over())
	{
		rstr << "轮到" << playerid[role] << endl;
	}
	rstr << setw(3) << "x\\y";
	for (int i = 1; i <= size; ++i)
	{
		rstr << setw(3) << i;
	}
	rstr << endl;
	string allstr[] = { " ※"," O"," √" };
	for (int i = 1; i <= size; ++i)
	{
		rstr << setw(3) << i;
		for (int j = 1; j <= size; ++j)
		{
			rstr << setw(3) << allstr[table[i][j]];
		}
		rstr << endl;
	}
	if (game_over())
	{
		rstr << playerid[!role];
		rstr << " 玩家胜利\n";
	}

	return rstr.str();
}
bool chess::game_over()
{
	const int check = !role + 1;

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