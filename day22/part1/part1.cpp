#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <regex>
#include <array>

using namespace std;

uint64_t constexpr uninitialized = std::numeric_limits<uint64_t>::max();
regex const numberEx = regex(R"(\d+)", regex::optimize);

struct caveMap
{
	vector<vector<uint64_t>> typeMap, elMap, giMap;
	uint64_t depth, tx, ty;
	caveMap(uint64_t depth, uint64_t tx, uint64_t ty)
		: typeMap(ty + 1, vector<uint64_t>(tx + 1, uninitialized)),
		elMap(ty + 1, vector<uint64_t>(tx + 1, uninitialized)),
		giMap(ty + 1, vector<uint64_t>(tx + 1, uninitialized)),
		depth(depth),
		tx(tx),
		ty(ty)
	{
	}

	uint64_t getGeologicIndex(uint64_t const x, uint64_t const y)
	{
		if (this->giMap[y][x] != uninitialized)
		{
			return this->giMap[y][x];
		}

		uint64_t result;
		if (x == this->tx && y == this->ty)
		{
			result = 0;
		}
		else
		{
			if (x != 0)
			{
				if (y != 0)
				{
					result = getErosionLevel(x - 1, y) * getErosionLevel(x, y - 1); // x && y != 0
				}
				else
				{
					result = x * 16807ULL; // y == 0
				}
			}
			else
			{
				if (y != 0)
				{
					result = y * 48271ULL; // x == 0
				}
				else
				{
					result = 0; // x && y == 0
				}
			}
		}

		this->giMap[y][x] = result;
		return result;
	}

	uint64_t getErosionLevel(uint64_t const x, uint64_t const y)
	{
		if (this->elMap[y][x] != uninitialized)
		{
			return this->elMap[y][x];
		}

		auto const result = (getGeologicIndex(x, y) + this->depth) % 20183ULL;
		this->elMap[y][x] = result;
		return result;
	}

	uint64_t getType(uint64_t const x, uint64_t const y)
	{
		if (this->typeMap[y][x] != uninitialized)
		{
			return this->typeMap[y][x];
		}

		auto const result = getErosionLevel(x, y) % 3ULL;
		this->typeMap[y][x] = result;
		return result;
	}

	uint64_t getRiskLevel()
	{
		uint64_t riskLevel = 0;
		for (uint64_t y = 0; y <= this->ty; y++)
		{
			for (uint64_t x = 0; x <= this->tx; x++)
			{
				riskLevel += this->getType(x, y);
			}
		}

		return riskLevel;
	}
};

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

	auto valIndex = 0;
	auto values = std::array<uint64_t, 3>();
	smatch numberMatch;
	auto numberIt = input.cbegin();
	while (regex_search(numberIt, input.cend(), numberMatch, numberEx))
	{
		values[valIndex++] = stoi(numberMatch[0]);
		numberIt = numberMatch.suffix().first;
	}

	auto [depth, tx, ty] = values;
	auto map = caveMap(depth, tx, ty);

	const auto answer = map.getRiskLevel();

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}