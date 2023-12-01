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
long long const GENERATION_COUNT = 50'000'000'000LL;

auto rules = map<string, char>();
auto largePatterns = map<string, int>(); // maps a larger pattern to the current number of plants

int getSum(string const& state, int offset)
{
	auto res = 0;
	for (auto i = 0; i < state.size(); i++)
	{
		if (state[i] == '#')
		{
			res += i - offset;
		}
	}
	
	return res;
}

string trimState(string& state, int& offset)
{
	auto f = state.find_first_of('#');
	auto l = state.find_last_of('#');
	offset += PATTERN_LENGTH - 1 - static_cast<int>(f);
	return "...."s + state.substr(f, l + 1 - f) + "...."s;
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

	auto offset = 0;
	string state;
	string line;
	auto parseStream = stringstream(input);
	if (getline(parseStream, line))
	{
		state += line.substr(15, line.size() - 15); // omit trailing new line
	}

	state = trimState(state, offset);
	parseStream.ignore(1, '\n'); // omit separating empty line
	while (getline(parseStream, line))
	{
		rules[line.substr(0, PATTERN_LENGTH)] = line[RES_POS];
	}

	auto answer = 0LL;
	auto lastState = state;
	auto lastSum = getSum(state, offset);
	for (auto generation = 0LL; generation < GENERATION_COUNT; generation++)
	{
		auto newState = state;
		for (auto i = 2U; i < state.size() - 2; i++)
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

		state = trimState(newState, offset);
		if (lastState.compare(state) == 0)
		{
			auto currentSum = getSum(state, offset);
			auto iterationsLeft = GENERATION_COUNT - generation - 1;
			auto iterationValue = currentSum - lastSum;
			answer = currentSum + iterationsLeft * iterationValue;
			generation = GENERATION_COUNT;
		}
		else
		{
			lastState = state;
			lastSum = getSum(state, offset);
		}
	}

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}