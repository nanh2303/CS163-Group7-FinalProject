#include "AVLTreeCore.h"
#include <iostream>
#include <algorithm>
#include <functional>
#include <random>
#include <fstream>
using namespace std;

using node = AVLTree::node;

AVLTree::AVLTree() {
    root = nullptr;
}

AVLTree::~AVLTree() {
    clear();
}

void AVLTree::clear() {
    deleteTree(root);
    root = nullptr;
}

const std::vector<AVLTree::AnimationState>& AVLTree::getAnimation() const {
    return animationFrames;
}

const std::vector<std::string>& AVLTree::getPseudoCode() const {
    return currentPseudoCode;
}

int AVLTree::getHeight(node root) {
    return (root == nullptr) ? 0 : root->height;
}

int AVLTree::getBalance(node root) {
    return (root == nullptr) ? 0 : getHeight(root->left) - getHeight(root->right);
}

void AVLTree::clearAnimation() {
    animationFrames.clear();
}

void AVLTree::updateHeight(node root) {
    if (root != nullptr) {
        root->height = 1 + std::max(getHeight(root->left), getHeight(root->right));
    }
}

bool AVLTree::loadFromFile(const char* inputFile) {
    initFromFile(inputFile);
    return true;
}

bool AVLTree::saveInOrderToFile(const char* outputFile) {
    std::ofstream fout(outputFile);
    if (!fout.is_open()) return false;
    inOrder(root, fout);
    return true;
}

bool AVLTree::hasFrames() const {
    return !animationFrames.empty();
}

void AVLTree::initEmpty() {
    clear();
    animationFrames.clear();
}

void AVLTree::initRandom(int n) {
    clear();
    animationFrames.clear();

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(1, 100);

    for (int i = 0; i < n; ++i) {
        insert(dist(rng));
    }
}

void AVLTree::initFromFile(const std::string& path) {
    initEmpty();
    clearAnimation();

    currentPseudoCode = {
        "open file;",
        "while(read x) insert(x);"
    };

    FILE* f = fopen(path.c_str(), "r");

    // fallback build/
    if (!f) {
        std::string alt = "build/" + path;
        f = fopen(alt.c_str(), "r");
    }

    if (!f) {
        recordState(-1, "Error: Cannot open file '" + path + "'");
        return;
    }

    int x;
    int count = 0;

    while (fscanf(f, "%d", &x) == 1) {
        insert(x);
        count++;
    }

    fclose(f);

    clearAnimation();
    recordState(1, "Loaded " + std::to_string(count) + " values from file");
}

void AVLTree::search(int key) {
    clearAnimation();

    node cur = root;

    currentPseudoCode = {
        "node cur = root;",
        "while (cur != nullptr)",
        "  if (key == cur->data) return FOUND;",
        "  else if (key < cur->data) go left;",
        "  else go right;",
        "return NOT FOUND;"
    };

    recordState(0, "Start search " + std::to_string(key), cur);

    while (cur != nullptr) {
        recordState(1, "Comparing " + std::to_string(key) + " with " + std::to_string(cur->data), cur);

        if (key == cur->data) {
            recordState(2, "Found " + std::to_string(key) + "!", cur);
            recordState(-1, "Search complete", cur);
             recordState(-1, "Final state", root);
             return;
        }
        
        cur = (key < cur->data) ? cur->left : cur->right;

        if (cur)
            recordState(1, "Move", cur);
    }

    recordState(3, "Not found", nullptr);
    recordState(-1, "Search complete");
    recordState(-1, "Final state", root);
}

const AVLTree::Node* AVLTree::getRoot() const {
    return root;
}

node AVLTree::rightRotate(node y) {
    currentPseudoCode = {
        "node x = y->left;",              //0
        "y->left = x->right;",            //1
        "x->right = y;",                  //2
        "updateHeight(y);",               //3
        "updateHeight(x);"                //4
    };

    node x = y->left;
    recordState(0, "Start RIGHT rotation at " + std::to_string(y->data), y, x);

    node T2 = x->right;

    y->left = T2;
    recordState(1, "Move subtree T2", y);

    x->right = y;
    recordState(2, "Rotate nodes", x);

    updateHeight(y);
    recordState(3, "Update height of y", y);

    updateHeight(x);
    recordState(4, "Update height of x", x);

    recordState(-1, "Right rotation complete", x);
    return x;
}

node AVLTree::leftRotate(node x) {
    currentPseudoCode = {
        "node y = x->right;",             //0
        "x->right = y->left;",            //1
        "y->left = x;",                   //2
        "updateHeight(x);",               //3
        "updateHeight(y);"                //4
    };

    node y = x->right;
    recordState(0, "Start LEFT rotation at " + std::to_string(x->data), x, y);

    node T2 = y->left;

    x->right = T2;
    recordState(1, "Move subtree T2", x);

    y->left = x;
    recordState(2, "Rotate nodes", y);

    updateHeight(x);
    recordState(3, "Update height of x", x);

    updateHeight(y);
    recordState(4, "Update height of y", y);

    recordState(-1, "Left rotation complete", y);
    return y;
}

void AVLTree::insert(int key) {
    clearAnimation();
    recordState(-1, "Start");
    root = insertNode(root, key);
    recordState(-1, "Inserted " + std::to_string(key));
}

void AVLTree::inOrder(node root, std::ofstream& fout) {
    if (root == nullptr) return;

    inOrder(root->left, fout);
    fout << root->data << " ";
    inOrder(root->right, fout);
}

node AVLTree::insertNode(node root, int key) {
    currentPseudoCode = {
        "if (root == nullptr) return new Node(key);",   //0
        "if (key < root->data)",                        //1
        "    root->left = insertNode(root->left, key);",//2
        "else if (key > root->data)",                   //3
        "    root->right = insertNode(root->right,key);",//4
        "updateHeight(root);",                          //5
        "balance = getBalance(root);",                  //6
        "rotate if needed"                              //7
    };

    // CREATE NODE
    if (root == nullptr) {
        node newNode = new Node(key);
        recordState(0, "Create " + std::to_string(key), newNode);
        return newNode;
    }

    recordState(1, "At " + std::to_string(root->data), root);

    if (key < root->data) {
        root->left = insertNode(root->left, key);
    }
    else if (key > root->data) {
        root->right = insertNode(root->right, key);
    }
    else return root;

    updateHeight(root);

    int balance = getBalance(root);

    // LL
    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);

    // LR
    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    // RR
    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);

    // RL
    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

void AVLTree::remove(int key) {
    clearAnimation();
    recordState(-1, "Start delete " + std::to_string(key));
    root = deleteNode(root, key);
    
    recordState(-1, "Deleted " + std::to_string(key));

    recordState(-1, "Final state", root);
}

void AVLTree::deleteTree(node root) {
    if (root == nullptr) return;

    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

node AVLTree::minValueNode(node root) {
    node cur = root;
    while (cur->left != nullptr)
        cur = cur->left;
    return cur;
}

node AVLTree::deleteNode(node root, int key) {
    currentPseudoCode = {
        "if (root == nullptr) return root;",         //0
        "if (key < root->data)",                     //1
        "    root->left = deleteNode(root->left, key);",  //2
        "else if (key > root->data)",                //3
        "    root->right = deleteNode(root->right, key);",//4
        "else delete this node",                     //5
        "updateHeight(root);",                       //6
        "balance = getBalance(root);",               //7
        "rebalance if needed"                        //8
    };

    if (root == nullptr) {
        recordState(0, "Tree empty or not found");
        return root;
    }

    recordState(1, "Compare with " + std::to_string(root->data), root);

    //  LEFT
    if (key < root->data) {
        recordState(2, "Go LEFT", root);
        root->left = deleteNode(root->left, key);
    }

    //  RIGHT
    else if (key > root->data) {
        recordState(3, "Go RIGHT", root);
        root->right = deleteNode(root->right, key);
    }

    //  FOUND NODE
    else {
        recordState(5, "Found node to delete", root, nullptr, root);

        // 1 or 0 child 
        if ((root->left == nullptr) || (root->right == nullptr)) {
            node temp = root->left ? root->left : root->right;

            // no child 
            if (temp == nullptr) {
                recordState(5, "Leaf node → delete", nullptr, nullptr, root);
                delete root;
                return nullptr;
            }
            else {
                recordState(5, "One child → replace node", root, temp);
                *root = *temp;
                delete temp;
            }
        }

        // 2 children
        else {
            node temp = minValueNode(root->right);
            recordState(5, "Two children → take inorder successor " + std::to_string(temp->data), root, temp);

            root->data = temp->data;

            root->right = deleteNode(root->right, temp->data);
        }
    }

    //  Empty
    if (root == nullptr) return root;

    //  UPDATE HEIGHT
    updateHeight(root);
    recordState(6, "Update height", root);

    int balance = getBalance(root);
    recordState(7, "Balance = " + std::to_string(balance), root);

    //  LL
    if (balance > 1 && getBalance(root->left) >= 0) {
        recordState(8, "LL case → Right rotate", root);
        return rightRotate(root);
    }

    //  LR
    if (balance > 1 && getBalance(root->left) < 0) {
        recordState(8, "LR case", root);
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    //  RR
    if (balance < -1 && getBalance(root->right) <= 0) {
        recordState(8, "RR case → Left rotate", root);
        return leftRotate(root);
    }

    //  RL
    if (balance < -1 && getBalance(root->right) > 0) {
        recordState(8, "RL case", root);
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

//UPDATE = REMOVE + INSERT
void AVLTree::update(int oldVal, int newVal) {
    clearAnimation();

    currentPseudoCode = {
        "if oldVal not found → stop",
        "if newVal already exists → stop",
        "delete oldVal",
        "insert newVal"
    };

    node cur = root;
    bool foundOld = false;

    while (cur) {
        recordState(0, "Searching for " + std::to_string(oldVal), cur);

        if (cur->data == oldVal) {
            foundOld = true;
            break;
        }
        cur = (oldVal < cur->data) ? cur->left : cur->right;
    }

    if (!foundOld) {
        recordState(0, "Value " + std::to_string(oldVal) + " not found → cancel update", nullptr);
        recordState(-1, "Update failed");
        return;
    }

    cur = root;
    while (cur) {
        recordState(1, "Checking duplicate " + std::to_string(newVal), cur);

        if (cur->data == newVal) {
            recordState(1, "Value " + std::to_string(newVal) + " already exists → cancel", cur);
            recordState(-1, "Update failed");
            return;
        }
        cur = (newVal < cur->data) ? cur->left : cur->right;
    }
    recordState(2, "Deleting " + std::to_string(oldVal), root);
    root = deleteNode(root, oldVal);

    recordState(3, "Inserting " + std::to_string(newVal), root);
    root = insertNode(root, newVal);

    recordState(-1, "Update complete", root);
}

void AVLTree::recordState(
    int lineOfCode,
    const std::string& message,
    node highlightedNode,
    node targetNode,
    node deleteNode
) {
    AnimationState frame;
    frame.activeLineOfCode = lineOfCode;
    frame.explanation = message;

    std::vector<node> nodesList;

    std::function<void(node)> collect = [&](node cur) {
        if (!cur) return;
        nodesList.push_back(cur);
        collect(cur->left);
        collect(cur->right);
    };

    collect(this->root);

    if (this->root == nullptr && highlightedNode != nullptr) {
        nodesList.push_back(highlightedNode);
    }

    std::unordered_map<node, int> id;
    for (int i = 0; i < (int)nodesList.size(); i++)
        id[nodesList[i]] = i;
    
    frame.rootIndex = (this->root && id.count(this->root)) ? id[this->root] : 
                      (!nodesList.empty() ? 0 : -1);

    for (auto cur : nodesList) {
        NodeState s;
        s.data = cur->data;
        s.height = cur->height;

        s.left = (cur->left && id.count(cur->left)) ? id[cur->left] : -1;
        s.right = (cur->right && id.count(cur->right)) ? id[cur->right] : -1;

        s.isHighlighted = (cur == highlightedNode);
        s.isRotating = (cur == targetNode);
        s.isDeleting = (cur == deleteNode);
        s.isFound = (cur == highlightedNode && lineOfCode == 2);

        s.x = 0;
        s.y = 0;

        frame.nodes.push_back(s);
    }

    animationFrames.push_back(frame);
}