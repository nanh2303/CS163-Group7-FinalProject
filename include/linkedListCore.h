#pragma once
#include <vector>
#include <string>
#include <memory>
#include <iostream>

// Represents a single node's visual state for UI
struct NodeState {
	int data;
	bool isHighlighted; //Used for current node during traversal
	bool isTarget; // Used for the found/updated/inserted node
	bool isDeleting; //Used for the node being deleted
};

//A snapshot of the entire linked list at a given moment, 
// used for playback and visualization
struct AnimationState {
	std::vector<NodeState> nodes;
	int activeLineOfCode; // For showing which line of code is executing during this step
	std::string explanation; //Optional text describing the current step
};

class LinkedList {
private:
	// Internal structure of a linked list node
	struct Node {
		int data;
		Node* next;
		Node(int val) : data(val), next(nullptr) {}
	};

	Node* head;
	
	//Playback state
	std::vector<AnimationState> animationFrames;
	std::vector<std::string> currentPseudoCode;

	//Internal helper to record the current state of the list for animation
	void recordState(
		int lineOfCode,
		const std::string& message,
		Node* highlightedNode = nullptr,
		Node* targetNode = nullptr,
		Node* deleteNode = nullptr
	);
public:
	LinkedList();
	~LinkedList();

	void clear();

	// Initialization
	void initEmpty();
	void initRandom(int cnt, int minVal = 0, int maxVal = 99);
	void initFromFile(const std::string& filepath);

	// Other operations
	void insertHead(int value);
	void insertTail(int value);
	void deleteValue(int value);
	void updateValue(int oldVal, int newVal);
	bool searchValue(int value);

	//Getters for the UI View
	const std::vector<AnimationState>& getFrames() const {
		return animationFrames;
	}
	const std::vector<std::string>& getPseudoCode() const {
		return currentPseudoCode;
	}

	//Check if frames are ready to be played
	bool hasFrames() const {
		return !animationFrames.empty();
	}
};