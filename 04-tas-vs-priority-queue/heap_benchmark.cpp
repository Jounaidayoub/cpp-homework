#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <queue>
#include <random>
#include <string>
#include <vector>

class ManualMaxHeap {
    std::vector<int> values_;

    void sift_up(std::size_t index) {
        while (index > 0) {
            std::size_t parent = (index - 1) / 2;
            if (values_[parent] >= values_[index])
                return;
            std::swap(values_[parent], values_[index]);
            index = parent;
        }
    }

    void sift_down(std::size_t index) {
        while (true) {
            std::size_t left = index * 2 + 1;
            std::size_t right = left + 1;
            std::size_t largest = index;

            if (left < values_.size() && values_[left] > values_[largest])
                largest = left;
            if (right < values_.size() && values_[right] > values_[largest])
                largest = right;
            if (largest == index)
                return;

            std::swap(values_[index], values_[largest]);
            index = largest;
        }
    }

public:
    void push(int value) {
        values_.push_back(value);
        sift_up(values_.size() - 1);
    }

    void build_heap(const std::vector<int>& input) {
        values_ = input;
        if (values_.size() < 2)
            return;
        for (std::size_t index = values_.size() / 2; index > 0; index--)
            sift_down(index - 1);
    }

    int top() const {
        return values_.front();
    }

    int pop() {
        int result = values_.front();
        values_.front() = values_.back();
        values_.pop_back();
        if (!values_.empty())
            sift_down(0);
        return result;
    }

    bool empty() const {
        return values_.empty();
    }

    std::size_t size() const {
        return values_.size();
    }

    std::size_t release() {
        std::size_t old_capacity = values_.capacity();
        std::vector<int>().swap(values_);
        return old_capacity;
    }
};

class StandardPriorityQueue {
    using Queue = std::priority_queue<int>;
    Queue queue_;

public:
    void push(int value) {
        queue_.push(value);
    }

    void build_heap(const std::vector<int>& input) {
        Queue candidate(std::less<int>(), input);
        queue_.swap(candidate);
    }

    int top() const {
        return queue_.top();
    }

    int pop() {
        int result = queue_.top();
        queue_.pop();
        return result;
    }

    bool empty() const {
        return queue_.empty();
    }

    std::size_t size() const {
        return queue_.size();
    }

    std::size_t release() {
        Queue empty_queue;
        queue_.swap(empty_queue);
        return 0;
    }
};

std::vector<int> make_values(std::size_t count, std::uint32_t seed) {
    std::vector<int> values(count);
    for (std::size_t index = 0; index < count; index++)
        values[index] = static_cast<int>(index + 1);
    std::mt19937 generator(seed);
    std::shuffle(values.begin(), values.end(), generator);
    return values;
}

struct Measurement {
    double build_push_us;
    double build_heapify_us;
    double top_all_us;
    double drain_us;
    double release_us;
    std::size_t drain_count;
    std::size_t capacity_before_release;
    bool order_ok;
};

template <typename Heap>
Measurement measure(const std::vector<int>& values) {
    Heap heap;
    auto start = std::chrono::steady_clock::now();
    for (int value : values)
        heap.push(value);
    auto finish = std::chrono::steady_clock::now();
    double build_push_us = std::chrono::duration<double, std::micro>(finish - start).count();

    Heap heapified;
    start = std::chrono::steady_clock::now();
    heapified.build_heap(values);
    finish = std::chrono::steady_clock::now();
    double build_heapify_us = std::chrono::duration<double, std::micro>(finish - start).count();
    heapified.release();

    std::uint64_t checksum = 0;
    start = std::chrono::steady_clock::now();
    for (std::size_t index = 0; index < values.size(); index++)
        checksum += static_cast<std::uint64_t>(heap.top());
    finish = std::chrono::steady_clock::now();
    double top_all_us = std::chrono::duration<double, std::micro>(finish - start).count();

    bool order_ok = true;
    std::size_t drain_count = 0;
    int previous = heap.top();
    start = std::chrono::steady_clock::now();
    while (!heap.empty()) {
        int current = heap.pop();
        if (drain_count > 0 && current > previous)
            order_ok = false;
        previous = current;
        drain_count++;
        checksum += static_cast<std::uint64_t>(current);
    }
    finish = std::chrono::steady_clock::now();
    double drain_us = std::chrono::duration<double, std::micro>(finish - start).count();

    start = std::chrono::steady_clock::now();
    std::size_t capacity_before_release = heap.release();
    finish = std::chrono::steady_clock::now();
    double release_us = std::chrono::duration<double, std::micro>(finish - start).count();

    volatile std::uint64_t observed = checksum;
    (void)observed;
    return {
        build_push_us,
        build_heapify_us,
        top_all_us,
        drain_us,
        release_us,
        drain_count,
        capacity_before_release,
        order_ok
    };
}

struct Average {
    double build_push_us = 0;
    double build_heapify_us = 0;
    double top_all_us = 0;
    double drain_us = 0;
    double release_us = 0;
    std::size_t drain_count = 0;
    std::size_t capacity_before_release = 0;
    bool order_ok = true;
};

template <typename Heap>
Average average_measurement(const std::vector<int>& values, std::size_t runs) {
    Average average;
    for (std::size_t run = 0; run < runs; run++) {
        Measurement measurement = measure<Heap>(values);
        average.build_push_us += measurement.build_push_us;
        average.build_heapify_us += measurement.build_heapify_us;
        average.top_all_us += measurement.top_all_us;
        average.drain_us += measurement.drain_us;
        average.release_us += measurement.release_us;
        average.drain_count = measurement.drain_count;
        average.capacity_before_release = measurement.capacity_before_release;
        average.order_ok = average.order_ok && measurement.order_ok;
    }
    average.build_push_us /= runs;
    average.build_heapify_us /= runs;
    average.top_all_us /= runs;
    average.drain_us /= runs;
    average.release_us /= runs;
    return average;
}

std::size_t parse_size(const char* text) {
    char* end = nullptr;
    unsigned long long value = std::strtoull(text, &end, 10);
    if (*text == '\0' || *end != '\0' || value == 0)
        throw std::string("valeur invalide: ") + text;
    return static_cast<std::size_t>(value);
}

std::uint32_t parse_seed(const char* text) {
    char* end = nullptr;
    unsigned long value = std::strtoul(text, &end, 10);
    if (*text == '\0' || *end != '\0' || value == 0)
        throw std::string("graine invalide: ") + text;
    return static_cast<std::uint32_t>(value);
}

void print_row(const std::string& structure, std::size_t n, std::uint32_t seed,
               std::size_t run, const Measurement& measurement) {
    std::cout << structure << ',' << n << ',' << seed << ',' << run << ','
              << std::fixed << std::setprecision(3)
              << measurement.build_push_us << ','
              << measurement.build_heapify_us << ','
              << measurement.top_all_us << ','
              << measurement.drain_us << ','
              << measurement.release_us << ','
              << measurement.drain_count << ','
              << measurement.capacity_before_release << ','
              << (measurement.order_ok ? 1 : 0) << '\n';
}

bool test_heap() {
    std::vector<int> values = {4, 1, 9, 2, 7, 3, 8};
    ManualMaxHeap manual;
    for (int value : values)
        manual.push(value);
    if (manual.top() != 9)
        return false;
    int previous = manual.pop();
    while (!manual.empty()) {
        int current = manual.pop();
        if (current > previous)
            return false;
        previous = current;
    }

    StandardPriorityQueue standard;
    standard.build_heap(values);
    if (standard.top() != 9)
        return false;
    previous = standard.pop();
    while (!standard.empty()) {
        int current = standard.pop();
        if (current > previous)
            return false;
        previous = current;
    }
    return true;
}

int main(int argc, char** argv) {
    try {
        std::vector<std::size_t> sizes = {100000, 500000, 1000000};
        std::size_t runs = 3;
        std::uint32_t seed = 2026;
        bool csv = false;
        bool test = false;

        for (int index = 1; index < argc; index++) {
            std::string argument = argv[index];
            if (argument == "--n" && index + 1 < argc)
                sizes = {parse_size(argv[++index])};
            else if (argument == "--runs" && index + 1 < argc)
                runs = parse_size(argv[++index]);
            else if (argument == "--seed" && index + 1 < argc)
                seed = parse_seed(argv[++index]);
            else if (argument == "--csv")
                csv = true;
            else if (argument == "--test")
                test = true;
            else
                throw std::string("argument inconnu: ") + argument;
        }

        if (test) {
            bool passed = test_heap();
            std::cout << (passed ? "OK heap" : "FAIL heap") << '\n';
            return passed ? 0 : 1;
        }

        (void)csv;
        std::cout << "structure,n,seed,run,build_push_us,build_heapify_us,top_all_us,drain_us,release_us,drain_count,capacity_before_release,order_ok\n";

        for (std::size_t n : sizes) {
            std::vector<int> values = make_values(n, seed);
            for (std::size_t run = 1; run <= runs; run++) {
                print_row("manual_vector", n, seed, run, measure<ManualMaxHeap>(values));
                print_row("std_priority_queue", n, seed, run, measure<StandardPriorityQueue>(values));
            }
        }
    } catch (const std::string& error) {
        std::cerr << error << '\n';
        return 2;
    }
    return 0;
}
