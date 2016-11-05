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
#include <fstream>

using namespace std;

//const int N = 32;
//const int cell_side_len = 30;
constexpr int SCREEN_X = 720;// N*cell_side_len;
constexpr int SCREEN_Y = 720;// N*cell_side_len;

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



sf::Uint8 random_weight() {
	static std::default_random_engine generator(std::chrono::steady_clock::now().time_since_epoch().count());
	static std::uniform_int_distribution<int> distribution(1, 255);
	return (sf::Uint8)distribution(generator);
}

struct Node {
	enum State {
		INVALID   = 0b000001,
		UNVISITED = 0b000010,
		GOAL      = 0b000100,
		START     = 0b001000,
		PATH      = 0b010000,
		VISITED   = 0b100000
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
};
using Grid = vector<vector<Node>>;

Point random_inbounds_pos(const Grid &grid) {
	static std::default_random_engine generator(std::chrono::steady_clock::now().time_since_epoch().count());
	static std::uniform_int_distribution<int> distribution(1, grid.size() - 2);
	return Point{distribution(generator), distribution(generator)};
}

namespace test {
void print_grid(const Grid &grid) {
	for(int y = 0; y < grid.size(); ++y) {
		for(int x = 0; x < grid[0].size(); ++x) {
			cout << (int)grid[x][y].state << " ";
		}
		cout << endl;
	}
}
}

Grid gen_grid(int N) {
	Grid grid(N, vector<Node>(N));

	// Make edges invalid boxes
	for(int i = 1; i < N; ++i) {
		grid[i][0].state = Node::State::INVALID;
		grid[N - i - 1][N - 1].state = Node::State::INVALID;
		grid[0][N - i - 1].state = Node::State::INVALID;
		grid[N - 1][i].state = Node::State::INVALID;
	}

	// Make random invalid 
	for(int i = 0; i < (N - 2)*(N - 2) / 5; ++i) {
		Point pos = random_inbounds_pos(grid);
		grid[pos.x][pos.y].state = Node::INVALID;
	}

	// random weights
	for(int y = 1; y < N - 1; ++y) {
		for(int x = 1; x < N - 1; ++x) {
			grid[x][y].weight = random_weight();
		}
	}
	return grid;
}

pair<Point, Point> gen_start_end(Grid &grid) {
	Point start = random_inbounds_pos(grid);
	Point goal;
	do {
		goal = random_inbounds_pos(grid);
	} while(start.x == goal.x && start.y == goal.y);
	grid[start.x][start.y].state = (Node::State)(Node::VISITED | Node::State::START);
	grid[start.x][start.y].weight = 0;
	grid[goal.x][goal.y].state = (Node::State)(Node::UNVISITED | Node::GOAL);
	return make_pair(start, goal);
}

// returns tuple of Grid, Start Point, Goal Point
tuple<Grid, Point, Point> read_grid(string file_name)  {

	ifstream ifs(file_name);

	int X_MAX, Y_MAX;
	ifs >> X_MAX >> Y_MAX;

	if(X_MAX != Y_MAX)
		throw "Grid is not a square";

	Grid grid(X_MAX, vector<Node>(Y_MAX));
	Point start, goal;

	for(int y = 0; y < Y_MAX; ++y) {
		for(int x = 0; x < X_MAX; ++x) {
			int state;
			ifs >> state;
			grid[x][y].state = (Node::State)state;
			if(grid[x][y].state == Node::START) {
				start = Point{x, y};
				grid[x][y].state = (Node::State)(Node::START | Node::VISITED);
			} else if(grid[x][y].state == Node::GOAL) {
				goal = Point{x, y};
				grid[x][y].state = (Node::State)(Node::GOAL | Node::UNVISITED);
			}
		}
	}

	return make_tuple(grid, start, goal);
}

void draw_grid(sf::RenderWindow &window, const Grid &grid, float cell_side_len) {
	for(int y = 0; y < grid.size(); ++y) {
		for(int x = 0; x < grid[0].size(); ++x) {
			sf::RectangleShape rect{{(float)cell_side_len, (float)cell_side_len}};
			rect.setPosition((float)x*cell_side_len, (float)y*cell_side_len);
			sf::Uint8 val = (sf::Uint8) 255- (grid[x][y].weight*0.5 + 64);
			if(grid[x][y].state == Node::INVALID) {
				rect.setFillColor(sf::Color{0x13131BFF});
			}
			if(grid[x][y].state & Node::UNVISITED) {
				rect.setFillColor(sf::Color{val,(sf::Uint8)(val + 32),val, 255});
			}
			if(grid[x][y].state & Node::VISITED) {
				rect.setFillColor(sf::Color{(sf::Uint8)(val + 32), val, val, 255});
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

bool in_bounds(const Grid &grid, const Point &pos) {
	return (pos.x >= 0 && pos.x < grid.size() && pos.y >= 0 && pos.y < grid.size());
}

vector<Point> new_dijkstra_neighbors(const Point &point, int curr_path_cost, const Grid &grid) {
	vector<Point> neighbors = {
		{point.x, point.y - 1},
		{point.x - 1, point.y},
		{point.x, point.y + 1},
		{point.x + 1, point.y}
	};
	vector<Point> final_neighbors = {};

	for(int i = 0; i < neighbors.size(); ++i) {
		if(in_bounds(grid, neighbors[i]) && (grid[neighbors[i].x][neighbors[i].y].state & Node::State::UNVISITED || curr_path_cost + grid[neighbors[i].x][neighbors[i].y].weight < grid[neighbors[i].x][neighbors[i].y].len_to)) {
			final_neighbors.push_back(neighbors[i]);
		}
	}
	return final_neighbors;
}

bool operator < (pair<Point, double> a, pair<Point, double> b) {
	return a.second > b.second;
}

int main() {
	Grid grid;
	Point start, goal;
	tie(grid, start, goal) = read_grid("maze2_51x51.csv");

	/*Grid grid = gen_grid(50);
	Point start, goal;
	tie(start, goal) = gen_start_end(grid);
*/
	int SIDE_LEN = grid.size();
	float cell_side_len = (float)(SCREEN_X) / SIDE_LEN;

	priority_queue<pair<Point, double>> frontier; //Point, priority
	frontier.push({start, 0});

	bool has_found_goal = false;
	sf::RenderWindow window(sf::VideoMode(SCREEN_X, SCREEN_Y), "Pathfinder");

	cout << "Beginning search." << endl;
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
				if(curr.x == goal.x && curr.y == goal.y) { // If has found goal
					has_found_goal = true;
					cout << "Goal Found." << endl;
					for(Point p = goal; p.x != -1 && p.y != -1; p = grid[p.x][p.y].came_from) {
						grid[p.x][p.y].state = (Node::State)((int)grid[p.x][p.y].state | (int)Node::PATH);
					}
					grid[start.x][start.y].state = (Node::State)(Node::PATH | Node::START);
					grid[goal.x][goal.y].state = (Node::State)(Node::PATH | Node::GOAL);
					cout << "Path Generated." << endl;
					cout << "Complete." << endl;
				}
				frontier.pop();
				for(const Point &point : new_dijkstra_neighbors(curr, curr_len, grid)) {
					grid[point.x][point.y].state = Node::VISITED;
					grid[point.x][point.y].came_from = curr;
					grid[point.x][point.y].len_to = curr_len + grid[point.x][point.y].weight;
					frontier.push({point, grid[point.x][point.y].len_to + manhattan_dist(point, goal)});
				}
			}
		}

		window.clear(sf::Color::Black);
		draw_grid(window, grid, cell_side_len);
		window.display();
	}

	return 0;
}