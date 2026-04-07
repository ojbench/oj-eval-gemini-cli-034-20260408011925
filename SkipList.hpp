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
        std::vector<Node*> forward;
        Node(const T& val, int level) : value(new T(val)), forward(level, nullptr) {}
        Node(int level) : value(nullptr), forward(level, nullptr) {} // For head node
        ~Node() { delete value; }
    };

    Node* head;
    int maxLevel;
    int currentLevel;

    int randomLevel() {
        int lvl = 1;
        while ((std::rand() % 2) == 0 && lvl < maxLevel) {
            lvl++;
        }
        return lvl;
    }

public:
    SkipList() {
        maxLevel = 32;
        currentLevel = 1;
        head = new Node(maxLevel);
        // std::srand(std::time(0)); // Avoid reseeding in case of multiple instances
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
        std::vector<Node*> update(maxLevel, nullptr);
        Node* current = head;

        for (int i = currentLevel - 1; i >= 0; i--) {
            while (current->forward[i] != nullptr && *(current->forward[i]->value) < item) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];

        if (current != nullptr && !(*(current->value) < item) && !(item < *(current->value))) {
            // Item already exists
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
        std::vector<Node*> update(maxLevel, nullptr);
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
