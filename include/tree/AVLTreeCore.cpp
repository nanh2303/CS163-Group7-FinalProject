#include "AVLTreeCore.h"
#include <iostream>
#include <algorithm>
#include <functional>
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

void AVLTree::remove(int key) {
    animationFrames.clear();
    root = deleteNode(root, key);
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

void AVLTree::updateHeight(node root) {
    if (root != nullptr) {
        root->height = 1 + std::max(getHeight(root->left), getHeight(root->right));
    }
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
    animationFrames.clear();
    root = insertNode(root, key);
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
        recordState(0, "Create node " + std::to_string(key), nullptr, newNode);
        recordState(-1, "Insertion complete", newNode);
        return newNode;
    }

    recordState(1, "Compare with " + std::to_string(root->data), root);

    // LEFT
    if (key < root->data) {
        recordState(2, "Go LEFT", root);
        root->left = insertNode(root->left, key);
    }

    //  RIGHT
    else if (key > root->data) {
        recordState(3, "Go RIGHT", root);
        root->right = insertNode(root->right, key);
    }

    else return root;

    // UPDATE HEIGHT
    updateHeight(root);
    recordState(5, "Update height", root);

    int balance = getBalance(root);
    recordState(6, "Balance = " + std::to_string(balance), root);

    //  LL
    if (balance > 1 && key < root->left->data) {
        recordState(7, "LL case (Right Rotate)", root);
        return rightRotate(root);
    }

    //  RR
    if (balance < -1 && key > root->right->data) {
        recordState(7, "RR case (Left Rotate)", root);
        return leftRotate(root);
    }

    //  LR
    if (balance > 1 && key > root->left->data) {
        recordState(7, "LR case", root);
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    //  RL
    if (balance < -1 && key < root->right->data) {
        recordState(7, "RL case", root);
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
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
        "    root->left = deleteNode(root->left);",  //2
        "else if (key > root->data)",                //3
        "    root->right = deleteNode(root->right);",//4
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

    std::function<void(node)> dfs = [&](node cur) {
        if (!cur) return;

        NodeState s;
        s.data = cur->data;
        s.height = cur->height;
        s.isHighlighted = (cur == highlightedNode);
        s.isRotating = (cur == targetNode);
        s.isDeleting = (cur == deleteNode);

        frame.nodes.push_back(s);

        dfs(cur->left);
        dfs(cur->right);
    };

    dfs(root);

    animationFrames.push_back(frame);
}
