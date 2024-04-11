#include <stdio.h>
#include <stdlib.h>

typedef unsigned char* pointer;

void print_bit(pointer a, int len) {
    unsigned char *ptr = (unsigned char *)a;
    int i, j;
    
    for (i = 0; i < len; i++) {
        for (j = 7; j >= 0; j--) {
            printf("%d", (ptr[i] >> j) & 1);
        }
        printf(" ");
    }
    printf("\n");
}

void reverse_bit(unsigned char *a, int len) {
    // 바이트의 비트 순서 뒤집기
    for (int i = 0; i < len; ++i) {
        unsigned char temp = 0;
        for (int j = 0; j < 8; ++j) {
            temp |= ((a[i] >> j) & 1) << (7 - j);
        }
        a[i] = temp;
    }

    // 바이트 배열 순서 뒤집기
    for (int i = 0; i < len / 2; ++i) {
        unsigned char temp = a[i];
        a[i] = a[len - i - 1];
        a[len - i - 1] = temp;
    }
}

void split_bit(unsigned char *a, unsigned char *out1, unsigned char *out2, int len) {
    *out1 = (unsigned char*)((*a & 0x55555555) >> 1);
    *out2 = (unsigned char*)(*a & 0xAAAAAAAA);
}

unsigned int convert_endian(unsigned int a) {
    unsigned int result = 0;
    
    result |= (a & 0xFF) << 24;
    result |= ((a >> 8) & 0xFF) << 16;
    result |= ((a >> 16) & 0xFF) << 8;
    result |= (a >> 24) & 0xFF;

    return result;
}

void get_date(unsigned int date, int* pYear, int* pMonth, int* pDay) {
    *pYear = (date >> 9) & 0x7FFFF;   // 23 bits for year
    *pMonth = (date >> 5) & 0xF;       // 4 bits for month
    *pDay = date & 0x1F;               // 5 bits for day
}

int main() {
    printf("Problem 1\n");
    unsigned int v1 = 0x1234CDEF;
    print_bit((pointer)&v1, sizeof(v1));
    reverse_bit((pointer)&v1, sizeof(v1));
    print_bit((pointer)&v1, sizeof(v1));
    printf("Problem 2\n");
    unsigned int v2 = 0x1234CDEF;
    unsigned short out1 = 0, out2 = 0;
    print_bit((pointer)&v2, sizeof(v2));
    split_bit((pointer)&v2, (pointer)&out1, (pointer)&out2, sizeof(v2));
    print_bit((pointer)&out1, sizeof(out1));
    print_bit((pointer)&out2, sizeof(out2));
    exit(0);
    printf("Problem 3\n");
    unsigned int v3 = 0x12345678;
    unsigned int v3_ret = convert_endian(v3);
    print_bit((pointer)&v3, sizeof(v3));
    print_bit((pointer)&v3_ret, sizeof(v3_ret));
    printf("Problem 4\n");
    unsigned int date = 1035391;
    int year, month, day;
    print_bit((pointer)&date, sizeof(date));
    get_date(date, &year, &month, &day);
    printf("%d -> %d/%d/%d\n", date, year, month, day);
    return 0;
}