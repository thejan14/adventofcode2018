#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <regex>
#include <algorithm>

using namespace std;

const regex inputEx = regex("(\\d+) players; last marble is worth (\\d+) points\\n");

void advanceInRing(vector<int>& ring, vector<int>::iterator& it, int steps)
{
	auto clockwise = steps > 0;
	for (auto i = 0; i < abs(steps); i++)
	{
		if (clockwise)
		{
			if (it == ring.end())
			{
				it = ring.begin();
			}

			it++;
		}
		else
		{
			if (it == ring.begin())
			{
				it = ring.end();
			}

			it--;
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

	auto answer = 0;
	smatch matches;
	if (regex_match(input, matches, inputEx))
	{
		auto const players = stoi(matches[1]);
		auto const lastMarble = stoi(matches[2]);

		auto scores = vector<int>(players);
		auto ring = vector<int>{ 0 };
		auto currentPlayer = 0;
		auto currentMarble = ring.begin();
		for (auto i = 1; i <= lastMarble; i++)
		{
			if (i % 23 == 0)
			{
				scores[currentPlayer] += i;
				advanceInRing(ring, currentMarble, -7);
				scores[currentPlayer] += *currentMarble;
				currentMarble = ring.erase(currentMarble);
			}
			else
			{
				advanceInRing(ring, currentMarble, 2);
				currentMarble = ring.insert(currentMarble, i);
			}

			currentPlayer = (currentPlayer + 1) % players;
		}

		answer = *ranges::max_element(scores);
	}

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}