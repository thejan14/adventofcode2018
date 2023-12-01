#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <ranges>
#include <map>
#include <array>
#include <queue>

using namespace std;

struct coord
{
	int x, y;

	bool operator<(const coord& pos) const
	{
		return std::tie(this->x, this->y) < std::tie(pos.x, pos.y);
	}
};

struct node
{
	node(coord const key) : key(key), next()
	{
	}

	coord key;
	array<node*, 4> next;
};

map<coord, node*> nodeMap;

void addNode(int& fromX, int& fromY, char const to)
{
	auto const fromNode = nodeMap[coord{ fromX, fromY }];
	auto dir = 0;
	coord toKey;
	switch (to)
	{
		case 'E':
			fromX += 1;
			toKey = coord{ fromX, fromY };
			dir = 1;
			break;
		case 'S':
			fromY += 1;
			toKey = coord{ fromX, fromY };
			dir = 2;
			break;
		case 'W':
			fromX -= 1;
			toKey = coord{ fromX, fromY };
			dir = 3;
			break;
		default:
			fromY -= 1;
			toKey = coord{ fromX, fromY };
			break;
	}

	node* toNode;
	if (nodeMap.contains(toKey))
	{
		toNode = nodeMap[toKey];
	}
	else
	{
		toNode = new node(toKey);
		nodeMap[toKey] = toNode;
	}

	fromNode->next[dir] = toNode;
	toNode->next[(dir + 2) % 4] = fromNode;
}

void parsePath(string& path, unsigned long long pos, int currX, int currY, vector<pair<int, int>> stack)
{
	stack.emplace_back(currX, currY);
	while (pos < path.size())
	{
		switch (auto const current = path[pos])
		{
			case '(':
				parsePath(path, pos + 1, currX, currY, stack);
				return;
			case '|':
				std::tie(currX, currY) = stack.back();
				break;
			case ')':
				std::tie(currX, currY) = stack.back();
				stack.pop_back();
				break;
			case '$':
				return;
			default:
				addNode(currX, currY, current);
				break;
		}

		pos += 1;
	}
}

int findMaxDoors()
{
	auto constexpr startKey = coord{ 0, 0 };
	auto frontier = queue<coord>();
	frontier.push(startKey);
	auto visited = map<coord, std::pair<coord, int>>();
	visited[startKey] = { coord{ 0, 0 } , 0 };
	auto maxDoors = 0;

	while (!frontier.empty())
	{
		auto const current = frontier.front();
		frontier.pop();
		auto const doorCount = visited[current].second + 1;


		for (auto const next : nodeMap[current]->next | std::views::filter([](auto const n) { return n != nullptr; }))
		{
			if (!visited.contains(next->key) || visited[next->key].second > doorCount)
			{
				frontier.push(next->key);
				visited[next->key] = { current, doorCount };
				if (doorCount > maxDoors)
				{
					maxDoors = doorCount;
				}
			}
		}
	}

	return maxDoors;
}

int main()
{
	string input;
	auto readStream = ifstream("../input.txt");
	if (readStream.is_open())
	{
		string line;
		while (getline(readStream, line))
		{
			input += line;
			input.push_back('\n');
		}
	}

	const auto execStart = chrono::high_resolution_clock::now();

	/* begin solution */

	auto pos = 1;
	nodeMap[coord{ 0, 0 }] = new node(coord{ 0, 0 });
	parsePath(input, pos, 0, 0, vector<pair<int, int>>());
	const auto answer = findMaxDoors();

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}