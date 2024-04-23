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
            PC += 4;
        } else if (funct == 0) {
            Register[rd] = Register[rt] << shamt;
            PC += 4;
        } else if (funct == 2) {
            Register[rd] = Register[rt] >> shamt;
            PC += 4;
        }
    } else if (op == 2) {
        unsigned int target;
        unsigned int temp;
        target = ((content << 6) & 0xfc000000) >> 26;
        PC += 4;
        temp = (PC >> 28) & 0b1111;
        PC = temp + target;
    } else if (op == 3) {
        Register[31] = PC + 4;
        unsigned int target;
        unsigned int temp;
        target = ((content << 6) & 0xfc000000) >> 26;
        PC += 4;
        temp = (PC >> 28) & 0b1111;
        PC = temp + target;
    } else {
        unsigned int rs = ((content << 6) & 0xf8000000) >> 27;
        unsigned int rt = ((content << 11) & 0xf8000000) >> 27;
        unsigned int imm = content & 0x0000ffff;
        if (op == 9) {
            Register[rt] = Register[rs] + imm;
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
            if (Register[rs] != Register[rt]) {
                PC = PC + 4 + (imm * 4);
            } else {
                PC += 4;
            }
        } else if (op == 15) {
            Register[rt] = imm << 16;
            PC += 4;
        } else if (op == 35) {
            Register[rt] = Register[rs + imm];
            PC += 4;
        } else if (op == 13) {
            Register[rt] = Register[rs] | imm;
            PC += 4;
        } else if (op == 11) {
            if (Register[rs] < imm) {
                Register[rt] = 1;
            } else {
                Register[rt] = 0;
            }
            PC += 4;
        } else if (op == 43) {
            // Register[rs + imm] = Register[rt];
            // unsigned int temp = Register[rt] & 0xffff;
            // if (imm % 4 == 0) {
            //     *(data_arr_ptr + (imm % 4)) = (*(data_arr_ptr + (imm % 4)) & 0x00ffffff) | (temp << 24);
            // } else if (imm % 4 == 1) {
            //     // *(data_ptr + (imm % 4)) = (*(data_ptr + (imm % 4)) & 0x00ffffff) | (temp << 24);
            // } else if (imm % 4 == 2) {
            //     Register[rs + (imm / 4)] &= 0xffff0000;
            //     Register[rs + (imm / 4)] = temp >> 16;
            //     Register[rs + (imm / 4) + 1] &= 0x0000ffff; 
            //     Register[rs + (imm / 4) + 1] = temp << 16;
            // } else if (imm % 4 == 3) {
            //     Register[rs + (imm / 4)] &= 0xffffff00;
            //     Register[rs + (imm / 4)] = temp >> 24;
            //     Register[rs + (imm / 4) + 1] &= 0x000000ff; 
            //     Register[rs + (imm / 4) + 1] = temp << 8;          
            // }
            PC += 4;
        } else if (op == 32) {
            Register[rt] = Register[rs + imm] & 0xff;
            PC += 4;
        } else if (op == 40) {
            int addr = rs + imm;
            int index = (addr - data) / 4;
            unsigned int temp = Register[rt] & 0xff;
            cout << addr << ", " << index << endl;
            if (addr % 4 == 0) {
                *(data_arr_ptr + index) = (*(data_arr_ptr + index) & 0x00ffffff) | (temp << 24);
            } else if (addr % 4 == 1) {
                *(data_arr_ptr + index) = (*(data_arr_ptr + index) & 0xff00ffff) | (temp << 16);
            } else if (addr % 4 == 2) {
                *(data_arr_ptr + index) = (*(data_arr_ptr + index) & 0xffff00ff) | (temp << 8);
            } else if (addr % 4 == 3) {
                *(data_arr_ptr + index) = (*(data_arr_ptr + index) & 0xffffff00) | temp;        
            }
            PC += 4;
        }
    }

    return PC;
}

int main(int argc, char* argv[]) {

    string filename = argv[argc - 1];
    string filepath = "assembly_codes/" + filename;
    int n = 2147483647;
    string m;
    string d;


    for (int i = 1; i < argc - 1; ++i) {
        if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            n = stoi(argv[i + 1]);
            ++i;
        } else if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
            m = argv[i + 1];
        ++i;
        } else if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
            d = argv[i + 1];
            ++i;
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

    if (text_count <= n) {
        n = text_count;
    }

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
    for (int i = 0; i < n; i++) {
        content = *(text_arr_ptr + i);
        cout << i << endl;
        PC = instructions(content, Register, register_pointer, PC, data, text, data_arr_ptr);
        // cout << PC << endl;
    }

    cout << "Current register values:\n" << "----------------------------------" << endl;
    cout << "PC: " << hex << PC << "\nRegistors:" << endl;
    for (int i = 0; i < 32; i++) {
        cout << "R" << dec << i << ": 0x" << hex << Register[i] << endl;
    }
    if (!m.empty()) {
        size_t colon_pos = m.find(':');

        string address1_str = m.substr(0, colon_pos);
        string address2_str = m.substr(colon_pos + 1);

        int address1 = stoul(address1_str, nullptr, 16);
        int address2 = stoul(address2_str, nullptr, 16);

        int count = (address2 - address1) / 4;
        // cout << dec << count << endl;
        // if (address1 < 268435456) {

        // }
    }

    delete[] data_arr_ptr;
    delete[] text_arr_ptr;
    return 0;   
}