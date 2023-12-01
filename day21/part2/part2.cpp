#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <regex>
#include <array>
#include <ranges>
#include <map>
#include <set>

using namespace std;

regex const instructionEx = regex("(\\w+) (\\d+) (\\d+) (\\d+)", regex::optimize);

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

map<std::string, Opcode> opcodeMap = map<std::string, Opcode>
{
	{ "addr", Opcode::addr },
	{ "addi", Opcode::addi },
	{ "mulr", Opcode::mulr },
	{ "muli", Opcode::muli },
	{ "banr", Opcode::banr },
	{ "bani", Opcode::bani },
	{ "borr", Opcode::borr },
	{ "bori", Opcode::bori },
	{ "setr", Opcode::setr },
	{ "seti", Opcode::seti },
	{ "gtir", Opcode::gtir },
	{ "gtri", Opcode::gtri },
	{ "gtrr", Opcode::gtrr },
	{ "eqir", Opcode::eqir },
	{ "eqri", Opcode::eqri },
	{ "eqrr", Opcode::eqrr },
};

inline void add(uint64_t a, uint64_t b, uint64_t& c)
{
	c = a + b;
}

inline void multiply(uint64_t a, uint64_t b, uint64_t& c)
{
	c = a * b;
}

inline void band(uint64_t a, uint64_t b, uint64_t& c)
{
	c = a & b;
}

inline void bor(uint64_t a, uint64_t b, uint64_t& c)
{
	c = a | b;
}

inline void assign(uint64_t a, uint64_t& c)
{
	c = a;
}

inline void gt(uint64_t a, uint64_t b, uint64_t& c)
{
	c = a > b ? 1 : 0;
}

inline void eq(uint64_t a, uint64_t b, uint64_t& c)
{
	c = a == b ? 1 : 0;
}

inline void runInstruction(array<uint64_t, 6>& reg, array<uint64_t, 4>& inst)
{
	switch (static_cast<Opcode>(inst[0]))
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

	auto ip = input[4] - '0';
	auto instructions = vector<array<uint64_t, 4>>();
	smatch instructionMatch;
	auto instructionIt = input.cbegin();
	while (regex_search(instructionIt, input.cend(), instructionMatch, instructionEx))
	{
		array<uint64_t, 4> instruction;
		instruction[0] = opcodeMap[instructionMatch[1]];
		instruction[1] = stoi(instructionMatch[2]);
		instruction[2] = stoi(instructionMatch[3]);
		instruction[3] = stoi(instructionMatch[4]);
		instructions.push_back(instruction);
		instructionIt = instructionMatch.suffix().first;
	}

	auto possibleValues = std::set<uint64_t>();
	auto lastValue = 0ULL;
	auto reg = array<uint64_t, 6>{0, 0, 0, 0, 0, 0};
	auto ipValue = 0ULL;
	while (ipValue < instructions.size())
	{
		reg[ip] = ipValue;
		runInstruction(reg, instructions[ipValue]);
		ipValue = reg[ip] + 1;

		if (ipValue == 28)
		{
			if (possibleValues.contains(reg[instructions[28][1]]))
			{
				break;
			}

			lastValue = reg[instructions[28][1]];
			possibleValues.insert(lastValue);
		}
	}

	// for reasoning please see part 1 comment
	auto const answer = lastValue;

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}