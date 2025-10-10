#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define HEADER_OFFSET 60



int main(int argc, char** argv) {

    if (argc > 5 || argc < 4 || strcmp(argv[1], "--help") == 0) {
        perror("Command should be used like this:\n stegoc <FLAG> <INPUT> <OUTPUT>\n");
        return 1;
    }

    const char* carrier_path = argv[2];
    const char* hidden_path = argv[3];
    const char* out_path = argv[4];


    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--hidden") == 0) {
        FILE *carrier;
        FILE *hidden;
        FILE *out;
        carrier = fopen(carrier_path, "rb");
        hidden = fopen(hidden_path, "rb");
        long carrier_len;
        long hidden_len;
        char signature[] = {'S', 'T', 'E', 'G', 'O', 'C'};


        if (carrier == NULL) {
            perror("Error opening carrier file \n");
            fclose(hidden);
            return 1;
        }

        if (hidden == NULL) {
            perror("Error opening hidden file \n");
            fclose(carrier);
            return 1;
        }

        // Get Byte Length from Carrier file
        fseek(carrier, 0, SEEK_END);
        carrier_len = ftell(carrier);
        rewind(carrier);

        // Get Byte Length from Hidden file
        fseek(hidden, 0, SEEK_END);
        hidden_len = ftell(hidden);
        rewind(hidden);


        // fread(carrier_buf, 1, carrier_len, carrier);
        // fread(hidden_buf, 1, hidden_len, hidden);


        out = fopen(out_path, "wb");
        if (out == NULL) {
            perror("Error opening output file \n");
        }


        for (int i = 0; i < carrier_len; i++) {
            char byte = fgetc(carrier);
            fputc(byte, out);
        }

        
        for (int i = 0; i < 6; i++) {
            fputc(signature[i], out);
        }

        for (int i = 0; i < hidden_len; i++) {
            char byte = fgetc(hidden);
            fputc(byte, out);
        }


        fclose(carrier);
        fclose(hidden);
        fclose(out);
    } else if (strcmp(argv[1], "-e") == 0 || strcmp(argv[1], "--extract") == 0) {
        FILE* input_file;
        FILE* output_file;
        const char* signature = "STEGOC";
        const size_t sig_len = strlen(signature);
        long input_len;

        input_file = fopen(carrier_path, "rb");

        if (input_file == NULL) {
            perror("Error opening input file \n");
            fclose(input_file);
            return 1;
        }

        fseek(input_file, 0, SEEK_END);
        input_len = ftell(input_file);

        long index = -1;

        for (long pos = input_len-1; pos >= sig_len-1; pos--) {
            fseek(input_file, pos, SEEK_SET);
            int ch = fgetc(input_file);

            if (ch == 'S') {
                int match = 1;
                for (size_t i = 0; i < sig_len; i++) {
                    fseek(input_file, pos - i, SEEK_SET);
                    int file_ch = fgetc(input_file);
                    if (file_ch != signature[i]) {
                        match = 0;
                        break;
                    }
                }

                if (match) {
                    index = pos + 1;
                    break;
                }
            }
        }

        output_file = fopen(hidden_path, "wb");
        if (output_file == NULL) {
            perror("Error opening output file \n");
        }

        fseek(input_file, index, SEEK_SET);
        for (long i = index; i < input_len; i++) {
            int byte = fgetc(input_file);
            fputc(byte, output_file);
        }


        fclose(output_file);
        fclose(input_file);
    } else {
        printf("Run the --help flag to get information on how to run this command");
    }




    return 0;
}
