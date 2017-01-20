#include <iostream>
#include <map>
#include <list>
#include <chrono>

#include <cstdlib>
#include <ctime>
#include <cmath>

template <typename IdType>
struct point {
    IdType id;
    int x;
    int y;
};

template <typename IdType>
struct fence {
    int start;
    int end;

    std::list<const struct point<IdType>*> points;
};

template <typename IdType, int size, int spacing>
class Axis {
public:
    static int coordinate_to_fence_index(const int& coordinate) {
        return coordinate / spacing;
    }

    static bool fence_index_changed(const int& c1, const int& c2) {
        return coordinate_to_fence_index(c1) != coordinate_to_fence_index(c2);
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
        this->axis_[index]->points.push_back(p);
    }

    void remove(const struct point<IdType> *p) override {
        auto index = this->coordinate_to_fence_index(p->x);
        this->axis_[index]->points.remove(p);
    }

    void find_neighborhood_fences(const struct point<IdType> *p, const int &distance,
                                  std::list<const struct fence<IdType> *> &fence_sets) override {
        this->do_find(p->x, distance, fence_sets);
    }
};

// implement the XAxis

template <typename IdType, int size, int spacing>
class Area
{
public:
    Area(): size_(size) {}

    ~Area() {
        for(auto& iter: position_) {
            delete iter.second;
        }

        position_.clear();
    }

    int get_size() {
        return size_;
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
            if(Axis<IdType, size, spacing>::fence_index_changed(p->x, x)) {
                x_axis_.remove(p);
                x_axis_.put(p);
            }

            p->x = x;
            p->y = y;
        }
    }

    void aoi(IdType id, int radius, std::list<IdType>& result) {
        auto p = position_[id];
        std::list<const struct fence<IdType>*> x_fence_sets;

        x_axis_.find_neighborhood_fences(p, radius, x_fence_sets);

        int r2 = pow(radius, 2);
        for(auto fence : x_fence_sets) {
            for(auto this_p : fence->points) {
                if(this_p->id == id) continue;

                auto y_diff = abs(this_p->y - p->y);
                if(y_diff > radius) continue;

                if((pow(abs(this_p->x - p->x), 2) + pow(y_diff, 2)) <= r2) {
                    result.push_back(this_p->id);
                }
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
    const int size_;
    std::map<IdType, struct point<IdType>*> position_;
    XAxis<IdType, size, spacing> x_axis_;
};


int main(int argc, char* argv[]) {
    int amount = atoi(argv[1]);
    int radius = atoi(argv[2]);

    Area<int, 10000, 100> area;

    auto c1 = area.benchmark_update(amount);
    auto c2 =  area.benchmark_aoi(amount, radius);

    printf("move cost: %ld us\n", c1);
    printf("aoi cost: %ld us\n", c2);

    printf("total cost: %f ms\n", (c1+c2)/1000.0f);
//
//    area.update(1, 0, 0);
//    area.update(2, 10, 10);
//    area.update(3, 50, 50);
//    area.update(4, 10, 60);
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