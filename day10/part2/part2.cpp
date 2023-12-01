#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <regex>
#include <ranges>

using namespace std;

const regex dataEx = regex("position=< *?(-?\\d+), *?(-?\\d+)> velocity=< *?(-?\\d+), *?(-?\\d+)>");

// point of light
struct pol
{
	int px, py, vx, vy;
};

struct bounds
{
	int n, e, s, w;
};

bounds getBounds(vector<pol> const& pols)
{
	return bounds{
		ranges::min(pols | views::transform([](pol p) { return p.py; })),
		ranges::max(pols | views::transform([](pol p) { return p.px; })),
		ranges::max(pols | views::transform([](pol p) { return p.py; })),
		ranges::min(pols | views::transform([](pol p) { return p.px; }))
	};
}

int getArea(bounds& b)
{
	return abs(b.w - b.e) + abs(b.n - b.s);
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

	auto pols = vector<pol>();
	smatch match;
	auto it = input.cbegin();
	while (regex_search(it, input.cend(), match, dataEx))
	{
		pols.push_back({
				stoi(match[1]),
				stoi(match[2]),
				stoi(match[3]),
				stoi(match[4]),
			});
		it = match.suffix().first;
	}

	auto bounds = getBounds(pols);
	auto area = getArea(bounds);
	auto shrink = true;
	auto seconds = 0;
	while (shrink)
	{
		auto newPols = vector<pol>();
		newPols.reserve(pols.size());
		for (auto& pol : pols)
		{
			newPols.push_back({ pol.px + pol.vx, pol.py + pol.vy, pol.vx, pol.vy });
		}

		auto newBounds = getBounds(newPols);
		auto newArea = getArea(newBounds);
		shrink = area > newArea;
		if (shrink)
		{
			bounds = newBounds;
			area = newArea;
			pols.swap(newPols);
			seconds += 1;
		}
	}

	auto const answer = seconds;

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}