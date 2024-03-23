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
#include <set>

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

void add_to_map(map<string, string*>& label_map, const string& key, const string& value) {
    cout << key << " : " << &key << endl;
    cout << value << " : " << &value << endl;
    string* memory_address = const_cast<string*>(&value);
    label_map[key] = memory_address;
}

bool is_instruction(const string &word) {
    static const set<string> instructions = {
        "addiu", "addu", "and", "andi", "beq", "bne", "j",
        "jal", "jr", "lui", "lw", "la", "nor", "or",
        "ori", "sltiu", "sltu", "sll", "srl", "sw", "subu",
        "lb", "sb"
    };
    return instructions.count(word) > 0;
}

int instructions() {
    return 0;
}

int main() {
    string filename = "sample/sample2.s";
    string line;
    ifstream file(filename);
    vector<string> words;
    string word;
    while (getline(file, line)) {
        stringstream sstream(line);
            while (getline(sstream, word, ' ')) {
                word = regex_replace(word, regex("[,]"), "");
                if (!word.empty()) {
                    words.push_back(word);
                }
            }
    }

    // 텍스트 출력하기
    // for (int i = 0; i < int(words.size()); i++) {
    //     // cout << words[i] << " : " << &words[i] << endl;
    //     cout << words[i] << endl;
    // }

    map<string, string*> label_map;
    vector<string> data_seg;
    vector<string> text_seg;
    bool in_text = false;

    bool is_label = 0;
    string label_name;
    int label_index;

    for (int i = 0; i < int(words.size()); i++) {
        if (words[i] == ".data") {
            continue;
        } else if (words[i] == ".text") {
            in_text = true;
            continue;
        }

        if (in_text) {
            if (words[i].back() == ':') {
                string label = words[i].substr(0, words[i].size() - 1);
                add_to_map(label_map, label, to_string(text_seg.size()));
                continue;
            }
            text_seg.push_back(words[i]);
        } else {
            if (words[i] != ".word") {
                i += 2;
                if (contains_hex_prefix(words[i])) {
                    data_seg.push_back(remove_hex_prefix(words[i]));
                    add_to_map(label_map, regex_replace(words[i - 2], regex("[:]"), ""), words[i]);
                } else {
                    data_seg.push_back(words[i]);       
                    add_to_map(label_map, regex_replace(words[i - 2], regex("[:]"), ""), words[i]);
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
    }

    // label_map 출력
    // for (const auto& pair : label_map) {
    //     // cout << pair.first << " : " << pair.second << endl;
    //     cout << pair.first << " : " << pair.second << "\n" << *(pair.second) << endl;
    // }
    cout << "======================================" << endl;
    // data_seg 출력
    for (size_t i = 0;i < data_seg.size();i++) {
        cout << data_seg[i] << " : " << &(data_seg[i])<< endl;
    }
    // cout << "======================================" << endl;
    // for (size_t i = 0;i < text_seg.size();i++) {
    //     cout << text_seg[i] << endl;
    // }

    return 0;
}