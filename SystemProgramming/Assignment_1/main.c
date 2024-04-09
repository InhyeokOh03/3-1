#include <stdio.h>

void print_bit(void *a, int len) {
    int i, j;
    unsigned char *ptr = (unsigned char *)a;
    
    for (i = len - 1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            printf("%d", (ptr[i] >> j) & 1);
        }
        printf(" ");
    }
}

void reverse_bit(void *a, int len) {
    unsigned char *ptr = (unsigned char *)a;
    int i, j;
    
    for (i = 0; i < len; i++) {
        unsigned char temp = 0;
        for (j = 0; j < 8; j++) {
            temp |= ((ptr[i] >> j) & 1) << (7 - j);
        }
        ptr[i] = temp;
    }
}

void split_bit(void *a, void *out1, void *out2, int len) {
    unsigned char *ptr_a = (unsigned char *)a;
    unsigned char *ptr_out1 = (unsigned char *)out1;
    unsigned char *ptr_out2 = (unsigned char *)out2;
    int i, j;

    for (i = 0; i < len; i++) {
        ptr_out1[i] = 0;
        ptr_out2[i] = 0;
        for (j = 0; j < 8; j++) {
            if (j % 2 == 0) {
                ptr_out2[i] |= (ptr_a[i] >> j) & 1;
            } else {
                ptr_out1[i] |= (ptr_a[i] >> j) & 1;
            }
        }
    }
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