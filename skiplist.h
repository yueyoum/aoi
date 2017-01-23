//
// Created by wang on 17-1-22.
//

#ifndef TESTONE_SKIPLIST_H
#define TESTONE_SKIPLIST_H

#include <cstdlib>
#include <ctime>

template <int range>
class RandomGenerator {
public:
    RandomGenerator() {
        std::srand(std::time(0));
    }

    int get() {
        return std::rand() % range;
    }
};

template <typename ValueType>
struct slNode {
    int level;
    int key;
    ValueType value;
    struct slNode<ValueType>* forward;
    struct slNode<ValueType>* down;
};

template <typename T>
using slNode = struct slNode<T>;

template <typename ValueType, int __level_count>
class SkipList {
public:
    SkipList() {
        for(int i=0; i<__level_count; i++) {
            levels_[i] = _create_level(i);
        }

        for(int i=1; i<__level_count; i++) {
            levels_[i]->down = levels_[i-1];
            levels_[i]->forward->down = levels_[i-1]->forward;
        }

        root_ = levels_[__level_count-1];
    }

    SkipList(const SkipList<ValueType, __level_count>&) = delete;
    SkipList&operator=(const SkipList<ValueType, __level_count>&) = delete;
    SkipList(SkipList<ValueType, __level_count>&&) = delete;

    ~SkipList() {
        slNode<ValueType>* tmp;
        slNode<ValueType>* forward;
        for(int i = 0; i < __level_count; i++) {
            tmp = levels_[i];
            while (tmp) {
                forward = tmp->forward;
                delete tmp;
                tmp = forward;
            }
        }
    }

    static int level_count() {
        return __level_count;
    }

    slNode<ValueType>* root() const {
        return root_;
    }

    slNode<ValueType>* find_node(const int& key) const {
        slNode<ValueType>* tmp = root_;

        while(true) {
            while (tmp->forward->forward && tmp->forward->key < key) tmp=tmp->forward;
            if(tmp->down) {
                tmp = tmp->down;
            } else {
                break;
            }
        }

        return tmp->forward;
    }

    void insert(const int& key, const ValueType value) {
        _locate_key(key);

        int lv = randomGen_.get();

        slNode<ValueType>* tmp;
        slNode<ValueType>* down = nullptr;
        for(int i = 0; i<lv+1; i++) {
            tmp = affected_nodes_[i]->forward;

            slNode<ValueType>* n = new slNode<ValueType>();
            n->level = i;
            n->key = key;
            n->value = value;
            n->forward = tmp;
            n->down = down;

            down = n;
            affected_nodes_[i]->forward = n;
        }
    }

    void remove(const int& key, const ValueType value) {
        _locate_key(key);
        slNode<ValueType>* tmp;
        slNode<ValueType>* forward;
        for(int i = 0; i<__level_count; i++) {
            tmp = affected_nodes_[i];

            while (tmp->forward->forward && tmp->forward->key == key) {
                if(tmp->forward->value == value) {
                    forward = tmp->forward->forward;
                    delete tmp->forward;
                    tmp->forward = forward;
                } else {
                    tmp = tmp->forward;
                }
            }
        }
    }

private:
    slNode<ValueType>* levels_[__level_count];
    slNode<ValueType>* root_;
    slNode<ValueType>* affected_nodes_[__level_count];
    RandomGenerator<__level_count> randomGen_;

    slNode<ValueType>* _create_level(int lv) {
        slNode<ValueType>* head = new slNode<ValueType>();
        slNode<ValueType>* tail = new slNode<ValueType>();

        head->level = lv;
        head->key = 0;
        head->forward = tail;
        head->down = nullptr;

        tail->level = lv;
        tail->key = 0;
        tail->forward = nullptr;
        tail->down = nullptr;

        return head;
    }

    void _locate_key(const int& key) {
        slNode<ValueType>* tmp = root_;

        do {
            while (tmp->forward->forward && tmp->forward->key < key) tmp=tmp->forward;
            affected_nodes_[tmp->level] = tmp;
            tmp = tmp->down;
        } while (tmp);
    }
};


#endif //TESTONE_SKIPLIST_H
