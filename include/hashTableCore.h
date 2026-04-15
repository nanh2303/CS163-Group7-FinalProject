#pragma once
#include<vector>
#include<string>

struct Node {
    int val;
    Node* next;
    Node(int x) : val(x), next(nullptr) {};
};

struct HashNodeState {
    int data;
    bool isHighlighted;
    bool isTarget;
    bool isDeleting;
};

struct HashAnimationState {
    int activeLineOfCode;
    std::string explanation;
    std::vector<std::vector<HashNodeState>> table;
    int activeBucketIndex = -1;
};

class HashTable {
public:
    HashTable() = default;
    HashTable(int capacity);
    ~HashTable();

    void clear();

    void initEmpty(int capacity);
    void initFromFile(const std::string& filename, int capacity);
    void insertValue(int value);
    void deleteValue(int value);
    void searchValue(int value);
    void updateValue(int oldValue, int newValue);

    const std::vector<HashAnimationState>& getFrames() const {
        return animationFrames;
    }
    const std::vector<std::string>& getPseudoCode() const {
        return currentPseudoCode;
    }
    bool hasFrames() const {
        return !animationFrames.empty();
    }

private: 
    int tableCapacity;
    Node** table = nullptr;

    std::vector<HashAnimationState> animationFrames;
    std::vector<std::string> currentPseudoCode;

    int getHash(int value);

    void recordState(
        int lineOfCode,
        const std::string& message,
        int activeBucket = -1,
        Node* highlightedNode = nullptr,
        Node* targetNode = nullptr,
        Node* deleteNode = nullptr
    );
};