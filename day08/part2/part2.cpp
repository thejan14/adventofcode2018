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

int readHeader(string const& input, int& current)
{
	auto total = 0;
	auto const childNodeCount = readNumber(input, current);
	auto const metadataCount = readNumber(input, current);
	if (childNodeCount == 0)
	{
		for (auto j = 0; j < metadataCount; j++)
		{
			total += readNumber(input, current);
		}
	}
	else
	{
		auto childNodes = vector<int>();
		childNodes.reserve(childNodeCount);
		for (auto i = 0; i < childNodeCount; i++)
		{
			childNodes.push_back(readHeader(input, current));
		}

		for (auto j = 0; j < metadataCount; j++)
		{
			auto index = readNumber(input, current) - 1; // account for 0 based indices
			if (index < childNodeCount)
			{
				total += childNodes[index];
			}
		}
	}

	return total;
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

	auto current = 0;
	auto const answer = readHeader(input, current);

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}