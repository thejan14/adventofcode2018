#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <regex>
#include <algorithm>

using namespace std;

const regex inputEx = regex("(\\d+) players; last marble is worth (\\d+) points\\n");

struct node
{
	unsigned long data;
	node* prev = nullptr;
	node* next = nullptr;

	node* insertAfter(node* n)
	{
		auto tmp = this->next;
		this->next = n;
		n->prev = this;
		n->next = tmp;
		tmp->prev = n;
		return n;
	}

	node* remove()
	{
		this->next->prev = this->prev;
		this->prev->next = this->next;
		return this->next;
	}
};

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

	auto answer = 0UL;
	smatch matches;
	if (regex_match(input, matches, inputEx))
	{
		auto const players = stoi(matches[1]);
		auto const lastMarble = stol(matches[2]) * 100UL;

		auto scores = vector<unsigned long>(players);
		auto currentPlayer = 0;
		auto currentMarble = new node{ 0 };
		currentMarble->prev = currentMarble;
		currentMarble->next = currentMarble;
		for (auto i = 1UL; i <= lastMarble; i++)
		{
			if (i % 23UL == 0)
			{
				scores[currentPlayer] += i;
				auto prev = currentMarble->prev;
				for (auto j = 0; j < 6; j++)
				{
					prev = prev->prev;
				}

				currentMarble = prev->remove();
				scores[currentPlayer] += prev->data;
				delete prev;
			}
			else
			{
				auto next = currentMarble->next;
				currentMarble = next->insertAfter(new node{ i });
			}

			currentPlayer = (currentPlayer + 1) % players;
		}

		while (currentMarble->next != currentMarble->next->next)
		{
			auto tmp = currentMarble->next;
			currentMarble = tmp->remove();
			delete tmp;
		}

		delete currentMarble;

		answer = *ranges::max_element(scores);
	}

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}