#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <regex>
#include <queue>
#include <ranges>

using namespace std;

enum State
{
	Sand,
	Clay,
	WaterAtRest,
	WaterFlowing,
};

struct Vein
{
	pair<int, int> xRange, yRange;
};

struct Bounds
{
	int n, e, s, w;
};

vector<vector<State>> mapState;
pair<int, int> const waterSpawn = { 500, 0 };
regex const veinEx = regex("([xy])=(\\d+), ([xy])=(\\d+)\\.\\.(\\d+)", regex::optimize);


struct Surface
{
	int y;
	pair<int, int> area;
	vector<pair<int, int>> leaks;

	void expandArea(int x)
	{
		if (x < this->area.first)
		{
			this->area.first = x;
		}

		if (x > this->area.second)
		{
			this->area.second = y;
		}
	}

	void materialize(State state)
	{
		for (auto x = area.first; x <= area.second; x++)
		{
			mapState[this->y][x] = state;
		}
	}
};

Bounds getBounds(vector<Vein>& veins)
{
	auto bounds = Bounds{
		INT_MAX,
		INT_MIN,
		INT_MIN,
		INT_MAX,
	};

	for (auto const& vein : veins)
	{
		if (vein.xRange.first < bounds.w)
		{
			bounds.w = vein.xRange.first;
		}

		if (vein.xRange.second > bounds.e)
		{
			bounds.e = vein.xRange.second;
		}

		if (vein.yRange.first < bounds.n)
		{
			bounds.n = vein.yRange.first;
		}

		if (vein.yRange.second > bounds.s)
		{
			bounds.s = vein.yRange.second;
		}
	}

	// expand horizontal bounds by one so that water can overflow any container (any x is valid)
	bounds.w -= 1;
	bounds.e += 1;
	return bounds;
}

bool inMap(int x, int y)
{
	return y > -1 && y < mapState.size() && x > -1 && x < mapState[y].size();
}

int isBlocking(State state)
{
	return state == State::Clay || state == State::WaterAtRest;
}

Surface makeSurface(int x, int y)
{
	auto surface = Surface{ y, {x, x} };
	auto nextY = y + 1;
	auto nextX = x - 1;
	auto foundBound = false;
	do
	{
		if (inMap(nextX, y) && !isBlocking(mapState[y][nextX]))
		{
			if (inMap(nextX, nextY) && isBlocking(mapState[nextY][nextX]))
			{
				nextX -= 1;
			}
			else
			{
				surface.leaks.push_back({ nextX, nextY });
				surface.area.first = nextX;
				foundBound = true;
			}
		}
		else
		{
			surface.area.first = nextX + 1;
			foundBound = true;
		}
	} while (!foundBound);


	nextX = x + 1;
	foundBound = false;
	do
	{
		if (inMap(nextX, y) && !isBlocking(mapState[y][nextX]))
		{
			if (inMap(nextX, nextY) && isBlocking(mapState[nextY][nextX]))
			{
				nextX += 1;
			}
			else
			{
				surface.leaks.push_back({ nextX, nextY });
				surface.area.second = nextX;
				foundBound = true;
			}
		}
		else
		{
			surface.area.second = nextX - 1;
			foundBound = true;
		}
	} while (!foundBound);

	return surface;
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

	auto veins = vector<Vein>();
	smatch match;
	auto it = input.cbegin();
	while (regex_search(it, input.cend(), match, veinEx))
	{
		auto vein = Vein();
		auto constAxis = match[1] == "x" ? &vein.xRange : &vein.yRange;
		auto rangeAxis = match[3] == "x" ? &vein.xRange : &vein.yRange;
		constAxis->first = stoi(match[2]);
		constAxis->second = constAxis->first;
		rangeAxis->first = stoi(match[4]);
		rangeAxis->second = stoi(match[5]);
		veins.push_back(vein);
		it = match.suffix().first;
	}

	auto const bounds = getBounds(veins);
	auto const width = bounds.e - bounds.w + 1;
	auto const height = bounds.s + 1; // assume water spawn is the upper bound (0)
	auto const spwanOffset = waterSpawn.second - bounds.n;
	mapState = vector<vector<State>>(height, vector<State>(width, State::Sand));
	for (auto const& vein : veins)
	{
		for (auto y = vein.yRange.first; y <= vein.yRange.second; y++)
		{
			for (auto x = vein.xRange.first; x <= vein.xRange.second; x++)
			{
				mapState[y - bounds.n - spwanOffset][x - bounds.w] = State::Clay;
			}
		}
	}

	auto queue = std::queue<pair<int, int>>();
	auto [waterX, waterY] = waterSpawn;
	queue.push({ waterX - bounds.w, waterY });
	while (!queue.empty())
	{
		auto [x, y] = queue.front();
		queue.pop();

		// if the water is already flowing this place has been visited before, no need to check
		if (mapState[y][x] != State::WaterFlowing)
		{
			if (inMap(x, y + 1))
			{
				if (isBlocking(mapState[y + 1][x]))
				{
					auto surface = makeSurface(x, y);
					if (surface.leaks.empty())
					{
						surface.materialize(State::WaterAtRest);

						// re-add position right above the new surface
						mapState[y - 1][x] = State::Sand;
						queue.push({ x, y - 1 });
					}
					else
					{
						surface.materialize(State::WaterFlowing);
						for (auto leak : surface.leaks)
						{
							queue.push(leak);
						}
					}
				}
				else
				{
					mapState[y][x] = State::WaterFlowing;
					queue.push({ x, y + 1 });
				}
			}
			else
			{
				mapState[y][x] = State::WaterFlowing;
			}
		}
	}

	auto const answer = ranges::count_if(
		mapState | views::drop(abs(spwanOffset)) | std::views::join,
		[](auto& state) { return state == State::WaterAtRest; });

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}