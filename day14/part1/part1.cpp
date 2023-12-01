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

	auto goal = stoi(input);
	auto board = new vector<int>{ 3, 7 };
	board->reserve(goal);
	auto elf1 = 0;
	auto elf2 = 1;
	while (board->size() < goal + 10)
	{
		auto combination = (*board)[elf1] + (*board)[elf2];
		auto newRecipes = vector<int>();
		newRecipes.reserve(2);
		do
		{
			newRecipes.push_back(combination % 10);
			combination /= 10;
		}
		while (combination > 0);

		board->insert(board->end(), newRecipes.crbegin(), newRecipes.crend());
		elf1 = (elf1 + (*board)[elf1] + 1) % board->size();
		elf2 = (elf2 + (*board)[elf2] + 1) % board->size();
	}

	auto i = 0;
	auto answer = 0ULL;
	for (auto const& n : (*board) | views::drop(goal) | views::take(10) | views::reverse)
	{
		answer += static_cast<unsigned long long>(n * pow(10, i++));
	}

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}