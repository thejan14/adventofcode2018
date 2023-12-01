#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <stack>

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

	auto polymer = stack<char>();
	auto const end = prev(input.end()); // omit trailing newline
	for (auto it = input.begin(); it != end; it++)
	{
		if (!polymer.empty() && polymer.top() != *it && tolower(polymer.top()) == tolower(*it))
		{
			polymer.pop();
		}
		else
		{
			polymer.push(*it);
		}
	}

	const auto answer = polymer.size();

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}