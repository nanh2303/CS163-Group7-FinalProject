#include "graph/graphCore.h"

#include <algorithm>
#include <fstream>
#include <queue>
#include <random>
#include <sstream>
#include <tuple>

namespace {

static void trimInPlace(std::string& s) {
	while (!s.empty() && (s.back() == '\r' || s.back() == '\n' || s.back() == ' ' || s.back() == '\t')) {
		s.pop_back();
	}
	size_t start = 0;
	while (start < s.size() && (s[start] == ' ' || s[start] == '\t')) {
		++start;
	}
	s = s.substr(start);
}

} // namespace

Graph::Graph() : n(0) {}

void Graph::clear() {
	n = 0;
	adj.clear();
	animationFrames.clear();
	currentPseudoCode.clear();
}

void Graph::recordSnapshotState(
	int lineOfCode,
	const std::string& message,
	int highlightVertex,
	int highlightEdgeFrom,
	int highlightEdgeTo,
	const std::vector<int>* dist,
	const std::vector<bool>* visited,
	int sourceVertex,
	int searchHitVertex
) {
	GraphAnimationState frame;
	frame.activeLineOfCode = lineOfCode;
	frame.explanation = message;

	for (int i = 0; i < n; ++i) {
		GraphVertexState vs;
		vs.id = i;
		vs.dist = (dist && static_cast<int>(dist->size()) > i) ? (*dist)[i] : GRAPH_INF;
		vs.visited = (visited && static_cast<int>(visited->size()) > i) ? (*visited)[i] : false;
		vs.highlighted = (i == highlightVertex);
		vs.isSource = (i == sourceVertex);
		vs.isSearchHit = (i == searchHitVertex);
		frame.vertices.push_back(vs);
	}

	for (int u = 0; u < n; ++u) {
		for (const auto& edge : adj[u]) {
			int v = edge.first;
			int w = edge.second;
			GraphEdgeSnapshot es;
			es.u = u;
			es.v = v;
			es.weight = w;
			es.highlight = (u == highlightEdgeFrom && v == highlightEdgeTo);
			frame.edges.push_back(es);
		}
	}

	animationFrames.push_back(frame);
}

void Graph::initEmpty() {
	clear();
}

void Graph::initRandom(int vertexCount, int edgeCount, int weightMin, int weightMax) {
	clear();
	if (vertexCount <= 0) {
		return;
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> vdist(0, vertexCount - 1);
	std::uniform_int_distribution<int> wdist(weightMin, weightMax);

	n = vertexCount;
	adj.assign(n, {});

	if (edgeCount <= 0) {
		recordSnapshotState(-1, "Random graph: vertices only (no edges).");
		return;
	}

	int maxAttempts = edgeCount * 50;
	int added = 0;
	while (added < edgeCount && maxAttempts-- > 0) {
		int u = vdist(gen);
		int v = vdist(gen);
		if (u == v) {
			continue;
		}

		bool exists = false;
		for (const auto& p : adj[u]) {
			if (p.first == v) {
				exists = true;
				break;
			}
		}
		if (exists) {
			continue;
		}

		adj[u].push_back({ v, wdist(gen) });
		++added;
	}

	recordSnapshotState(-1, "Random graph initialized.");
}

void Graph::initFromFile(const std::string& filepath) {
	clear();
	std::ifstream fin(filepath);
	if (!fin) {
		return;
	}

	std::vector<std::tuple<int, int, int>> edges;
	int maxId = -1;
	std::string line;
	while (std::getline(fin, line)) {
		trimInPlace(line);
		if (line.empty() || line[0] == '#') {
			continue;
		}
		std::stringstream ss(line);
		int a = 0;
		int b = 0;
		int c = 0;
		if (!(ss >> a >> b >> c)) {
			continue;
		}
		if (a < 0 || b < 0 || c < 0) {
			continue;
		}
		edges.push_back({ a, b, c });
		maxId = std::max(maxId, std::max(a, b));
	}

	if (edges.empty() || maxId < 0) {
		return;
	}

	n = maxId + 1;
	adj.assign(n, {});
	for (const auto& e : edges) {
		int u = std::get<0>(e);
		int v = std::get<1>(e);
		int w = std::get<2>(e);
		if (u >= n || v >= n || u == v) {
			continue;
		}
		bool exists = false;
		for (const auto& p : adj[u]) {
			if (p.first == v) {
				exists = true;
				break;
			}
		}
		if (!exists) {
			adj[u].push_back({ v, w });
		}
	}

	recordSnapshotState(-1, "Loaded graph from file.");
}

int Graph::addVertex() {
	animationFrames.clear();
	currentPseudoCode.clear();

	adj.push_back({});
	++n;
	recordSnapshotState(-1, std::string("Added vertex ") + std::to_string(n - 1) + ".", n - 1);
	return n - 1;
}

void Graph::addEdge(int u, int v, int weight) {
	animationFrames.clear();
	currentPseudoCode.clear();

	if (u < 0 || v < 0 || u >= n || v >= n || u == v || weight < 0) {
		recordSnapshotState(-1, "Invalid edge (check vertices and non-negative weight).");
		return;
	}

	for (auto& p : adj[u]) {
		if (p.first == v) {
			p.second = weight;
			recordSnapshotState(-1, "Edge updated (same direction).", -1, u, v);
			return;
		}
	}

	adj[u].push_back({ v, weight });
	recordSnapshotState(-1, "Edge added.", -1, u, v);
}

void Graph::removeVertex(int v) {
	animationFrames.clear();
	currentPseudoCode.clear();

	if (v < 0 || v >= n) {
		recordSnapshotState(-1, "Invalid vertex id.");
		return;
	}

	if (n <= 1) {
		clear();
		recordSnapshotState(-1, "Graph is now empty.");
		return;
	}

	std::vector<std::vector<std::pair<int, int>>> newAdj(n - 1);

	auto remap = [v](int x) -> int {
		if (x == v) {
			return -1;
		}
		return x > v ? x - 1 : x;
	};

	for (int u = 0; u < n; ++u) {
		if (u == v) {
			continue;
		}
		int nu = remap(u);
		for (const auto& edge : adj[u]) {
			int to = edge.first;
			int w = edge.second;
			int nt = remap(to);
			if (nt < 0) {
				continue;
			}
			newAdj[nu].push_back({ nt, w });
		}
	}

	adj = std::move(newAdj);
	--n;
	recordSnapshotState(-1, "Vertex removed (ids may have shifted).");
}

void Graph::removeEdge(int u, int v) {
	animationFrames.clear();
	currentPseudoCode.clear();

	if (u < 0 || v < 0 || u >= n || v >= n) {
		recordSnapshotState(-1, "Invalid edge endpoints.");
		return;
	}

	auto& list = adj[u];
	auto it = std::find_if(list.begin(), list.end(), [v](const std::pair<int, int>& p) {
		return p.first == v;
	});
	if (it == list.end()) {
		recordSnapshotState(-1, "Edge not found.");
		return;
	}
	list.erase(it);
	recordSnapshotState(-1, "Edge removed.");
}

void Graph::updateEdgeWeight(int u, int v, int newWeight) {
	animationFrames.clear();
	currentPseudoCode.clear();

	if (u < 0 || v < 0 || u >= n || v >= n || newWeight < 0) {
		recordSnapshotState(-1, "Invalid update.");
		return;
	}

	for (auto& p : adj[u]) {
		if (p.first == v) {
			p.second = newWeight;
			recordSnapshotState(-1, "Edge weight updated.", -1, u, v);
			return;
		}
	}
	recordSnapshotState(-1, "Edge not found.");
}

void Graph::searchVertex(int id) {
	animationFrames.clear();
	currentPseudoCode.clear();

	currentPseudoCode = {
		"if (id < 0 || id >= n) return not found;", // 0
		"Highlight vertex id in the graph;",        // 1
	};

	if (n == 0) {
		recordSnapshotState(0, "Graph is empty — nothing to search.");
		return;
	}
	if (id < 0 || id >= n) {
		recordSnapshotState(0, "Vertex id is out of range.");
		return;
	}

	std::vector<int> dist(static_cast<size_t>(n), GRAPH_INF);
	std::vector<bool> vis(static_cast<size_t>(n), false);
	recordSnapshotState(
		1,
		std::string("Found vertex ") + std::to_string(id) + ".",
		-1,
		-1,
		-1,
		&dist,
		&vis,
		-1,
		id
	);
}

void Graph::dijkstra(int source) {
	animationFrames.clear();
	currentPseudoCode = {
		"dist[u] = INF for all u; dist[s] = 0;",                    // 0
		"priority_queue Q; push (0, s);",                           // 1
		"while Q not empty:",                                       // 2
		"  (d, u) = pop min;",                                       // 3
		"  if d != dist[u] continue; // stale key",                 // 4
		"  mark u as settled;",                                     // 5
		"  for each edge (u -> v) with weight w:",                  // 6
		"    if dist[u] + w < dist[v]:",                           // 7
		"      dist[v] = dist[u] + w; push (dist[v], v);",         // 8
		"// done",                                                  // 9
	};

	if (n == 0) {
		recordSnapshotState(-1, "Graph is empty.");
		return;
	}
	if (source < 0 || source >= n) {
		recordSnapshotState(-1, "Invalid source vertex.");
		return;
	}

	std::vector<int> dist(static_cast<size_t>(n), GRAPH_INF);
	std::vector<bool> settled(static_cast<size_t>(n), false);
	dist[static_cast<size_t>(source)] = 0;

	using P = std::pair<int, int>;
	std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
	pq.push({ 0, source });

	recordSnapshotState(0, "Initialize distances: source = 0, others = INF.", -1, -1, -1, &dist, &settled, source);
	recordSnapshotState(1, "Push source into the priority queue.", source, -1, -1, &dist, &settled, source);

	while (!pq.empty()) {
		int d = pq.top().first;
		int u = pq.top().second;
		pq.pop();

		recordSnapshotState(2, "Main loop: pick the smallest tentative distance.", u, -1, -1, &dist, &settled, source);
		recordSnapshotState(3, std::string("Pop vertex ") + std::to_string(u) + " with key " + std::to_string(d) + ".", u, -1, -1, &dist, &settled, source);

		if (d != dist[static_cast<size_t>(u)]) {
			recordSnapshotState(4, "Stale entry — skip.", u, -1, -1, &dist, &settled, source);
			continue;
		}

		settled[static_cast<size_t>(u)] = true;
		recordSnapshotState(5, std::string("Settle vertex ") + std::to_string(u) + ".", u, -1, -1, &dist, &settled, source);

		for (const auto& edge : adj[u]) {
			int v = edge.first;
			int w = edge.second;
			recordSnapshotState(6, std::string("Relax outgoing edge ") + std::to_string(u) + " -> " + std::to_string(v) + ".", u, u, v, &dist, &settled, source);

			long long du = dist[static_cast<size_t>(u)];
			long long nd = du + static_cast<long long>(w);
			if (nd < dist[static_cast<size_t>(v)]) {
				dist[static_cast<size_t>(v)] = static_cast<int>(nd);
				pq.push({ dist[static_cast<size_t>(v)], v });
				recordSnapshotState(7, "Found shorter path — update distance.", v, u, v, &dist, &settled, source);
				recordSnapshotState(8, "Push updated vertex into the priority queue.", v, u, v, &dist, &settled, source);
			} else {
				recordSnapshotState(7, "No improvement for this neighbor.", u, u, v, &dist, &settled, source);
			}
		}
	}

	recordSnapshotState(9, "Dijkstra finished — distances are final from the source.");
	recordSnapshotState(-1, "Algorithm complete.");
}
