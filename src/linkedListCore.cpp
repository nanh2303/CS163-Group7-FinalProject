#include "linked_list/linkedListCore.h"
#include <random>
#include <fstream>

LinkedList::LinkedList() : head(nullptr) {}

LinkedList::~LinkedList() {
	clear();
}

void LinkedList::clear() {
	Node* cur = head;
	while (cur) {
		Node* nxt = cur->next;
		delete cur;
		cur = nxt;
	}
	head = nullptr;
	animationFrames.clear();
	currentPseudoCode.clear();
}

// Core animation engine

void LinkedList::recordState(int lineOfCode, const std::string& message, Node* highlightedNode, Node* targetNode, Node* deleteNode) {
	AnimationState frame;
	frame.activeLineOfCode = lineOfCode;
	frame.explanation = message;
	//Traverse the list and capture the state of each node
	Node* cur = head;
	while (cur) {
		NodeState nState;
		nState.data = cur->data;
		nState.isHighlighted = (cur == highlightedNode);
		nState.isTarget = (cur == targetNode);
		nState.isDeleting = (cur == deleteNode);

		frame.nodes.push_back(nState);
		cur = cur->next;
	}
	animationFrames.push_back(frame);
}

void LinkedList::initEmpty() {
	clear();
}

void LinkedList::initRandom(int cnt, int minVal, int maxVal) {
	clear();
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(minVal, maxVal);
	currentPseudoCode = {
		"Generate a random linked list with " + std::to_string(cnt) + " nodes."
	};

	Node* tail = nullptr;
	for (int i = 0; i < cnt; ++i) {
		Node* newNode = new Node(dis(gen));

		if (head == nullptr) {
			head = newNode;
			tail = newNode;
		}
		else {
			tail->next = newNode;
			tail = newNode;
		}
	}
	recordState(0, "Randomly initialized " + std::to_string(cnt) + " nodes.");
}

void LinkedList::initFromFile(const std::string& filepath) {
	clear();
	std::ifstream fin(filepath);
	if (!fin) {
		return;
	}
	int value;
	while (fin >> value) {
		insertTail(value);
	}
}

void LinkedList::insertHead(int value) {
	animationFrames.clear();
	currentPseudoCode = {
		"Node* newNode = new Node(value);", // Line 0
		"newNode->next = head;",			// Line 1
		"head = newNode;"					// Line 2
	};
	
	Node* newNode = new Node(value);
	recordState(0, "Create new node with value " + std::to_string(value), nullptr, newNode);
	newNode->next = head;
	recordState(1, "Link new node to current head", nullptr, newNode);
	head = newNode;
	recordState(2, "Update head to the new node", head);
	recordState(-1, "Insertion complete");
}

void LinkedList::insertTail(int value) {
	animationFrames.clear();
	currentPseudoCode = {
		"Node* newNode = new Node(value);",					// Line 0
		"if (head == nullptr) { head = newNode; return;}",	// Line 1
		"Node* temp = head;",								// Line 2
		"while (temp->next != nullptr) ",					// Line 3
		"	temp = temp->next; ",							// Line 4
		"temp->next = newNode;",							// Line 5
	};
	// Begin algorithm & recording
	Node* newNode = new Node(value);
	recordState(0, "Create new node with value " + std::to_string(value));
	if (head == nullptr) {
		head = newNode;
		recordState(1, "List is empty, new node becomes head", nullptr, head);
		return;
	}
	recordState(1, "List is not empty, start traversing from head");
	Node* temp = head;
	recordState(2, "Initialize temp pointer to head", temp);
	while (temp->next) {
		recordState(3, "Check if temp is the last node", temp);
		temp = temp->next;
		recordState(4, "Move temp to the next node", temp);
	}
	recordState(3, "Temp is now at the last node", temp);
	temp->next = newNode;
	recordState(5, "Linked the last node to the new node", temp, temp->next);
	recordState(-1, "Insertion complete");
}

void LinkedList::deleteValue(int value) {
	animationFrames.clear();
	currentPseudoCode = {
		"if (head == nullptr) return;",				//Line 0
		"Node* temp = head, *prev = nullptr;",		//Line 1
		"while (temp && temp->data != value) {",	//Line 2
		"	prev = temp;",							//Line 3
		"	temp = temp->next;",					//Line 4
		"}",										//Line 5
		"if (temp == nullptr) return;",				//Line 6
		"if (prev == nullptr) head = temp->next;",	//Line 7
		"else prev->next = temp->next;",			//Line 8
		"delete temp;"								//Line 9
	};

	if (!head) {
		recordState(0, "List is empty, nothing to delete");
		return;
	}
	Node* temp = head, *prev = nullptr;
	recordState(1, "Initialize temp to head and prev to nullptr", temp);
	while (temp && temp->data != value) {
		recordState(2, "Check if temp node has the target value", temp);
		prev = temp;
		recordState(3, "Move prev to temp", prev);
		temp = temp->next;
		recordState(4, "Move temp to the next node", temp);
	}
	recordState(2, "Exit loop", temp);
	recordState(5, "Exit loop");
	if (!temp) {
		recordState(6, "Reached end of list without finding value");
		return;
	}
	recordState(6, "Target found, preparing to delete", nullptr, nullptr, temp);
	if (!prev) {
		head = temp->next;
		recordState(7, "Target is at head, update head to next node", head);
	}
	else {
		prev->next = temp->next;
		recordState(8, "Bypass the target node by linking prev to temp's next", prev);
	}
	delete temp;
	recordState(9, "Deletion complete");
}

void LinkedList::updateValue(int oldVal, int newVal) {
	animationFrames.clear();
	currentPseudoCode = {
		"Node* temp = head;",						//Line 0
		"while (temp && temp->data != oldVal) {",	//Line 1
		"	temp = temp->next;",					//Line 2
		"}",										//Line 3
		"if (temp)",								//Line 4
		"	temp->data = newVal;"					//Line 5
	};

	Node* temp = head;
	recordState(0, "Initialize temp to head", temp);
	while (temp && temp->data != oldVal) {
		recordState(1, "Check if temp is not nullptr and has the target value", temp);
		temp = temp->next;
		recordState(2, "Move temp to the next node", temp);
	}
	recordState(3, "Exit loop", temp);
	if (temp) {
		recordState(4, "Target value found in the list", nullptr,temp);
		temp->data = newVal;
		recordState(5, "Updated node value", nullptr,temp);
	}
	else {
		recordState(4, "Reached end of list without finding value");
	}
	recordState(-1, "Update complete");
}

bool LinkedList::searchValue(int value) {
	animationFrames.clear();
	currentPseudoCode = {
		"Node* temp = head;",					//Line 0
		"while (temp && temp->data != value)",	//Line 1
		"	temp = temp->next;",				//Line 2
		"if (temp) return true;",				//Line 3
		"return false;"							//Line 4
	};
	Node* temp = head;
	recordState(0, "Initialize temp to head", temp);
	while (temp && temp->data != value) {
		recordState(1, "Check if temp is not nullptr and has the target value", temp);
		temp = temp->next;
		recordState(2, "Move temp to the next node", temp);
	}
	recordState(1, "Exit loop", temp);
	if (temp) {
		recordState(3, "Target value found in the list", nullptr,temp);
		recordState(-1, "Search complete");
		return true;
	}
	recordState(4, "Reach end of list without finding value");
	recordState(-1, "Search complete");
	return false;
}