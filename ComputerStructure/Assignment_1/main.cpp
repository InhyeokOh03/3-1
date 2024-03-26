#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>
#include <map>
#include <set>
#include <cstdlib>


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

void instructions(const string& inst, map<string, int> new_label_map, int counter);


int main(int argc, char* argv[]) {
    string filepath = "sample/";
    string filename = argv[1];
    filepath = filepath + filename;
    string line;
    ifstream file(argv[1]);



    vector<string> words;
    string word;


    while (getline(file, line)) {
        stringstream sstream(line);
            while (getline(sstream, word, ' ')) {
                word = regex_replace(word, regex("[,$]"), "");
                if (!word.empty()) {
                    words.push_back(word);
                }
            }
    }

    vector<string> data_seg;
    bool in_text = false;
    bool is_la = false;
    bool is_label = false;
    string label_name;
    int label_index = 0;
    int data_memory = 0;
    int text_memory = 0;

START:
    map<string, int> label_index_map_data;
    map<string, int> label_index_map_text;
    vector<string> text_seg;
    vector<string> instruction_seg;

    // cout << words.size() << endl;
    for (int i = 0; i < int(words.size()); i++) {
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
            label_index_map_text[words[i]] = -1;
            
            break;
        } else {
            if (words[i].back() == ':') {
                is_label = true;
                label_name = words[i];
                label_index = i;

                continue;
            } else {
                if (is_instruction(words[i])) {
                    instruction_seg.push_back(words[i]);
                }
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

    map<string, int> new_label_map;
    int counter = 0;
    int now_index = 0;
    for (const string& word : words) {
        if (word == ".text") {
            break; 
        } else if (word == ".word") {
            if (words[now_index - 1].back() == ':') {
                new_label_map[words[now_index - 1]] = 0x10000000 + 4 * counter;
            }
            counter++;
        }
        now_index++;
    }
    counter = 0;
    for (now_index;now_index < words.size();now_index++) {
        if (counter == 0) {
            new_label_map["main:"] = 0x00400000 + 4 * counter;
            counter = counter + 1;
        } else if (is_instruction(words[now_index])) {
            if (words[now_index - 1].back() == ':') {
                new_label_map[words[now_index - 1]] = 0x00400000 + 4 * (counter - 1);
            }
            counter = counter + 1;
        } else if (now_index == words.size() - 1 && words[now_index].back() == ':') {
            new_label_map[words[now_index]] = 0x00400000 + 4 * (counter - 1);
        }
    }

    int la_index = 0;
    for (int i = 0; i < words.size(); i++) {
        if (words[i] == "la") {
            int address = new_label_map[words[la_index + 2] + ':'];
            stringstream ss;
            ss << hex << address;
            string hex_number = ss.str();

            string register_ = words[la_index + 1];
            words.erase(words.begin() + la_index);
            words.erase(words.begin() + la_index);
            words.erase(words.begin() + la_index);
            if ((address << 4) > 0) {
                words.insert(words.begin() + la_index, "ori");
                words.insert(words.begin() + la_index + 1, register_);
                words.insert(words.begin() + la_index + 2, register_);
                words.insert(words.begin() + la_index + 3, "0x" + hex_number.substr(4, 7));
            }
            words.insert(words.begin() + la_index, "lui");
            words.insert(words.begin() + la_index + 1, register_);
            words.insert(words.begin() + la_index + 2, "0x" + hex_number.substr(0, 4));
            
            goto START;
        }
        la_index++;
    }


    int count = 0;
    for (const auto& word : text_seg) {
        if (is_instruction(word)) {
            count++;
        }
    }

    int text_section_size = count;
    int data_section_size = data_seg.size();

    cout << "0x" << hex << text_section_size * 4 << endl;
    cout << "0x" << hex << data_section_size * 4 << endl;
    for (int i = 0; i < int(text_seg.size()); i++) {
        if (is_instruction(text_seg[i])) {
            instructions(text_seg[i], new_label_map, counter);
        }
    }
    
    // data_seg 출력
    for (size_t i = 0;i < data_seg.size();i++) {
        if (data_seg[i].substr(0, 2) == "0x") {
            cout << data_seg[i] << endl;
            continue;
        }
        cout << "0x" <<  data_seg[i] << endl;
    }

    file.close();
    return 0;
}

void instructions(const string& inst, map<string, int> new_label_map, int counter = 0) {
    const string* ptr = &inst;
    // cout << *(ptr)<< endl;
    int rd;
    int rt;
    int rs;
    int shamt;
    string temp;
    int imm;
    int result;
    string target;
    int target_address;
    int offset;
    string address;
    int text_address;
    int data_address;

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
        result = (rd << 11) + (rt << 16) + (shamt << 6);
    } else if (inst == "srl") {
        rd = stoi(*(ptr + 1));
        rt = stoi(*(ptr + 2));
        shamt = stoi(*(ptr + 3));
        result = (rd << 11) + (rt << 16) + 2 + (shamt << 6);
    } else if (inst == "subu") {
        rd = stoi(*(ptr +1));
        rs = stoi(*(ptr +2));
        rt = stoi(*(ptr +3));
        result = (rd << 11) + (rs << 21) + (rt << 16) + 0x23;
    } else if (inst == "lui") {
        rt = stoi(*(ptr + 1));
        temp = *(ptr + 2);
        if (temp.substr(0, 2) == "0x") {
            imm = stoi(*(ptr + 2), 0, 16);
        } else {
            imm = stoi(*(ptr + 2));
        }
        result = (0xf << 26) + (rt << 16) + imm;
    } else if (inst == "ori") {
        rt = stoi(*(ptr + 1));
        rs = stoi(*(ptr + 2));
        temp = *(ptr + 3);
        // cout << temp << endl;
        if (temp.substr(0, 2) == "0x") {
            imm = stoi(*(ptr + 3), 0, 16);
        } else {
            imm = stoi(*(ptr + 3));
        }
        result = (0xd << 26) + (rs << 21) + (rt << 16) + imm;
    } else if (inst == "sltiu") {
        rt = stoi(*(ptr + 1));
        rs = stoi(*(ptr + 2));
        temp = *(ptr + 3);
        if (temp.substr(0, 2) == "0x") {
            imm = stoi(*(ptr + 3), 0, 16);
        } else {
            imm = stoi(*(ptr + 3));
        }
        result = (0xb << 26) + (rs << 21) + (rt << 16) + imm;
    } else if (inst == "addiu") {
        rt = stoi(*(ptr + 1));
        rs = stoi(*(ptr + 2));
        temp = *(ptr + 3);
        // cout << temp << endl;
        if (temp.substr(0, 2) == "0x") {
            imm = stoi(*(ptr + 3), 0, 16);
        } else {
            imm = stoi(*(ptr + 3));
        }
        // cout << imm << endl;
        result = (9 << 26) + (rs << 21) + (rt << 16) + imm;
    } else if (inst == "andi") {
        rt = stoi(*(ptr + 1));
        rs = stoi(*(ptr + 2));
        temp = *(ptr + 3);
        if (temp.substr(0, 2) == "0x") {
            imm = stoi(*(ptr + 3), 0, 16);
        } else {
            imm = stoi(*(ptr + 3));
        }
        result = (0xc << 26) + (rs << 21) + (rt << 16) + imm;
    } else if (inst == "j") {
        target = *(ptr + 1);
        target_address = new_label_map[target + ':'];
        result = (2 << 26) + (target_address >> 2);
    } else if (inst == "jal") {
        target = *(ptr + 1);
        target_address = new_label_map[target + ':'];
        result = (3 << 26) + (target_address >> 2);
    } else if (inst == "lw") {
        rt = stoi(*(ptr + 1));
        address = *(ptr + 2);
        if (address.back() == ')') {
            string register_string;
            string front_int;
            int open = address.find('(');
            int close = address.find(')');
            rs = stoi(address.substr(open + 1, close - open - 1));
            offset = stoi(address.substr(0, open));
        }
        result = (0x23 << 26) + (rs << 21) + (rt << 16) + offset;
    } else if (inst == "lb") {
        rt = stoi(*(ptr + 1));
        address = *(ptr + 2);
        if (address.back() == ')') {
            string register_string;
            string front_int;
            int open = address.find('(');
            int close = address.find(')');
            rs = stoi(address.substr(open + 1, close - open - 1));
            offset = stoi(address.substr(0, open));
        }
        result = (0x20 << 26) + (rs << 21) + (rt << 16) + offset;
    } else if (inst == "sw") {
        rt = stoi(*(ptr + 1));
        address = *(ptr + 2);
        if (address.back() == ')') {
            string register_string;
            string front_int;
            int open = address.find('(');
            int close = address.find(')');
            rs = stoi(address.substr(open + 1, close - open - 1));
            offset = stoi(address.substr(0, open));
        }
        result = (0x2b << 26) + (rs << 21) + (rt << 16) + offset;
    } else if (inst == "sb") {
        rt = stoi(*(ptr + 1));
        address = *(ptr + 2);
        if (address.back() == ')') {
            string register_string;
            string front_int;
            int open = address.find('(');
            int close = address.find(')');
            rs = stoi(address.substr(open + 1, close - open - 1));
            offset = stoi(address.substr(0, open));
        }
        result = (0x28 << 26) + (rs << 21) + (rt << 16) + offset;
    } else if (inst == "addu") {
        rd = stoi(*(ptr + 1));
        rs = stoi(*(ptr + 2));
        rt = stoi(*(ptr + 3));
        result = (rs << 21) + (rt << 16) + (rd << 11) + (0x21);
    } else if (inst == "beq") {
        rs = stoi(*(ptr + 1));
        rt = stoi(*(ptr + 2));
        target = *(ptr + 3);
        target_address = new_label_map[target + ':'];
        offset = ((0x00400000 + (counter + 1) * 4) - target_address - 4) / 4;
        result = (4 << 26) + (rs << 21) + (rt << 16) + offset;
    } else if (inst == "bne") {
        rs = stoi(*(ptr + 1));
        rt = stoi(*(ptr + 2));
        target = *(ptr + 3);
        target_address = new_label_map[target + ':'];
        offset = ((0x00400000 + (counter + 1) * 4) - target_address - 4) / 4;
        result = (5 << 26) + (rs << 21) + (rt << 16) + offset;
    }

    cout << "0x" << hex << result << endl;
    counter++;
}
