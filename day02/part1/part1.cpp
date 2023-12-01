#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <map>

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

	auto twoCharCount = 0;
	auto threeCharCount = 0;
	string line;
	auto parseStream = stringstream(input);
	while (getline(parseStream, line))
	{
		map<char, int> characterCount;
		for (char& c : line)
		{
			characterCount[c] += 1;
		}


		auto hasTwo = false;
		auto hasThree = false;
		for (auto const& [_, num] : characterCount)
		{
			if (num == 2)
			{
				hasTwo = true;
			}

			if (num == 3)
			{
				hasThree = true;
			}
		}

		if (hasTwo)
		{
			twoCharCount += 1;
		}

		if (hasThree)
		{
			threeCharCount += 1;
		}
	}


	const auto answer = twoCharCount * threeCharCount;

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}