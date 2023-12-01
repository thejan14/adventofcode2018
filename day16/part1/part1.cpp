#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <regex>
#include <array>
#include <functional>
#include <ranges>

using namespace std;

const regex stateEx = regex("Before: \\[(\\d), (\\d), (\\d), (\\d)\\]\\n(\\d+) (\\d+) (\\d+) (\\d+)\\nAfter:  \\[(\\d), (\\d), (\\d), (\\d)\\]\\n", regex::optimize);

struct Sample
{
	array<int, 4> before;
	array<int, 4> instruction;
	array<int, 4> after;
	int getA(bool immediate)
	{
		return immediate
			? this->instruction[1]
			: this->before[this->instruction[1]];
	}

	int getB(bool immediate)
	{
		return immediate
			? this->instruction[2]
			: this->before[this->instruction[2]];
	}
};

static void add(int a, int b, int& c)
{
	c = a + b;
}

static void multiply(int a, int b, int& c)
{
	c = a * b;
}

static void band(int a, int b, int& c)
{
	c = a & b;
}

static void bor(int a, int b, int& c)
{
	c = a | b;
}

static void assign(int a, int b, int& c)
{
	c = a;
}

static void gt(int a, int b, int& c)
{
	c = a > b ? 1 : 0;
}

static void eq(int a, int b, int& c)
{
	c = a == b ? 1 : 0;
}

static bool check_sample(Sample& sample, function<void(int, int, int&)> op, bool immediateA, bool immediateB)
{
	if (!immediateA && sample.instruction[1] > 3)
	{
		return false;
	}

	if (!immediateB && sample.instruction[2] > 3)
	{
		return false;
	}

	auto newState = sample.before;
	op(sample.getA(immediateA), sample.getB(immediateB), newState[sample.instruction[3]]);
	return ranges::equal(sample.after, newState);
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

	auto answer = 0;
	smatch match;
	auto it = input.cbegin();
	while (regex_search(it, input.cend(), match, stateEx))
	{
		auto sample = Sample{
			{stoi(match[1]), stoi(match[2]), stoi(match[3]), stoi(match[4])},
			{stoi(match[5]), stoi(match[6]), stoi(match[7]), stoi(match[8])},
			{stoi(match[9]), stoi(match[10]), stoi(match[11]), stoi(match[12])}
		};

		auto possibleOpcodes = 0;
		possibleOpcodes += check_sample(sample, add, false, true);			// addi
		possibleOpcodes += check_sample(sample, add, false, false);			// addr
		possibleOpcodes += check_sample(sample, multiply, false, true);		// muli
		possibleOpcodes += check_sample(sample, multiply, false, false);	// mulr
		possibleOpcodes += check_sample(sample, band, false, true);			// bani
		possibleOpcodes += check_sample(sample, band, false, false);		// banr
		possibleOpcodes += check_sample(sample, bor, false, true);			// bori
		possibleOpcodes += check_sample(sample, bor, false, false);			// borr
		possibleOpcodes += check_sample(sample, assign, true, false);		// seti
		possibleOpcodes += check_sample(sample, assign, false, false);		// setr
		possibleOpcodes += check_sample(sample, gt, true, false);			// gtir
		possibleOpcodes += check_sample(sample, gt, false, true);			// gtri
		possibleOpcodes += check_sample(sample, gt, false, false);			// gtrr
		possibleOpcodes += check_sample(sample, eq, true, false);			// eqir
		possibleOpcodes += check_sample(sample, eq, false, true);			// eqri
		possibleOpcodes += check_sample(sample, eq, false, false);			// eqrr

		if (possibleOpcodes > 2)
		{
			answer += 1;
		}

		it = match.suffix().first;
	}

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}