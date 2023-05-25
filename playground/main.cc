#include <chrono>
#include <iostream>

#include <vector>
#include <random>

#include <fibers/sched.h>
#include <fibers/wait_group.h>
#include <executors/thread_pool.h>

using namespace fast;

template <typename T>
struct Generator {
    std::vector<T> genVec(std::size_t size) const {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<T> dist(0, 100);
        
        std::vector<T> v;
        v.reserve(size);
        for (size_t i = 0; i < size; ++i) {
            v.push_back(dist(mt));
        }
        return v;
    }
};

template <typename T>
struct SortTask {
    SortTask(std::vector<T>& v, int l, int r) : v(v), l(l), r(r) {}

    void operator()() {
        if (r - l <= 1) {
            return;
        }
        int m = l + (r - l) / 2;

        fibers::WaitGroup group;
        group.add(2);

        fibers::go([m, this, &group]{
            SortTask(v, l, m)();
            group.done();
        });
        fibers::go([m, this, &group]{
            SortTask(v, m, r)();
            group.done();
        });

        group.wait();

        int lidx = l;
        int ridx = m;
        int idx = 0;

        std::vector<T> nv(r - l);
        while (lidx < m && ridx < r) {
            if (v[lidx] < v[ridx]) {
                nv[idx++] = v[lidx++];
            } else {
                nv[idx++] = v[ridx++];
            }
        }

        while (lidx < m) {
            nv[idx++] = v[lidx++];
        }
        while (ridx < r) {
            nv[idx++] = v[ridx++];
        }

        for (size_t i = 0; i < nv.size(); ++i) {
            v[l + i] = nv[i];
        }
    };

    std::vector<T>& v;
    int l;
    int r;
};

template <typename T>
struct SortTaskThreads {
    SortTaskThreads(std::vector<T>& v, int l, int r) : v(v), l(l), r(r) {}

    void operator()() {
        if (r - l <= 1) {
            return;
        }
        int m = l + (r - l) / 2;

        auto t1 = std::thread([m, this]{
            SortTaskThreads(v, l, m)();
        });
        auto t2 = std::thread([m, this]{
            SortTaskThreads(v, m, r)();
        });

        t1.join();
        t2.join();

        int lidx = l;
        int ridx = m;
        int idx = 0;

        std::vector<T> nv(r - l);
        while (lidx < m && ridx < r) {
            if (v[lidx] < v[ridx]) {
                nv[idx++] = v[lidx++];
            } else {
                nv[idx++] = v[ridx++];
            }
        }

        while (lidx < m) {
            nv[idx++] = v[lidx++];
        }
        while (ridx < r) {
            nv[idx++] = v[ridx++];
        }

        for (size_t i = 0; i < nv.size(); ++i) {
            v[l + i] = nv[i];
        }
    };

    std::vector<T>& v;
    int l;
    int r;
};

int main() {
    Generator<int> gen;

    auto e1 = gen.genVec(1e4);
    auto e2 = e1;

    // for(auto& e : elements) std::cout << e << '\n'; std::cout << '\n';


    executors::ThreadPool pool(std::thread::hardware_concurrency());
    pool.start();

    auto fs = std::chrono::high_resolution_clock::now();
    fibers::go(pool, [&]{
        SortTask(e1, 0, e1.size())();
    });

    pool.wait();
    auto fe = std::chrono::high_resolution_clock::now();
    std::cout << "fibers sort : " << std::chrono::duration_cast<std::chrono::milliseconds>(fe - fs).count() << '\n';

    // for(auto& e : elements) std::cout << e << '\n'; std::cout << '\n';

    for (size_t i = 0; i + 1 < e1.size(); ++i) {
        if (e1[i] > e1[i + 1]) {
            std::abort();
        }
    }

    pool.stop();

    std::cout << "fibers ok\n";

    auto ts = std::chrono::high_resolution_clock::now();
    auto t = std::thread([&]{
        SortTaskThreads(e2, 0, e2.size())();
    });
    t.join();
    auto te = std::chrono::high_resolution_clock::now();
    std::cout << "threads sort : " << std::chrono::duration_cast<std::chrono::milliseconds>(te - ts).count() << '\n';

    for (size_t i = 0; i + 1 < e2.size(); ++i) {
        if (e2[i] > e2[i + 1]) {
            std::abort();
        }
    }
}
