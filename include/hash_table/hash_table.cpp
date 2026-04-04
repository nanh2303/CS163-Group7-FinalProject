#include "hash_table.h"

HashTable::HashTable(int capacity) {
    table = new Node* [capacity];

    for(int i = 0; i < capacity; ++i) {
        table[i] = nullptr;
    }
}

HashTable::~HashTable() {
    for(int i = 0; i < capacity; ++i) {
        Node* current = table[i];
        while(current != nullptr) {
            Node* prev = current;
            current = current->next;
            delete prev;
        }
    }

    delete[] table;
}

HashTable::insert(int value) {
    int index = getHash(value);

    Node* temp = new Node (value);
    temp->next = table[index];
    table[index] = temp;
}

HashTable::remove(int value) {
    int index = getHash(value);

    Node* current = table[index];
    Node* prev = nullptr;
    
    while(current!=nullptr) {
        if(current->val == value) {
            if(prev == nullptr) {
                table[index] = current -> next;
            }
            else {
                prev -> next = current -> next;
            }

            delete current;
            return;
        }
        prev = current;
        current = current->next;
    }
}

HashTable::search(int value) {
    int index = getHash(value);
    Node* current = table[index];

    while(current!=nullptr) {
        if(current->val == value) {

        }
        current = current -> next;
    }
}