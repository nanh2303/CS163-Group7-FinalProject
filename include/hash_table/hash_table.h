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

    

public:
    HashTable(int capacity);
    ~HashTable();
    void insert(int value);
    void remove(int value);
    bool search(int value);
    int getHash(int value);
    Node* getBucket(int index);
    int getCapacity();
};