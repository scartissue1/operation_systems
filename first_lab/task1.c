#include <stdio.h>

enum status_codes {
    ok,
    eof,
    error
};

enum status_codes file_fill(FILE * out) {
    unsigned char pi[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
    if (fwrite(pi, sizeof(unsigned char), 11, out) == 11) return ok;
    else return error;
}

enum status_codes file_read(const unsigned int bytes_size, unsigned char bytes[bytes_size], FILE * in) {
    if (fread(bytes, sizeof(unsigned char), bytes_size, in) != bytes_size) {
        if (feof(in)) return eof;
        else return error;
    }
    else return ok;
}

int main (int argc, char * argv[]) {
    if (argc != 2) {
        printf("No filename\n");
        return -1;
    }
    FILE * binary_w = fopen(argv[1], "wb");
    if (!binary_w) {
        printf("Cannot open the file\n");
        return -1;
    }
    switch (file_fill(binary_w)) {
        case ok:
            break;
        case error:
            printf("Error occured\n");
            fclose(binary_w);
            return -1;
    }
    fclose(binary_w);
    
    FILE * binary_r = fopen(argv[1], "rb");
    if (!binary_w) {
        printf("Cannot open the file\n");
        return -1;
    }
    unsigned char byte[1];
    enum status_codes status;
    while ((status = file_read(1, byte, binary_r)) != eof) {
        if (status == ok) {
            printf("%u ", byte[0]);
            long curr = ftell(binary_r);
            if (curr != -1L) printf("%ld\n", curr);
            else printf("Error occured\n");
            printf("%d ", binary_r->_flags);
            printf("%s ", binary_r->_IO_read_ptr);
            printf("%s ", binary_r->_IO_read_end);
            printf("%s ", binary_r->_IO_read_base);
            printf("%s ", binary_r->_IO_write_ptr);
            printf("%s ", binary_r->_IO_write_end);
            printf("%s ", binary_r->_IO_write_base);
            printf("%s ", binary_r->_IO_backup_base);
            printf("%s ", binary_r->_IO_save_base);
            printf("%s\n", binary_r->_IO_save_end);
        }
        else {
            printf("Error!\n");
            fclose(binary_r);
            return -1;
        }
    }
    fclose(binary_r);

    FILE * binary_seek = fopen(argv[1], "rb");
    if (!binary_seek) {
        printf("Cannot open the file\n");
        return -1;
    }
    if (fseek(binary_seek, 3 * sizeof(unsigned char), SEEK_SET) != 0) {
        fclose(binary_seek);
        printf("fseek error\n");
        return -1;
    }
    const unsigned int buff_seek_size = 4;
    unsigned char buff_seek[buff_seek_size];
    switch (file_read(buff_seek_size, buff_seek, binary_seek)) {
        case ok:
            for (int i = 0; i < buff_seek_size; i++) {
                printf("%u ", buff_seek[i]);
            }
            printf("\n");
            break;
        case eof:
            printf("EOF is reached\n");
            break;
        case error:
            printf("Error occured\n");
            fclose(binary_seek);
            return -1;
    }
    fclose(binary_seek);
    return 0;
}
