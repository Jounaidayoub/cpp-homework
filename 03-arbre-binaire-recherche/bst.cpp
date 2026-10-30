#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <type_traits>
#include <vector>

class BinarySearchTree {
    struct Node {
        int value;
        Node* left;
        Node* right;

        explicit Node(int value) : value(value), left(nullptr), right(nullptr) {}
    };

    Node* root_ = nullptr;
    std::size_t size_ = 0;
    std::uint64_t comparisons_ = 0;
    std::size_t height_ = 0;

public:
    BinarySearchTree() = default;

    ~BinarySearchTree() {
        clear();
    }

    BinarySearchTree(const BinarySearchTree&) = delete;
    BinarySearchTree& operator=(const BinarySearchTree&) = delete;

    bool insert(int value) {
        if (root_ == nullptr) {
            root_ = new Node(value);
            size_ = 1;
            height_ = 1;
            return true;
        }

        Node* current = root_;
        std::size_t depth = 1;
        while (true) {
            comparisons_++;
            if (value == current->value)
                return false;

            if (value < current->value) {
                if (current->left == nullptr) {
                    current->left = new Node(value);
                    size_++;
                    height_ = height_ > depth + 1 ? height_ : depth + 1;
                    return true;
                }
                current = current->left;
            } else {
                if (current->right == nullptr) {
                    current->right = new Node(value);
                    size_++;
                    height_ = height_ > depth + 1 ? height_ : depth + 1;
                    return true;
                }
                current = current->right;
            }
            depth++;
        }
    }

    bool contains(int value) const {
        const Node* current = root_;
        while (current != nullptr) {
            if (value == current->value)
                return true;
            current = value < current->value ? current->left : current->right;
        }
        return false;
    }

    bool erase(int value) {
        Node** link = &root_;
        while (*link != nullptr) {
            Node* current = *link;
            comparisons_++;
            if (value == current->value) {
                if (current->left == nullptr) {
                    *link = current->right;
                    delete current;
                    size_--;
                    return true;
                }
                if (current->right == nullptr) {
                    *link = current->left;
                    delete current;
                    size_--;
                    return true;
                }

                Node** successor_link = &current->right;
                while ((*successor_link)->left != nullptr)
                    successor_link = &(*successor_link)->left;
                Node* successor = *successor_link;
                current->value = successor->value;
                *successor_link = successor->right;
                delete successor;
                size_--;
                return true;
            }
            link = value < current->value ? &current->left : &current->right;
        }
        return false;
    }

    void inorder(std::vector<int>& values) const {
        std::vector<const Node*> stack;
        const Node* current = root_;
        while (current != nullptr || !stack.empty()) {
            while (current != nullptr) {
                stack.push_back(current);
                current = current->left;
            }
            current = stack.back();
            stack.pop_back();
            values.push_back(current->value);
            current = current->right;
        }
    }

    void clear() {
        if (root_ == nullptr)
            return;

        std::vector<Node*> stack;
        stack.reserve(size_);
        stack.push_back(root_);
        while (!stack.empty()) {
            Node* current = stack.back();
            stack.pop_back();
            if (current->left != nullptr)
                stack.push_back(current->left);
            if (current->right != nullptr)
                stack.push_back(current->right);
            delete current;
        }
        root_ = nullptr;
        size_ = 0;
        comparisons_ = 0;
        height_ = 0;
    }

    std::size_t size() const {
        return size_;
    }

    std::uint64_t comparisons() const {
        return comparisons_;
    }

    std::size_t height() const {
        return height_;
    }
};

std::vector<int> make_values(std::size_t count, std::uint32_t seed) {
    std::vector<int> values(count);
    for (std::size_t i = 0; i < count; i++)
        values[i] = static_cast<int>(i + 1);
    std::mt19937 generator(seed);
    std::shuffle(values.begin(), values.end(), generator);
    return values;
}

struct OperationTimes {
    double insert_us = 0;
    double search_us = 0;
    double erase_us = 0;
    std::uint64_t control = 0;
    std::size_t height = 0;
};

template <typename Container>
OperationTimes benchmark_container(const std::vector<int>& values) {
    Container container;
    OperationTimes result;

    auto start = std::chrono::steady_clock::now();
    for (int value : values) {
        if constexpr (std::is_same_v<Container, BinarySearchTree>)
            container.insert(value);
        else
            container.emplace(value, value);
    }
    auto finish = std::chrono::steady_clock::now();
    result.insert_us = std::chrono::duration<double, std::micro>(finish - start).count();

    result.height = [&]() {
        if constexpr (std::is_same_v<Container, BinarySearchTree>)
            return container.height();
        return static_cast<std::size_t>(0);
    }();

    start = std::chrono::steady_clock::now();
    for (int value : values) {
        if constexpr (std::is_same_v<Container, BinarySearchTree>)
            result.control += container.contains(value);
        else
            result.control += container.find(value) != container.end();
    }
    finish = std::chrono::steady_clock::now();
    result.search_us = std::chrono::duration<double, std::micro>(finish - start).count();

    start = std::chrono::steady_clock::now();
    for (int value : values) {
        if constexpr (std::is_same_v<Container, BinarySearchTree>)
            result.control += container.erase(value);
        else
            result.control += container.erase(value);
    }
    finish = std::chrono::steady_clock::now();
    result.erase_us = std::chrono::duration<double, std::micro>(finish - start).count();

    return result;
}

struct Average {
    double insert_us = 0;
    double search_us = 0;
    double erase_us = 0;
    std::uint64_t control = 0;
    std::size_t height = 0;
};

template <typename Container>
Average benchmark_average(std::size_t count, std::uint32_t seed, std::size_t repetitions) {
    Average average;
    for (std::size_t repetition = 0; repetition < repetitions; repetition++) {
        std::vector<int> values = make_values(count, seed);
        OperationTimes result = benchmark_container<Container>(values);
        average.insert_us += result.insert_us;
        average.search_us += result.search_us;
        average.erase_us += result.erase_us;
        average.control += result.control;
        average.height = result.height;
    }
    average.insert_us /= repetitions;
    average.search_us /= repetitions;
    average.erase_us /= repetitions;
    return average;
}

void print_csv_row(const std::string& structure, const std::string& operation,
                  std::size_t count, std::uint32_t seed, std::size_t repetitions,
                  double microseconds, std::uint64_t control, std::size_t height) {
    std::cout << structure << ',' << operation << ',' << count << ',' << seed << ','
              << repetitions << ',' << std::fixed << std::setprecision(3)
              << microseconds << ',' << control << ',' << height << '\n';
}

bool check(bool condition, const std::string& name) {
    std::cout << (condition ? "OK " : "FAIL ") << name << '\n';
    return condition;
}

int run_tests() {
    BinarySearchTree tree;
    bool passed = true;
    passed &= check(tree.size() == 0 && tree.height() == 0, "arbre vide");
    passed &= check(tree.insert(50), "racine");
    passed &= check(tree.insert(30) && tree.insert(70), "fils gauche et droit");
    passed &= check(tree.insert(20) && tree.insert(40) && tree.insert(60) && tree.insert(80), "feuilles");
    passed &= check(!tree.insert(50), "doublon ignore");
    passed &= check(tree.contains(40) && !tree.contains(45), "recherche");

    std::vector<int> ordered;
    tree.inorder(ordered);
    passed &= check(ordered == std::vector<int>({20, 30, 40, 50, 60, 70, 80}), "parcours infixe");
    passed &= check(tree.erase(20) && tree.erase(30) && tree.erase(50), "suppressions feuille et deux fils");
    passed &= check(!tree.contains(20) && !tree.contains(30) && !tree.contains(50), "valeurs supprimees");
    passed &= check(tree.size() == 4, "taille apres suppressions");

    BinarySearchTree chain;
    for (int value = 1; value <= 1000; value++)
        chain.insert(value);
    passed &= check(chain.height() == 1000, "hauteur croissante");
    passed &= check(chain.comparisons() == 499500, "comparaisons croissantes");
    passed &= check(chain.erase(500), "suppression avec un fils");

    return passed ? 0 : 1;
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

int main(int argc, char** argv) {
    try {
        if (argc == 2 && std::string(argv[1]) == "--test")
            return run_tests();

        std::size_t count = 200000;
        std::size_t repetitions = 3;
        std::uint32_t seed = 2026;
        bool csv = false;

        for (int i = 1; i < argc; i++) {
            std::string argument = argv[i];
            if (argument == "--n" && i + 1 < argc)
                count = parse_size(argv[++i]);
            else if (argument == "--seed" && i + 1 < argc)
                seed = parse_seed(argv[++i]);
            else if (argument == "--runs" && i + 1 < argc)
                repetitions = parse_size(argv[++i]);
            else if (argument == "--csv")
                csv = true;
            else
                throw std::string("argument inconnu: ") + argument;
        }

        Average abr = benchmark_average<BinarySearchTree>(count, seed, repetitions);
        Average map = benchmark_average<std::map<int, int>>(count, seed, repetitions);
        if (abr.control != repetitions * count * 2 || map.control != repetitions * count * 2)
            throw std::string("echec du controle fonctionnel");

        if (csv) {
            std::cout << "structure,operation,n,seed,runs,average_us,control,height\n";
            print_csv_row("abr", "insert", count, seed, repetitions, abr.insert_us, abr.control, abr.height);
            print_csv_row("abr", "search", count, seed, repetitions, abr.search_us, abr.control, abr.height);
            print_csv_row("abr", "erase", count, seed, repetitions, abr.erase_us, abr.control, abr.height);
            print_csv_row("std_map", "insert", count, seed, repetitions, map.insert_us, map.control, map.height);
            print_csv_row("std_map", "search", count, seed, repetitions, map.search_us, map.control, map.height);
            print_csv_row("std_map", "erase", count, seed, repetitions, map.erase_us, map.control, map.height);
        } else {
            std::cout << "n: " << count << '\n'
                      << "graine: " << seed << '\n'
                      << "repetitions: " << repetitions << '\n'
                      << "controle_abr: " << abr.control << '\n'
                      << "controle_map: " << map.control << '\n'
                      << "abr_insert_us: " << std::fixed << std::setprecision(3) << abr.insert_us << '\n'
                      << "abr_search_us: " << abr.search_us << '\n'
                      << "abr_erase_us: " << abr.erase_us << '\n'
                      << "abr_height: " << abr.height << '\n'
                      << "map_insert_us: " << map.insert_us << '\n'
                      << "map_search_us: " << map.search_us << '\n'
                      << "map_erase_us: " << map.erase_us << '\n';
        }
    } catch (const std::string& error) {
        std::cerr << error << '\n';
        return 2;
    }
    return 0;
}
