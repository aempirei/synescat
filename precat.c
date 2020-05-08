#include <stdio.h>
#include <stdlib.h>

int put(const char *s) {
	return ( fputs(s, stdout) != EOF && fputc(' ', stdout) != EOF ) ? ~EOF : EOF;
}

int main(int argc, char **argv) {
	for(int i = 1; i <= argc; i++) {
		FILE *fp = fopen(argv[i], "r");
		if(fp != NULL) {
			char ch;
			put(argv[i]);
			while((ch = getchar()) != EOF) {
				if(ch == '\n')
					put(argv[i]);
				fputc(ch, stdout);
			}
			fclose(fp);
		}
	}
	exit(EXIT_SUCCESS);
}
