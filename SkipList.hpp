#ifndef SKIP_LIST_HPP
#define SKIP_LIST_HPP

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <climits>

template<typename T>
class SkipList {
private:
    struct Node {
        T* value;
        Node** forward;
        int level;
        
        Node(const T& val, int lvl) : value(new T(val)), level(lvl) {
            forward = new Node*[lvl];
            for (int i = 0; i < lvl; ++i) forward[i] = nullptr;
        }
        
        Node(int lvl) : value(nullptr), level(lvl) {
            forward = new Node*[lvl];
            for (int i = 0; i < lvl; ++i) forward[i] = nullptr;
        }
        
        ~Node() {
            delete value;
            delete[] forward;
        }
    };

    Node* head;
    int maxLevel;
    int currentLevel;
    unsigned int seed;

    int randomLevel() {
        int lvl = 1;
        // Fast Xorshift PRNG
        seed ^= seed << 13;
        seed ^= seed >> 17;
        seed ^= seed << 5;
        unsigned int r = seed;
        
        while ((r & 1) == 0 && lvl < maxLevel) {
            lvl++;
            r >>= 1;
        }
        return lvl;
    }

public:
    SkipList() {
        maxLevel = 32;
        currentLevel = 1;
        head = new Node(maxLevel);
        seed = 123456789;
    }

    ~SkipList() {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->forward[0];
            delete current;
            current = next;
        }
    }

    void insert(const T & item) {
        Node* update[32];
        for (int i = 0; i < 32; ++i) update[i] = nullptr;
        
        Node* current = head;

        for (int i = currentLevel - 1; i >= 0; i--) {
            while (current->forward[i] != nullptr && *(current->forward[i]->value) < item) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];

        if (current != nullptr && !(*(current->value) < item) && !(item < *(current->value))) {
            return;
        }

        int lvl = randomLevel();

        if (lvl > currentLevel) {
            for (int i = currentLevel; i < lvl; i++) {
                update[i] = head;
            }
            currentLevel = lvl;
        }

        Node* newNode = new Node(item, lvl);

        for (int i = 0; i < lvl; i++) {
            newNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = newNode;
        }
    }

    bool search(const T & item) {
        Node* current = head;

        for (int i = currentLevel - 1; i >= 0; i--) {
            while (current->forward[i] != nullptr && *(current->forward[i]->value) < item) {
                current = current->forward[i];
            }
        }

        current = current->forward[0];

        if (current != nullptr && !(*(current->value) < item) && !(item < *(current->value))) {
            return true;
        }

        return false;
    }

    void deleteItem(const T & item) {
        Node* update[32];
        for (int i = 0; i < 32; ++i) update[i] = nullptr;
        
        Node* current = head;

        for (int i = currentLevel - 1; i >= 0; i--) {
            while (current->forward[i] != nullptr && *(current->forward[i]->value) < item) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];

        if (current != nullptr && !(*(current->value) < item) && !(item < *(current->value))) {
            for (int i = 0; i < currentLevel; i++) {
                if (update[i]->forward[i] != current) {
                    break;
                }
                update[i]->forward[i] = current->forward[i];
            }

            delete current;

            while (currentLevel > 1 && head->forward[currentLevel - 1] == nullptr) {
                currentLevel--;
            }
        }
    }
};

#endif