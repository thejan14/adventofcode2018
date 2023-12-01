#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <vector>

using namespace std;

string getMatch(const string& id1, const string& id2)
{
	auto diffPos = -1;
	for (auto pos = 0; pos < id1.length(); pos++)
	{
		if (id1[pos] != id2[pos])
		{
			if (diffPos == -1)
			{
				diffPos = pos; // no other diff was found yet
			}
			else
			{
				return string(); // more than one diff was found, no match
			}
		}
	}


	return id1.substr(0, diffPos) + id1.substr(diffPos + 1);
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

	vector<string> ids;
	string line;
	auto parseStream = stringstream(input);
	while (getline(parseStream, line))
	{
		ids.push_back(line);
	}

	string answer;
	for (auto i = 0; i < ids.size(); i++)
	{
		auto const id1 = ids[i];
		for (auto j = i; j < ids.size(); j++)
		{
			auto const id2 = ids[j];
			auto const match = getMatch(id1, id2);
			if (!match.empty())
			{
				answer = match;
				break;
			}
		}
	}

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}