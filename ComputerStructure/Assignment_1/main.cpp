#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>
#include <map>
#include <set>

using namespace std;

bool is_instruction(const string &word) {
    static const set<string> instructions = {
        "addiu", "addu", "and", "andi", "beq", "bne", "j",
        "jal", "jr", "lui", "lw", "la", "nor", "or",
        "ori", "sltiu", "sltu", "sll", "srl", "sw", "subu",
        "lb", "sb"
    };
    return instructions.count(word) > 0;
}

void instructions(const string& inst, map<string, string*> label_map);

int main() {
    string filename = "sample/sample.s";
    string line;
    ifstream file(filename);
    vector<string> words;
    string word;
    bool is_la = false;
    while (getline(file, line)) {
        stringstream sstream(line);
            while (getline(sstream, word, ' ')) {
                word = regex_replace(word, regex("[,$]"), "");
                if (!word.empty()) {
                    words.push_back(word);
                }
            }
    }

    map<string, int> label_index_map_data;
    map<string, int> label_index_map_text;
    vector<string> data_seg;
    vector<string> text_seg;
    bool in_text = false;

    bool is_label = false;
    string label_name;
    int label_index = 0;

    for (int i = 0; i < int(words.size()); i++) {
        // cout << i << " : " << words[i] << endl;
        if (words[i] == ".data") {
            continue;
        } else if (words[i] == ".text") {
            in_text = true;
            continue;
        }

        if (!in_text) {
            if (words[i].back() == ':') { //label
                is_label = true;
                label_name = words[i];
                label_index = i;
                continue;
            } else {
                i++;
                data_seg.push_back(words[i]);
            }
        } else if (in_text && i + 1 == int(words.size()) && words[i].back() == ':') {
            // cout << words[i] << endl;
            label_index_map_text[words[i]] = -1;
            break;
        } else {
            if (words[i].back() == ':') {
                is_label = true;
                label_name = words[i];
                label_index = i;
                continue;
            } else {
                text_seg.push_back(words[i]);
            }
        }

        if (is_label) {
            if (!in_text) {
                label_index_map_data[words[label_index]] = data_seg.size();
            } else {
                label_index_map_text[words[label_index]] = text_seg.size() - 1;
            }
            is_label = false;
        }
    }

    map<string, string*> label_map;
    for (const auto& entry : label_index_map_data) {
        label_map[(entry.first.substr(0, entry.first.size() - 1))] = &data_seg[entry.second - 1];
    }
    for (const auto& entry : label_index_map_text) {
        if (entry.second == -1) {
            // cout << entry.first << " : " << text_seg.back() << " : " << &text_seg.back() << " -> " << &text_seg.back() + 1 << endl;
            label_map[entry.first.substr(0, entry.first.size() - 1)] = &text_seg[entry.second] + 1;
        } else {
            label_map[entry.first.substr(0, entry.first.size() - 1)] = &text_seg[entry.second];
        }
    }


    // la 확인 코드 필요
    if (!is_la) {
        for (const auto& word: text_seg) {
            if (word == "la") {
                is_la = true;
                // 분기 존재 여부 확인
                // goto ?
            }
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
    for (size_t i = 0;i < text_seg.size();i++) {
        // cout << text_seg[i] << " : " << &(text_seg[i]) << " : " << *(&text_seg[i]) << endl;
        cout << text_seg[i] << endl;
    }
    cout << "======================================" << endl;
    int count = 0;
    for (const auto& word : text_seg) {
        if (is_instruction(word)) {
            count++;
        }
    }
    string text_section_size = to_string(count);
    cout << text_section_size << endl;
    string data_section_size = to_string(data_seg.size());
    cout << data_section_size << endl;
    cout << "===" << endl;
    for (int i = 0; i < int(text_seg.size()); i++) {
        if (is_instruction(text_seg[i])) {
            // cout << text_seg[i] << " : " << &text_seg[i] << endl;
            instructions(text_seg[i], label_map);
        }
    }
    

    return 0;
}

void instructions(const string& inst, map<string, string*> label_map) {
    const string* ptr = &inst;
    cout << *(ptr)<< endl;
    int rd;
    int rt;
    int rs;
    int shamt;
    int imm;
    int result;
    int target;

    if (inst == "and") {
        rd = stoi(*(ptr + 1));
        rs = stoi(*(ptr + 2));
        rt = stoi(*(ptr + 3));
        result = (rd << 11) + (rs << 21) + (rt <<16) + 0x24;
    } else if (inst == "jr") {
        rs = stoi(*(ptr + 1));
        result = (rs << 21) + 8;
    } else if (inst == "nor") {
        rd = stoi(*(ptr +1));
        rs = stoi(*(ptr +2));
        rt = stoi(*(ptr +3));
        result = (rd << 11) + (rs << 21) + (rt << 16) + 0x27;
    } else if (inst == "or") {
        rd = stoi(*(ptr +1));
        rs = stoi(*(ptr +2));
        rt = stoi(*(ptr +3));
        result = (rd << 11) + (rs << 21) + (rt << 16) + 0x25;
    } else if (inst == " sltu") {
        rd = stoi(*(ptr +1));
        rs = stoi(*(ptr +2));
        rt = stoi(*(ptr +3));
        result = (rd << 11) + (rs << 21) + (rt << 16) + 0x2b;
    } else if (inst == "sll") {
        rd = stoi(*(ptr + 1));
        rt = stoi(*(ptr + 2));
        shamt = stoi(*(ptr + 3));
        result = (rd << 11) + (rt << 16);
    } else if (inst == "srl") {
        rd = stoi(*(ptr + 1));
        rt = stoi(*(ptr + 2));
        shamt = stoi(*(ptr + 3));
        result = (rd << 11) + (rt << 16) + 2;
    } else if (inst == "subu") {
        rd = stoi(*(ptr +1));
        rs = stoi(*(ptr +2));
        rt = stoi(*(ptr +3));
        result = (rd << 11) + (rs << 21) + (rt << 16) + 0x23;
    } else if (inst == "lui") {
        rt = stoi(*(ptr + 1));
        imm = stoi(*(ptr + 2));
        result = (0xf << 26) + (rt << 16) + imm;
    } else if (inst == "ori") {
        rt = stoi(*(ptr + 1));
        rs = stoi(*(ptr + 2));
        imm = stoi(*(ptr + 3));
        result = (0xd << 26) + (rs << 21) + (rt << 16) + imm;
    } else if (inst == "sltiu") {
        rt = stoi(*(ptr + 1));
        rs = stoi(*(ptr + 2));
        imm = stoi(*(ptr + 3));
        result = (0xb << 26) + (rs << 21) + (rt << 16) + imm;
    } else if (inst == "addiu") {
        rt = stoi(*(ptr + 1));
        rs = stoi(*(ptr + 2));
        imm = stoi(*(ptr + 3));
        result = (9 << 26) + (rs << 21) + (rt << 16) + imm;
    } else if (inst == "andi") {
        rt = stoi(*(ptr + 1));
        rs = stoi(*(ptr + 2));
        imm = stoi(*(ptr + 3));
        result = (0xc << 26) + (rs << 21) + (rt << 16) + imm;
    } else if (inst == "j") {
        target = stoi(*(ptr + 1));
        result = (3 << 26) + (target >> 2);
    }

    cout << "0x" << hex << result << endl;
}
