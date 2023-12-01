#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <regex>
#include <array>
#include <functional>
#include <ranges>
#include <map>
#include <set>

using namespace std;

const regex sampleEx = regex("Before: \\[(\\d), (\\d), (\\d), (\\d)\\]\\n(\\d+) (\\d+) (\\d+) (\\d+)\\nAfter:  \\[(\\d), (\\d), (\\d), (\\d)\\]\\n", regex::optimize);
const regex instructionEx = regex("(\\d+) (\\d+) (\\d+) (\\d+)", regex::optimize);

enum Opcode
{
	addr,
	addi,
	mulr,
	muli,
	banr,
	bani,
	borr,
	bori,
	setr,
	seti,
	gtir,
	gtri,
	gtrr,
	eqir,
	eqri,
	eqrr,
};

const set<Opcode> opcodes = {
	addr,
	addi,
	mulr,
	muli,
	banr,
	bani,
	borr,
	bori,
	setr,
	seti,
	gtir,
	gtri,
	gtrr,
	eqir,
	eqri,
	eqrr,
};

struct Sample
{
	array<int, 4> before;
	array<int, 4> instruction;
	array<int, 4> after;
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

static void assign(int a, int& c)
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

static void run(array<int, 4>& reg, array<int, 4>& inst, Opcode op)
{
	switch (op)
	{
		case Opcode::addr:
			add(reg[inst[1]], reg[inst[2]], reg[inst[3]]);
			break;
		case Opcode::addi:
			add(reg[inst[1]], inst[2], reg[inst[3]]);
			break;
		case Opcode::mulr:
			multiply(reg[inst[1]], reg[inst[2]], reg[inst[3]]);
			break;
		case Opcode::muli:
			multiply(reg[inst[1]], inst[2], reg[inst[3]]);
			break;
		case Opcode::banr:
			band(reg[inst[1]], reg[inst[2]], reg[inst[3]]);
			break;
		case Opcode::bani:
			band(reg[inst[1]], inst[2], reg[inst[3]]);
			break;
		case Opcode::borr:
			bor(reg[inst[1]], reg[inst[2]], reg[inst[3]]);
			break;
		case Opcode::bori:
			bor(reg[inst[1]], inst[2], reg[inst[3]]);
			break;
		case Opcode::setr:
			assign(reg[inst[1]], reg[inst[3]]);
			break;
		case Opcode::seti:
			assign(inst[1], reg[inst[3]]);
			break;
		case Opcode::gtir:
			gt(inst[1], reg[inst[2]], reg[inst[3]]);
			break;
		case Opcode::gtri:
			gt(reg[inst[1]], inst[2], reg[inst[3]]);
			break;
		case Opcode::gtrr:
			gt(reg[inst[1]], reg[inst[2]], reg[inst[3]]);
			break;
		case Opcode::eqir:
			eq(inst[1], reg[inst[2]], reg[inst[3]]);
			break;
		case Opcode::eqri:
			eq(reg[inst[1]], inst[2], reg[inst[3]]);
			break;
		case Opcode::eqrr:
			eq(reg[inst[1]], reg[inst[2]], reg[inst[3]]);
			break;
	}
}

static map<int, set<Opcode>> relationMap = {
	{ 0, opcodes },
	{ 1, opcodes },
	{ 2, opcodes },
	{ 3, opcodes },
	{ 4, opcodes },
	{ 5, opcodes },
	{ 6, opcodes },
	{ 7, opcodes },
	{ 8, opcodes },
	{ 9, opcodes },
	{ 10, opcodes },
	{ 11, opcodes },
	{ 12, opcodes },
	{ 13, opcodes },
	{ 14, opcodes },
	{ 15, opcodes },
};

static map<int, Opcode> opcodeMap;

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

	auto samples = vector<Sample>();
	smatch sampleMatch;
	auto sampleIt = input.cbegin();
	while (regex_search(sampleIt, input.cend(), sampleMatch, sampleEx))
	{
		auto sample = Sample{
			{stoi(sampleMatch[1]), stoi(sampleMatch[2]), stoi(sampleMatch[3]), stoi(sampleMatch[4])},
			{stoi(sampleMatch[5]), stoi(sampleMatch[6]), stoi(sampleMatch[7]), stoi(sampleMatch[8])},
			{stoi(sampleMatch[9]), stoi(sampleMatch[10]), stoi(sampleMatch[11]), stoi(sampleMatch[12])}
		};

		auto code = sample.instruction[0];
		for (auto it = relationMap[code].cbegin(); it != relationMap[code].cend(); /* no increment */)
		{
			auto newState = sample.before;
			run(newState, sample.instruction, *it);
			if (ranges::equal(newState, sample.after))
			{
				it++;
			}
			else
			{
				it = relationMap[code].erase(it);
			}
		}

		sampleIt = sampleMatch.suffix().first;
	}

	while (opcodeMap.size() != opcodes.size())
	{
		for (auto it = relationMap.cbegin(); it != relationMap.cend(); /* no increment */)
		{
			auto removed = false;
			auto& [code, ops] = *it;
			if (ops.size() == 1)
			{
				auto opcode = *ops.begin();
				opcodeMap[code] = opcode;
				it = relationMap.erase(it);
				removed = true;
				for (auto& [c, ops] : relationMap)
				{
					ops.erase(opcode);
				}
			}

			if (!removed)
			{
				it++;
			}
		}
	}

	auto reg = array<int, 4> { 0, 0, 0, 0 };
	smatch instructionMatch;
	auto instructionsIt = next(sampleIt, 3); // skip three newlines
	while (regex_search(instructionsIt, input.cend(), instructionMatch, instructionEx))
	{
		auto inst = array<int, 4> { stoi(instructionMatch[1]), stoi(instructionMatch[2]), stoi(instructionMatch[3]), stoi(instructionMatch[4]) };
		run(reg, inst, opcodeMap.at(inst[0]));
		instructionsIt = instructionMatch.suffix().first;
	}

	auto const answer = reg[0];

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}