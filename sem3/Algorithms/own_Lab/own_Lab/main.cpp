#include "RabinKarp.h"
#include <iostream>

using namespace std;

int main() {
    int textRows, textCols, patternRows, patternCols;

    // Input for text matrix dimensions
    cout << "Enter the number of rows for the text: ";
    cin >> textRows;
    cout << "Enter the number of columns for the text: ";
    cin >> textCols;

    vector<string> text(textRows);
    cout << "Enter the text matrix row by row:\n";
    for (int i = 0; i < textRows; ++i) {
        cin >> text[i];
    }

    // Input for pattern matrix dimensions
    cout << "Enter the number of rows for the pattern: ";
    cin >> patternRows;
    cout << "Enter the number of columns for the pattern: ";
    cin >> patternCols;

    vector<string> pattern(patternRows);
    cout << "Enter the pattern matrix row by row:\n";
    for (int i = 0; i < patternRows; ++i) {
        cin >> pattern[i];
    }

    // Create RabinKarp object and execute search
    RabinKarp rk;
    vector<pair<int, int>> results = rk.rabinKarpSearch(text, pattern);

    // Output results
    if (results.empty()) {
        cout << "Pattern not found in the text.\n";
    }
    else {
        cout << "Pattern found at the following coordinates (row, column):\n";
        for (const auto& res : results) {
            cout << "(" << res.first << ", " << res.second << ")\n";
        }
    }

    return 0;
}
