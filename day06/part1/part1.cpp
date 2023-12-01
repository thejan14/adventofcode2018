#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <vector>

using namespace std;

int cantorPair(int x, int y)
{
	return static_cast<int>(0.5 * (x + y) * (x + y + 1) + y);
}

struct Coord
{
	int x, y;
};

struct Bounds
{
	int n, e, s, w;
};

int manhattan(Coord a, Coord b)
{
	return abs(a.x - b.x) + abs(a.y - b.y);
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

	string line;
	auto parseStream = stringstream(input);
	auto coordinates = vector<Coord>();
	auto bounds = Bounds();
	while (getline(parseStream, line))
	{
		auto const sep = line.find(',');
		auto const coord = Coord { stoi(line.substr(0, sep)), stoi(line.substr(sep + 1)) };
		if (coord.x < bounds.w)
		{
			bounds.w = coord.x;
		}

		if (coord.x > bounds.e)
		{
			bounds.e = coord.x;
		}

		if (coord.y < bounds.n)
		{
			bounds.n = coord.y;
		}

		if (coord.y > bounds.s)
		{
			bounds.s = coord.y;
		}

		coordinates.push_back(coord);
	}

	auto areas = vector<int>(coordinates.size());
	for (auto x = bounds.w; x <= bounds.e; x++)
	{
		for (auto y = bounds.n; y <= bounds.s; y++) {
			struct { int id; int dist; bool tied; } closest = { -1, INT_MAX, false };
			for (auto i = 0; i < coordinates.size(); i++)
			{
				auto dist = manhattan(Coord{ x, y }, coordinates[i]);
				if (dist == closest.dist)
				{
					closest.tied = true;
				}
				else if(dist < closest.dist)
				{
					closest.id = i;
					closest.dist = dist;
					closest.tied = false;
				}
			}

			if (closest.id != -1 && !closest.tied && areas[closest.id] > -1)
			{
				if (x == bounds.w || x == bounds.e || y == bounds.n || y == bounds.s)
				{
					areas[closest.id] = -1;
				}
				else
				{
					areas[closest.id] += 1;
				}
			}
		}
	}

	auto maxArea = max_element(areas.begin(), areas.end());
	auto const answer = *maxArea;

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	std::cout << std::format("{} ({})", answer, micros);
}