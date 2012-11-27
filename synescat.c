#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <locale.h>
#include <wchar.h>
#include <wctype.h>

typedef struct configuration {
    int numeric_markup;
    int alpha_markup;
    int dynamic_mode;
    int compression_mode;
    const char *locale;
} configuration_t;

static void usage(const char *arg0) {
    const int option_width = -11;
    const int desc_width = -40;

    fprintf(stderr, "\nusage: %s [options] < inputfile\n\n", arg0);
    fprintf(stderr, "\t%*s%*s\n", option_width, "-h", desc_width, "show this help");
    fprintf(stderr, "\t%*s%*s\n", option_width, "-n", desc_width, "numeric markup");
    fprintf(stderr, "\t%*s%*s\n", option_width, "-a", desc_width, "alpha markup");
    fprintf(stderr, "\t%*s%*s\n", option_width, "-d", desc_width, "dynamic mode");
    fprintf(stderr, "\t%*s%*s\n", option_width, "-c", desc_width, "compression mode");
    fprintf(stderr, "\t%*s%*s\n", option_width, "-l locale", desc_width, "use specified locale string");
    fputc('\n', stderr);
}

void cliconfig(configuration_t * config, int argc, char **argv) {

    const configuration_t default_config = {
        .numeric_markup = 0,
        .alpha_markup = 0,
        .dynamic_mode = 0,
        .compression_mode = 0,
        .locale = ""
    };

    int opt;

    *config = default_config;

    opterr = 0;

    while ((opt = getopt(argc, argv, "hdcnavl:")) != -1) {
        switch (opt) {
            /*
               case 'd':
               config->dynamic_mode = 1;
               break;
               case 'c':
               config->compression_mode = 1;
               break;
             */
        case 'n':
            config->numeric_markup = 1;
            break;
        case 'a':
            config->alpha_markup = 1;
            break;
        case 'l':
            config->locale = optarg;
            break;
        case 'h':
            usage(argv[0]);
            exit(EXIT_SUCCESS);
        case '?':
            fprintf(stderr, "unknown option: -%c\n", optopt);
            usage(argv[0]);
            exit(EXIT_FAILURE);
        default:
            fprintf(stderr, "unimplemented option: -%c\n", opt);
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }
}

void synescat(configuration_t * config, FILE * fpin, FILE * fpout) {

    wint_t ch;

    if (setlocale(LC_CTYPE, config->locale) == NULL) {
        fprintf(stderr, "failed to set locale LC_CTYPE=\"%s\"\n", config->locale);
        exit(EXIT_FAILURE);
    }

    for (;;) {

        ch = fgetwc(fpin);

        if (ch == WEOF) {
            if (feof(fpin)) {
                break;
            } else if (errno == EILSEQ) {
                ch = fgetc(fpin);
                printf("\\x%02x", ch);
            } else {
                perror("fgetwc()");
                exit(EXIT_FAILURE);
            }
        } else {

            if (config->numeric_markup) {
            }
            if (config->alpha_markup) {
            }

				if (fputwc(ch, fpout) == WEOF) {
					fprintf(stderr, "fputwc(0x%02x,...)", ch);
                perror("");
                exit(EXIT_FAILURE);
            }
        }
    }

}

int main(int argc, char **argv) {

    configuration_t config;

    cliconfig(&config, argc, argv);

    synescat(&config, stdin, stdout);

    exit(EXIT_SUCCESS);
}
