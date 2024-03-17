#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100

typedef struct {
    int data[100];
    int length;
} DataSection;

typedef struct {
    char label[20];
    int instruction[5];
    int length;
} TextSection;

void parse_file(const char *file_path, DataSection *data_section, TextSection * text_section) {
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        printf("Can't open a file. \n");
        exit(1);
    }

    char line[MAX_LINE_LENGTH];
    char *token;

    DataSection *current_data_section = NULL;
    TextSection *current_text_section = NULL;

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        // printf("%s\n", line);
        if (line[strlen(line) - 1] == '\n') {
            printf("%s", line);
            line[strlen(line) - 1] = '\0';
        }
        if (strcmp(line, ".data") == 0) {
            printf("%s", line);
            current_data_section = data_section;
            current_data_section->length =0;
            continue;
        }
        else if (strcmp(line, ".text") == 0) {
            printf("%s", line);
            current_data_section = NULL;
            current_text_section = text_section;
            current_text_section->length = 0;
            continue;
        }

        if (current_data_section != NULL) {
            token = strtok(line, " ");
            strcpy(current_text_section->label, token);

            token = strtok(NULL, " ");
            while (token != NULL) {
                current_data_section->data[current_data_section->length++] = strtol(token, NULL, 0);
                token = strtok(NULL, " ");
            }
        }
        else if (current_text_section != NULL) {
            token =strtok(line, " ");
            strcpy(current_text_section->label, token);

            token = strtok(NULL, " ");
            while (token != NULL) {
                current_text_section->instruction[current_text_section->length++] = strtol(token, NULL, 0);
                token = strtok(NULL, " ");
            }
        }
    }
    fclose(file);
}

int main() {
    DataSection data_section;
    TextSection text_section;

    parse_file("sample/sample.s", &data_section, &text_section);

    printf("Data Section:\n");
    for (int i = 0; i < data_section.length; i++) {
        printf("%d\n", data_section.data[i]);
    }

    printf("\nText Section:\n");
    for (int i = 0; i < text_section.length; i++) {
        printf("%s: ", text_section.label);
        for (int j = 0; j < text_section.instruction[i]; j++) {
            printf("%d ", text_section.instruction[j]);
        }
        printf("\n");
    }

    return 0;
}