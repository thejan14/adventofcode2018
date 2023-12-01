#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>

using namespace std;

int readNumber(string const& input, int& current)
{
	auto const next = input.find(' ', current);
	if (next != string::npos)
	{
		auto const number = stoi(input.substr(current, next - current));
		current = static_cast<int>(next + 1);
		return number;
	}
	else
	{
		// end of input, account for trailing newline
		return stoi(input.substr(current, input.size() - current - 1));
	}
}

void readHeader(string const& input, int& current, int& total)
{
	auto const childNodes = readNumber(input, current);
	auto const metadata = readNumber(input, current);
	for (auto i = 0; i < childNodes; i++)
	{
		readHeader(input, current, total);
	}

	for (auto j = 0; j < metadata; j++)
	{
		total += readNumber(input, current);
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
	auto current = 0;
	readHeader(input, current, answer);

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}