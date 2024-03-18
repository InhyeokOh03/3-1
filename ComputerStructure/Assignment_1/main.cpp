#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>

#define MAX_LINE_LENGTH 100

using namespace std;

// string readText(const string &filename) {
//     ifstream file(filename);
//     string text;
//     string line;
//     if (file.is_open()) {
//         while (getline(file, line)) {
//             text += line;
//             text += '\n';
//         }
//         file.close();
//     } else {
//         cerr << "Unable to open file: " << filename << endl;
//     }
//     return text;
// }

int main() {
    string filename = "sample/sample.s";
    // string fileText = readText(filename);
    string line;
    ifstream file(filename);

    while (getline(file, line)) {
        // cout << line << endl;
        vector<string> words;
        string word;
        stringstream sstream(line);
            while (getline(sstream, word, ' ')) {
                word = regex_replace(word, std::regex("[,:]"), "");
                if (!word.empty()) {
                    words.push_back(word);


                    
                    cout << word << endl;


                    
                }
            }
    }



    return 0;
}