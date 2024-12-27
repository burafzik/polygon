#include <iostream>
#include <unordered_map>
#include <vector>
#include <stack>
#include <queue>
#include <cmath>
#include <limits>
#include <fstream>
#include <sstream>
#include <chrono>
#include <functional>
#include <algorithm>

using namespace std;

struct Node {
	double leng, widt;

	bool operator==(const Node& other) const {
		return leng == other.leng && widt == other.widt;
	}

	bool operator!=(const Node& other) const {
		return !(*this == other);
	}

	bool operator<(const Node& other) const {
		return tie(leng, widt) < tie(other.leng, other.widt);
	}
};

namespace std {
	template <>
	struct hash<Node> {
		size_t operator()(const Node& node) const {
			return hash<double>()(node.leng) ^ (hash<double>()(node.widt) << 1);
		}
	};
}

class Graph {
public:
	void parse_data(const string& data) {
		stringstream ss(data);
		string line;

		while (getline(ss, line)) {
			stringstream lineStream(line);
			double lon1, lat1, lon2, lat2, weight;
			char s;

			if (lineStream >> lon1 >> s >> lat1 && s == ',' && lineStream.get() == ':') {
				Node node1{ lon1, lat1 };
				while (lineStream >> lon2 >> s >> lat2 >> s >> weight) {
					Node node2{ lon2, lat2 };
					adjList[node1].emplace_back(node2, weight);
					adjList[node2].emplace_back(node1, weight);
					if (!(lineStream >> s) || s != ';') break;
				}
			}
		}
	}

	pair<double, vector<Node>> search(const Node& start, const Node& end,
		function<bool(priority_queue<pair<double, Node>, vector<pair<double, Node>>, greater<>>&, const Node&, double, double)> heuristic = nullptr) {

		using pq_type = priority_queue<pair<double, Node>, vector<pair<double, Node>>, greater<>>;
		pq_type pq;
		unordered_map<Node, double> distances;
		unordered_map<Node, Node> parent;
		unordered_map<Node, bool> visited;

		for (const auto& pair : adjList) distances[pair.first] = numeric_limits<double>::infinity();
		distances[start] = 0.0;
		pq.push({ 0.0, start });
		parent[start] = start;

		while (!pq.empty()) {
			double currentDistance = pq.top().first;
			Node current = pq.top().second;
			pq.pop();

			if (current == end) {
				vector<Node> path;
				for (Node at = end; at != start; at = parent[at]) path.push_back(at);
				path.push_back(start);
				reverse(path.begin(), path.end());
				return { currentDistance, path };
			}

			if (visited[current]) continue;
			visited[current] = true;

			for (const auto& neighbor_pair : adjList[current]) {
				const Node& neighbor = neighbor_pair.first;
				double weight = neighbor_pair.second;
				double newDist = currentDistance + weight;

				if (newDist < distances[neighbor]) {
					distances[neighbor] = newDist;
					parent[neighbor] = current;

					double priority = newDist;
					if (heuristic) priority += heuristic(pq, neighbor, newDist, currentDistance);

					pq.push({ priority, neighbor });
				}
			}
		}
		return { numeric_limits<double>::infinity(), {} };
	}

	pair<double, vector<Node>> bfs(const Node& start, const Node& end) {
		return search(start, end);
	}

	pair<double, vector<Node>> dfs(const Node& start, const Node& end) {
		stack<Node> s;
		unordered_map<Node, bool> visited;
		unordered_map<Node, Node> parent;
		unordered_map<Node, double> distances;

		for (const auto& pair : adjList) distances[pair.first] = numeric_limits<double>::infinity();
		distances[start] = 0.0;

		s.push(start);
		parent[start] = start;

		while (!s.empty()) {
			Node current = s.top();
			s.pop();

			if (current == end) {
				vector<Node> path;
				for (Node at = end; at != start; at = parent[at]) path.push_back(at);
				path.push_back(start);
				reverse(path.begin(), path.end());
				return { distances[end], path };
			}

			if (visited[current]) continue;
			visited[current] = true;

			for (const auto& neighbor_pair : adjList[current]) {
				const Node& neighbor = neighbor_pair.first;
				double weight = neighbor_pair.second;

				if (!visited[neighbor]) {
					distances[neighbor] = distances[current] + weight;
					parent[neighbor] = current;
					s.push(neighbor);
				}
			}
		}

		// Если путь не найден
		return { numeric_limits<double>::infinity(), {} };
	}


	pair<double, vector<Node>> dijkstra(const Node& start, const Node& end) {
		return search(start, end);
	}

	pair<double, vector<Node>> a_star(const Node& start, const Node& end) {
		auto heuristic = [&end](auto&, const Node& node, double, double) {
			return sqrt(pow(node.leng - end.leng, 2) + pow(node.widt - end.widt, 2));
		};
		return search(start, end, heuristic);
	}

private:
	unordered_map<Node, vector<pair<Node, double>>> adjList;
};

void test_algorithms(Graph& graph, const Node& start, const Node& end, double expectedDistance) {
	auto bfs_result = graph.bfs(start, end);
	cout << "BFS: Distance: " << bfs_result.first
		<< " (Expected: " << expectedDistance << ")\n";

	auto dfs_result = graph.dfs(start, end);
	cout << "DFS: Distance: " << dfs_result.first
		<< " (Expected: " << expectedDistance << ")\n";

	auto dijkstra_result = graph.dijkstra(start, end);
	cout << "Dijkstra: Distance: " << dijkstra_result.first
		<< " (Expected: " << expectedDistance << ")\n";

	auto a_star_result = graph.a_star(start, end);
	cout << "A*: Distance: " << a_star_result.first
		<< " (Expected: " << expectedDistance << ")\n";
}

int main_test() {
	Graph graph;

	ifstream inputFile("spb_graph.txt");
	if (!inputFile.is_open()) {
		cerr << "Не удалось открыть файл!" << endl;
		return 1;
	}
	cout << "File open!" << endl;

	stringstream buffer;
	buffer << inputFile.rdbuf();
	string data = buffer.str();
	graph.parse_data(data);

	Node start{ 30.216413, 59.9554621 };
	Node end{ 30.4680341, 59.8621847 };

	auto start_bfs = chrono::steady_clock::now();
	auto bfs_result = graph.bfs(start, end);
	auto end_bfs = chrono::steady_clock::now();
	chrono::duration<double> elapsed_bfs = end_bfs - start_bfs;
	cout << "Algorithm BFS:\n Distance: " << bfs_result.first << " Edges: " << bfs_result.first << " Time: "
		<< elapsed_bfs.count() << " ms" << endl;

	auto start_dfs = chrono::steady_clock::now();
	auto dfs_result = graph.dfs(start, end);
	auto end_dfs = chrono::steady_clock::now();
	chrono::duration<double> elapsed_dfs = end_dfs - start_dfs;
	cout << "Algorithm DFS:\n Distance: " << dfs_result.first << " Edges: " << dfs_result.first << " Time: "
		<< elapsed_dfs.count() << " ms" << endl;

	auto start_dijkstra = chrono::steady_clock::now();
	auto dijkstra_result = graph.dijkstra(start, end);
	auto end_dijkstra = chrono::steady_clock::now();
	chrono::duration<double> elapsed_dijkstra = end_dijkstra - start_dijkstra;
	cout << "Algorithm Dijkstra:\n Distance: " << dijkstra_result.first << " Time: "
		<< elapsed_dijkstra.count() << " ms" << endl;

	auto start_a_star = chrono::steady_clock::now();
	auto a_star_result = graph.a_star(start, end);
	auto end_a_star = chrono::steady_clock::now();
	chrono::duration<double> elapsed_a_star = end_a_star - start_a_star;
	cout << "Algorithm A*:\n Distance: " << a_star_result.first << " Time: "
		<< elapsed_a_star.count() << " ms" << endl;

	return 0;
}

int main() {
	Graph graph;

	string data = R"(
        0.0,0.0:1.0,1.0,1.41;2.0,0.0,2.0
        1.0,1.0:2.0,0.0,1.0;3.0,1.0,1.41
        3.0,1.0:0.0,0.0,3.0
    )";
	graph.parse_data(data);

	Node start{ 0.0, 0.0 };
	Node end{ 3.0, 1.0 };
	test_algorithms(graph, start, end, 3.0);

	data.clear();

	main_test();

	return 0;
}
