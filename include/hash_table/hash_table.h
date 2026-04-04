#pragma once
struct Node {
    int val;
    Node* next;
    Node(int x) {
        val = x;
        next = nullptr;
    }
};

class HashTable {
private: 
    Node** table;
    int capacity;

    int getHash(int value) {
        return value % capacity;
    }

public:
    HashTable(int capacity);
    ~HashTable();
    void insert(int value);
    void remove(int value);
    void search(int value);
};