#include <iostream>
#include <map>
#include <list>
#include <chrono>

#include "skiplist.h"

#include <cstdlib>
#include <ctime>
#include <cmath>

template <typename IdType>
struct point {
    IdType id;
    int x;
    int y;
};

template <typename T>
using sl_value_t = const struct point<T>;

template <typename IdType>
struct fence {
    int start;
    int end;

    SkipList<sl_value_t<IdType>*, 3> sl;
};

template <typename IdType, int size, int spacing>
class Axis {
public:
    static int coordinate_to_fence_index(const int& coordinate) {
        return coordinate / spacing;
    }

    constexpr static int fence_count() {
        return (size%spacing)?(size/spacing):(size/spacing+1);
    }

    Axis() {
        constexpr int count = fence_count();
        for(int i=0; i<count; i++) {
            int start = i*spacing;
            int end = start+spacing;
            if(end > size) end = size;

            struct fence<IdType>* fc = new struct fence<IdType>();
            fc->start = start;
            fc->end = end;

            axis_[i] = fc;
        }
    }

    virtual ~Axis() {
        for(auto& i: axis_) delete i;
    }

    virtual void put(const struct point<IdType>* p) = 0;

    virtual void remove(const struct point<IdType>* p) = 0;

    virtual void find_neighborhood_fences(const struct point<IdType>* p, const int& distance,
                                          std::list<const struct fence<IdType>*>& fence_sets) = 0;

    void do_find(const int& coordinate, const int& distance,
                 std::list<const struct fence<IdType>*>& fence_sets) {
        auto min_coor = coordinate - distance;
        if(min_coor<0) min_coor=0;
        auto max_coor = coordinate + distance;
        if(max_coor>size) max_coor = size;

        auto min_index = coordinate_to_fence_index(min_coor);
        auto max_index = coordinate_to_fence_index(max_coor);

        for(int i=min_index; i<max_index+1; i++) {
            fence_sets.push_back(axis_[i]);
        }
    }

protected:
    struct fence<IdType>* axis_[fence_count()];
};

template <typename IdType, int size, int spacing>
class XAxis : public Axis<IdType, size, spacing> {
public:
    void put(const struct point<IdType> *p) override {
        auto index = this->coordinate_to_fence_index(p->x);
        this->axis_[index]->sl.insert(p->y, p);
    }

    void remove(const struct point<IdType> *p) override {
        auto index = this->coordinate_to_fence_index(p->x);
        this->axis_[index]->sl.remove(p->y, p);
    }

    void find_neighborhood_fences(const struct point<IdType> *p, const int &distance,
                                  std::list<const struct fence<IdType>*>& fence_sets) override {
        this->do_find(p->x, distance, fence_sets);
    }
};


template <typename IdType, int size, int spacing>
class Area
{
public:
    Area() {}

    Area(const Area<IdType, size, spacing>&) = delete;
    Area&operator=(const Area<IdType, size, spacing>&) = delete;
    Area(Area<IdType, size, spacing>&&) = delete;

    ~Area() {
        for(auto& iter: position_) {
            delete iter.second;
        }

        position_.clear();
    }

    int get_size() const {
        return size;
    }

    void leave(IdType id) {
        auto p = position_[id];
        x_axis_.remove(p);

        delete p;
        position_.erase(id);
    }

    void update(IdType id, int x, int y) {
        bool exists = position_.count(id) > 0;
        if(!exists) {
            struct point<IdType>* p = new struct point<IdType>();
            p->id = id;
            p->x = x;
            p->y = y;
            position_[id] = p;

            x_axis_.put(p);
        } else {
            auto p = position_[id];
            x_axis_.remove(p);

            p->x = x;
            p->y = y;

            x_axis_.put(p);
        }
    }

    void aoi(IdType id, int radius, std::list<IdType>& result) {
        auto p = position_[id];
        std::list<const struct fence<IdType>*> x_fence_sets;

        x_axis_.find_neighborhood_fences(p, radius, x_fence_sets);

        int r2 = radius*radius;
        int x_diff;
        int y_diff;

        for(const struct fence<IdType>* fence : x_fence_sets) {
            slNode<sl_value_t<IdType>*>* node = fence->sl.find_node(p->y-radius);
            while (node->forward && node->forward->key <= p->y+radius) {
                if(node->value->id != id) {
                    x_diff = abs(node->value->x - p->x);
                    y_diff = abs(node->value->y - p->y);
                    if((x_diff*x_diff + y_diff*y_diff) <= r2) {
                        result.push_back(node->value->id);
                    }
                }

                node = node->forward;
            }
        }
    }

    long benchmark_update(const int& amount) {
        auto start = std::chrono::steady_clock::now();

        srand(time(NULL));
        int x, y;

        for(int i=0; i<amount; i++) {
            x = rand() % get_size();
            y = rand() % get_size();
            update(i, x, y);
        }

        auto end = std::chrono::steady_clock::now();
        long cost = std::chrono::duration_cast <std::chrono::microseconds> (end-start).count();
        return cost;
    }


    long benchmark_aoi(const int& amount, const int& radius) {
        auto start = std::chrono::steady_clock::now();

        std::list<int> result;
        for(int i=0; i<amount; i++) {
            aoi(i, radius, result);
            result.clear();
        }

        auto end = std::chrono::steady_clock::now();
        long cost = std::chrono::duration_cast <std::chrono::microseconds> (end-start).count();
        return cost;
    }


private:
    std::map<IdType, struct point<IdType>*> position_;
    XAxis<IdType, size, spacing> x_axis_;
};


int main(int argc, char* argv[]) {
    int amount = atoi(argv[1]);
    int radius = atoi(argv[2]);
    int times = atoi(argv[3]);
    float cost, total_cost=0.f;

    Area<int, 20000, 100> area;

    for(int i =0; i< times; i++) {
        auto c1 = area.benchmark_update(amount);
        auto c2 =  area.benchmark_aoi(amount, radius);

        printf("move cost: %ld us\n", c1);
        printf("aoi cost: %ld us\n", c2);

        cost = (c1+c2)/1000.0f;
        printf("total cost: %f ms\n", cost);

        total_cost += cost;
    }

    printf("average: %f\n", total_cost / times);


//    SkipList<int, 3> sl;
//    sl.insert(1, 1);
//    sl.insert(10, 10);
//    sl.insert(5, 5);
//    sl.insert(6, 6);
//    sl.insert(6, 61);
//
//    auto x = sl.find_node(0);
//    while (x->forward) {
//        printf("[%d, %d]; ", x->key, x->value);
//        x = x->forward;
//    }
//
//    printf("\n");


//

//    area.update(1, 0, 0);
//    area.update(2, 10, 10);
//    area.update(3, 50, 50);
//    area.update(4, 10, 60);
//
//
//    std::list<int> result;
//
//    area.aoi(2, 30, result);
//    printf("AOI\n");
//    for(auto& i: result) {
//        printf("%d, ", i);
//    }
//    printf("\n");
//
//    result.clear();
//
//    area.update(2, 31, 31);
//
//    area.aoi(2, 45, result);
//    printf("AOI\n");
//    for(auto& i: result) {
//        printf("%d, ", i);
//    }
//    printf("\n");
//
//    result.clear();
//
//    area.leave(3);
//    area.aoi(2, 30, result);
//    printf("AOI\n");
//    for(auto& i: result) {
//        printf("%d, ", i);
//    }
//    printf("\n");

    return 0;
}