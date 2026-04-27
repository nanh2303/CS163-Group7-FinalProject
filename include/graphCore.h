#pragma once

#include <string>
#include <vector>

static const int GRAPH_INF = 1000000000;

// Per-vertex visual state for one animation frame
struct GraphVertexState {
	int id;
	int dist;
	bool visited;
	bool highlighted;
	bool isSource;
	bool isSearchHit;
};

// One directed edge snapshot for drawing
struct GraphEdgeSnapshot {
	int u;
	int v;
	int weight;
	bool highlight;
};

// Full graph snapshot at one step
struct GraphAnimationState {
	std::vector<GraphVertexState> vertices;
	std::vector<GraphEdgeSnapshot> edges;
	int activeLineOfCode;
	std::string explanation;
};

class Graph {
private:
	int n;
	std::vector<std::vector<std::pair<int, int>>> adj;

	std::vector<GraphAnimationState> animationFrames;
	std::vector<std::string> currentPseudoCode;

	void recordSnapshotState(
		int lineOfCode,
		const std::string& message,
		int highlightVertex = -1,
		int highlightEdgeFrom = -1,
		int highlightEdgeTo = -1,
		const std::vector<int>* dist = nullptr,
		const std::vector<bool>* visited = nullptr,
		int sourceVertex = -1,
		int searchHitVertex = -1
	);

public:
	Graph();
	~Graph() = default;

	void clear();

	// Initialization
	void initEmpty();
	void initRandom(int vertexCount, int edgeCount, int weightMin, int weightMax);
	void initFromFile(const std::string& filepath);

	// Structure operations
	int addVertex();
	void addEdge(int u, int v, int weight);
	void removeVertex(int v);
	void removeEdge(int u, int v);
	void updateEdgeWeight(int u, int v, int newWeight);

	// Highlight a vertex if it exists (search)
	void searchVertex(int id);

	// Dijkstra from source; records step-by-step frames
	void dijkstra(int source);

	// UI
	const std::vector<GraphAnimationState>& getFrames() const { return animationFrames; }
	const std::vector<std::string>& getPseudoCode() const { return currentPseudoCode; }
	bool hasFrames() const { return !animationFrames.empty(); }

	int getVertexCount() const { return n; }
	const std::vector<std::vector<std::pair<int, int>>>& getAdjacency() const { return adj; }
};
