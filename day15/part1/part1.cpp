#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <ranges>
#include <array>
#include <queue>
#include <map>
#include <set>

using namespace std;

struct Unit
{
	int id;
	int hp;
	bool isGoblin; // otherwise: is elf
};

struct Coord
{
	int x, y;

	bool operator<(const Coord& coord) const
	{
		return this->y == coord.y
			? this->x < coord.x
			: this->y < coord.y;
	}

	bool operator==(const Coord& coord) const
	{
		return this->x == coord.x && this->y == coord.y;
	}

	bool operator!=(const Coord& coord) const
	{
		return !this->operator==(coord);
	}
};

struct Pos
{
	Coord coord;
	shared_ptr<Unit> unit;
	bool blocking;

	bool isBlocked() const
	{
		return this->blocking || this->unit != nullptr;
	}
};

int const DIMENSION = 32;
static Coord const NOWHERE = { -1, -1 };
static auto const directions = array<Coord, 4>{ { {0, -1}, { -1, 0 }, { 1, 0 }, { 0, 1 } } }; // order by read position
static auto grid = array<array<Pos, DIMENSION>, DIMENSION>();

auto getNeighbours(Coord const& current)
{
	auto getAdjPos = [&current](auto const& d) { return &grid[current.y + d.y][current.x + d.x]; };
	return directions | views::transform(getAdjPos);
}

void makePathMap(map<Coord, Coord>& pathMap, set<Coord> const& targets, Coord const& goal)
{
	auto nextStep = NOWHERE;
	auto frontier = queue<Coord>();
	for (auto const& target : targets)
	{
		frontier.push(target);
		pathMap[target] = NOWHERE;
	}

	while (!frontier.empty())
	{
		auto current = frontier.front();
		frontier.pop();
		for (auto const next : getNeighbours(current) | views::filter([&goal](auto const& p) { return p->coord == goal || !p->isBlocked(); }))
		{
			if (next->coord == goal)
			{
				pathMap[next->coord] = current;
				return;
			}
			else if (!pathMap.contains(next->coord))
			{
				frontier.push(next->coord);
				pathMap[next->coord] = current;
			}
		}
	}
}

Coord getNextStep(Pos* currentPos, set<Coord>& targets)
{
	map<Coord, Coord> targetsPathMap;
	makePathMap(targetsPathMap, targets, currentPos->coord);
	Coord bestTarget = NOWHERE;
	if (targetsPathMap.contains(currentPos->coord))
	{
		bestTarget = targetsPathMap[currentPos->coord];
		while (targetsPathMap[bestTarget] != NOWHERE)
		{
			bestTarget = targetsPathMap[bestTarget];
		}
	}
	else
	{
		return NOWHERE;
	}

	map<Coord, Coord> stepsPathMap;
	makePathMap(stepsPathMap, set<Coord> { currentPos->coord }, bestTarget);
	auto nextStep = bestTarget;
	while (stepsPathMap[nextStep] != currentPos->coord)
	{
		nextStep = stepsPathMap[nextStep];
	}

	return nextStep;
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

	auto unitId = 0;
	auto unitsPos = map<int, Pos*>();
	for (auto y = 0; y < DIMENSION; y++)
	{
		auto lineEnd = (y * (DIMENSION + 1)) + DIMENSION;
		for (auto x = 0; x < DIMENSION; x++)
		{
			grid[y][x].coord = { x, y };
			switch (input[(y * (DIMENSION + 1)) + x])
			{
				case 'E':
					grid[y][x].unit = make_shared<Unit>(Unit{ unitId, 200 });
					unitsPos[unitId] = &grid[y][x];
					unitId += 1;
					break;
				case 'G':
					grid[y][x].unit = make_shared<Unit>(Unit{ unitId, 200, true });
					unitsPos[unitId] = &grid[y][x];
					unitId += 1;
					break;
				case '#':
					grid[y][x].blocking = true;
					break;
			}
		}
	}
	
	auto round = 0;
	auto combat = true;
	while (combat)
	{
		auto currentUnitsPos = vector<Pos*>();
		currentUnitsPos.reserve(unitsPos.size());
		ranges::transform(unitsPos, back_inserter(currentUnitsPos), [](auto const& p) { return p.second; });
		sort(currentUnitsPos.begin(), currentUnitsPos.end(), [](auto const& a, auto const& b) { return a->coord < b->coord; });
		for (auto& currentPos : currentUnitsPos | views::filter([](auto const& p) { return p->unit != nullptr; }))
		{
			auto enemyTeam = !currentPos->unit->isGoblin;
			auto isEnemy = [enemyTeam](auto const& p) { return p->unit != nullptr && p->unit->isGoblin == enemyTeam; };
			auto adjEnemies = getNeighbours(currentPos->coord) | views::filter(isEnemy);
			if (ranges::empty(adjEnemies))
			{
				auto targets = set<Coord>();
				auto totalEnemies = 0;
				for (auto const enemyPos : unitsPos | views::transform([](auto const& p) { return p.second; }) | views::filter(isEnemy))
				{
					totalEnemies++;
					for (auto const target : getNeighbours(enemyPos->coord) | views::filter([](auto const p) { return !p->isBlocked(); }))
					{
						targets.insert(target->coord);
					}
				}

				if (totalEnemies != 0)
				{
					auto nextStep = getNextStep(currentPos, targets);
					if (nextStep != NOWHERE)
					{
						auto movePos = &grid[nextStep.y][nextStep.x];
						unitsPos[currentPos->unit->id] = movePos;
						movePos->unit = currentPos->unit;
						currentPos->unit = nullptr;
						adjEnemies = getNeighbours(movePos->coord) | views::filter(isEnemy);
					}
				}
				else
				{
					combat = false;
					break;
				}
			}

			if (!ranges::empty(adjEnemies))
			{
				auto attackPos = *adjEnemies.begin();
				for (auto const& enemyPos : adjEnemies | views::drop(1))
				{
					if (enemyPos->unit->hp < attackPos->unit->hp)
					{
						attackPos = enemyPos;
					}
				}

				attackPos->unit->hp -= 3;
				if (attackPos->unit->hp <= 0)
				{
					unitsPos.erase(attackPos->unit->id);
					attackPos->unit = nullptr;
				}
			}
		}

		if (combat)
		{
			round++;
		}
	}

	auto sumHp = 0;
	for (auto const& [coord, unitPos] : unitsPos)
	{
		sumHp += unitPos->unit->hp;
	}

	std::cout << round << " * " << sumHp << " = ";
	const auto answer = round * sumHp;

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	std::cout << std::format("{} ({})", answer, micros);

}