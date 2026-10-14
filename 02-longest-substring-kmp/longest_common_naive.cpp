#include <iostream>
#include <string>
using namespace std;

// Naive longest common SUBSTRING (contiguous).
// Tries every pair (i, j) and extends while characters match.
// Time: O(n * m * L), L = length of match. Space: O(1) extra.
struct LCSResult {
    string value;
    size_t len = 0;
    size_t posA = 0;
    size_t posB = 0;
};

LCSResult longestCommonSubstringNaive(const string &a, const string &b) {
    LCSResult best;
    for (size_t i = 0; i < a.size(); i++) {
        for (size_t j = 0; j < b.size(); j++) {
            size_t k = 0;
            while (i + k < a.size() && j + k < b.size() && a[i + k] == b[j + k])
                k++;
            if (k > best.len) {
                best.len = k;
                best.posA = i;
                best.posB = j;
                best.value = a.substr(i, k);
            }
        }
    }
    return best;
}

int main() {
    // Built-in demo cases (no input needed for quick check)
    struct Case { string a, b, expected; };
    Case cases[] = {
        {"abcdef", "zabcde", "abcde"},
        {"computer", "commuter", "uter"},
        {"hello", "world", "l"},
        {"abc", "xyz", ""},
        {"", "abc", ""},
    };
    for (auto &c : cases) {
        LCSResult r = longestCommonSubstringNaive(c.a, c.b);
        cout << "\"" << c.a << "\" vs \"" << c.b << "\" -> \""
             << r.value << "\" (len=" << r.len << ")"
             << (r.value == c.expected ? " [OK]" : " [FAIL, expected \"" + c.expected + "\"]")
             << endl;
    }
    return 0;
}
