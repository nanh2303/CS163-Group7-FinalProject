#include "hashTableCore.h"

int HashTable::getHash(int value) {
    return value % tableCapacity;
}

HashTable::HashTable(int capacity) {
    tableCapacity = capacity;
    table = new Node* [tableCapacity]();
}

void HashTable::initEmpty(int capacity) {
    clear();
    tableCapacity = capacity;
    table = new Node* [tableCapacity]();
}

HashTable::~HashTable() {
    clear();
    delete[] table;
}

void HashTable::clear() {
    if(!table) return;

    for(int i = 0; i < tableCapacity; ++i) {
        Node* cur = table[i];
        while(cur) {
            Node* temp = cur->next;
            delete cur;
            cur = temp;
        }
    }

    delete[] table;
    table = nullptr;
    tableCapacity = 0;
    animationFrames.clear();
    currentPseudoCode.clear();
}

void HashTable::recordState(int lineOfCode, const std::string& message, int activeBucket, Node* highlightedNode, Node* targetNode, Node* deleteNode) {
    HashAnimationState frame;
    frame.activeLineOfCode = lineOfCode;
    frame.explanation = message;
    frame.activeBucketIndex = activeBucket;

    for(int i = 0; i < tableCapacity; ++i) {
        std::vector<HashNodeState> bucketState;
        Node* cur = table[i];

        while(cur) {
            HashNodeState nodeState;
            nodeState.data = cur->val;
            nodeState.isHighlighted = (cur == highlightedNode);
            nodeState.isTarget = (cur == targetNode);
            nodeState.isDeleting = (cur == deleteNode);

            bucketState.push_back(nodeState);
            cur = cur->next;
        }
        frame.table.push_back(bucketState);
    }
    animationFrames.push_back(frame);
}

void HashTable::insertValue(int value) {
    animationFrames.clear();
    currentPseudoCode = {
        "int index = value % capacity;",
        "Node* temp = table[index];",
        "while(temp != nullptr) {",
        "   if(temp->data == value) return;",
        "   temp = temp->next;",
        "}",
        "Node* newNode = new Node (value);",
        "newNode->next = table[index];",
        "table[index] = newNode]"
    };

    recordState(-1, "Starting insertion for " + std::to_string(value));
    int index = getHash(value);
    recordState(0, "Calculate hash: " + std::to_string(value) + "%" + std::to_string(tableCapacity) + "=" + std::to_string(index), index);

    Node* temp = table[index];
    recordState(1, "Initialize temp pointer to head of bucket " + std::to_string(index), index, temp);

    while (temp != nullptr) {
        recordState(2, "Checking node...", index, temp);
        if(temp->val == value) {
            recordState(3, "Value already exists. Aborting.", index, nullptr, temp);
            return;
        }
        temp = temp->next;
        recordState(4, "Move to next node", index, temp);
    }
    recordState(5, "Reach end of bucket chain", index);

    Node* newNode = new Node(value);
    recordState(6, "Create new node", index, newNode);

    newNode->next = table[index];
    recordState(7, "Point new node to head of current bucket", index, newNode);

    table[index] = newNode;
    recordState(8, "Update bucket head to new node", index, nullptr, newNode);

    recordState(-1, "Insertion complete");
}