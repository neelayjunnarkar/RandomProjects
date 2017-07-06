#include <SFML\Graphics.hpp>
#include <array>
#include <stack>
#include <vector>
#include <iostream>
#include <chrono>
#include <random>
#include <functional>
#include <algorithm>
#include <thread>
#include <fstream>

using namespace std;


struct Cell {
	enum State {
		WALL  = 0b0001,
		SPACE = 0b0010,
		END   = 0b0100,
		START = 0b1000
	} state;
	int len_from_start;
};

constexpr int N = 150;
constexpr int SCREEN_SIDE = 720;
constexpr float CELL_SIDE = (float)(SCREEN_SIDE) / (float)(N);

using Grid = array<array<Cell, N>, N>;

struct Point {
	int x;
	int y;
};
Point operator + (const Point &lhs, const Point &rhs) {
	return Point{lhs.x + rhs.x, lhs.y + rhs.y};
}
bool operator == (const Point &lhs, const Point &rhs) {
	return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}
bool operator != (const Point &lhs, const Point &rhs) {
	return !(lhs == rhs);
}

Point random_beginning() {
	static default_random_engine generator(std::chrono::steady_clock::now().time_since_epoch().count());
	static uniform_int_distribution<int> distribution(0, N - 1);
	return Point{distribution(generator), distribution(generator)};
}

bool in_bounds(const Point &point) {
	return point.x >= 0 && point.x < N && point.y >= 0 && point.y < N;
}

Cell at(const Grid &grid, const Point &pos) {
	return grid[pos.x][pos.y];
}

int n_neighbors(const Grid &grid, const Point &pos) {
	int n = 0;
	if(in_bounds(pos + Point{0, -1}) && (at(grid, pos + Point{0, -1}).state & Cell::SPACE))
		++n;
	if(in_bounds(pos + Point{-1, 0}) && (at(grid, pos + Point{-1, 0}).state & Cell::SPACE))
		++n;
	if(in_bounds(pos + Point{0, 1}) && (at(grid, pos + Point{0, 1}).state & Cell::SPACE))
		++n;
	if(in_bounds(pos + Point{1, 0}) && (at(grid, pos + Point{1, 0}).state & Cell::SPACE))
		++n;
	return n;
}

Point next_pos(const Grid &grid, const Point &pos) {
	static default_random_engine generator(std::chrono::steady_clock::now().time_since_epoch().count());
	static array<Point, 4> potential_dpos = {Point{0, -1}, Point{-1, 0}, Point{0, 1}, Point{1, 0}};

	shuffle(potential_dpos.begin(), potential_dpos.end(), generator);

	for(const auto &dpos : potential_dpos) {
		if(in_bounds(pos + dpos) && (at(grid, pos+dpos).state & Cell::WALL) && n_neighbors(grid, pos + dpos) == 1)
			return pos + dpos;
	}
	return Point{-1, -1}; //error Point value
}

void draw(sf::RenderWindow &window, const Grid &grid) {
	for(int y = 0; y < N; ++y) {
		for(int x = 0; x < N; ++x) {
			sf::RectangleShape rect{{CELL_SIDE, CELL_SIDE}};
			rect.setPosition(CELL_SIDE*x, CELL_SIDE*y);
			if(at(grid, Point{x, y}).state & Cell::WALL) {
				rect.setFillColor(sf::Color::Black);
			}
			if(at(grid, Point{x, y}).state & Cell::SPACE) {
				rect.setFillColor(sf::Color::White);
			}
			if(at(grid, Point{x, y}).state & Cell::END) {
				rect.setFillColor(sf::Color{0xc4410dFF});
			}
			if(at(grid, Point{x, y}).state & Cell::START) {
				rect.setFillColor(sf::Color{0x006400FF});
			}
			window.draw(rect);
		}
	}
}

void export_maze(const Grid &grid, string file_name) {
	ofstream ofs(file_name);
	//print dimensions of grid
	ofs << N << " " << N << endl;
	
	//print state of grid
	for(int y = 0; y < N; ++y) {
		for(int x = 0; x < N; ++x) {
			if(at(grid, Point{x, y}).state & Cell::END) {
				ofs << (int)Cell::END << " ";
			} else if (at(grid, Point{x, y}).state & Cell::START) {
				ofs << (int)Cell::START << " ";
			} else {
				ofs << (int)at(grid, Point{x, y}).state << " ";
			}
		}
		ofs << endl;
	}
}

namespace test {
void print_state(const Grid &grid) {
	for(int y = 0; y < N; ++y) {
		for(int x = 0; x < N; ++x) {
			cout << (int)grid[x][y].state << " ";
		}
		cout << endl;
	}
}
void print_len(const Grid &grid) {
	for(int y = 0; y < N; ++y) {
		for(int x = 0; x < N; ++x) {
			cout << (int)grid[x][y].len_from_start << " ";
		}
		cout << endl;
	}
}
}

int main() {
	bool completed_generation{false};

	Grid grid;
	for(auto &arr : grid) {
		arr.fill({Cell::WALL, 0});
	}

	Point start = random_beginning();
	grid[start.x][start.y].state = (Cell::State)(Cell::START|Cell::SPACE);

	stack<Point> stack;
	stack.push(start);

	sf::RenderWindow window(sf::VideoMode(SCREEN_SIDE, SCREEN_SIDE), "Maze Generator");
	window.setFramerateLimit(120);
	while(window.isOpen()) {
		sf::Event event;
		while(window.pollEvent(event)) {
			if(event.type == sf::Event::Closed)
				window.close();
		}

		if(!completed_generation) {
			if(!stack.empty()) {
				Point next = next_pos(grid, stack.top());
				if(next == Point{-1, -1}) { //no valid next point -> need to backtrack
					stack.pop();
				} else {
					grid[next.x][next.y].state = Cell::SPACE;
					grid[next.x][next.y].len_from_start = at(grid, stack.top()).len_from_start + 1;
					stack.push(next);
				}
			} else {
				Point end{-1, -1};

				// Find cell which is the furthest distance from the start (distance, not displacement)
				// Not necesarily unique
				int curr_max_len_found = -1;
				for(int y = 0; y < N; ++y) {
					for(int x = 0; x < N; ++x) {
						if(at(grid, Point{x, y}).len_from_start > curr_max_len_found) {
							curr_max_len_found = at(grid, Point{x, y}).len_from_start;
							end = Point{x, y};
						}
					}
				}

				grid[end.x][end.y].state = (Cell::State)(Cell::END | Cell::SPACE);
				completed_generation = true;

				export_maze(grid, "maze3_150x150.csv");
			}
		}

		window.clear(sf::Color::Black);
		draw(window, grid);
		window.display();
	}

	return 0;
}