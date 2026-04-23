#ifndef AVLTREECORE_H
#define AVLTREECORE_H

#include <fstream>
#include <vector>
#include <string>

class AVLTree {
public:
    // ================= NODE =================
    struct Node {
        int data;
        int height;
        Node* left;
        Node* right;

        Node(int value) {
            data = value;
            left = nullptr;
            right = nullptr;
            height = 1;
        }
    };

    using node = Node*;

    node root;

    // ================= ANIMATION =================
    struct NodeState {
        int data;
        int height;
        bool isHighlighted = false;
        bool isRotating = false;
        bool isDeleting = false;
    };

    struct AnimationState {
        std::vector<NodeState> nodes;
        int activeLineOfCode;
        std::string explanation;
    };

    std::vector<AnimationState> animationFrames;
    std::vector<std::string> currentPseudoCode;

    // ================= INTERNAL =================
    void recordState(
        int lineOfCode,
        const std::string& message,
        node highlightedNode = nullptr,
        node targetNode = nullptr,
        node deleteNode = nullptr
    );

    int getHeight(node root);
    int getBalance(node root);
    void updateHeight(node root);

    node rightRotate(node y);
    node leftRotate(node x);

    node insertNode(node root, int key);
    node deleteNode(node root, int key);
    node minValueNode(node root);

    void inOrder(node root, std::ofstream& fout);
    void deleteTree(node root);

public:
    AVLTree();
    ~AVLTree();

    void insert(int key);
    void remove(int key);
    void clear();

    void initEmpty();
    void initRandom(int n);
    void initFromFile(const std::string& path);
    void search(int key);
    const Node* getRoot() const;

    bool loadFromFile(const char* inputFile);
    bool saveInOrderToFile(const char* outputFile);

    const std::vector<AnimationState>& getAnimation() const;
    const std::vector<std::string>& getPseudoCode() const;
    bool hasFrames() const;
};

#endif