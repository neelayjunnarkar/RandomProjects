// An implementation of K-means Clustering algorithm
// author: Neelay Junnarkar

#include <SFML\Graphics.hpp>
#include <array>
#include <cmath>
#include <random>
#include <thread>
#include <map>
#include <algorithm>
#include <vector>
#include <functional>
#include <numeric>

int main() {

	using Point = std::array<int, 2>;

	const struct {
		float width;
		float height;
	} screen{640, 640};

	const int NUM_CLUSTERS = 6;
	const int NUM_POINTS = 300;

	std::array<Point, NUM_CLUSTERS> centroids;
	std::array<std::vector<Point>, NUM_CLUSTERS> clusters; // vector of points tied to each centroid, which is identified by index
	const std::array<sf::Color, NUM_CLUSTERS> centroid_colors{sf::Color::Blue, sf::Color::Red, sf::Color::Green, sf::Color::Yellow, sf::Color::Magenta, sf::Color::Cyan};
	
	std::array<Point, NUM_POINTS> points; // value is index of cluster centroid in other arrays
	
	std::random_device r;
	std::default_random_engine x_eng(r());
	std::default_random_engine y_eng(r());
	const std::uniform_int_distribution<int> x_dist(0, screen.width);
	const std::uniform_int_distribution<int> y_dist(0, screen.height);
	auto x_coord = std::bind(x_dist, x_eng);
	auto y_coord = std::bind(y_dist, y_eng);

	// Initialize centroids with random points
	for(int i = 0; i < NUM_CLUSTERS; ++i) {
		centroids[i] = {x_coord(), y_coord()};
	}

	auto dist_sqrd = [] (const Point &a, const Point &b) {
		return std::pow(b[1] - a[1], 2) + std::pow(b[0] - a[0], 2);
	};

	// Create random points, and map them to current closest centroid
	for(int i = 0; i < NUM_POINTS; ++i) {
		const Point p {x_coord(), y_coord()};
		std::array<int, NUM_CLUSTERS> distances_sqrd;
		std::transform(centroids.begin(), centroids.end(), distances_sqrd.begin(), [dist_sqrd, p] (const Point &centroid) { return dist_sqrd(p, centroid); });

		std::pair<int, int> min(distances_sqrd[0], 0); // <min value, index>
		for(int j = 1; j < NUM_CLUSTERS; ++j) {
			if(distances_sqrd[j] < min.first) {
				min.first = distances_sqrd[j];
				min.second = j;
			}
		}
		points[i] = p;
		clusters[min.second].push_back(p);
	}

	bool changed = true;
	sf::RenderWindow window(sf::VideoMode(screen.width, screen.height), "K-Means Clustering");
	window.setFramerateLimit(1);

	while(window.isOpen()) {
		sf::Event event;
		while(window.pollEvent(event)) {
			if(event.type == sf::Event::Closed)
				window.close();
		}

		// Update clusters if there may be changes left to do
		if(changed) {
			std::array<std::vector<Point>, NUM_CLUSTERS> old_clusters = clusters;

			std::transform(clusters.begin(), clusters.end(), centroids.begin(), [] (const std::vector<Point> &pts) {
				Point mean = std::accumulate(pts.begin(), pts.end(), Point{0, 0}, [] (Point acc, const Point &p) { return Point{acc[0] + p[0], acc[1] + p[1]}; });
				return Point{(int)std::round(mean[0] / pts.size()), (int)std::round(mean[1] / pts.size())};
			});

			// Clear clusters
			std::transform(clusters.begin(), clusters.end(), clusters.begin(), [] (const std::vector<Point> &pts) { return std::vector<Point>{}; });

			for(int i = 0; i < NUM_POINTS; ++i) {
				std::array<int, NUM_CLUSTERS> distances_sqrd;
				std::transform(centroids.begin(), centroids.end(), distances_sqrd.begin(), [dist_sqrd, points, i](const Point &centroid) { return dist_sqrd(points[i], centroid); });
				std::pair<int, int> min(distances_sqrd[0], 0);
				for(int j = 1; j < NUM_CLUSTERS; ++j) {
					if(distances_sqrd[j] < min.first) {
						min.first = distances_sqrd[j];
						min.second = j;
					}
				}
				clusters[min.second].push_back(points[i]);
			}

			if(old_clusters == clusters)
				changed = false;
		}

		window.clear(sf::Color::Black);

		for(int cluster_i = 0; cluster_i < NUM_CLUSTERS; ++cluster_i) {
			// Draw centroid
			sf::RectangleShape rect{{10, 10}};
			rect.setPosition(centroids[cluster_i][0], centroids[cluster_i][1]);
			rect.setFillColor(centroid_colors[cluster_i]);
			window.draw(rect);

			// Draw cluster points
			for(const Point &pt : clusters[cluster_i]) {
				sf::CircleShape circle(5);
				circle.setPosition(pt[0], pt[1]);
				circle.setFillColor(centroid_colors[cluster_i]);
				window.draw(circle);
			}
		}
		window.display();
	}

	return 0;
}