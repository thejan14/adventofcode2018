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

static void runInstruction(array<int, 6>& reg, array<int, 4>& inst)
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
	auto instructions = vector<array<int, 4>>();
	smatch instructionMatch;
	auto instructionIt = input.cbegin();
	while (regex_search(instructionIt, input.cend(), instructionMatch, instructionEx))
	{
		array<int, 4> instruction;
		instruction[0] = opcodeMap[instructionMatch[1]];
		instruction[1] = stoi(instructionMatch[2]);
		instruction[2] = stoi(instructionMatch[3]);
		instruction[3] = stoi(instructionMatch[4]);
		instructions.push_back(instruction);
		instructionIt = instructionMatch.suffix().first;
	}

	auto reg = array<int, 6>{0, 0, 0, 0, 0, 0};
	auto ipValue = 0;
	while (ipValue < instructions.size())
	{
		reg[ip] = ipValue;
		runInstruction(reg, instructions[ipValue]);
		ipValue = reg[ip] + 1;
	}

	auto const answer = reg[0];

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}