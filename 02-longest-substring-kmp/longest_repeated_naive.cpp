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

static int runFile(const char *path, std::size_t maxChars) {
    std::ifstream f(path, std::ios::binary);
    if (!f.is_open()) {
        std::cerr << "Cannot open file: " << path << std::endl;
        return 1;
    }
    std::ostringstream ss;
    ss << f.rdbuf();
    std::string s = ss.str();
    if (s.size() > maxChars)
        s.resize(maxChars);
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
    if (argc >= 2) {
        std::size_t maxChars = 4000;
        if (argc >= 3)
            maxChars = (std::size_t)std::stoul(argv[2]);
        return runFile(argv[1], maxChars);
    }
    return runDemo();
}
