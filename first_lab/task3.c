#include <stdio.h>

int file_copy(FILE * in, FILE * out) {
    unsigned char input;
    while (1) {
        if (fread(&input, sizeof(unsigned char), 1, in) != 1) {
            if (feof(in)) return 1;
            else return 0;
        }
        if (fwrite(&input, sizeof(unsigned char), 1, out) != 1) return 0;
    }
}

int main(int argc, char * argv[]) {
    if (argc != 3) {
        printf("No filename\n");
        return -1;
    }
    FILE * in = fopen(argv[1], "rb");
    if (!in) {
        printf("Unable to open a file\n");
        return -1;
    }
    FILE * out = fopen(argv[2], "wb");
    if (!out) {
        printf("Unable to open a file\n");
        fclose(in);
        return -1;
    }
    if (!file_copy(in, out)) {
        printf("Error\n");
        return -1;
    }
    fclose(in);
    fclose(out);
    return 0;
}