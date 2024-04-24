#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>
#include <set>
#include <cstdlib>

using namespace std;

int instructions(unsigned int content, int Register[32], int* register_pointer, unsigned int PC, unsigned int data, unsigned int text, unsigned int* data_arr_ptr) {
    unsigned int op;
    op = content >> 26;

    if (op == 0) {
        unsigned int rs = ((content << 6) & 0xf8000000) >> 27;
        unsigned int rt = ((content << 11) & 0xf8000000) >> 27;
        unsigned int rd = ((content << 16) & 0xf8000000) >> 27;
        unsigned int shamt = ((content << 21) & 0xf8000000) >> 27;
        unsigned int funct = ((content << 26) & 0xfc000000) >> 26;

        if (funct == 33) {
            Register[rd] = Register[rs] + Register[rt];
            PC += 4;
        } else if (funct == 36) {
            Register[rd] = Register[rs] & Register[rt];
            PC += 4;
        } else if (funct == 39) {
            Register[rd] = ~(Register[rs] | Register[rt]);
            PC += 4;
        } else if (funct == 37) {
            Register[rd] = Register[rs] | Register[rt];
            PC += 4;
        } else if (funct == 43) {
            if (Register[rs] < Register[rt]) {
                Register[rd] == 1;
                PC += 4;
            } else {
                Register[rd] == 0;
                PC += 4;
            }
        } else if (funct == 35) {
            if (Register[rs] >= Register[rt]) {
                Register[rd] = Register[rs] - Register[rt];
            } else {
                Register[rd] = ~(Register[rt] - Register[rs]) + 1;
            }
            PC += 4;
        } else if (funct == 8) {
            PC = Register[31];
            // PC += 4;
        } else if (funct == 0) {
            Register[rd] = Register[rt] << shamt;
            PC += 4;
        } else if (funct == 2) {
            Register[rd] = Register[rt] >> shamt;
            PC += 4;
        }
    } else if (op == 2) {
        PC += 4;
        // cout << "PC : 0x" << hex << PC << endl;
        // Register[31] = PC;
        unsigned int target;
        unsigned int temp;
        target = (content & 0x03ffffff);
        // cout << "target 0x" << hex << target << endl;
        temp = PC & 0xf0000000;
        PC = temp | (target << 2);
    } else if (op == 3) {
        PC += 4;
        // cout << "PC : 0x" << hex << PC << endl;
        Register[31] = PC;
        unsigned int target;
        unsigned int temp;
        target = (content & 0x03ffffff);
        // cout << "target 0x" << hex << target << endl;
        temp = PC & 0xf0000000;
        PC = temp | (target << 2);
        // cout << "PC : 0x" << hex << PC << endl;
    } else {
        unsigned int rs = ((content << 6) & 0xf8000000) >> 27;
        unsigned int rt = ((content << 11) & 0xf8000000) >> 27;
        unsigned int imm = content & 0x0000ffff;
        if (op == 9) {
            if ((imm & 0x8000) == 0x8000) {
                imm |= 0xffff0000;
            }
            // cout << "imm : 0x" << hex << imm << endl;
            // cout << "R[rs] : 0x" << hex << Register[rs] << endl;
            Register[rt] = Register[rs] + imm;
            // cout << "R[rt] (더해진 값) : 0x" << hex << Register[rt] << endl;
            PC += 4;
        } else if (op == 12) {
            Register[rt] = Register[rs] & imm;
            PC += 4;
        } else if (op == 4) {
            if (Register[rs] == Register[rt]) {
                PC = PC + 4 + (imm * 4);
            } else {
                PC +=4;
            }
        } else if (op == 5) {
            // cout << "R[rs] : 0x" << hex << Register[rs] << ", R[rt] : 0x" << hex << Register[rt] << endl; 
            if (Register[rs] != Register[rt]) {
                cout << "PC : 0x" << hex << PC << endl;
                PC = PC + 4 + (imm * 4);
                cout << "PC : 0x" << hex << PC << endl;
            } else {
                PC += 4;
            }
            // cout << "PC : 0x" << hex << PC << endl;
        } else if (op == 15) {
            Register[rt] = imm << 16;
            PC += 4;
        } else if (op == 35) {
            int addr = (Register[rs] + imm) - 0x10000000;
            // cout << addr << endl;
            Register[rt] = *(data_arr_ptr + addr);
            PC += 4;
        } else if (op == 13) {
            Register[rt] = Register[rs] | imm;
            PC += 4;
        } else if (op == 11) {
            if ((imm & 0x8000) == 0x8000) {
                imm |= 0xffff0000;
            }

            if (Register[rs] < imm) {
                Register[rt] = 1;
            } else {
                Register[rt] = 0;
            }
            PC += 4;
        } else if (op == 43) {
            int addr = rs + imm;
            *(data_arr_ptr + (addr / 4)) = Register[rt];
            PC += 4;
        } else if (op == 32) {
            int addr = rs + imm;
            unsigned int temp =  *(data_arr_ptr + (addr / 4)) & 0xff;
            if (addr % 4 == 0) {
                Register[rt] = (Register[rt] & 0x00ffffff) |  (temp << 24);
            } else if (addr % 4 == 1) {
                Register[rt] = (Register[rt] & 0xff00ffff) |  (temp << 16);
            } else if (addr % 4 == 2) {
                Register[rt] = (Register[rt] & 0xffff00ff) |  (temp << 8);
            } else if (addr % 4 == 3) {
                Register[rt] = (Register[rt] & 0xffffff00) |  temp;
            }
            PC += 4;
        } else if (op == 40) {
            int addr = rs + imm;
            unsigned int temp = Register[rt] & 0xff;
            if (addr % 4 == 0) {
                *(data_arr_ptr + (addr / 4)) = (*(data_arr_ptr + (addr / 4)) & 0x00ffffff) | (temp << 24);
            } else if (addr % 4 == 1) {
                *(data_arr_ptr + (addr / 4)) = (*(data_arr_ptr + (addr / 4)) & 0xff00ffff) | (temp << 16);
            } else if (addr % 4 == 2) {
                *(data_arr_ptr + (addr / 4)) = (*(data_arr_ptr + (addr / 4)) & 0xffff00ff) | (temp << 8);
            } else if (addr % 4 == 3) {
                *(data_arr_ptr + (addr / 4)) = (*(data_arr_ptr + (addr / 4)) & 0xffffff00) | temp;        
            }
            PC += 4;
        }
    }

    return PC;
}

int main(int argc, char* argv[]) {

    string filename = argv[argc - 1];
    string filepath = filename;
    int n = 2147483647;
    string m;
    bool d = false;

    for (int i = 1; i < argc - 1; ++i) {
        if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            n = stoi(argv[i + 1]);
            ++i;
        } else if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
            m = argv[i + 1];
        ++i;
        } else if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
            d = true;
        }
    }

    ifstream file(filepath);

    vector<string> contents;
    string line;
    unsigned int PC = 0x400000;
    unsigned int text = 0x400000;
    unsigned int data = 0x10000000;
    int Register[32] = {0x0};
    int* register_pointer = Register;

    while (getline(file, line)) {
        contents.push_back(line);
    }
    file.close();

    int text_count;
    int data_count;

    text_count = stoi(contents[0], nullptr, 16) / 4;
    data_count = stoi(contents[1], nullptr, 16) / 4;

    // if (text_count <= n) {
    //     n = text_count;
    // }

    // 주어진 배열 크기 계산
    unsigned int text_arr_size = (data - text) / 4;
    unsigned int data_arr_size = (0x7ffffffc - data) / 4;

    // 동적 할당을 위한 포인터 선언
    unsigned int* text_arr_ptr = new unsigned int[text_arr_size];
    unsigned int* data_arr_ptr = new unsigned int[data_arr_size];

    // 모든 값을 0으로 초기화
    memset(text_arr_ptr, 0, text_arr_size * sizeof(unsigned int));
    memset(data_arr_ptr, 0, data_arr_size * sizeof(unsigned int));

    for (int i = 0; i < data_count; i++) {
        *(data_arr_ptr + i) = stoul(contents[2 + text_count + i], nullptr, 16);
    }

    for (int i = 0; i < text_count; i++) {
        *(text_arr_ptr + i) = stoul(contents[2 + i], nullptr, 16);
    }

    unsigned int content;
    int counter = 0;

    // cout << "N : " << dec << n << endl;

    while (PC < text + text_count * 4) {
        // cout << "PC :0x" << hex << PC << ", until :0x" << hex << text + text_count * 4 << endl;
        cout << "n / counter : " << n << "/" << counter << endl;
        if (counter == n && n < 2147483647) {
            cout << "boom!!" << endl;
            break;
        }
        int index;
        index = (PC - 4194304) / 4;
        // cout << "------------Index : " << index << endl;
        content = *(text_arr_ptr + index);
        // cout << "Content : 0x" << hex << content << endl;
        // cout << i << endl;

        PC = instructions(content, Register, register_pointer, PC, data, text, data_arr_ptr);

        if (d) {
            cout << "Current register values:\n" << "----------------------------------" << endl;
            cout << "PC: 0x" << hex << PC << "\nRegistors:" << endl;
            for (int i = 0; i < 32; i++) {
                cout << "R" << dec << i << ": 0x" << hex << Register[i] << endl;
            }
        }

        counter += 1;
        // cout << "--------------" << endl;
    }

    if (!d) {
        cout << "Current register values:\n" << "----------------------------------" << endl;
        cout << "PC: 0x" << hex << PC << "\nRegistors:" << endl;
        for (int i = 0; i < 32; i++) {
            cout << "R" << dec << i << ": 0x" << hex << Register[i] << endl;
        }
    }

    if (!m.empty()) {
        size_t colon_pos = m.find(':');

        string address1_str = m.substr(0, colon_pos);
        string address2_str = m.substr(colon_pos + 1);

        int address1 = stoul(address1_str, nullptr, 16);
        int address2 = stoul(address2_str, nullptr, 16);

        int count = (address2 - address1) / 4;
        cout << "\nMemory content [0x" << hex << address1 << "..0x" << hex << address2 << "]:\n-----------------------------------" << endl; 

        if (address1 < 268435456) {
            int start_index = (address1 - 4194304) / 4;
            text += start_index * 4;
            for (int i = 0; i < count; i++) {
                cout << "0x" << hex << text << ": 0x" << hex << *(text_arr_ptr + i) << endl;
                text += 4;
            }
        } else {
            int start_index = (address1 - 268435456) / 4;
            data += start_index * 4;
            for (int i = 0; i < count; i++) {
                cout << "0x" << hex << data << ": 0x" << hex << *(data_arr_ptr + i) << endl;
                data += 4;
            }
        }
    }

    delete[] data_arr_ptr;
    delete[] text_arr_ptr;
    return 0;   
}