#include <SFML\Graphics.hpp>
#include <algorithm>
#include <functional>
#include <iostream>
#include <array>
#include <vector>
#include <utility>
#include <random>
#include <chrono>
#include <queue>
#include <thread>

using namespace std;

const int N = 32;
const int cell_side_len = 30;
const int SCREEN_X = N*cell_side_len;
const int SCREEN_Y = N*cell_side_len;

struct Point {
	int x;
	int y;
};
int manhattan_dist(const Point &a, const Point &b) {
	return abs(b.x - a.x) + abs(b.y - a.y);
}
double line_dist(const Point &a, const Point &b) {
	return sqrt((double)pow(b.x - a.x, 2) + (double)pow(b.y - a.y, 2));
}

Point random_inbounds_pos() {
	static std::default_random_engine generator(std::chrono::steady_clock::now().time_since_epoch().count());
	static std::uniform_int_distribution<int> distribution(1, N - 2);
	return Point{distribution(generator), distribution(generator)};
}

sf::Uint8 random_weight() {
	static std::default_random_engine generator(std::chrono::steady_clock::now().time_since_epoch().count());
	static std::uniform_int_distribution<int> distribution(1, 255);
	return (sf::Uint8)distribution(generator);
}

struct Node {
	enum State {
		INVALID = 0b00000,
		UNVISITED = 0b00001,
		VISITED = 0b00010,
		START = 0b00100,
		GOAL = 0b01000,
		PATH = 0b10000
	} state;
	Point came_from;
	sf::Uint8 weight;
	int len_to;
	Node() :
		state{State::UNVISITED},
		came_from{-1, -1},
		weight{1},
		len_to{-1} {
	}
	Node(State _state, Point _came_from, sf::Uint8 _weight, int _len_to) :
		state{_state},
		came_from{_came_from.x, _came_from.y},
		weight{_weight},
		len_to{_len_to} {
	}
};
using Grid = array<array<Node, N>, N>;

namespace test {
void print_grid(const Grid &grid) {
	for(int y = 0; y < N; ++y) {
		for(int x = 0; x < N; ++x) {
			cout << (int)grid[x][y].state << " ";
		}
		cout << endl;
	}
}
}

void draw_grid(sf::RenderWindow &window, const Grid &grid) {
	for(int y = 0; y < N; ++y) {
		for(int x = 0; x < N; ++x) {
			sf::RectangleShape rect{{(float)cell_side_len, (float)cell_side_len}};
			rect.setPosition((float)x*cell_side_len, (float)y*cell_side_len);
			sf::Uint8 val = (sf::Uint8) 255- (grid[x][y].weight*0.5 + 64);
			if(grid[x][y].state == Node::INVALID) {
				rect.setFillColor(sf::Color{0x13131BFF});
			}
			if(grid[x][y].state & Node::UNVISITED) {
				rect.setFillColor(sf::Color{val,(sf::Uint8)(val + 16),val, 255});
			}
			if(grid[x][y].state & Node::VISITED) {
				rect.setFillColor(sf::Color{(sf::Uint8)(val + 16), val, val, 255});
			}
			if(grid[x][y].state & Node::START) {
				rect.setFillColor(sf::Color{0x006400FF});
			}
			if(grid[x][y].state & Node::GOAL) {
				rect.setFillColor(sf::Color{0xc4410dFF});
			}
			window.draw(rect);
			if(grid[x][y].state & Node::PATH) {
				sf::RectangleShape path_rect{{(float)(cell_side_len / 2.0), (float)(cell_side_len / 2.0)}};
				path_rect.setPosition((float)x*cell_side_len + (float)(cell_side_len / 4.0), (float)y*cell_side_len + (float)(cell_side_len / 4.0));
				path_rect.setFillColor(sf::Color(0x003445FF));
				window.draw(path_rect);
			}

		}
	}
}

vector<Point> new_neighbors(const Point &point, const Grid &grid) {
	vector<Point> neighbors = {};
	if(grid[point.x][point.y - 1].state & Node::State::UNVISITED) {
		neighbors.push_back({point.x, point.y - 1});
	}
	if(grid[point.x - 1][point.y].state & Node::State::UNVISITED) {
		neighbors.push_back({point.x - 1, point.y});
	}
	if(grid[point.x][point.y + 1].state & Node::State::UNVISITED) {
		neighbors.push_back({point.x, point.y + 1});
	}
	if(grid[point.x + 1][point.y].state & Node::State::UNVISITED) {
		neighbors.push_back({point.x + 1, point.y});
	}
	return neighbors;
}

vector<Point> new_dijkstra_neighbors(const Point &point, int curr_path_cost, const Grid &grid) {
	vector<Point> neighbors = {};
	if(grid[point.x][point.y - 1].state & Node::State::UNVISITED || curr_path_cost + grid[point.x][point.y - 1].weight < grid[point.x][point.y - 1].len_to) {
		neighbors.push_back({point.x, point.y - 1});
	}
	if(grid[point.x - 1][point.y].state & Node::State::UNVISITED || curr_path_cost + grid[point.x][point.y - 1].weight < grid[point.x][point.y - 1].len_to) {
		neighbors.push_back({point.x - 1, point.y});
	}
	if(grid[point.x][point.y + 1].state & Node::State::UNVISITED || curr_path_cost + grid[point.x][point.y - 1].weight < grid[point.x][point.y - 1].len_to) {
		neighbors.push_back({point.x, point.y + 1});
	}
	if(grid[point.x + 1][point.y].state & Node::State::UNVISITED || curr_path_cost + grid[point.x][point.y - 1].weight < grid[point.x][point.y - 1].len_to) {
		neighbors.push_back({point.x + 1, point.y});
	}
	return neighbors;
}
bool operator < (pair<Point, double> a, pair<Point, double> b) {
	return a.second > b.second;
}

int main() {
	Grid grid;

	// Make edges invalid boxes
	for(int i = 1; i < N; ++i) {
		grid[i][0].state = Node::State::INVALID;
		grid[N - i - 1][N - 1].state = Node::State::INVALID;
		grid[0][N - i - 1].state = Node::State::INVALID;
		grid[N - 1][i].state = Node::State::INVALID;
	}

	// Make random invalid 
	for(int i = 0; i < (N - 2)*(N - 2) / 5; ++i) {
		Point pos = random_inbounds_pos();
		grid[pos.x][pos.y].state = Node::INVALID;
	}

	// random weights
	for(int y = 1; y < N - 1; ++y) {
		for(int x = 1; x < N - 1; ++x) {
			grid[x][y].weight = random_weight();
		}
	}

	Point start = random_inbounds_pos();
	Point goal;
	do {
		goal = random_inbounds_pos();
	} while(start.x == goal.x && start.y == goal.y);
	grid[start.x][start.y].state = (Node::State)(Node::VISITED | Node::State::START);
	grid[start.x][start.y].weight = 0;
	grid[goal.x][goal.y].state = (Node::State)(Node::UNVISITED | Node::GOAL);


	priority_queue<pair<Point, double>> frontier; //Point, priority
	frontier.push({start, 0});

	bool has_found_goal = false;
	sf::RenderWindow window(sf::VideoMode(SCREEN_X, SCREEN_Y), "Pathfinder");
	window.setFramerateLimit(60);
	// Main loop
	while(window.isOpen()) {
		sf::Event event;
		while(window.pollEvent(event)) {
			if(event.type == sf::Event::Closed)
				window.close();
		}
		if(!frontier.empty()) {
			if(!has_found_goal) {
				Point curr = frontier.top().first;
				int curr_len = frontier.top().second;
				if(curr.x == goal.x && curr.y == goal.y) {
					has_found_goal = true;
					for(Point p = goal; p.x != -1 && p.y != -1; p = grid[p.x][p.y].came_from) {
						grid[p.x][p.y].state = (Node::State)((int)grid[p.x][p.y].state | (int)Node::PATH);
					}
					grid[start.x][start.y].state = (Node::State)(Node::PATH | Node::START);
					grid[goal.x][goal.y].state = (Node::State)(Node::PATH | Node::GOAL);
				}
				frontier.pop();
				for(const Point &point : new_dijkstra_neighbors(curr, curr_len, grid)) {
					grid[point.x][point.y].state = Node::VISITED;
					grid[point.x][point.y].came_from = curr;
					grid[point.x][point.y].len_to = curr_len + grid[point.x][point.y].weight;
					frontier.push({point, grid[point.x][point.y].len_to + line_dist(point, goal)});
				}
			}
		}

		window.clear(sf::Color::Black);
		draw_grid(window, grid);
		window.display();
	}

	return 0;
}