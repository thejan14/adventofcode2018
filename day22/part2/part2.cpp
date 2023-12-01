#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <regex>
#include <array>
#include <map>
#include <queue>
#include <ranges>
#include <unordered_map>

using namespace std;

uint64_t constexpr uninitialized = std::numeric_limits<uint64_t>::max();
regex const numberEx = regex(R"(\d+)", regex::optimize);
struct { int64_t dx, dy; } constexpr directions[] = { {0, -1}, {1, 0}, {0, 1}, {-1, 0},  };

enum tool
{
	none,
	gear,
	torch
};

struct pos
{
	int64_t x, y;
	bool operator<(const pos& pos) const
	{
		return std::tie(this->x, this->y) < std::tie(pos.x, pos.y);
	}

	bool operator==(const pos& pos) const
	{
		return std::tie(this->x, this->y) == std::tie(pos.x, pos.y);
	}
};

inline int64_t manhattan_distance(pos const& a, pos const& b)
{
	return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

struct searchState
{
	pos p;
	tool t;
	bool operator<(const searchState& state) const
	{
		return std::tie(this->p, this->t) < std::tie(state.p, state.t);
	}

	bool operator==(const searchState& state) const
	{
		return std::tie(this->p, this->t) == std::tie(state.p, state.t);
	}
};

template <>
struct std::hash<searchState>
{
	std::size_t operator()(const searchState& s) const noexcept
	{
		return ((hash<int64_t>()(s.p.x)
			^ (hash<int64_t>()(s.p.y) << 1)) >> 1)
			^ (hash<tool>()(s.t) << 1);
	}
};
template <>
struct std::hash<pos>
{
	std::size_t operator()(const pos& p) const noexcept
	{
		return (hash<int64_t>()(p.x)
			^ (hash<int64_t>()(p.y) << 1)) >> 1;
	}
};

struct caveMap
{
	std::unordered_map<pos, uint64_t> typeMap, elMap, giMap;
	uint64_t depth;
	pos target;
	caveMap(uint64_t depth, int64_t tx, int64_t ty) : depth(depth), target(pos{ tx, ty })
	{
	}

	uint64_t getGeologicIndex(pos const& p)
	{
		if (this->giMap.contains(p))
		{
			return this->giMap[p];
		}

		uint64_t result;
		if (p == this->target)
		{
			result = 0;
		}
		else
		{
			if (p.x != 0)
			{
				if (p.y != 0)
				{
					result = getErosionLevel(pos{ p.x - 1, p.y }) * getErosionLevel(pos{ p.x, p.y - 1 }); // x && y != 0
				}
				else
				{
					result = p.x * 16807ULL; // y == 0
				}
			}
			else
			{
				if (p.y != 0)
				{
					result = p.y * 48271ULL; // x == 0
				}
				else
				{
					result = 0; // x && y == 0
				}
			}
		}

		this->giMap[p] = result;
		return result;
	}

	uint64_t getErosionLevel(pos const& p)
	{
		if (this->elMap.contains(p))
		{
			return this->elMap[p];
		}

		auto const result = (getGeologicIndex(p) + this->depth) % 20183ULL;
		this->elMap[p] = result;
		return result;
	}

	uint64_t getType(pos const& p)
	{
		if (this->typeMap.contains(p))
		{
			return this->typeMap[p];
		}

		auto const result = getErosionLevel(p) % 3ULL;
		this->typeMap[p] = result;
		return result;
	}

	bool isToolValid(pos p, tool t)
	{
		auto const type = this->getType(p);
		if (type == 0)
		{
			return t == gear || t == torch;
		}
		else if (type == 1)
		{
			return t == gear || t == none;
		}
		else
		{
			return t == torch || t == none;
		}
	}

	auto getAdjacent(pos const& currentPos) const
	{
		return directions
			| std::views::transform([&currentPos](auto const& direction) { return pos{ currentPos.x + direction.dx, currentPos.y + direction.dy }; })
			| std::views::filter([](auto const& adjPos) { return !(adjPos.x < 0 || adjPos.y < 0); });
	}
};

inline auto getValidTools(pos const nextPos)
{
	return std::ranges::iota_view{ 0, 3 }
		| std::views::transform([](auto i) { return static_cast<tool>(i); });
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

	auto valIndex = 0;
	auto values = std::array<int64_t, 3>();
	smatch numberMatch;
	auto numberIt = input.cbegin();
	while (regex_search(numberIt, input.cend(), numberMatch, numberEx))
	{
		values[valIndex++] = stoi(numberMatch[0]);
		numberIt = numberMatch.suffix().first;
	}

	auto [depth, tx, ty] = values;
	auto cave = caveMap(depth, tx, ty);

	typedef std::pair<int64_t, searchState> queueElement;
	auto frontier = std::priority_queue<queueElement, std::vector<queueElement>, std::greater<>>();
	frontier.push({ 0, { { 0, 0 }, torch } });
	auto visited = std::unordered_map<searchState, uint64_t>();
	visited[{ 0, 0, torch }] = 0;

	auto bestPath = uninitialized;
	while (!frontier.empty())
	{
		auto const [priority, current] = frontier.top();
		frontier.pop();
		auto const& currentMinutes = visited[current];

		for (auto const nextPos : cave.getAdjacent(current.p))
		{
			for (auto const nextTool : std::ranges::iota_view{ 0, 3 } 
										| std::views::transform([](auto i){return static_cast<tool>(i); })
										| std::views::filter([&cave, &current, &nextPos](auto t) { return cave.isToolValid(nextPos, t) && cave.isToolValid(current.p, t); }))
			{
				auto const traversalMinutes = nextTool == current.t ? 1 : 8;
				auto const nextMinutes = currentMinutes + traversalMinutes;
				auto const nextState = searchState{ nextPos, nextTool };
				uint64_t visitedMinutes = uninitialized;
				if (visited.contains(nextState))
				{
					visitedMinutes = visited[nextState];
				}

				if (nextMinutes < bestPath && nextMinutes < visitedMinutes)
				{
					frontier.emplace(nextMinutes + manhattan_distance(nextPos, cave.target), nextState);
					visited[nextState] = nextMinutes;
					if (nextPos == cave.target)
					{
						bestPath = nextTool == torch ? nextMinutes : nextMinutes + 7;
					}
				}
			}
		}
	}

	auto const answer = bestPath;

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}