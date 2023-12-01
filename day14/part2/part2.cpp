#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <ranges>

using namespace std;

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

	auto answer = 0;
	auto pattern = vector<int>();
	auto goal = stoi(input);
	while (goal > 0)
	{
		pattern.push_back(goal % 10);
		goal /= 10;
	}

	ranges::reverse(pattern);

	auto board = new vector<int>{ 3, 7 };
	auto elf1 = 0;
	auto elf2 = 1;
	auto current = 2;
	while (answer == 0)
	{
		auto combination = (*board)[elf1] + (*board)[elf2];
		if (combination > 9)
		{
			board->push_back(1);
			board->push_back(combination % 10);
		}
		else
		{
			board->push_back(combination);
		}

		elf1 = (elf1 + (*board)[elf1] + 1) % board->size();
		elf2 = (elf2 + (*board)[elf2] + 1) % board->size();

		while (current + pattern.size() < board->size() && answer == 0)
		{
			auto patternFound = ranges::equal(
				(*board) | views::drop(current) | views::take(pattern.size()),
				pattern
			);

			if (patternFound)
			{
				answer = current;
			}

			current += 1;
		}
	}

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}