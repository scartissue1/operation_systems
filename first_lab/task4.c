#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int xor8(FILE * in, unsigned char * xor8_summ) {
    unsigned char input;
    while (1) {
        if (fread(&input, sizeof(unsigned char), 1, in) != 1) {
            if (feof(in)) return 1;
            else return 0;
        }
        *xor8_summ ^= input;
    }
}

void _4_byte_xor(unsigned char * first, unsigned char second[4]) {
    for (int i = 0; i < 4; i++) {
        *first ^= second[i];
    }
}

int xor32(FILE * in, unsigned char * xor32_summ) {
    unsigned char input[4];
    while (1) {
        int fread_count = fread(input, sizeof(unsigned char), 4, in);
        if (fread_count != 4) {
            if (feof(in)) return 0;
            else {
                for (int i = fread_count - 1; i < 4; i++) input[i] = 0;
                _4_byte_xor(&(*xor32_summ), input);
                return 0;
            }
        }
        _4_byte_xor(&(*xor32_summ), input);
    }
    return 0;
}

unsigned int from_base_to_int (char * number, int number_size, int base) {
    if (!number) return 0;

    int power = 1, sign_flag = 0;
    long long int decimal = 0;
    if (number[0] == '-') {
        sign_flag = 1;
    }
    else if (number[0] == '+') sign_flag = 1;
    for (int i = number_size - 2; i >= sign_flag; i--) {
        decimal += power * ((isdigit(number[i])) ? number[i] - '0' : number[i] - 'A' + 10);
        power *= base;
    }
    return decimal;
}

int mask_validation(unsigned char * mask) {
    int size = strlen(mask);
    int index = 0;
    while (index < size) {
        int num = (isdigit(mask[index])) ? mask[index] - '0' : mask[index] - 'A' + 10;
        if (0 > num || num > 15) return 0;
        index++;
    }
    return 1;
}

int mask_check(FILE * in, unsigned char * mask) {
    unsigned int input;
    unsigned int mask_int = from_base_to_int(mask, strlen(mask), 16);
    int count = 0;
    while (1) {
        if (fread(&input, sizeof(unsigned int), 1, in) != 1) return -1;
        if (input & mask_int) count++;
    }
    return count;
}

int main(int argc, char * argv[]) {
    if (!(3 <= argc && argc <= 4)) {
        printf("Wrong flag\n");
        return -1;
    }
    FILE * in = fopen(argv[1], "rb");
    if (!in) {
        printf("Unable to open the file\n");
        return -1;
    }
    if (strcmp(argv[2], "xor8") == 0) {
        unsigned char xor8_summ = 0;
        if (xor8(in, &xor8_summ)) {
            printf("%u\n", xor8_summ);
        }
        else {
            printf("Error occured\n");
            return -1;
        }
    }
    else if (strcmp(argv[2], "xor32") == 0) {
        unsigned char xor32_summ = 0;
        xor32(in, &xor32_summ);
        printf("%u\n", xor32_summ);
    }
    else if (strcmp(argv[2], "mask") == 0) {
        unsigned char * mask = argv[3];
        if (!mask_validation(mask)) {
            printf("Wrong mask\n");
            return -1;
        }
        printf("%d\n", mask_check(in, mask));
    }
    fclose(in);
    return 0;
}
