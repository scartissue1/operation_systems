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

void _4_byte_xor(unsigned char first[4], unsigned char second[4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            first[i] ^= second[j];
        }
    }
}

int xor32(FILE * in, unsigned char xor32_summ[4]) {
    unsigned char input[4];
    while (1) {
        int fread_count = fread(input, sizeof(unsigned char), 4, in);
        if (fread_count != 4) {
            if (feof(in)) return 0;
            else {
                for (int i = fread_count - 1; i < 4; i++) input[i] = 0;
                _4_byte_xor(xor32_summ, input);
                return 0;
            }
        }
        _4_byte_xor(xor32_summ, input);
    }
    return 0;
}

int mask_check(FILE * in, unsigned char * mask) {
    unsigned char input[4];
    int mask_size = strlen(mask);
    int len = mask_size > 4 ? 4 : mask_size;
    int count = 0;
    while (1) {
        if (fread(input, sizeof(unsigned char), 4, in) != 4) break;
        if (memcmp(mask, input, len) == 0) count++;

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
        unsigned char xor32_summ[4];
        xor32(in, xor32_summ);
        for (int i = 0; i < 4; i++) {
            printf("%u", xor32_summ[i]);
        }
        printf("\n");
    }
    else if (strcmp(argv[2], "mask") == 0) {
        unsigned char * mask = argv[3];
        printf("%d\n", mask_check(in, mask));
    }
    fclose(in);
    return 0;
}