#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>
#include <map>

#define MAX_LINE_LENGTH 100

using namespace std;

bool contains_hex_prefix(const string& str) {
    return str.find("0x") != string::npos;
}

string remove_hex_prefix(const string& str) {
    if (str.substr(0, 2) == "0x") {
        return str.substr(2);
    } else {
        return str;
    }
}

int instructions() {
    return 0;
}

int main() {
    string filename = "sample/sample.s";
    string line;
    ifstream file(filename);
    vector<string> words;
    string word;
    while (getline(file, line)) {
        stringstream sstream(line);
            while (getline(sstream, word, ' ')) {
                word = regex_replace(word, regex("[,:]"), "");
                if (!word.empty()) {
                    words.push_back(word);
                }
            }
    }

    // 텍스트 출력하기
    // for (int i = 0; i < int(words.size()); i++) {
    //     cout << words[i] << " : " << &words[i] << endl;
    // }

    // cout << "======================================" << endl;

    map<string, string*> label_map;
    vector<string> data_seg;
    int instruction_index = 0;
    for (int i = 0; i < int(words.size()); i++) {
        if (words[i] == ".data") {
        } else if (words[i] == ".text") {
            instruction_index = i + 1;
            break;
        } else if (words[i] != ".word") {
            i += 2;
            if (contains_hex_prefix(words[i])) {
                data_seg.push_back(remove_hex_prefix(words[i]));
                string *memory_address = &data_seg.back();
                label_map[words[i - 2]] = memory_address;
            } else {
                data_seg.push_back(words[i]);
                string *memory_address = &data_seg.back();          
                label_map[words[i - 2]] = memory_address;
            }
        } else {
            i++;
            if (contains_hex_prefix(words[i])) {
                data_seg.push_back(remove_hex_prefix(words[i]));
            } else {
                data_seg.push_back(words[i]);
            }
        }
    }

    // label_map 출력
    // for (const auto& pair : label_map) {
    //     cout << pair.first << " : " << pair.second << endl;
    // }

    // data_seg 출력
    // for (int i = 0;i < data_seg.size();i++) {
    //     cout << data_seg[i] << endl;
    // }

    // 다음에 시작할 부분 출력
    // cout << instruction_index << endl;

    return 0;
}