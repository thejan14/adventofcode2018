#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <array>
#include <ranges>

using namespace std;

int const DIMENSION = 50;

enum State
{
	Open,
	Trees,
	Lumberyard
};


// keep two state maps to write the next results in the respectively other one
typedef array<array<State, DIMENSION>, DIMENSION> stateMap;
stateMap stateA;
stateMap stateB;

vector<State> getAdjacent(stateMap& currentState, int x, int y)
{
	vector<State> adj;
	adj.reserve(8);
	auto iMax = std::min(DIMENSION, y + 2);
	auto jMax = std::min(DIMENSION, x + 2);
	for (auto i = std::max(0, y - 1); i < iMax; i++)
	{
		for (auto j = std::max(0, x - 1); j < jMax; j++)
		{
			if (i != y || j != x)
			{
				adj.push_back(currentState[i][j]);
			}
		}
	}

	return adj;
}

void processMinute(stateMap& currentState, stateMap& nextState)
{
	for (auto y = 0; y < DIMENSION; y++)
	{
		for (auto x = 0; x < DIMENSION; x++)
		{
			auto adjacent = getAdjacent(currentState, x, y);
			switch (currentState[y][x])
			{
				case State::Trees:
				{
					auto adjLumberyards = ranges::count_if(adjacent, [](auto& state) { return state == State::Lumberyard; });
					nextState[y][x] = adjLumberyards > 2 ? State::Lumberyard : State::Trees;
					break;
				}
				case State::Lumberyard:
				{
					auto hasLumberyardAdj = ranges::any_of(adjacent, [](auto& state) { return state == State::Lumberyard; });
					auto hasTreesAdj = ranges::any_of(adjacent, [](auto& state) { return state == State::Trees; });
					nextState[y][x] = hasLumberyardAdj && hasTreesAdj ? State::Lumberyard : State::Open;
					break;
				}
				default:
				{
					auto adjTrees = ranges::count_if(adjacent, [](auto& state) { return state == State::Trees; });
					nextState[y][x] = adjTrees > 2 ? State::Trees : State::Open;
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

	for (auto y = 0; y < DIMENSION; y++)
	{
		auto lineEnd = (y * (DIMENSION + 1)) + DIMENSION;
		for (auto x = 0; x < DIMENSION; x++)
		{
			switch (input[(y * (DIMENSION + 1)) + x])
			{
				case '|':
					stateA[y][x] = State::Trees;
					break;
				case '#':
					stateA[y][x] = State::Lumberyard;
					break;
				default:
					stateA[y][x] = State::Open;
					break;
			}
		}
	}

	auto& currentState = stateA;
	auto& nextState = stateB;
	for (auto m = 0; m < 10; m++)
	{
		processMinute(currentState, nextState);
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
			if (currentState[y][x] == State::Trees)
			{
				trees += 1;
			}

			if (currentState[y][x] == State::Lumberyard)
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