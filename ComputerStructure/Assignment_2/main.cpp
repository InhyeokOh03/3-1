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

int sign_extend(int value) {
    return static_cast<int>(static_cast<int32_t>(value));
}

int instructions(unsigned int content, int Register[32], int* register_pointer, unsigned int PC, unsigned int data, unsigned int text ) {
    unsigned int op;
    op = content >> 26;
    // cout << op << endl;
    if (op == 0) {
        unsigned int rs = ((content << 6) & 0xf8000000) >> 27;
        unsigned int rt = ((content << 11) & 0xf8000000) >> 27;
        unsigned int rd = ((content << 16) & 0xf8000000) >> 27;
        unsigned int shamt = ((content << 21) & 0xf8000000) >> 27;
        unsigned int funct = ((content << 26) & 0xfc000000) >> 26;
        // cout << op << endl;
        // cout << "->" << rs << endl;
        // cout << "-->" << rt << endl;
        // cout << "--->" << rd << endl;
        // cout << "---->" << shamt << endl;
        // cout << "----->" << funct << endl;
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
            // cout << imm << endl;
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
            Register[rs + imm] = Register[rt];
            PC += 4;
        } else if (op == 32) {
            Register[rt] = Register[rs + imm] & 0xff;
            PC += 4;
        } else if (op == 40) {
            Register[rs + imm] = Register[rt] & 0xff;
            PC += 4;
        }
    }



    return PC;
}

int main(int argc, char* argv[]) {

    string filename = argv[argc - 1];
    string filepath = "assembly_codes/" + filename;
    int n;
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

    // // contents 배열 출력해보기
    // for (const auto& str : contents) {
    //     cout << str << endl;
    // }

    int text_count;
    int data_count;

    text_count = stoi(contents[0], nullptr, 16) / 4;
    data_count = stoi(contents[1], nullptr, 16) / 4;

    if (text_count <= n) {
        n = text_count;
    }

    unsigned int text_arr[text_count];
    unsigned int data_arr[data_count];

    for (int i = 0; i < data_count; i++) {
        data_arr[i] = stoul(contents[2 + text_count + i], nullptr, 16);
    }

    for (int i = 0; i < text_count; i++) {
        text_arr[i] = stoul(contents[2 + i], nullptr, 16);
    }

    // // text_arr와 data_arr에 복사된 데이터 출력
    // cout << "text_arr에 복사된 데이터:" << endl;
    // for (int i = 0; i < text_count; i++) {
    //     cout << text_arr[i] << " ";
    // }
    // cout << endl;
    // cout << "data_arr에 복사된 데이터:" << endl;
    // for (int i = 0; i < data_count; i++) {
    //     cout << data_arr[i] << " ";
    // }
    // cout << endl;

    unsigned int content;
    for (int i = 0; i < n; i++) {
        content = text_arr[i];
        PC = instructions(content, Register, register_pointer, PC, data, text);
        // cout << PC << endl;
    }

    cout << "Current register values:\n" << "----------------------------------" << endl;
    cout << "PC: " << hex << PC << "\nRegistors:" << endl;
    for (int i = 0; i < 32; i++) {
        cout << "R" << dec << i << ": 0x" << hex << Register[i] << endl;
    }

    return 0;
}