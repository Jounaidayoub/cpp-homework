#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>

struct RepeatResult {
    std::string value;
    std::size_t len = 0;
    std::size_t posA = 0;
    std::size_t posB = 0;
};

RepeatResult longestRepeatedNaive(const std::string &s) {
    RepeatResult best;
    for (std::size_t i = 0; i < s.size(); i++) {
        for (std::size_t j = i + 1; j < s.size(); j++) {
            std::size_t k = 0;
            while (j + k < s.size() && s[i + k] == s[j + k])
                k++;
            if (k > best.len) {
                best.len = k;
                best.posA = i;
                best.posB = j;
                best.value = s.substr(i, k);
            }
        }
    }
    return best;
}

std::string readFile(const std::string &path) {
    std::ifstream f(path, std::ios::binary);
    if (!f.is_open())
        throw std::runtime_error("Cannot open file: " + path);
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

static int runDemo() {
    struct Case { std::string text, expected; };
    Case cases[] = {
        {"banana", "ana"},
        {"abcdabc", "abc"},
        {"aaaa", "aaa"},
        {"abcdef", ""},
        {"", ""},
        {"the cat sat. the dog ran. the cat sat.", "the cat sat."},
    };
    for (auto &c : cases) {
        RepeatResult r = longestRepeatedNaive(c.text);
        std::cout << "\"" << c.text << "\" -> \"" << r.value
                  << "\" (len=" << r.len << ", at " << r.posA << " and " << r.posB << ")"
                  << (r.value == c.expected ? " [OK]" : " [FAIL, expected \"" + c.expected + "\"]")
                  << std::endl;
    }
    return 0;
}

static int runFile(const std::string &path, std::size_t limit) {
    std::string s;
    try {
        s = readFile(path);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    if (limit > 0 && s.size() > limit)
        s.resize(limit);
    auto t0 = std::chrono::high_resolution_clock::now();
    RepeatResult r = longestRepeatedNaive(s);
    auto t1 = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
    std::cout << "chars=" << s.size() << " time=" << ms << " ms" << std::endl;
    std::cout << "len=" << r.len << " at " << r.posA << " and " << r.posB << std::endl;
    std::cout << "repeat: \"" << r.value << "\"" << std::endl;
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc == 1)
        return runDemo();
    std::string path;
    std::size_t limit = 0;
    for (int i = 1; i < argc; i++) {
        std::string a = argv[i];
        if (a == "--limit" && i + 1 < argc)
            limit = (std::size_t)std::stoul(argv[++i]);
        else if (a.rfind("--", 0) == 0) {
            std::cerr << "Unknown flag: " << a << std::endl;
            return 1;
        } else if (path.empty())
            path = a;
        else {
            std::cerr << "Usage: " << argv[0] << " [file [--limit N]]" << std::endl;
            return 1;
        }
    }
    if (path.empty()) {
        std::cerr << "Usage: " << argv[0] << " [file [--limit N]]" << std::endl;
        return 1;
    }
    return runFile(path, limit);
}
