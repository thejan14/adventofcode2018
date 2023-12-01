#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <array>
#include <algorithm>

using namespace std;

// change to 6 for test input
int const CHARACTER_COUNT = 26;

// upper case 'A' has ASCII code 65
int const ASCII_OFFSET = 65;

bool stepReady(int& edge, bool finished)
{
	if (edge == 0)
	{
		return true;
	}
	else
	{
		return finished;
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

	auto adjacencyMatrix = array<array<int, CHARACTER_COUNT>, CHARACTER_COUNT>();
	string line;
	auto parseStream = stringstream(input);
	while (getline(parseStream, line))
	{
		auto to = (int)line[5] - ASCII_OFFSET;
		auto from = (int)line[36] - ASCII_OFFSET;
		adjacencyMatrix[from][to] = 1;
	}

	auto order = vector<char>();
	order.reserve(CHARACTER_COUNT);
	auto finished = array<bool, CHARACTER_COUNT>();
	while (order.size() != CHARACTER_COUNT)
	{
		for (auto i = 0; i < CHARACTER_COUNT; i++)
		{
			if (!finished[i])
			{
				auto j = 0;
				if (ranges::all_of(adjacencyMatrix[i], [&finished, &j](int edge) { return stepReady(edge, finished[j++]); }))
				{
					order.push_back(static_cast<char>(i + ASCII_OFFSET));
					finished[i] = true;
					break; // start from first letter after finishing one to ensure alphabetical order if multiple steps are ready
				}
			}
		}
	}

	const auto answer = string(order.begin(), order.end());

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}