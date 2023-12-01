#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <regex>
#include <ranges>

using namespace std;


regex const sensorEx = regex(R"(pos=<(-?\d+),(-?\d+),(-?\d+)>, r=(\d+))", regex::optimize);

struct sensorData
{
	int64_t id, x, y, z, r;
};

inline int64_t distance(sensorData const& a, sensorData const& b)
{
	return abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z - b.z);
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

	auto sensors = vector<sensorData>();
	sensors.reserve(1000);
	smatch sensorMatch;
	auto sensorIt = input.cbegin();
	auto maxSensorIndex = 0LL;
	auto id = 0LL;
	while (regex_search(sensorIt, input.cend(), sensorMatch, sensorEx))
	{
		sensors.emplace_back(
			id,
			stoll(sensorMatch[1]),
			stoll(sensorMatch[2]),
			stoll(sensorMatch[3]),
			stoll(sensorMatch[4]));

		if (sensors[maxSensorIndex].r < sensors.back().r)
		{
			maxSensorIndex = id;
		}

		id += 1;
		sensorIt = sensorMatch.suffix().first;
	}

	auto answer = 0;
	auto const& maxSensor = sensors[maxSensorIndex];
	for (auto const& sensor : sensors)
	{
		if (distance(sensor, maxSensor) <= maxSensor.r)
		{
			++answer;
		}
	}

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}