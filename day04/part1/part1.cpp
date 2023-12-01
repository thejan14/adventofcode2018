#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <regex>
#include <map>
#include <vector>
#include <numeric>

using namespace std;

enum Info
{
	begin,
	sleep,
	wake,
};

struct Record
{
	int id, date, minute;
	Info info;
};

const regex recordEx = regex("\\[\\d+-(\\d+)-(\\d+) (\\d+):(\\d+)\\] (Guard #(\\d+) begins shift|falls asleep|wakes up)");

bool tryParseRecord(string& recordLine, Record& record)
{
	smatch matches;
	if (regex_match(recordLine, matches, recordEx))
	{
		auto monthStr = matches[1].str();
		auto dayStr = matches[2].str();
		auto hour = stoi(matches[3]);
		auto minute = stoi(matches[4]);

		Info info;
		auto infoText = matches[5].str();
		auto id = -1;
		if (infoText.rfind("Guard", 0) == 0)
		{
			info = Info::begin;
			id = stoi(matches[6]);
		}
		else if (infoText.rfind("falls", 0) == 0)
		{
			info = Info::sleep;
		}
		else
		{
			info = Info::wake;
		}
		
		record.id = id;
		record.date = stoi(monthStr + dayStr);
		record.minute = minute;
		record.info = info;

		// guard started before midnight
		if (hour != 0)
		{
			record.date += 1;
			record.minute = 0;
		}

		return true;
	}
	else
	{
		return false;
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

	vector<Record> records;
	string line;
	auto parseStream = stringstream(input);
	while (getline(parseStream, line))
	{
		Record record;
		if (tryParseRecord(line, record))
		{
			records.push_back(record);
		}
	}

	sort(records.begin(), records.end(), [](Record r1, Record r2)
		{
			auto const timeKey1 = r1.date * 100 + r1.minute;
			auto const timeKey2 = r2.date * 100 + r2.minute;
			return timeKey1 < timeKey2;
		});

	map<int, vector<int>> guardSchedules;
	auto currentGuard = 0;
	auto asleepSince = 0;
	for (auto const& record : records)
	{
		switch (record.info)
		{
			case Info::begin:
				currentGuard = record.id;
				if (!guardSchedules.contains(currentGuard))
				{
					guardSchedules.insert({ currentGuard, vector<int>(60, 0) });
				}
				break;
			case Info::sleep:
				asleepSince = record.minute;
				break;
			case Info::wake:
				for (auto i = asleepSince; i < record.minute; i++)
				{
					guardSchedules[currentGuard][i] += 1;
				}
		}
	}

	struct { int id; int sleep; } maxSleepGuard = { -1, -1 };
	for (auto const& record : records)
	{
		auto const sumSleep = reduce(guardSchedules[record.id].begin(), guardSchedules[record.id].end());
		if (sumSleep > maxSleepGuard.sleep)
		{
			maxSleepGuard = { record.id, sumSleep };
		}
	}

	auto const maxSleepMinute = max_element(guardSchedules[maxSleepGuard.id].begin(), guardSchedules[maxSleepGuard.id].end()) - guardSchedules[maxSleepGuard.id].begin();
	const auto answer = maxSleepGuard.id * maxSleepMinute;

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}