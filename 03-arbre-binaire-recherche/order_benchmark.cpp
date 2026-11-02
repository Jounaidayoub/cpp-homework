#define main bst_original_main
#include "bst.cpp"
#undef main
#include <numeric>

void balanced(std::vector<int>& result, int low, int high) {
    if (low > high) return;
    int middle = low + (high - low) / 2;
    result.push_back(middle);
    balanced(result, low, middle - 1);
    balanced(result, middle + 1, high);
}
int main() {
    constexpr int n = 5000;
    std::vector<int> ascending(n); std::iota(ascending.begin(), ascending.end(), 1);
    auto descending = ascending; std::reverse(descending.begin(), descending.end());
    auto random = make_values(n, 2026);
    std::vector<int> ideal; balanced(ideal, 1, n);
    auto nearly = ascending; std::mt19937 gen(2026); std::uniform_int_distribution<int> dist(0, n-1);
    for (int i=0;i<n/100;++i) std::swap(nearly[dist(gen)], nearly[dist(gen)]);
    std::cout << "order,n,runs,height,insert_us,search_us,erase_us,control\n";
    for (const auto& item : std::vector<std::pair<std::string,std::vector<int>>>{{"ascending",ascending},{"descending",descending},{"random",random},{"balanced",ideal},{"almost_sorted",nearly}}) {
        double ins=0, sea=0, del=0; std::uint64_t check=0; std::size_t height=0;
        for (int run=0;run<3;++run) { auto m=benchmark_container<BinarySearchTree>(item.second); ins+=m.insert_us; sea+=m.search_us; del+=m.erase_us; check+=m.control; height=m.height; }
        std::cout << item.first << ',' << n << ",3," << height << ',' << ins/3 << ',' << sea/3 << ',' << del/3 << ',' << check << '\n';
        if (check != 30000) return 1;
    }
}
