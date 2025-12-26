#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <string.h>


#define HEADER_OFFSET 60

const char *argp_program_version = "STEGOC 1.1";
const static char doc[] = "STEGOC Tool for Steganography";
const static char args_doc[] = "CARRIER_FILE";

static struct argp_option ops[] = {
  {"extract", 'e', 0, 0, "Path to file that has another file hidden in it with the STEGOC tool"},
  {"hidden", 's', 0, 0, "Path to file that needs to be hidden in another file"},
  {"output", 'o', 0, 0, "Define the resulting file of the command"},
  {0}
};

typedef struct {
  const char* extract;
  const char* hidden;
  const char* output;
  const char* carrier;
} arguments;

static error_t parse_opts(int key, char* arg, struct argp_state* state) {
  arguments* args = (arguments*)state->input;
  
  switch (key) {
    case 'e':
      args->extract = arg;
      break;
    case 's':
      args->hidden = arg;
      break;
    case 'o':
      args->output = arg;
      break;
    case ARGP_KEY_ARG:
      if (state->arg_num > 1) argp_usage(state);
      args->carrier = arg;
      break;
    case ARGP_KEY_END:
      if (state->arg_num < 1) argp_usage(state);
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp parser = {ops, parse_opts, args_doc, doc};

void hide(const char* hiddenStr, const char* carrierStr, const char* output) {

  FILE *carrier;
  FILE *hidden;
  FILE *out;
  carrier = fopen(carrierStr, "rb");
  hidden = fopen(hiddenStr, "rb");
  long carrierLen;
  long hiddenLen;
  char signature[] = {'S', 'T', 'E', 'G', 'O', 'C'};


  if (carrier == NULL) {
      perror("Error opening carrier file \n");
      fclose(hidden);
  }

  if (hidden == NULL) {
      perror("Error opening hidden file \n");
      fclose(carrier);
  }

  // Get Byte Length from Carrier file
  fseek(carrier, 0, SEEK_END);
  carrierLen = ftell(carrier);
  rewind(carrier);

  // Get Byte Length from Hidden file
  fseek(hidden, 0, SEEK_END);
  hiddenLen = ftell(hidden);
  rewind(hidden);

  out = fopen(output, "wb");
  if (out == NULL) {
      perror("Error opening output file \n");
  }


  for (int i = 0; i < carrierLen; i++) {
      char byte = fgetc(carrier);
      fputc(byte, out);
  }

  
  for (int i = 0; i < 6; i++) {
      fputc(signature[i], out);
  }

  for (int i = 0; i < hiddenLen; i++) {
      char byte = fgetc(hidden);
      fputc(byte, out);
  }


  fclose(carrier);
  fclose(hidden);
  fclose(out);
}

void extract(const char* carrierStr, const char* output) {
  FILE* in;
  FILE* out;
  const char* signature = "STEGOC";
  const size_t sigLen = strlen(signature);
  long inLen;

  in = fopen(carrierStr, "rb");

  if (in == NULL) {
      perror("Error opening input file \n");
      fclose(in);
  }

  fseek(in, 0, SEEK_END);
  inLen = ftell(in);

  long index = -1;

  for (long pos = inLen-1; pos >= sigLen-1; pos--) {
      fseek(in, pos, SEEK_SET);
      int ch = fgetc(in);

      if (ch == 'S') {
          int match = 1;
          for (size_t i = 0; i < sigLen; i++) {
              fseek(in, pos - i, SEEK_SET);
              int fileChar = fgetc(in);
              if (fileChar != signature[i]) {
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

  out = fopen(output, "wb");
  if (out == NULL) {
      perror("Error opening output file \n");
  }

  fseek(in, index, SEEK_SET);
  for (long i = index; i < inLen; i++) {
      int byte = fgetc(in);
      fputc(byte, out);
  }


  fclose(out);
  fclose(in);
}


int main(int argc, char** argv) {
  arguments args;
  args.extract = NULL;
  args.hidden = NULL;
  args.output = "output";

  argp_parse(&parser, argc, argv, 0, 0, &args);

  if (args.extract == NULL && args.hidden == NULL) {
    exit(EXIT_FAILURE);
  }

  if (args.extract != NULL && args.hidden != NULL) {
    perror("the extract and the hidden flag cant both be set");
    exit(EXIT_FAILURE);
  }

  if (args.hidden != NULL) {
    hide(args.hidden, args.carrier, args.output);
  } else {
    extract(args.carrier, args.output);
  }

  return EXIT_SUCCESS;
}

/*
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

*/
