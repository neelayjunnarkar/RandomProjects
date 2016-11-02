#include <SFML\Graphics.hpp>
#include <SFML\Main.hpp>
#include <array>
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <chrono>
#include <random>

bool random_bool() {
	static std::default_random_engine generator(std::chrono::steady_clock::now().time_since_epoch().count());
	static std::uniform_int_distribution<int> distribution(0, 1);
	return (bool) distribution(generator);
}

const int N = 55;
const int CELL_X = 10;
const int CELL_Y = 10;
using Board = std::array<std::array<bool, N>, N>;

enum class State {
	PAUSED  = 0b00,
	RUNNING = 0b01
};

int wrap_x(int x) {
	return (N + x)%N;
}

int wrap_y(int y) {
	return (N + y) % N;
}

void draw_board(sf::RenderWindow &window, const Board &board) {
	for(int y = 0; y < N; ++y) {
		for(int x = 0; x < N; ++x) {
			sf::RectangleShape rect{{(float)CELL_X, (float)CELL_Y}};
			rect.setPosition((float)x*CELL_X, (float)y*CELL_Y);
			rect.setFillColor(board[x][y] ? sf::Color::White : sf::Color::Black);
			window.draw(rect);
		}
	}
}

bool rule_1(const Board &in_board, Board &out_board, int x, int y, int n_neighbors) {
	if(in_board[x][y] && n_neighbors < 2) {
		out_board[x][y] = false;
		return true;
	}
	return false;
}
bool rule_2(const Board &in_board, Board &out_board, int x, int y, int n_neighbors) {
	if(in_board[x][y] && (n_neighbors == 2 || n_neighbors == 3)) {
		out_board[x][y] = true;
		return true;
	}
	return false;
}
bool rule_3(const Board &in_board, Board &out_board, int x, int y, int n_neighbors) {
	if(in_board[x][y] && n_neighbors > 3) {
		out_board[x][y] = false;
		return true;
	}
	return false;
}
bool rule_4(const Board &in_board, Board &out_board, int x, int y, int n_neighbors) {
	if(!in_board[x][y] && n_neighbors == 3) {
		out_board[x][y] = true;
		return true;
	}
	return false;
}

int main() {

	sf::RenderWindow window(sf::VideoMode(CELL_X*N, CELL_Y*N), "Conways Game of Life");
	State state = State::PAUSED;
	
	std::vector<std::function<bool(const Board &, Board &, int, int, int)>> rules = {rule_1, rule_2, rule_3, rule_4};

	Board board;
	for(auto &arr : board) {
		arr.fill(false);
	}

	bool last_mouse_state = false;
	while(window.isOpen()) {
		sf::Event event;
		while(window.pollEvent(event)) {
			if(event.type == sf::Event::Closed)
				window.close();
			if(event.key.code == sf::Keyboard::S) {
				state = State::RUNNING;
			}
			if(event.key.code == sf::Keyboard::P) {
				state = State::PAUSED;
			}
			if(event.key.code == sf::Keyboard::R && state == State::PAUSED) {
				//randomize
				for(auto &arr : board) {
					std::generate(arr.begin(), arr.end(), random_bool);
				}
			}
			if(event.key.code == sf::Keyboard::C && state == State::PAUSED) {
				//clear
				for(auto &arr : board) {
					arr.fill(false);
				}
			}
		}
		
		//Take into account mouse input
		if(!last_mouse_state && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
			mouse_pos.x /= CELL_X;
			mouse_pos.y /= CELL_Y;
			board[wrap_x(mouse_pos.x)][wrap_y(mouse_pos.y)] = !board[wrap_x(mouse_pos.x)][wrap_y(mouse_pos.y)];
		}
		last_mouse_state = sf::Mouse::isButtonPressed(sf::Mouse::Left);


		if(state == State::RUNNING) {
			Board copy = board;
			for(int y = 0; y < N; ++y) {
				for(int x = 0; x < N; ++x) {

					int num_neighbors = 0;
					for(int dy = -1; dy <= 1; ++dy) {
						for(int dx = -1; dx <= 1; ++dx) {
							if(!(dy == 0 && dx == 0)) {
								if(copy[wrap_x(x + dx)][wrap_y(y + dy)])
									++num_neighbors;
							}
						}
					}

					for(auto rule : rules) {
						if(rule(copy, board, x, y, num_neighbors)) {
							break;
						}
					}
				}
			}
		}

		window.clear(sf::Color::Blue);
		draw_board(window, board);
		window.display();
	}
	
	return 0;
}