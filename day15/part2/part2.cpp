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
static auto originalUnitsPos = map<int, Pos*>();
static auto combatUnitsPos = map<int, Pos*>();
static auto combatUnits = vector<shared_ptr<Unit>>();
static auto originalGrid = array<array<Pos, DIMENSION>, DIMENSION>();
static auto combatGrid = array<array<Pos, DIMENSION>, DIMENSION>();
static auto neighbours = array<array<vector<Pos*>, DIMENSION>, DIMENSION>();

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
		for (auto const next : neighbours[current.y][current.x] | views::filter([&goal](auto const& p) { return p->coord == goal || !p->isBlocked(); }))
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
	auto readSream = ifstream("../input.txt");
	if (readSream.is_open())
	{
		string line;
		while (getline(readSream, line))
		{
			input += line;
			input.push_back('\n');
		}
	}

	const auto execStart = chrono::high_resolution_clock::now();

	/* begin solution */

	auto answer = 0;
	auto elfCount = 0;
	auto unitId = 0;
	for (auto y = 0; y < DIMENSION; y++)
	{
		auto lineEnd = (y * (DIMENSION + 1)) + DIMENSION;
		for (auto x = 0; x < DIMENSION; x++)
		{
			combatGrid[y][x].coord = { x, y };
			switch (input[(y * (DIMENSION + 1)) + x])
			{
			case 'E':
				combatUnits.push_back(make_shared<Unit>(Unit{ unitId, 200 }));
				combatGrid[y][x].unit = combatUnits[combatUnits.size() - 1];
				combatUnitsPos[unitId] = &combatGrid[y][x];
				elfCount += 1;
				unitId += 1;
				break;
			case 'G':
				combatUnits.push_back(make_shared<Unit>(Unit{ unitId, 200, true }));
				combatGrid[y][x].unit = combatUnits[combatUnits.size() - 1];
				combatUnitsPos[unitId] = &combatGrid[y][x];
				unitId += 1;
				break;
			case '#':
				combatGrid[y][x].blocking = true;
				break;
			}
		}
	}

	originalUnitsPos = combatUnitsPos;
	originalGrid = combatGrid;
	for (auto& row : combatGrid)
	{
		for (auto& current : row)
		{
			vector<Pos*> n;
			auto toCoord = [&current](auto const& c) { return Coord{ current.coord.x + c.x, current.coord.y + c.y }; };
			auto filterBounds = [](auto const& c) { return c.x > -1 && c.x < DIMENSION&& c.y > -1 && c.y < DIMENSION; };
			auto toPos = [&current](auto const& c) { return &combatGrid[c.y][c.x]; };
			for (auto const& next : directions | views::transform(toCoord) | views::filter(filterBounds) | views::transform(toPos))
			{
				n.push_back(next);
			}

			neighbours[current.coord.y][current.coord.x] = n;
		}
	}

	auto elfAttack = 4;
	auto elfSurvival = false;
	while (!elfSurvival)
	{
		auto round = 0;
		auto combat = true;
		while (combat)
		{
			auto currentUnitsPos = vector<Pos*>();
			currentUnitsPos.reserve(combatUnitsPos.size());
			ranges::transform(combatUnitsPos, back_inserter(currentUnitsPos), [](auto const& p) { return p.second; });
			sort(currentUnitsPos.begin(), currentUnitsPos.end(), [](auto const& a, auto const& b) { return a->coord < b->coord; });
			for (auto& currentPos : currentUnitsPos | views::filter([](auto const& p) { return p->unit != nullptr; }))
			{
				auto enemyTeam = !currentPos->unit->isGoblin;
				auto isEnemy = [enemyTeam](auto const& p) { return p->unit != nullptr && p->unit->isGoblin == enemyTeam; };
				auto adjEnemies = neighbours[currentPos->coord.y][currentPos->coord.x] | views::filter(isEnemy);
				if (ranges::empty(adjEnemies))
				{
					auto targets = set<Coord>();
					auto totalEnemies = 0;
					for (auto const enemyPos : combatUnitsPos | views::transform([](auto const& p) { return p.second; }) | views::filter(isEnemy))
					{
						totalEnemies++;
						for (auto const target : neighbours[enemyPos->coord.y][enemyPos->coord.x] | views::filter([](auto const p) { return !p->isBlocked(); }))
						{
							targets.insert(target->coord);
						}
					}

					if (totalEnemies != 0)
					{
						auto nextStep = getNextStep(currentPos, targets);
						if (nextStep != NOWHERE)
						{
							auto movePos = &combatGrid[nextStep.y][nextStep.x];
							combatUnitsPos[currentPos->unit->id] = movePos;
							movePos->unit = currentPos->unit;
							currentPos->unit = nullptr;
							adjEnemies = neighbours[movePos->coord.y][movePos->coord.x] | views::filter(isEnemy);
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

					attackPos->unit->hp -= enemyTeam == false ? 3 : elfAttack;
					if (attackPos->unit->hp <= 0)
					{
						if (!attackPos->unit->isGoblin)
						{
							combat = false;
							break;
						}

						combatUnitsPos.erase(attackPos->unit->id);
						attackPos->unit = nullptr;
					}
				}
			}

			if (combat)
			{
				round++;
			}
		}


		elfSurvival = combatUnitsPos.size() == elfCount && ranges::none_of(combatUnitsPos, [](auto const& p) { return p.second->unit->isGoblin; });
		if (elfSurvival)
		{
			auto sumHp = 0;
			for (auto const& [coord, unitPos] : combatUnitsPos)
			{
				sumHp += unitPos->unit->hp;
			}

			answer = round * sumHp;
		}
		else
		{
			elfAttack += 1;
			combatUnitsPos = originalUnitsPos;
			combatGrid = originalGrid;
			for (auto& unit : combatUnits)
			{
				unit->hp = 200;
			}
		}
	}

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	std::cout << std::format("{} ({})", answer, micros);
}