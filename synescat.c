/* synescat
 * Synesthesia "cat" Command
 * Copyright(c) 2012 by Christopher Abad | 20 GOTO 10
 *
 * email: aempirei@gmail.com aempirei@256.bz
 * http://www.256.bz/ http://www.twentygoto10.com/
 * git: git@github.com:aempirei/synescat.git
 * aim: ambientempire
 */

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
    int wide_markup;
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
    fprintf(stderr, "\t%*s%*s\n", option_width, "-w", desc_width, "wide markup");
    fprintf(stderr, "\t%*s%*s\n", option_width, "-d", desc_width, "dynamic mode");
    fprintf(stderr, "\t%*s%*s\n", option_width, "-c", desc_width, "compression mode");
    fprintf(stderr, "\t%*s%*s\n", option_width, "-l locale", desc_width, "use specified locale string");
    fputc('\n', stderr);
}

void cliconfig(configuration_t * config, int argc, char **argv) {

    const configuration_t default_config = {
        .numeric_markup = 0,
        .alpha_markup = 0,
        .wide_markup = 0,
        .dynamic_mode = 0,
        .compression_mode = 0,
        .locale = ""
    };

    int opt;

    *config = default_config;

    opterr = 0;

    while ((opt = getopt(argc, argv, "hdcnawvl:")) != -1) {
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
        case 'w':
            config->wide_markup = 1;
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

int print_eilseq(int ch, FILE * fp) {
    return fwprintf(fp, L"\33[46;31m\\x%02x\33[0m", ch);
}

int print_wide(int ch, FILE * fp) {
    return fwprintf(fp, L"\33[1;31m%lc\33[0m", ch);
}

int print_numeric(wint_t ch, FILE * fp) {
    int value = ch - L'0';
    const int codea[] = { 30, 37, 37, 31, 34, 36, 32, 35, 34, 31 };
    const int codeb[] = { 1, 0, 1, 0, 1, 0, 0, 0, 0, 1 };
    return fwprintf(fp, L"\33[%d;%dm%lc\33[0m", codeb[value], codea[value], ch);
}

int print_alpha(wint_t ch, FILE * fp) {
    wint_t value = towlower(ch) - L'a';

    return fwprintf(fp, L"\33[%d;%dm%lc\33[0m", (value / 6) & 1, 31 + (value % 6), ch);
}

void synescat(configuration_t * config, FILE * fpin, FILE * fpout) {

    wint_t wch;
    int ch;

    if (setlocale(LC_CTYPE, config->locale) == NULL) {
        fprintf(stderr, "failed to set locale LC_CTYPE=\"%s\"\n", config->locale);
        exit(EXIT_FAILURE);
    }

    for (;;) {

        wch = fgetwc(fpin);

        if (wch == WEOF) {
            if (feof(fpin)) {
                break;
            } else if (errno == EILSEQ) {
                if ((ch = fgetc(fpin)) == EOF) {
                    if (feof(fpin)) {
                        break;
                    } else {
                        perror("fgetc()");
                        exit(EXIT_FAILURE);
                    }
                }
                print_eilseq(ch, fpout);
            } else {
                perror("fgetwc()");
                exit(EXIT_FAILURE);
            }
        } else {

            if (config->numeric_markup && iswdigit(wch)) {
                print_numeric(wch, fpout);
            } else if (config->alpha_markup && iswalpha(wch)) {
                print_alpha(wch, fpout);
            } else if (config->wide_markup && wch > 255) {
                print_wide(wch, fpout);
            } else if (fputwc(wch, fpout) == WEOF) {
                fprintf(stderr, "fputwc(0x%02x,...)", wch);
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
