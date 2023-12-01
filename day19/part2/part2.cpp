#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <regex>
#include <array>
#include <functional>
#include <ranges>
#include <map>
#include <set>

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

	/*
	* Reverse engineered input program to this simple loop. Unfortunately this solution only works for my input.
	* I stronlgy assume that other inputs will only differ in the number produced to register 5. So if you
	* want to adjust this programm to your input you can step through the input program and see what number
	* is stored to register 5 before the program gets stuck in a loop. Then put your number to r5 here and let it run.
	*/

	auto r0 = 0;
	auto r1 = 1;
	auto r3 = 0;
	auto r4 = 1;
	auto r5 = 10551331;
	while (r1 <= r5)
	{
		r4 = 1;
		if (r5 % r1 == 0)
		{
			r0 += r1;
		}

		r1 += 1;
	}

	auto const answer = r0;

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	std::cout << std::format("{} ({})", answer, micros);
}