#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <tuple>
#include <regex>
#include <vector>
#include <iterator>
#include <algorithm>
#include <map>

using namespace std;

struct Claim
{
	int id, top, right, bottom, left;
};

const regex claimEx = regex("#(\\d+) @ (\\d+),(\\d+): (\\d+)x(\\d+)");

Claim parseClaim(string& claimLine)
{
	smatch matches;
	if (regex_match(claimLine, matches, claimEx))
	{
		auto x = stoi(matches[2]);
		auto y = stoi(matches[3]);
		auto w = stoi(matches[4]);
		auto h = stoi(matches[5]);
		return { stoi(matches[1]), y, x + w, y + h, x };
	}
	else
	{
		return {};
	}
}

bool overlap(const Claim& claim1, const Claim& claim2)
{
	auto const [id1, t1, r1, b1, l1] = claim1;
	auto const [id2, t2, r2, b2, l2] = claim2;
	if (l1 < r2
		&& r1 > l2
		&& t1 < b2
		&& b1 > t2)
	{
		return true;
	}
	else
	{
		return false;
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

	vector<Claim> claims;
	string line;
	auto parseStream = stringstream(input);
	while (getline(parseStream, line))
	{
		claims.push_back(parseClaim(line));
	}

	auto answer = 0;
	for (auto i = 0; i < claims.size(); i++)
	{
		const auto claim = claims[i];
		auto const anyOverlap = any_of(
			claims.begin(),
			claims.end(),
			[&claim](Claim otherClaim)
			{
				return otherClaim.id != claim.id
					? overlap(claim, otherClaim)
					: false;
			});

		if (!anyOverlap)
		{
			answer = claim.id;
			break;
		}
	}

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}