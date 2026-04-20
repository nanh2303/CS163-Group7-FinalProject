#include "hashTableCore.h"
#include <fstream>

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

    animationFrames.clear();
    currentPseudoCode = {
        "table = new Node* [capacity]();"
    };
    recordState(0, "Create hash table with " + std::to_string(capacity) + " buckets.");
}

void HashTable::initFromFile(const std::string& filename, int capacity) {
    // 1. Clean memory and set up the empty table first
    clear(); 
    tableCapacity = capacity;
    table = new Node*[tableCapacity]();

    // 2. Setup the pseudo-code for the UI
    currentPseudoCode = {
        "ifstream file(filename);",
        "while(file >> value) insert(value);"
    };

    // 3. Try to open the file
    std::ifstream file(filename);
    if (!file.is_open()) {
        recordState(-1, "Error: Could not open file '" + filename + "'.");
        return;
    }

    // 4. Read values and insert silently (without creating hundreds of animation frames)
    int value;
    int count = 0;
    while (file >> value) {
        int index = getHash(value);
        
        // Check for duplicates before inserting
        Node* temp = table[index];
        bool exists = false;
        while (temp != nullptr) {
            if (temp->val == value) {
                exists = true;
                break;
            }
            temp = temp->next;
        }

        // Insert at head if it doesn't exist
        if (!exists) {
            Node* newNode = new Node(value);
            newNode->next = table[index];
            table[index] = newNode;
            count++;
        }
    }

    file.close();

    // 5. Record a single frame showing the completely built table
    recordState(1, "Successfully loaded " + std::to_string(count) + " values from " + filename);
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
        "table[index] = newNode;"
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

void HashTable::searchValue(int value) {
    animationFrames.clear();
    currentPseudoCode = {
        "int index = getHash(value);",
        "Node* cur = table[index];",
        "while(cur!=nullptr) {",
        "   if(cur->val == value) return true;",
        "   cur = cur->next;",
        "}",
        "return false;"
    };

    recordState(-1, "Starting searching for value: " + std::to_string(value));

    int index = getHash(value);
    recordState(0, "Calculate hash: " + std::to_string(value) + " % " + std::to_string(tableCapacity) + " = " + std::to_string(index), index);
    
    Node* cur = table[index];
    recordState(1, "Initialize 'cur' pointer to head of bucket " + std::to_string(index), index, cur);

    while(cur!=nullptr) {
        recordState(2, "Checking if current node is valid...", index, cur);

        recordState(3, "Comparing current node (" + std::to_string(cur->val) + ") with target (" + std::to_string(value) + ")", index, cur);
        if(cur->val == value) {
            recordState(3, "Value found! Returning true.", index, nullptr, cur);
            return;
        }
        recordState(4, "No match. Moving to next node.", index, cur->next);
        cur = cur->next;
    }

    recordState(6, "Reach end of bucket chain. Value not found.", index);
    return;
}

void HashTable::deleteValue(int value) {
    animationFrames.clear();
    currentPseudoCode = {
        "int index = getHash(value);",
        "Node* prev = nullptr;",   
        "Node* cur = table[index];",
        "while(cur!=nullptr) {",
        "   if(cur->val == value) {",
        "       if(prev == nullptr)", 
        "            table[index] = cur->next;",
        "       else", 
        "            prev->next = cur->next;",
        "       delete cur;",
        "       return;",
        "   }",
        "   prev = cur;",
        "   cur = cur->next;",
        "   return;",
        "}"
    };
    recordState(-1, "Starting deletion for value: " + std::to_string(value));

    int index = getHash(value);
    recordState(0, "Calculate hash: " + std::to_string(value) + " % " + std::to_string(tableCapacity) + " = " + std::to_string(index), index);

    Node* prev = nullptr;
    recordState(1, "Initialize 'prev' pointer to nullptr", index);

    Node* cur = table[index];
    recordState(2, "Initialize 'cur' pointer to head of bucket " + std::to_string(index), index, cur);

    while (cur != nullptr) {
        recordState(3, "Checking if current node is valid...", index, cur);

        recordState(4, "Comparing current node (" + std::to_string(cur->val) + ") with target (" + std::to_string(value) + ")", index, cur);
        
        if (cur->val == value) {
            // Passing 'cur' as the 6th argument triggers the deletion color in your renderer!
            recordState(4, "Value found! Preparing to delete.", index, nullptr, nullptr, cur);

            if (prev == nullptr) {
                recordState(5, "Node is the head. Bypassing it by pointing bucket head to the next node.", index, nullptr, nullptr, cur);
                table[index] = cur->next;
            } else {
                recordState(6, "Node is in the chain. Bypassing it by linking 'prev' to 'cur->next'.", index, prev, nullptr, cur);
                prev->next = cur->next;
            }

            delete cur;
            // Record state AFTER deletion so the node physically disappears from the screen
            recordState(9, "Node successfully deleted from memory.", index);
            
            recordState(10, "Deletion complete.", index);
            return; // EXIT HERE to prevent the use-after-free crash!
        }

        recordState(12, "No match. Updating 'prev' to point to 'cur'.", index, prev);
        prev = cur;

        Node* nextNode = cur->next;
        recordState(13, "Moving 'cur' pointer to the next node.", index, nextNode);
        cur = nextNode;
    }

    recordState(14, "Reached end of chain. Value " + std::to_string(value) + " not found.", index);
}

void HashTable::updateValue(int oldVal, int newVal) {
    animationFrames.clear();
    
    currentPseudoCode = {
        "if (!exists(oldVal)) return;",                 // 0
        "deleteNode(oldVal);",                          // 1
        "int newIndex = getHash(newVal);",              // 2
        "table[newIndex] = new Node(newVal);"           // 3
    };

    recordState(-1, "Starting update: Replace " + std::to_string(oldVal) + " with " + std::to_string(newVal));

    // --- STEP 1: DELETION ---
    int oldIndex = getHash(oldVal);
    Node* prev = nullptr;
    Node* cur = table[oldIndex];
    bool found = false;

    while (cur != nullptr) {
        if (cur->val == oldVal) {
            recordState(0, "Found old value " + std::to_string(oldVal) + ".", oldIndex, nullptr, nullptr, cur);
            
            if (prev == nullptr) table[oldIndex] = cur->next;
            else prev->next = cur->next;
            
            delete cur;
            found = true;
            recordState(1, "Deleted " + std::to_string(oldVal) + " from bucket " + std::to_string(oldIndex), oldIndex);
            break;
        }
        prev = cur;
        cur = cur->next;
    }

    if (!found) {
        recordState(0, "Old value " + std::to_string(oldVal) + " not found. Aborting update.");
        return;
    }

    // --- STEP 2: INSERTION ---
    int newIndex = getHash(newVal);
    recordState(2, "Calculate hash for new value: " + std::to_string(newVal) + " % " + std::to_string(tableCapacity) + " = " + std::to_string(newIndex), newIndex);

    // Check if the new value already exists to prevent duplicates
    cur = table[newIndex];
    while (cur != nullptr) {
        if (cur->val == newVal) {
            recordState(3, "New value " + std::to_string(newVal) + " already exists in the table. Aborting insertion.", newIndex, nullptr, cur);
            return;
        }
        cur = cur->next;
    }

    // Insert the new node at the head of the correct bucket
    Node* newNode = new Node(newVal);
    newNode->next = table[newIndex];
    table[newIndex] = newNode;
    
    recordState(3, "Successfully inserted " + std::to_string(newVal) + " into bucket " + std::to_string(newIndex), newIndex, nullptr, newNode);
}