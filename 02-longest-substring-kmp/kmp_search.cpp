#include <iostream>
#include <string>
#include <vector>
using namespace std;

// KMP prefix function (LPS array).
// lps[i] = length of longest proper prefix of pattern[0..i]
//          which is also a suffix of pattern[0..i].
// Time: O(m), Space: O(m).
vector<int> computeLPS(const string &pat) {
    vector<int> lps(pat.size(), 0);
    int len = 0; // length of current longest prefix-suffix
    for (size_t i = 1; i < pat.size();) {
        if (pat[i] == pat[len]) {
            lps[i++] = ++len;
        } else if (len > 0) {
            len = lps[len - 1]; // fall back, do not advance i
        } else {
            lps[i++] = 0;
        }
    }
    return lps;
}

// KMP search: returns all start indices where pat occurs in txt (overlaps included).
// Time: O(n + m), Space: O(m) for LPS.
vector<int> kmpSearch(const string &txt, const string &pat) {
    vector<int> occ;
    if (pat.empty())
        return occ;
    vector<int> lps = computeLPS(pat);
    size_t i = 0; // index in txt
    size_t j = 0; // index in pat
    while (i < txt.size()) {
        if (txt[i] == pat[j]) {
            i++;
            j++;
            if (j == pat.size()) {
                occ.push_back((int)(i - j));
                j = lps[j - 1]; // allow overlapping matches
            }
        } else if (j > 0) {
            j = lps[j - 1]; // skip re-comparisons, key KMP idea
        } else {
            i++;
        }
    }
    return occ;
}

static void show(const string &txt, const string &pat) {
    vector<int> occ = kmpSearch(txt, pat);
    cout << "text=\"" << txt << "\" pat=\"" << pat << "\" -> ";
    if (occ.empty()) {
        cout << "no match";
    } else {
        cout << occ.size() << " hit(s) at ";
        for (size_t k = 0; k < occ.size(); k++)
            cout << (k ? "," : "") << occ[k];
    }
    cout << endl;
}

int main() {
    // Classic demo + edge cases
    show("ABABDABACDABABCABAB", "ABABCABAB"); // expect 1 hit at 10
    show("aaaaa", "aaa");                     // expect hits at 0,1,2 (overlap)
    show("hello world", "world");             // expect hit at 6
    show("hello", "xyz");                     // expect no match
    show("abc", "");                          // expect no match (empty pattern)

    // Print LPS for the classic pattern so the report can show it
    string pat = "ABABCABAB";
    vector<int> lps = computeLPS(pat);
    cout << "LPS(\"" << pat << "\") = [";
    for (size_t i = 0; i < lps.size(); i++)
        cout << (i ? "," : "") << lps[i];
    cout << "]" << endl;
    return 0;
}
