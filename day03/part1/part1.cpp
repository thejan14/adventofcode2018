#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <tuple>
#include <regex>

using namespace std;

const regex claimEx = regex("#\\d+ @ (\\d+),(\\d+): (\\d+)x(\\d+)");

tuple<int, int, int, int> parseClaim(string& claimLine)
{
	smatch matches;
	if (regex_match(claimLine, matches, claimEx))
	{
		return { stoi(matches[1]), stoi(matches[2]), stoi(matches[3]), stoi(matches[4]) };
	}
	else
	{
		return {};
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
	auto fabric = new int[1000][1000]();
	string line;
	auto parseStream = stringstream(input);
	while (getline(parseStream, line))
	{
		auto const [dl, dt, w, h] = parseClaim(line);
		for (auto y = dt; y < dt + h; y++)
		{
			for (auto x = dl; x < dl + w; x++)
			{
				fabric[x][y] += 1;

				// check for exactly 2 to count square inch exactly once when claimed 2+ times
				if (fabric[x][y] == 2)
				{
					answer += 1;
				}
			}
		}
	}

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}