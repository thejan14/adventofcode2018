#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <array>
#include <algorithm>
#include <map>

using namespace std;

// change to 6 for test input
int const CHARACTER_COUNT = 26;

// upper case 'A' has ASCII code 65
int const ASCII_OFFSET = 65;

// one added to account that A starts with 0 in the implementation
int const WORK_BASE = 61;

int const NUM_WORKERS = 5;

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
	auto working = map<int, int>();
	auto time = 0;
	while (order.size() != CHARACTER_COUNT)
	{
		if (!working.empty())
		{
			auto [_, min] = *ranges::min_element(working, [](auto& a, auto& b) { return a.second < b.second; });
			time += min;
			auto it = working.begin();
			while (it != working.end())
			{
				auto& [step, work] = *it;
				work -= min;
				if (work == 0)
				{
					finished[step] = true;
					order.push_back(static_cast<char>(step + ASCII_OFFSET));
					it = working.erase(it);
				}
				else
				{
					it++;
				}
			}
		}

		for (auto w = working.size(); w < NUM_WORKERS; w++)
		{
			for (auto i = 0; i < CHARACTER_COUNT; i++)
			{
				if (!finished[i] && !working.contains(i))
				{
					auto j = 0;
					if (ranges::all_of(adjacencyMatrix[i], [&finished, &j](int edge) { return stepReady(edge, finished[j++]); }))
					{
						working[i] = WORK_BASE + i;
						break; // start from first letter after finishing one to ensure alphabetical order if multiple steps are ready
					}
				}
			}
		}
	}

	const auto answer = time;

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}