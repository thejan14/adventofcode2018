#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <array>

using namespace std;

int const GRID_SIZE = 300;
auto grid = array<array<int, GRID_SIZE>, GRID_SIZE>();
auto sat = array<array<int, GRID_SIZE>, GRID_SIZE>();

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

int getCellPower(int x, int y, int s)
{
	if (s == 0)
	{
		return grid[y][x];
	}
	else
	{
		return sat[y - s][x - s] + sat[y][x] - sat[y][x - s] - sat[y - s][x];
	}
}

void makeGridAndAreaTable(int serial)
{
	for (auto y = 0; y < GRID_SIZE; y++)
	{
		for (auto x = 0; x < GRID_SIZE; x++)
		{
			grid[y][x] = getPowerLevel(x + 1, y + 1, serial);
			sat[y][x] = grid[y][x];
			if (x > 0 && y > 0) [[likely]]
			{
				sat[y][x] += sat[y][x - 1] + sat[y - 1][x] - sat[y - 1][x - 1];
			}
			else if (x > 0)
			{
				sat[y][x] += sat[y][x - 1];
			}
			else if (y > 0)
			{
				sat[y][x] += sat[y - 1][x];
			}
		}
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

	makeGridAndAreaTable(stoi(input));
	struct { int x; int y; int s; } square = { 0, 0, 0 };
	auto max = INT_MIN;
	for (auto s = 0; s < GRID_SIZE; s++)
	{
		for (auto y = s; y < GRID_SIZE; y++)
		{
			for (auto x = s; x < GRID_SIZE; x++)
			{
				auto total = getCellPower(x, y, s);
				if (total > max)
				{
					max = total;

					// account for index shift in summed area table calculation and 1 based grid
					square.x = x - s + 1 + 1;
					square.y = y - s + 1 + 1;
					square.s = s;
				}
			}
		}
	}

	auto const answer = std::format("{},{},{}", square.x, square.y, square.s);

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}