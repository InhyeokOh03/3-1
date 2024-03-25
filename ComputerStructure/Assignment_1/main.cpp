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
#include <cstdint> 

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

string is_resgister(string value) {
    if (value[0] == '$') {
        return value.substr(1);
    } else {
        return value;
    }
}

bitset<32> string_to_binary(const string str) {
    int num = stoi(str);
    return bitset<32>(num);
}

string bitset_to_hex(const bitset<32>& bitset_val) {
    unsigned long long int_val = bitset_val.to_ulong();
    
    stringstream ss;
    ss << hex << int_val;
    string hex_str = ss.str();
    
    return hex_str;
}

bitset<32> hex_to_binary(string hex_string) {
    stringstream ss;
    ss << hex << hex_string;
    unsigned int num;
    ss >> num;
    bitset<32> bits(num);
    return bits;
}

string pointer_to_string(const string* ptr) {
    stringstream ss;
    ss << ptr;
    string ptr_string = ss.str();

    if (ptr_string.substr(0, 2) == "0x") {
        ptr_string = ptr_string.substr(2);
    }
    return ptr_string;
}

void instructions(const string& inst, map<string, const string*> label_map) {
    const string* ptr = &inst;
    cout << *ptr << endl;

    const string* rt_ptr;
    const string* rs_ptr;
    const string* imm_ptr;
    const string* rd_ptr;
    const string* target_ptr;
    const string* shamt_ptr;
    bitset<32> shifted_rt_binary;
    bitset<32> shifted_rs_binary;
    bitset<32> shifted_imm_binary;
    bitset<32> shifted_rd_binary;
    bitset<32> shifted_target_binary;
    bitset<32> shifted_shamt_binary;

    bitset<32> result;

    if (inst == "addiu") {
        rt_ptr = (ptr + 1);
        rs_ptr = (ptr + 2);
        imm_ptr = (ptr + 3);

        string rt = is_resgister(*rt_ptr);
        string rs = is_resgister(*rs_ptr);
        string imm = *imm_ptr;

        bitset<32> rt_binary = string_to_binary(rt);
        bitset<32> rs_binary = string_to_binary(rs);
        bitset<32> imm_binary = string_to_binary(imm);

        shifted_rt_binary = rt_binary << 16;
        shifted_rs_binary = rs_binary << 21;
        shifted_imm_binary = imm_binary << 0;

        bitset<32> addiu_binary = string_to_binary("9");
        bitset<32> shifted_addiu_binary = addiu_binary << 26;

        result = shifted_addiu_binary | shifted_rt_binary | shifted_rs_binary | shifted_imm_binary;
    } else if (inst == "addu") {
        rd_ptr = (ptr + 1);
        rs_ptr = (ptr + 2);
        rt_ptr = (ptr + 3);
        string rd = is_resgister(*rd_ptr);
        string rs = is_resgister(*rs_ptr);
        string rt = is_resgister(*rt_ptr);
        bitset<32> rd_binary = string_to_binary(rd);
        bitset<32> rs_binary = string_to_binary(rs);
        bitset<32> rt_binary = string_to_binary(rt);
        shifted_rd_binary = rd_binary << 11;
        shifted_rs_binary = rs_binary << 21;
        shifted_rt_binary = rt_binary << 16;

        bitset<32> addu_binary = string_to_binary("0");
        bitset<32> shifted_addu_binary = addu_binary << 26;

        bitset<32> addu_21 = hex_to_binary("21");

        result = shifted_addu_binary | shifted_rt_binary | shifted_rs_binary | shifted_rd_binary | addu_21;
    } else if (inst == "and") {
        rd_ptr = (ptr + 1);
        rs_ptr = (ptr + 2);
        rt_ptr = (ptr + 3);
        string rd = is_resgister(*rd_ptr);
        string rs = is_resgister(*rs_ptr);
        string rt = is_resgister(*rt_ptr);
        bitset<32> rd_binary = string_to_binary(rd);
        bitset<32> rs_binary = string_to_binary(rs);
        bitset<32> rt_binary = string_to_binary(rt);
        shifted_rd_binary = rd_binary << 11;
        shifted_rs_binary = rs_binary << 21;
        shifted_rt_binary = rt_binary << 16;

        bitset<32> addu_binary = string_to_binary("0");
        bitset<32> shifted_addu_binary = addu_binary << 26;

        bitset<32> addu_24 = hex_to_binary("24");

        result = shifted_addu_binary | shifted_rt_binary | shifted_rs_binary | shifted_rd_binary | addu_24;
    } else if (inst == "andi") {
        rt_ptr = (ptr + 1);
        rs_ptr = (ptr + 2);
        imm_ptr = (ptr + 3);

        string rt = is_resgister(*rt_ptr);
        string rs = is_resgister(*rs_ptr);
        string imm = *imm_ptr;

        bitset<32> rt_binary = string_to_binary(rt);
        bitset<32> rs_binary = string_to_binary(rs);
        bitset<32> imm_binary = string_to_binary(imm);

        shifted_rt_binary = rt_binary << 16;
        shifted_rs_binary = rs_binary << 21;
        shifted_imm_binary = imm_binary << 0;

        bitset<32> addiu_binary = hex_to_binary("c");
        bitset<32> shifted_addiu_binary = addiu_binary << 26;

        result = shifted_addiu_binary | shifted_rt_binary | shifted_rs_binary | shifted_imm_binary;
    } else if (inst == "beq") {
        // 특수
    } else if (inst == "bne") {
        // rs_ptr = (ptr + 1);
        // rt_ptr = (ptr + 2);
        // imm_ptr = (ptr + 3);

        // string rt = is_resgister(*rt_ptr);
        // string rs = is_resgister(*rs_ptr);
        // string imm = pointer_to_string(imm_ptr);

        // bitset<32> rt_binary = string_to_binary(rt);
        // bitset<32> rs_binary = string_to_binary(rs);
        // bitset<32> imm_binary = string_to_binary(imm);

        // shifted_rt_binary = rt_binary << 16;
        // shifted_rs_binary = rs_binary << 21;
        // // shifted_imm_binary = imm_binary << 0;

        // bitset<32> addiu_binary = string_to_binary("5");
        // bitset<32> shifted_addiu_binary = addiu_binary << 26;

        // result = shifted_addiu_binary | shifted_rt_binary | shifted_rs_binary | shifted_imm_binary;

    } else if (inst == "j") {
        target_ptr = (ptr + 1);
        const string* target = label_map[*target_ptr];
        cout << target << endl;
        string target = *target;
        cout << target << endl;
        // bitset<32> target_binary = string_to_binary(target);
        // shifted_target_binary = target_binary << 0;

        bitset<32> j_binary = string_to_binary("2");
        bitset<32> shifted_j_binary = j_binary << 26;


        result = shifted_target_binary | shifted_j_binary;
    } else if (inst == "jal") {
        // j
    } else if (inst == "jr") {

    } else if (inst == "lui") {
        rt_ptr = (ptr + 1);
        imm_ptr = (ptr + 2);

        string rt = is_resgister(*rt_ptr);
        string imm = *imm_ptr;

        bitset<32> rt_binary = string_to_binary(rt);
        bitset<32> imm_binary = string_to_binary(imm);

        shifted_rt_binary = rt_binary << 16;
        shifted_imm_binary = imm_binary << 0;

        bitset<32> addiu_binary = hex_to_binary("f");
        bitset<32> shifted_addiu_binary = addiu_binary << 26;

        result = shifted_addiu_binary | shifted_rt_binary | shifted_rs_binary | shifted_imm_binary;
    } else if (inst == "lw") {
        // rt_ptr = (ptr + 1);
        // imm_ptr = (ptr + 2); // Address

        // string rt = is_resgister(*rt_ptr);
        // string imm = *imm_ptr;

        // bitset<32> rt_binary = string_to_binary(rt);
        // bitset<32> imm_binary = string_to_binary(imm);

        // shifted_rt_binary = rt_binary << 16;
        // shifted_imm_binary = imm_binary << 0;

        // bitset<32> addiu_binary = string_to_binary("23");
        // bitset<32> shifted_addiu_binary = addiu_binary << 26;

        // result = shifted_addiu_binary | shifted_rt_binary | shifted_rs_binary | shifted_imm_binary;
  
    } else if (inst == "lb") {
        
    } else if (inst == "nor") {
        rd_ptr = (ptr + 1);
        rs_ptr = (ptr + 2);
        rt_ptr = (ptr + 3);
        string rd = is_resgister(*rd_ptr);
        string rs = is_resgister(*rs_ptr);
        string rt = is_resgister(*rt_ptr);
        bitset<32> rd_binary = string_to_binary(rd);
        bitset<32> rs_binary = string_to_binary(rs);
        bitset<32> rt_binary = string_to_binary(rt);
        shifted_rd_binary = rd_binary << 11;
        shifted_rs_binary = rs_binary << 21;
        shifted_rt_binary = rt_binary << 16;

        bitset<32> addu_binary = string_to_binary("0");
        bitset<32> shifted_addu_binary = addu_binary << 26;

        bitset<32> addu_27 = hex_to_binary("27");

        result = shifted_addu_binary | shifted_rt_binary | shifted_rs_binary | shifted_rd_binary | addu_27;

    } else if (inst == "or") {
        rd_ptr = (ptr + 1);
        rs_ptr = (ptr + 2);
        rt_ptr = (ptr + 3);
        string rd = is_resgister(*rd_ptr);
        string rs = is_resgister(*rs_ptr);
        string rt = is_resgister(*rt_ptr);
        bitset<32> rd_binary = string_to_binary(rd);
        bitset<32> rs_binary = string_to_binary(rs);
        bitset<32> rt_binary = string_to_binary(rt);
        shifted_rd_binary = rd_binary << 11;
        shifted_rs_binary = rs_binary << 21;
        shifted_rt_binary = rt_binary << 16;

        bitset<32> addu_binary = string_to_binary("0");
        bitset<32> shifted_addu_binary = addu_binary << 26;

        bitset<32> addu_25 = hex_to_binary("25");

        result = shifted_addu_binary | shifted_rt_binary | shifted_rs_binary | shifted_rd_binary | addu_25;

    } else if (inst == "ori") {
        rt_ptr = (ptr + 1);
        rs_ptr = (ptr + 2);
        imm_ptr = (ptr + 3);

        string rt = is_resgister(*rt_ptr);
        string rs = is_resgister(*rs_ptr);
        string imm = *imm_ptr;

        bitset<32> rt_binary = string_to_binary(rt);
        bitset<32> rs_binary = string_to_binary(rs);
        bitset<32> imm_binary = string_to_binary(imm);

        shifted_rt_binary = rt_binary << 16;
        shifted_rs_binary = rs_binary << 21;
        shifted_imm_binary = imm_binary << 0;

        bitset<32> addiu_binary = string_to_binary("d");
        bitset<32> shifted_addiu_binary = addiu_binary << 26;

        result = shifted_addiu_binary | shifted_rt_binary | shifted_rs_binary | shifted_imm_binary;
    } else if (inst == "sltiu") {
        rt_ptr = (ptr + 1);
        rs_ptr = (ptr + 2);
        imm_ptr = (ptr + 3);

        string rt = is_resgister(*rt_ptr);
        string rs = is_resgister(*rs_ptr);
        string imm = *imm_ptr;

        bitset<32> rt_binary = string_to_binary(rt);
        bitset<32> rs_binary = string_to_binary(rs);
        bitset<32> imm_binary = string_to_binary(imm);

        shifted_rt_binary = rt_binary << 16;
        shifted_rs_binary = rs_binary << 21;
        shifted_imm_binary = imm_binary << 0;

        bitset<32> addiu_binary = hex_to_binary("b");
        bitset<32> shifted_addiu_binary = addiu_binary << 26;

        result = shifted_addiu_binary | shifted_rt_binary | shifted_rs_binary | shifted_imm_binary;
    } else if (inst == "sltu") {
        rd_ptr = (ptr + 1);
        rs_ptr = (ptr + 2);
        rt_ptr = (ptr + 3);
        string rd = is_resgister(*rd_ptr);
        string rs = is_resgister(*rs_ptr);
        string rt = is_resgister(*rt_ptr);
        bitset<32> rd_binary = string_to_binary(rd);
        bitset<32> rs_binary = string_to_binary(rs);
        bitset<32> rt_binary = string_to_binary(rt);
        shifted_rd_binary = rd_binary << 11;
        shifted_rs_binary = rs_binary << 21;
        shifted_rt_binary = rt_binary << 16;

        bitset<32> addu_binary = string_to_binary("0");
        bitset<32> shifted_addu_binary = addu_binary << 26;

        bitset<32> addu_2b = hex_to_binary("2b");

        result = shifted_addu_binary | shifted_rt_binary | shifted_rs_binary | shifted_rd_binary | addu_2b;

    } else if (inst == "sll") {
        rd_ptr = (ptr + 1);
        rt_ptr = (ptr + 2);
        shamt_ptr = (ptr + 3);
        string rd = is_resgister(*rd_ptr);
        string rt = is_resgister(*rt_ptr);
        string shamt = is_resgister(*shamt_ptr);
        bitset<32> rd_binary = string_to_binary(rd);
        bitset<32> rt_binary = string_to_binary(rt);
        bitset<32> shamt_binary = string_to_binary(shamt);
        shifted_rd_binary = rd_binary << 11;
        shifted_rt_binary = rt_binary << 21;
        shifted_shamt_binary = shamt_binary << 6;

        result = shifted_rt_binary | shifted_shamt_binary | shifted_rd_binary;

    } else if (inst == "srl") {
        rd_ptr = (ptr + 1);
        rt_ptr = (ptr + 2);
        shamt_ptr = (ptr + 3);
        string rd = is_resgister(*rd_ptr);
        string rt = is_resgister(*rt_ptr);
        string shamt = is_resgister(*shamt_ptr);
        bitset<32> rd_binary = string_to_binary(rd);
        bitset<32> rt_binary = string_to_binary(rt);
        bitset<32> shamt_binary = string_to_binary(shamt);
        shifted_rd_binary = rd_binary << 11;
        shifted_rt_binary = rt_binary << 21;
        shifted_shamt_binary = shamt_binary << 6;

        bitset<32> addu_binary = string_to_binary("0");
        bitset<32> shifted_addu_binary = addu_binary << 26;

        bitset<32> addu_2 = string_to_binary("2");

        result = shifted_addu_binary | shifted_rt_binary | shifted_shamt_binary | shifted_rd_binary | addu_2;
    } else if (inst == "sw") {
        
    } else if (inst == "sb") {
        
    } else if (inst == "subu") {
        rd_ptr = (ptr + 1);
        rs_ptr = (ptr + 2);
        rt_ptr = (ptr + 3);
        string rd = is_resgister(*rd_ptr);
        string rs = is_resgister(*rs_ptr);
        string rt = is_resgister(*rt_ptr);
        bitset<32> rd_binary = string_to_binary(rd);
        bitset<32> rs_binary = string_to_binary(rs);
        bitset<32> rt_binary = string_to_binary(rt);
        shifted_rd_binary = rd_binary << 11;
        shifted_rs_binary = rs_binary << 21;
        shifted_rt_binary = rt_binary << 16;

        bitset<32> addu_binary = string_to_binary("0");
        bitset<32> shifted_addu_binary = addu_binary << 26;

        bitset<32> addu_23 = hex_to_binary("23");

        result = shifted_addu_binary | shifted_rt_binary | shifted_rs_binary | shifted_rd_binary | addu_23;
    }



    cout << "0x" << bitset_to_hex(result) << endl;
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
        } else if (in_text && i + 1 == int(words.size()) && words[i].back() == ':') {
            // cout << words[i] << endl;
            label_index_map_text[regex_replace(words[i], regex("[:]"), "")] = -1;
            break;
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
                // const string& last_element = data_seg.back();
                // cout << last_element << " : " << &last_element << endl;
                label_index_map_data[words[label_index]] = data_seg.size();
                // cout << data_seg[data_seg.size() - 1] << endl;
            } else {
                // const string& last_element = text_seg.back();
                // cout << last_element << " : " << &last_element << endl;
                label_index_map_text[words[label_index]] = text_seg.size() - 1;
                // cout << text_seg[text_seg.size() - 1] << endl;
            }
            is_label = false;
        }
    }

    map<string, const string*> label_map;
    for (const auto& entry : label_index_map_data) {
        label_map[entry.first] = &data_seg[entry.second - 1];
    }
    for (const auto& entry : label_index_map_text) {
        if (entry.second == -1) {
            // cout << entry.first << " : " << text_seg.back() << " : " << &text_seg.back() << " -> " << &text_seg.back() + 1 << endl;
            label_map[entry.first] = &text_seg[entry.second] + 1;
        } else {
            label_map[entry.first] = &text_seg[entry.second];
        }
    }


    cout << "======================================" << endl;
    // label_map 출력
    for (const auto& pair : label_map) {
        cout << pair.first << " : " << pair.second << endl;
    }
    cout << "======================================" << endl;
    // // data_seg 출력
    // for (size_t i = 0;i < data_seg.size();i++) {
    //     cout << data_seg[i] << " : " << &(data_seg[i])<< endl;
    // }
    // text_seg 출력
    // cout << "======================================" << endl;
    // for (size_t i = 0;i < text_seg.size();i++) {
    //     cout << text_seg[i] << " : " << &(text_seg[i]) << " : " << *(&text_seg[i]) << endl;
    // }
    // cout << "======================================" << endl;

    for (int i = 0; i < int(text_seg.size()); i++) {
        if (is_instruction(text_seg[i])) {
            // cout << text_seg[i] << " : " << &text_seg[i] << endl;
            instructions(text_seg[i], label_map);
        }
    }
    

    return 0;
}