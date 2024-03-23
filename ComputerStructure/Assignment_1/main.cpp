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
#include <bitset>

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

bool is_instruction(const string &word) {
    static const set<string> instructions = {
        "addiu", "addu", "and", "andi", "beq", "bne", "j",
        "jal", "jr", "lui", "lw", "la", "nor", "or",
        "ori", "sltiu", "sltu", "sll", "srl", "sw", "subu",
        "lb", "sb"
    };
    return instructions.count(word) > 0;
}

void add_to_map(map<string, string*>& label_map, const string& key, const string& value) {
    // cout << key << " : " << &key << endl;
    // cout << value << " : " << &value << endl;
    string* memory_address = const_cast<string*>(&value);
    label_map[key] = memory_address;
}

string int_to_binary_string(unsigned int num) {
    return bitset<32>(num).to_string();
}

void instructions(const string& inst) {
    // cout << inst << " : " << &inst << endl;
    const string* ptr = &inst;
    unsigned int num;

    const string* rt_ptr;
    const string* rs_ptr;
    const string* imm_ptr;


    // cout << ptr << " : " << *(ptr) << endl;
    // cout << (ptr + 1) << " : " << *(ptr + 1) << endl;
    // cout << (ptr + 2) << " : " << *(ptr + 2) << endl;
    // cout << (ptr + 3) << " : " << *(ptr + 3) << endl;
    // cout << (ptr + 4) << " : " << *(ptr + 4) << endl;


    if (inst == "addiu") {
        cout << ptr << " : " << *(ptr) << endl;
        rt_ptr = (ptr + 1);
        rs_ptr = (ptr + 2);
        imm_ptr = (ptr + 3);
        cout << "rt : " << *rt_ptr << "\nrs : " << *rs_ptr << "\nimm : " << *imm_ptr << endl;
        // num = 
        // cout << inst << " : " << (&inst) <<  endl;
        // cout << (inst + 1) << " : " << &(inst + 1) <<  endl;
    }
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

    // // 텍스트 출력하기
    // for (int i = 0; i < int(words.size()); i++) {
    //     // cout << words[i] << " : " << &words[i] << endl;
    //     cout << words[i] << endl;
    // }

    // exit(0);

    map<string, const string*> label_map;
    vector<string> data_seg;
    vector<string> text_seg;
    bool in_text = false;

    bool is_label = false;
    string label_name;
    int label_index = 0;

    for (int i = 0; i < int(words.size()); i++) {
        // cout << i << endl;
        if (words[i] == ".data") {
            continue;
        } else if (words[i] == ".text") {
            in_text = true;
            continue;
        }

        if (!in_text) {
            if (words[i].back() == ':') { //label
                is_label = true;
                label_name = regex_replace(words[i], regex("[:]"), "");
                label_index = i;
                continue;
            } else {
                i++;
                if (contains_hex_prefix(words[i])) {
                    data_seg.push_back(remove_hex_prefix(words[i]));
                } else {
                    data_seg.push_back(words[i]);
                }
            }
        } else {
            if (words[i].back() == ':') {
                is_label = true;
                label_name = regex_replace(words[i], regex("[:]"), "");
                label_index = i;
                continue;
            } else {
                text_seg.push_back(words[i]);
            }
        }

        if (is_label) {
            if (!in_text) {
                const string& last_element = data_seg.back();
                // cout << last_element << " : " << &last_element << endl;
                label_map[words[label_index]] = &last_element;
            } else {
                const string& last_element = text_seg.back();
                // cout << last_element << " : " << &last_element << endl;
                label_map[words[label_index]] = &last_element;
            }
            is_label = false;
        }
    }

    // cout << "======================================" << endl;
    // // label_map 출력
    // for (const auto& pair : label_map) {
    //     cout << pair.first << " : " << pair.second << endl;
    // }
    // cout << "======================================" << endl;
    // // data_seg 출력
    // for (size_t i = 0;i < data_seg.size();i++) {
    //     cout << data_seg[i] << " : " << &(data_seg[i])<< endl;
    // }
    // // text_seg 출력
    // cout << "======================================" << endl;
    // for (size_t i = 0;i < text_seg.size();i++) {
    //     cout << text_seg[i] << " : " << &(text_seg[i]) << " : " << *(&text_seg[i]) << endl;
    // }

    for (int i = 0; i < int(text_seg.size()); i++) {
        if (is_instruction(text_seg[i])) {
            // cout << text_seg[i] << " : " << &text_seg[i] << endl;
            instructions(text_seg[i]);
        }
    }
    

    return 0;
}