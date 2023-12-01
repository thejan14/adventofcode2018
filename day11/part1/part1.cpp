#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <array>

using namespace std;

int const GRID_SIZE = 300;
int const CELL_SIZE = 3;

int getPowerLevel(int x, int y, int serial)
{
	auto const rackId = x + 10;
	auto powerLevel = (rackId * y) + serial;
	powerLevel = powerLevel * rackId;
	if (powerLevel < 100)
	{
		powerLevel = 0;
	}
	else
	{
		powerLevel = (powerLevel / 100) % 10;
	}

	return powerLevel - 5;
}

int getCellPower(int x, int y, array<array<int, GRID_SIZE>, GRID_SIZE>& grid)
{
	auto res = 0;
	for (auto i = y; i < y + CELL_SIZE; i++)
	{
		for (auto j = x; j < x + CELL_SIZE; j++)
		{
			res += grid[i][j];
		}
	}

	return res;
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

	auto const serial = stoi(input);
	auto grid = array<array<int, GRID_SIZE>, GRID_SIZE>();
	for (auto y = 0; y < GRID_SIZE; y++)
	{
		for (auto x = 0; x < GRID_SIZE; x++)
		{
			grid[y][x] = getPowerLevel(x + 1, y + 1, serial);
		}
	}


	auto coord = pair { 0, 0 };
	auto max = INT_MIN;
	auto const lastIndex = GRID_SIZE - CELL_SIZE + 1;
	for (auto y = 0; y < lastIndex; y++)
	{
		for (auto x = 0; x < lastIndex; x++)
		{
			auto total = getCellPower(x, y, grid);
			if (total > max)
			{
				max = total;
				coord.first = x + 1;
				coord.second = y + 1;
			}
		}
	}

	auto const answer = std::format("{},{}", coord.first, coord.second);

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}