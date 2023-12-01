#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <algorithm>
#include <ranges>
#include <map>

using namespace std;

int const RES_POS = 9;
int const PATTERN_LENGTH = 5;
int const GENERATION_COUNT = 20;

auto rules = map<string, char>();

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

	auto offset = 4;
	auto state = "...."s; // leave enough empty space for pattern matching to the left
	string line;
	auto parseStream = stringstream(input);
	if (getline(parseStream, line))
	{
		state += line.substr(15, line.size() - 15); // omit trailing new line
	}

	state += "...."s; // leave enough empty space for pattern matching to the right

	parseStream.ignore(1, '\n'); // omit separating empty line
	while (getline(parseStream, line))
	{
		rules[line.substr(0, PATTERN_LENGTH)] = line[RES_POS];
	}

	for (auto generation = 0; generation < GENERATION_COUNT; generation++)
	{
		auto newState = state;
		for (auto i = 2U; i < state.size(); i++)
		{
			auto pattern = state.substr(i - 2, PATTERN_LENGTH);
			if (rules.contains(pattern))
			{
				newState[i] = rules[pattern];
			}
			else
			{
				newState[i] = '.';
			}
		}

		auto f = newState.find_first_of('#');
		if (f < PATTERN_LENGTH)
		{
			newState = "...." + newState;
			offset += 4;
		}

		auto l = newState.find_last_of('#');
		if (newState.size() - l < PATTERN_LENGTH)
		{
			newState += "....";
		}

		state = newState;
	}

	auto answer = 0;
	for (auto i = 0; i < state.size(); i++)
	{
		if (state[i] == '#')
		{
			answer += i - offset;
		}
	}

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}