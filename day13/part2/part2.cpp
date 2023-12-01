#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <map>
#include <ranges>
#include <set>

using namespace std;

enum Direction
{
	North,
	East,
	South,
	West
};

pair<unsigned long, unsigned long> getCoordinates(unsigned long key)
{
	auto x = key & 0xFFFF;
	auto y = key >> 16;
	return { x, y };
}

struct Cart
{
	unsigned long location;
	Direction direction;
	int decisionState; // 0 = left, 1 = straigt, 2 = right
	bool crashed;

	bool operator<(const Cart& cart) const
	{
		auto [x1, y1] = getCoordinates(this->location);
		auto [x2, y2] = getCoordinates(cart.location);
		return y1 == y2
			? x1 < x2
			: y1 < y2;
	}
};

struct Location
{
	map<Direction, unsigned long> paths;
	set<shared_ptr<Cart>> carts;

	bool operator<(const Location& coord) const
	{
		return this->paths < coord.paths;
	}
};

inline int modulo(int a, int b)
{
	auto const result = a % b;
	return result >= 0 ? result : result + b;
}

unsigned long getLocationKey(unsigned long x, unsigned long y)
{
	return x ^ (y << 16);
}

Direction getOppositeDirection(Direction d)
{
	 return static_cast<Direction>(modulo(static_cast<int>(d) + 2, 4));
}

Direction getTurnDirection(Direction currentDirection, int& decisionState)
{
	auto newDir = static_cast<Direction>(modulo(static_cast<int>(currentDirection) + (decisionState - 1), 4));
	decisionState = (decisionState + 1) % 3;
	return newDir;
}

Direction getNextDirection(Location& location, Direction cartDirection)
{
	auto oppositeDirection = getOppositeDirection(cartDirection);
	for (auto const& [direction, _] : location.paths)
	{
		if (direction != oppositeDirection)
		{
			return direction;
		}
	}
}

map<unsigned long, Location> pathMap;

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

	auto carts = vector<shared_ptr<Cart>>();

	auto width = input.find_first_of('\n'); // omit trailing new lines
	auto height = input.size() / (width + 1);

	auto y = 0UL;
	string line;
	auto parseStream = stringstream(input);
	while (getline(parseStream, line))
	{
		auto x = 0UL;
		for (auto const& c : line)
		{
			auto key = getLocationKey(x, y);
			auto paths = map<Direction, unsigned long>();
			auto location = Location{};
			shared_ptr<Cart> cart;
			switch (c)
			{
			case '^':
				cart = shared_ptr<Cart>(new Cart{ key, Direction::North });
				carts.push_back(cart);
				location.carts.insert(cart);
				break;
			case '>':
				cart = shared_ptr<Cart>(new Cart{ key, Direction::East });
				carts.push_back(cart);
				location.carts.insert(cart);
				break;
			case 'v':
				cart = shared_ptr<Cart>(new Cart{ key, Direction::South });
				carts.push_back(cart);
				location.carts.insert(cart);
				break;
			case '<':
				cart = shared_ptr<Cart>(new Cart{ key, Direction::West });
				carts.push_back(cart);
				location.carts.insert(cart);
				break;
			}

			switch (c)
			{
			case '-':
			case '<':
			case '>':
				paths[Direction::East] = getLocationKey(x + 1, y);
				paths[Direction::West] = getLocationKey(x - 1, y);
				break;
			case '|':
			case '^':
			case 'v':
				paths[Direction::North] = getLocationKey(x, y - 1);
				paths[Direction::South] = getLocationKey(x, y + 1);
				break;
			case '/':
				if (x > 0 && "-+<>"s.find(line[x - 1]) != string::npos)
				{
					paths[Direction::North] = getLocationKey(x, y - 1);
					paths[Direction::West] = getLocationKey(x - 1, y);
				}
				else
				{
					paths[Direction::South] = getLocationKey(x, y + 1);
					paths[Direction::East] = getLocationKey(x + 1, y);
				}
				break;
			case '\\':
				if (x > 0 && "-+<>"s.find(line[x - 1]) != string::npos)
				{
					paths[Direction::South] = getLocationKey(x, y + 1);
					paths[Direction::West] = getLocationKey(x - 1, y);
				}
				else
				{
					paths[Direction::North] = getLocationKey(x, y - 1);
					paths[Direction::East] = getLocationKey(x + 1, y);
				}
				break;
			case '+':
				paths[Direction::North] = getLocationKey(x, y - 1);
				paths[Direction::East] = getLocationKey(x + 1, y);
				paths[Direction::South] = getLocationKey(x, y + 1);
				paths[Direction::West] = getLocationKey(x - 1, y);
				break;
			}

			if (paths.size() > 0)
			{
				location.paths = paths;
				pathMap[key] = location;
			}

			x++;
		}

		y++;
	}

	while (carts.size() > 1)
	{
		static auto const& sortFn = [](shared_ptr<Cart> const& a, shared_ptr<Cart> const& b) { return *a < *b; };
		sort(carts.begin(), carts.end(), sortFn);
		for (auto cart : carts | views::filter([](auto const& c) { return !c->crashed; }))
		{
			auto& location = pathMap[cart->location];
			auto newDirection = location.paths.size() > 2
				? getTurnDirection(cart->direction, cart->decisionState)
				: getNextDirection(location, cart->direction);

			auto newLocationKey = location.paths[newDirection];
			auto& newLocation = pathMap[newLocationKey];
			if (auto crashCart = ranges::find_if(newLocation.carts, [&cart](auto const& c) { return c->direction != cart->direction; });
				crashCart != newLocation.carts.end())
			{
				cart->crashed = true;
				(*crashCart)->crashed = true;
				location.carts.erase(location.carts.find(cart));
				newLocation.carts.erase(newLocation.carts.find(*crashCart));
			}
			else
			{
				cart->direction = newDirection;
				cart->location = newLocationKey;
				location.carts.erase(location.carts.find(cart));
				newLocation.carts.insert(cart);
			}
		}

		carts.erase(
			remove_if(carts.begin(), carts.end(), [](auto const& cart) { return cart->crashed; }),
			carts.end());
	}

	auto crashCoordinates = getCoordinates(carts[0]->location);
	const auto answer = std::format("{},{}", crashCoordinates.first, crashCoordinates.second);

	/* end solution */

	auto const execEnd = chrono::high_resolution_clock::now();
	auto const micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}