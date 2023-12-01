#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <array>
#include <ranges>
#include <map>

using namespace std;

int const DIMENSION = 50;
long long const TARGET_MINUTES = 1'000'000'000;

// keep two state maps to write the next results in the respectively other one
std::string stateA;
std::string stateB;


char& getStateAt(std::string& state, int x, int y)
{
	return state[(y * (DIMENSION + 1)) + x];
}

vector<char> getAdjacent(std::string& currentState, int x, int y)
{
	vector<char> adj;
	adj.reserve(8);
	auto iMax = std::min(DIMENSION, y + 2);
	auto jMax = std::min(DIMENSION, x + 2);
	for (auto i = std::max(0, y - 1); i < iMax; i++)
	{
		for (auto j = std::max(0, x - 1); j < jMax; j++)
		{
			if (i != y || j != x)
			{
				adj.push_back(getStateAt(currentState, j, i));
			}
		}
	}

	return adj;
}

void processMinute(std::string& currentState, std::string& nextState)
{
	for (auto y = 0; y < DIMENSION; y++)
	{
		for (auto x = 0; x < DIMENSION; x++)
		{
			auto adjacent = getAdjacent(currentState, x, y);
			switch (getStateAt(currentState, x, y))
			{
				case '|':
				{
					auto adjLumberyards = ranges::count_if(adjacent, [](auto& state) { return state == '#'; });
					getStateAt(nextState, x, y) = adjLumberyards > 2 ? '#' : '|';
					break;
				}
				case '#':
				{
					auto hasLumberyardAdj = ranges::any_of(adjacent, [](auto& state) { return state == '#'; });
					auto hasTreesAdj = ranges::any_of(adjacent, [](auto& state) { return state == '|'; });
					getStateAt(nextState, x, y) = hasLumberyardAdj && hasTreesAdj ? '#' : '.';
					break;
				}
				default:
				{
					auto adjTrees = ranges::count_if(adjacent, [](auto& state) { return state == '|'; });
					getStateAt(nextState, x, y) = adjTrees > 2 ? '|' : '.';
					break;
				}
			}
		}
	}
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

	stateA = input;
	stateB = input;
	auto& currentState = stateA;
	auto& nextState = stateB;
	auto minutes = 0LL;
	auto stateMap = std::map<string, int>();
	auto minutesMap = std::map<long long, const string*>();
	while (minutes < TARGET_MINUTES)
	{
		processMinute(currentState, nextState);
		minutes += 1;
		if (stateMap.contains(nextState))
		{
			auto patternLength = minutes - stateMap[nextState];
			auto patternStart = minutes - patternLength;
			auto remainingMinutes = TARGET_MINUTES - minutes;
			auto skips = remainingMinutes / patternLength;
			minutes += skips * patternLength;
			remainingMinutes = TARGET_MINUTES - minutes;
			nextState = *minutesMap[patternStart + remainingMinutes];
			minutes = TARGET_MINUTES;
		}
		else
		{
			auto res = stateMap.insert(std::map<string, int>::value_type({ nextState, minutes }));
			minutesMap[minutes] = &((*res.first).first);
		}

		auto& tmp = currentState;
		currentState = nextState;
		nextState = tmp;
	}

	auto trees = 0;
	auto lumberyards = 0;
	for (auto y = 0; y < DIMENSION; y++)
	{
		for (auto x = 0; x < DIMENSION; x++)
		{
			if (getStateAt(currentState, x, y) == '|')
			{
				trees += 1;
			}

			if (getStateAt(currentState, x, y) == '#')
			{
				lumberyards += 1;
			}
		}
	}

	auto const answer = trees * lumberyards;

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}