/*
2024 Spring, Computer Architecture 
Project 1: Simple MIPS assembler

Student ID : 201811118
Name       : Gu Lee
*/

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <bitset>
#include <regex>

using namespace std;

struct Data{
    string name;
    vector<int> values;
    int addr; // if data is array, save first value's address (Data Section)
};

struct Label{
    string name;
    int addr;
};

int search_label_idx(vector<Label> labels, string target){
    string temp_target;
    string temp_label;
    int result = -1;
    
    temp_target = target;
    if (temp_target.find("\n") != string::npos){
        temp_target.erase(find(temp_target.begin(), temp_target.end(), '\n'));
    }
    for (size_t i = 0; i < labels.size(); i++){
        temp_label = labels[i].name;
        if (temp_label.find("\n") != string::npos){ 
            temp_label.erase(find(temp_label.begin(), temp_label.end(), '\n'));
        }

        if (temp_target == temp_label){   
            result = i;
            break;
        }
    }
    return result;
}

int search_data_idx(vector<Data> datas, string target){
    string temp_target;
    string temp_data;
    int result = -1;

    temp_target = target;
    if (temp_target.find("\n") != string::npos){
        temp_target.erase(find(temp_target.begin(), temp_target.end(), '\n'));
    }
    for (size_t i = 0; i < datas.size(); i++){
        temp_data = datas[i].name;
        if (temp_data.find("\n") != string::npos){ 
            temp_data.erase(find(temp_data.begin(), temp_data.end(), '\n'));
        }
        if (temp_target == temp_data){   
            result = i;
            break;
        }
    }
    return result;
}

vector<string> split_line(string input, char delimiter){ // split line to words
    vector<string> words;    
    stringstream sstream(input);    
    string word;
    while (getline(sstream, word, delimiter)) {
        if (!word.empty())
            words.push_back(word);    
    }    
    return words;
}

int str2int(string num_str){ // Make string to int. Support both decimal, hexadecimal.
    int num_int = 0;
    if (num_str.rfind("0x", 0) == 0){ // for hex
        int str_length = num_str.length();
        char num_char[str_length+1];
        strcpy(num_char, num_str.c_str());
        num_int = (int) strtol(num_char, NULL, 0);
    }
    else{ // for dec
        num_int = stoi(num_str);
    }
    return num_int;
}

int calc_r_format(int op, int rs, int rt, int rd, int shamt, int funct){
    int result = 0;
    result += (op << 26);
    result += (rs << 21);
    result += (rt << 16);
    result += (rd << 11);
    result += (shamt << 6);
    result += funct;
    return result;
}

int calc_i_format(int op, int rs, int rt,  int imm){
    int result = 0;
    result += (op << 26);
    result += (rs << 21);
    result += (rt << 16);
    result += ((unsigned int) imm << 16) >> 16; 
    return result;
}

int calc_j_format(int op, int jmp){
    int result = 0;
    result += (op << 26);
    result += jmp;
    return result;
}

int main(int argc, const char* argv[]){
    vector<Label> labels; 
    vector<Data> datas;

    int program_cnt = 0x00400000; // init pc
    int general_ptr = 0x10000000; // init gp
    int cnt_text = 0; // counting number of instruction
    int cnt_data = 0; // counting number of data
    int cur_data = -1; // flag for section check
    int cur_text = -1; // flag for section check
    int inst_idx = 0; // check instrunction idx

    // set output file path
    string input_name(argv[1]);
    string input_name_wo_ext = input_name.substr(0, input_name.find('.'));
    string output_name = input_name_wo_ext + ".o";
    string inst;
    ofstream output;

    // read/writ assembly file
    vector<vector<string>> lines; // used in second loop
    vector<vector<string>> lines_w_la; // used in third loop
    vector<vector<string>> lines_wo_la; // used in final loop

    string data_name;
    string label_name;
    string word;

    int op, rs, rd, rt, funct, imm, jmp, shamt, label, target;
    char str[128];
    int verbose = 1; // flag for debugging

    FILE* asm_file_s = fopen(argv[1], "r");
    if(asm_file_s == NULL){
        return EXIT_FAILURE;
    }

    if (verbose){
        cout << "\n--------------- First Loop ---------------" << endl;
        cout << "[FUNCTION]" << endl;
        cout << "- Read assembly file. " << endl;
        cout << "- Execute simple parsing. Split line by '\\n', '\\t'." << endl;
        cout << "- Erase ',', '$' mark." << endl;
        cout << "- Save contents(label, insturction) in text section to vector<vector<string>> format" << endl;
        cout << "- Save data info(data name, value, address) in data section to Data struct. " << endl;
        cout << "- Save label info(label name, address) in text section to Label struct. " << endl;
        cout << endl;
    }

    while (fgets(str, 128, asm_file_s) != NULL){
        inst_idx = 0; // The 0-th word is treated as an instruction.

        string line(str);
        vector<string> words;
        vector<string> temp_words;

        replace(line.begin(), line.end(), '\t', ' ');

        temp_words = split_line(line, ' ');

        if (temp_words.size()==1 && (temp_words[0]=="\n" or temp_words[0]=="\t")){ // If a blank line is read, moves to next line.
            continue;
        }

        for(size_t i=0; i < temp_words.size(); i++){
            if (temp_words[i] != "\n" && temp_words[i] != "\t"){
                words.push_back(temp_words[i]);
            }
        }


        if (words[0].find(".data") != string::npos){ // set flag for 'reading data section'
            cur_data = 1;
            cur_text = 0;
            continue;
        }

        else if (words[0].find(".text") != string::npos){ // set flag for 'reading text section'
            cur_data = 0;
            cur_text = 1;
            continue;
        }

        if (cur_data){ // in data section, don't save the line.
            if (words.size() == 3 && words[0].find(":") != string::npos){ // Create new data 
                Data new_data;
                words[0].erase(find(words[0].begin(), words[0].end(), ':'));
                data_name=words[0];
                new_data.values.push_back(str2int(words[2]));
                new_data.name = data_name;
                new_data.addr = general_ptr; 
                datas.push_back(new_data);
                general_ptr += 4;
                cnt_data += 1;
            }
            else{ //if (words.size() == 2) { // Add value to previous data.
                datas.back().values.push_back(str2int(words[1]));
                general_ptr += 4;
                cnt_data += 1;
            }
        }
        else if (cur_text){ // in text section, save all lines after parsing.
            if (words[0].find(":") != string::npos){ // Create new label
                Label new_label;
                words[0].erase(find(words[0].begin(), words[0].end(), ':'));
                if (words[0].find("\n") != string::npos){ // new label
                    words[0].erase(find(words[0].begin(), words[0].end(), '\n'));
                }
                label_name=words[0];
                new_label.name = label_name;
                new_label.addr = program_cnt;
                labels.push_back(new_label);
                
                if (words.size()!=1){ // If there is an instruction after the label, set instruction idx.
                    inst_idx = 1;
                    vector<string> temp;
                    temp.push_back(words[0]);
                    lines.push_back(temp);       
                }
                else{ // If there only exist label, move to next line.
                    lines.push_back(words);       
                    continue;
                }
            }
            inst = words[inst_idx];

            if (inst_idx){
                words.erase(words.begin());
            }    
            program_cnt += 4;

            if (words.back().find('\n') != string::npos){ // remove '\n' in last word.
                words.back().erase(find(words.back().begin(), words.back().end(), '\n'));
            }

            if (words.back().find("(") != string::npos){ // If word inclue '(', split it 
                string first = words.back().substr(0, words.back().find("("));
                string second = words.back().substr(words.back().find("(")+1, words.back().length());
                second = second.substr(0, second.find(")"));
                words.pop_back();
                words.push_back(first);
                words.push_back(second);
            }          

            for (size_t i = 0; i < words.size(); i++){ // erase '$', and ',' .
                if (words[i].find('$') != string::npos){
                    words[i].erase(find(words[i].begin(), words[i].end(), '$'));
                }
                if (words[i].find(',') != string::npos){
                    words[i].erase(find(words[i].begin(), words[i].end(), ','));
                }
            }
            lines.push_back(words);       
        }
    }

    if (verbose){
        cout << "[Saved lines]" << endl; 
        for(size_t i=0; i < lines.size(); i++){
            for(size_t j=0; j<lines[i].size(); j++){
                cout << lines[i][j] << " ";
            }
            cout << endl;
        }
        printf("\n[Saved Datas]\n");
        for (size_t i = 0; i < datas.size(); i++){ 
            printf("name: %s \naddr(dec): %d \naddr(hex): 0x%x \nvals(dec): ", datas[i].name.c_str(), datas[i].addr, datas[i].addr);
            for (size_t j = 0; j < datas[i].values.size(); j++){ 
                printf("%d ", datas[i].values[j]);
            }
            printf("\nvals(hex): ");
            for (size_t j = 0; j < datas[i].values.size(); j++){ 
                printf("0x%x ", datas[i].values[j]);
            }
            printf("\n\n");
        }
        printf("[Saved Labels]\n");
        for (size_t i = 0; i < labels.size(); i++){ 
            printf("name: %s\naddr(dec): %d \naddr(hex): 0x%x \n", labels[i].name.c_str(), labels[i].addr, labels[i].addr);
        }
    }

    fclose(asm_file_s);
    
    if(verbose){
        cout << "\n--------------- Second Loop ---------------" << endl;
        cout << "[FUNCTION]" << endl;
        cout << "- Count number of instruction considering 'la' translation." << endl;
        cout << "- If there exist label in text section, convert to address." << endl;
        cout << "- Convert from (Data name) to (address.)" << endl;
        cout << endl;
        cout << "[Convert/Translation process]\n" << endl;
    }

    program_cnt = 0x400000;
    for (size_t i = 0; i < lines.size(); i++){ 
        vector<string> line = lines[i];
        string last_word = line.back();
        string inst = line.front();

        int label_idx, data_idx;
        
        label_idx = search_label_idx(labels, last_word);
        data_idx = search_data_idx(datas, last_word);
        
        if (verbose){
            cout << "(pc) - 0x" << hex << program_cnt << " ";
            cout << "(input instruction) - ";
            for (size_t j=0; j < line.size(); j++){
                cout << line[j] << " ";
            }
            cout << endl;
        }

        if (line.size()==1){ // text section label留� �덈뒗 寃쎌슦. ex) line = 'sum\n'
            labels[label_idx].addr = program_cnt;
            if (verbose){
                cout << "  --> Label {" << labels[label_idx].name << "} address update. Changed to {0x" << hex << program_cnt << "}." << endl;
            }
            continue;
        }

        // Convert Data name to address.
        if (data_idx != -1){
            line.erase(line.end());
            line.push_back(to_string(datas[data_idx].addr));
            if (verbose){
                cout << "  --> Detect Data {" << datas[data_idx].name << "}. Converted." << endl;
            }
        }


        // Treat 'la' as 'lui' or 'lui'+'ori'.
        if (inst.compare("la")){ // inst != la 
            lines_w_la.push_back(line);
            program_cnt += 4;
        }
        else{ // inst == la
            int addr;
            int upper_addr;
            int lower_addr;

            if (label_idx != -1){ // 
                addr = labels[label_idx].addr;
            }

            else{
                addr = stoi(line[2]);
            }
            upper_addr = addr >> 16;
            lower_addr = (addr << 16) >> 16;
            lines_w_la.push_back(line);

            if (verbose){
                printf("addr: 0x%x", addr);
                printf("\nupper addr: 0x%x", upper_addr); // 1000 | 1000
                printf("\nlower addr: 0x%x\n", lower_addr); // 0000 | 0004
            }
            if (lower_addr == 0){
                program_cnt += 4;
                if (verbose){
                    printf("  --> Treat 'la' as 'lui'.\n");
                }
            }
            else{
                program_cnt += 8;
                if (verbose){
                    printf("  --> Treat 'la' as 'lui'+'ori'.\n");
                }
            }
        }
    }

    if (verbose){
        printf("\n[Saved Labels]\n");
        for (size_t i = 0; i < labels.size(); i++){ 
            printf("name: %s\naddr(dec): %d \naddr(hex): 0x%x \n", labels[i].name.c_str(), labels[i].addr, labels[i].addr);
        }
    }

    if (verbose){
        cout << "\n--------------- Third Loop ---------------" << endl;
        cout << "[FUNCTION]" << endl;
        cout << "- Translate from 'la' to 'lui' or 'lui' + 'ori'." << endl;
        cout << "- Convert from (Label name) to (address)." << endl;
        cout << "- Counting number of instruction.\n" << endl;
    }

    program_cnt = 0x400000;
    for (size_t i = 0; i < lines_w_la.size(); i++){ 
        vector<string> line = lines_w_la[i];
        string last_word = line.back();
        string inst = line.front();

        int label_idx, data_idx;
        
        label_idx = search_label_idx(labels, last_word);
        
        if(label_idx != -1){ // Convert from (Label name) to (address).
            line.erase(line.end());
            line.push_back(to_string(labels[label_idx].addr));
            if (verbose){
                cout << "  --> Detect Label {" << labels[label_idx].name << "}. Converted." << endl;
            }
        }

        // Translate from 'la' to 'lui' or 'lui'+'ori'.
        if (inst.compare("la")){ // inst != la 
            cnt_text += 1;
            program_cnt += 4;
            lines_wo_la.push_back(line);
        }
        else{ // inst == la
            int addr;
            int upper_addr;
            int lower_addr;
            addr = stoi(line[2]);
            upper_addr = addr >> 16;
            lower_addr = (addr << 16) >> 16;

            if (verbose){
                printf("addr: 0x%x", addr);
                printf("\nupper addr: 0x%x", upper_addr); // 1000 | 1000
                printf("\nlower addr: 0x%x\n", lower_addr); // 0000 | 0004
            }

            if (lower_addr == 0){ // la -> lui
                cnt_text += 1;
                program_cnt += 4;

                vector<string> new_line;
                string inst("lui");
                new_line.push_back(inst);
                new_line.push_back(line[1]);
                new_line.push_back(to_string(upper_addr));
                lines_wo_la.push_back(new_line);

                if (verbose){
                    printf("  --> Translated from 'la' to 'lui'.\n");
                }
            }
            else{ // la -> lui + ori
                cnt_text += 2;
                program_cnt += 8;

                vector<string> first_line;
                vector<string> second_line;
                string inst1("lui");
                first_line.push_back(inst1);
                first_line.push_back(line[1]);
                first_line.push_back(to_string(upper_addr));
                lines_wo_la.push_back(first_line);

                string inst2("ori");
                second_line.push_back(inst2);
                second_line.push_back(line[1]);
                second_line.push_back(line[1]);
                second_line.push_back(to_string(lower_addr));
                lines_wo_la.push_back(second_line);

                if (verbose){
                    printf("  --> Translated from 'la' to 'lui'+'ori'.\n");
                }
            }
        }
    }

    FILE* asm_file_o = fopen(output_name.c_str(), "w");

    if (verbose){
        cout << "\n--------------- Fourth Loop ---------------" << endl;
        cout << "[FUNCTION]" << endl;
        cout << "- Write output file" << endl;
        cout << "- Calculate final result(R, I, J format) from instruction and arguments." << endl;
        cout << endl;
        cout << "[text/data section size]" << endl;
        printf("Total number of text: %d, text section size(dec): %d, text section size(hex): 0x%x\n", cnt_text, cnt_text*4, cnt_text*4);
        printf("Total number of data: %d, data section size(dec): %d, data section size(hex): 0x%x\n\n", cnt_data, cnt_data*4, cnt_data*4);
    }
    
    // write text/data section size
    fprintf(asm_file_o, "0x%x\n", cnt_text * 4);
    fprintf(asm_file_o, "0x%x\n", cnt_data * 4);
    
    program_cnt = 0x00400000; // init pc (regarding change via 'la')

    // write instruction
    if (verbose){
        cout << "[instruction]" << endl;
    }
    for (size_t i = 0; i < lines_wo_la.size(); i++){ 
        vector<string> line;
        int result;
        line = lines_wo_la[i];
        if (verbose){
            cout << "(pc) 0x" << hex << program_cnt << endl;
        }
        if (!line[0].compare("addiu")){ // i-type
            op = 9;
            rt = str2int(line[1]);
            rs = str2int(line[2]);
            imm = str2int(line[3]);
            result = calc_i_format(op, rs, rt, imm);
            fprintf(asm_file_o, "0x%x", result);
            if (verbose){
                printf("(intput instruction) - %s rt: %d, rs: %d, imm: %d\n", line[0].c_str(), rt, rs, imm);
                printf("(hex) 0x%x\n", result);
                cout << "(bin) " << bitset<32>(result) << endl;
                cout << endl;
            }
            program_cnt += 4; 
        }
        else if (!line[0].compare("addu")){ // r-type
            op = 0;
            rd = str2int(line[1]);
            rs = str2int(line[2]);
            rt = str2int(line[3]);
            shamt = 0;
            funct = 0x21;
            result = calc_r_format(op, rs, rt, rd, shamt, funct);
            fprintf(asm_file_o, "0x%x", result);
            if (verbose){
                printf("(intput instruction) - %s rd: %d, rs: %d, rt: %d\n", line[0].c_str(), rd, rs, rt);
                printf("(hex) 0x%x\n", result);
                cout << "(bin) " << bitset<32>(result) << endl;
                cout << endl;
            }
            program_cnt += 4; 
        }
        else if (!line[0].compare("and")){ // r-type
            op = 0;
            rd = str2int(line[1]);
            rs = str2int(line[2]);
            rt = str2int(line[3]);
            shamt = 0;
            funct = 0x24;
            result = calc_r_format(op, rs, rt, rd, shamt, funct);
            fprintf(asm_file_o, "0x%x", result);
            if (verbose){
                printf("(intput instruction) - %s rd: %d, rs: %d, rt: %d\n", line[0].c_str(), rd, rs, rt);
                printf("(hex) 0x%x\n", result);
                cout << "(bin) " << bitset<32>(result) << endl;
                cout << endl;
            }
            program_cnt += 4; 
        }
        else if (!line[0].compare("andi")){ // i-type
            op = 0xc;
            rt = str2int(line[1]);
            rs = str2int(line[2]);
            imm = str2int(line[3]);
            result = calc_i_format(op, rs, rt, imm);
            fprintf(asm_file_o, "0x%x", result);
            if (verbose){
                printf("(intput instruction) - %s rt: %d, rs: %d, imm: %d\n", line[0].c_str(), rt, rs, imm);
                printf("(hex) 0x%x\n", result);
                cout << "(bin) " << bitset<32>(result) << endl;
                cout << endl;       
            }     
            program_cnt += 4; 
        }
        else if (!line[0].compare("beq")){ // i-type
            op = 4;
            rs = str2int(line[1]);
            rt = str2int(line[2]);
            label = str2int(line[3]);
            int offset = (label - (program_cnt + 4)) / 4; 
            result = calc_i_format(op, rs, rt, offset);
            fprintf(asm_file_o, "0x%x", result);
            if (verbose){
                printf("(intput instruction) - %s rs: %d, rt: %d, offset: %d\n", line[0].c_str(), rs, rt, offset);
                printf("label: 0x%x, pc: 0x%x, offset: %d\n", label, program_cnt, offset);

                printf("offset = (label - (program_cnt + 4)) / 4\n");
                printf("(hex) 0x%x\n", result);
                cout << "(bin) " << bitset<32>(result) << endl;
                cout << endl;       
            }
            program_cnt += 4;
        }
        else if (!line[0].compare("bne")){ // i-type
            op = 5;
            rs = str2int(line[1]);
            rt = str2int(line[2]);
            label = str2int(line[3]); // label
            int offset = (label - (program_cnt + 4)) / 4; 
            result = calc_i_format(op, rs, rt, offset);

            fprintf(asm_file_o, "0x%x", result);
            if (verbose){
                printf("(intput instruction) - %s rs: %d, rt: %d, offset: %d\n", line[0].c_str(), rs, rt, offset);
                printf("label: 0x%x, pc: 0x%x, offset: %d\n", label, program_cnt, offset);
                printf("offset = (label - (program_cnt + 4)) / 4\n");
                printf("(hex) 0x%x\n", result);
                cout << "(bin) " << bitset<32>(result) << endl;
                cout << endl;       
            }
            program_cnt += 4; 
        }
        else if (!line[0].compare("j")){ // j-type
            op = 2;
            target = str2int(line[1]);
            int jmp = target / 4;
            result = calc_j_format(op, jmp);

            fprintf(asm_file_o, "0x%x", result);
            if (verbose){
                printf("(intput instruction) - %s target: %d\n", line[0].c_str(), target);
                printf("jmp (addr / 4): %d\n", jmp);
                printf("(hex) 0x%x\n", result);
                cout << "(bin) " << bitset<32>(result) << endl;
                cout << endl;       
            }
            program_cnt += 4;
        }
        else if (!line[0].compare("jal")){ // j-type
            op = 3;
            target = str2int(line[1]);
            int jmp = target / 4;
            result = calc_j_format(op, jmp);

            fprintf(asm_file_o, "0x%x", result);
            if (verbose){
                printf("(intput instruction) - %s target: %d\n", line[0].c_str(), target);
                printf("jmp (addr / 4): %d\n", jmp);
                printf("(hex) 0x%x\n", result);
                cout << "(bin) " << bitset<32>(result) << endl;
                cout << endl;
            }       
            program_cnt += 4;
        }
        else if (!line[0].compare("jr")){ // r-type
            op = 0;
            rs = str2int(line[1]);
            rt = 0;
            rd = 0;
            shamt = 0;
            funct = 8;
            result = calc_r_format(op, rs, rt, rd, shamt, funct);

            fprintf(asm_file_o, "0x%x", result);
            if (verbose){
                printf("(intput instruction) - %s rs: %d\n", line[0].c_str(), rs);
                printf("(hex) 0x%x\n", result);
                cout << "(bin) " << bitset<32>(result) << endl;
                cout << endl;
            }
            program_cnt += 4; 
        }
        else if (!line[0].compare("lui")){ // i-type
            op = 0xf;
            rt = str2int(line[1]);
            rs = 0;
            imm = str2int(line[2]);
            result = calc_i_format(op, rs, rt, imm);

            fprintf(asm_file_o, "0x%x", result);
            if (verbose){
                printf("(intput instruction) - %s rt: %d, imm: %d\n", line[0].c_str(), rt, imm);
                printf("(hex) 0x%x\n", result);
                cout << "(bin) " << bitset<32>(result) << endl;
                cout << endl;       
            }
            program_cnt += 4; 
        }
        else if (!line[0].compare("lw")){ // i-type
            op = 0x23;
            rt = str2int(line[1]);
            int offset = str2int(line[2]);
            rs = str2int(line[3]);
            result = calc_i_format(op, rs, rt, offset);

            fprintf(asm_file_o, "0x%x", result);
            if (verbose){
                printf("(intput instruction) - %s rt: %d, offset: %d, rs: %d\n", line[0].c_str(), rt, offset, rs);
                printf("(hex) 0x%x\n", result);
                cout << "(bin) " << bitset<32>(result) << endl;
                cout << endl;       
            }
            program_cnt += 4; 
        }
        else if (!line[0].compare("lb")){ // i-type
            op = 0x20;
            rt = str2int(line[1]);
            int offset = str2int(line[2]);
            rs = str2int(line[3]);
            result = calc_i_format(op, rs, rt, offset);

            fprintf(asm_file_o, "0x%x", result);
            if (verbose){
                printf("(intput instruction) - %s rt: %d, offset: %d, rs: %d\n", line[0].c_str(), rt, offset, rs);
                printf("(hex) 0x%x\n", result);
                cout << "(bin) " << bitset<32>(result) << endl;
                cout << endl;       
            }
            program_cnt += 4; 
        }
        else if (!line[0].compare("nor")){ // r-type
            op = 0;
            rd = str2int(line[1]);
            rs = str2int(line[2]);
            rt = str2int(line[3]);
            shamt = 0;
            funct = 0x27;
            result = calc_r_format(op, rs, rt, rd, shamt, funct);

            fprintf(asm_file_o, "0x%x", result);
            if (verbose){
                printf("(intput instruction) - %s rd: %d, rs: %d, rt: %d\n", line[0].c_str(), rd, rs, rt);
                printf("(hex) 0x%x\n", result);
                cout << "(bin) " << bitset<32>(result) << endl;
                cout << endl;
            }
            program_cnt += 4; 
        }
        else if (!line[0].compare("or")){ // r-type
            op = 0;
            rd = str2int(line[1]);
            rs = str2int(line[2]);
            rt = str2int(line[3]);
            shamt = 0;
            funct = 0x25;
            result = calc_r_format(op, rs, rt, rd, shamt, funct);

            fprintf(asm_file_o, "0x%x", result);
            if (verbose){
                printf("(intput instruction) - %s rd: %d, rs: %d, rt: %d\n", line[0].c_str(), rd, rs, rt);
                printf("(hex) 0x%x\n", result);
                cout << "(bin) " << bitset<32>(result) << endl;
                cout << endl;
            }
            program_cnt += 4; 
        }
        else if (!line[0].compare("ori")){ // i-type
            op = 0xd;
            rt = str2int(line[1]);
            rs = str2int(line[2]);
            imm = str2int(line[3]);
            result = calc_i_format(op, rs, rt, imm);

            fprintf(asm_file_o, "0x%x", result);
            if (verbose){
                printf("(intput instruction) - %s rt: %d, rs: %d, imm: %d\n", line[0].c_str(), rt, rs, imm);
                printf("(hex) 0x%x\n", result);
                cout << "(bin) " << bitset<32>(result) << endl;
                cout << endl;
            }
            program_cnt += 4; 
        }
        else if (!line[0].compare("sltiu")){ // i-type
            op = 0xb;
            rt = str2int(line[1]);
            rs = str2int(line[2]);
            imm = str2int(line[3]);
            result = calc_i_format(op, rs, rt, imm);

            fprintf(asm_file_o, "0x%x", result);
            if (verbose){
                printf("(intput instruction) - %s rt: %d, rs: %d, imm: %d\n", line[0].c_str(), rt, rs, imm);
                printf("(hex) 0x%x\n", result);
                cout << "(bin) " << bitset<32>(result) << endl;
                cout << endl;
            }
            program_cnt += 4; 
        }
        else if (!line[0].compare("sltu")){ // r-type
            op = 0;
            rd = str2int(line[1]);
            rs = str2int(line[2]);
            rt = str2int(line[3]);
            shamt = 0;
            funct = 0x2b;
            result = calc_r_format(op, rs, rt, rd, shamt, funct);

            fprintf(asm_file_o, "0x%x", result);
            if (verbose){
                printf("(intput instruction) - %s rd: %d, rs: %d, rt: %d\n", line[0].c_str(), rd, rs, rt);
                printf("(hex) 0x%x\n", result);
                cout << "(bin) " << bitset<32>(result) << endl;
                cout << endl;
            }
            program_cnt += 4; 
        }
        else if (!line[0].compare("sll")){ // r-type
            op = 0;
            rs = 0;
            rd = str2int(line[1]);
            rt = str2int(line[2]);
            shamt = str2int(line[3]);
            funct = 0;
            result = calc_r_format(op, rs, rt, rd, shamt, funct);

            fprintf(asm_file_o, "0x%x", result);
            if (verbose){
                printf("(intput instruction) - %s rd: %d, rt: %d, shamt: %d\n", line[0].c_str(), rd, rt, shamt);
                printf("(hex) 0x%x\n", result);
                cout << "(bin) " << bitset<32>(result) << endl;
                cout << endl;
            }
            program_cnt += 4; 
        }
        else if (!line[0].compare("srl")){ // r-type
            op = 0;
            rs = 0;
            rd = str2int(line[1]);
            rt = str2int(line[2]);
            shamt = str2int(line[3]);
            funct = 2;
            result = calc_r_format(op, rs, rt, rd, shamt, funct);

            fprintf(asm_file_o, "0x%x", result);
            if (verbose){
                printf("(intput instruction) - %s rd: %d, rt: %d, shamt: %d\n", line[0].c_str(), rd, rt, shamt);
                printf("(hex) 0x%x\n", result);
                cout << "(bin) " << bitset<32>(result) << endl;
                cout << endl;
            }
            program_cnt += 4; 
        }
        else if (!line[0].compare("sw")){ // i-type
            op = 0x2b;
            rt = str2int(line[1]);
            int offset = str2int(line[2]);
            rs = str2int(line[3]);
            result = calc_i_format(op, rs, rt, offset);

            fprintf(asm_file_o, "0x%x", result);
            if (verbose){
                printf("(intput instruction) - %s rt: %d, offset: %d, rs: %d\n", line[0].c_str(), rt, offset, rs);
                printf("(hex) 0x%x\n", result);
                cout << "(bin) " << bitset<32>(result) << endl;
                cout << endl;       
            }
            program_cnt += 4; 
        }
        else if (!line[0].compare("subu")){ // r-type
            op = 0;
            rd = str2int(line[1]);
            rs = str2int(line[2]);
            rt = str2int(line[3]);
            shamt = 0;
            funct = 0x23;
            result = calc_r_format(op, rs, rt, rd, shamt, funct);

            fprintf(asm_file_o, "0x%x", result);
            if (verbose){
                printf("(intput instruction) - %s rd: %d, rs: %d, rt: %d\n", line[0].c_str(), rd, rs, rt);
                printf("(hex) 0x%x\n", result);
                cout << "(bin) " << bitset<32>(result) << endl;
                cout << endl;
            }
            program_cnt += 4; 
        }
        else if (!line[0].compare("sb")){ // i-type 
            op = 0x28;
            rt = str2int(line[1]);
            int offset = str2int(line[2]);
            rs = str2int(line[3]);
            result = calc_i_format(op, rs, rt, offset);

            fprintf(asm_file_o, "0x%x", result);
            if (verbose){
                printf("(intput instruction) - %s rt: %d, offset: %d, rs: %d\n", line[0].c_str(), rt, offset, rs);
                printf("(hex) 0x%x\n", result);
                cout << "(bin) " << bitset<32>(result) << endl;
                cout << endl;
            }       
            program_cnt += 4; 
        }

        if (i < lines_wo_la.size()-1){
            fprintf(asm_file_o, "\n");
        }
    }

    if (verbose){
        printf("[Value]\n");
        for (size_t i = 0; i < datas.size(); i++){ 
            for (size_t j = 0; j < datas[i].values.size(); j++){ 
                printf("value(dec): %d, value(hex): 0x%x\n", datas[i].values[j], datas[i].values[j]);
            }
        }        
    }

    // write value
    if (cnt_data != 0){
        fprintf(asm_file_o, "\n"); 
    }

    for (size_t i = 0; i < datas.size(); i++){ 
        for (size_t j = 0; j < datas[i].values.size(); j++){ 
            if (i == datas.size()-1 && j == datas[i].values.size()-1){
                fprintf(asm_file_o, "0x%x", datas[i].values[j]); // If the value is end of output file, don't print '\n'.
            }
            else{
                fprintf(asm_file_o, "0x%x\n", datas[i].values[j]); 
            }
        }
    }
    fclose(asm_file_o);
    return 0;
}