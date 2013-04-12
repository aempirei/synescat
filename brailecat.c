/* brailecat
 * Braile "cat" Command
 * Use the 8 braile dot positions as bit on/off indicators
 * use any flag to only braile encode non-printables
 * Copyright(c) 2013 by Christopher Abad | 20 GOTO 10
 *
 * email: aempirei@gmail.com aempirei@256.bz
 * http://www.256.bz/ http://www.twentygoto10.com/
 * git: git@github.com:aempirei/brailecat.git
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
#include <ctype.h>
#include <stdarg.h>

wint_t putbrailebyte(int ch, FILE *fp) {
	const wint_t wch = 0x2800;
	if(ch == EOF)
		return WEOF;
	return fputwc(wch + ch, fp);
}

void brailecat(FILE * fpin, FILE * fpout, int all) {

	const char *dfl_locale = "";
	int ch;

	if (setlocale(LC_CTYPE, dfl_locale) == NULL) {
		fprintf(stderr, "failed to set locale LC_CTYPE=\"%s\"\n", dfl_locale);
		exit(EXIT_FAILURE);
	}

	while((ch = fgetc(fpin)) != EOF) {
		if(all || !isprint(ch)) 
			putbrailebyte(ch, fpout);
		else
			fputwc(ch, fpout);
	}

	if (!feof(fpin)) {
		perror("fgetc()");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char **argv) {
	int all = 1;
	if(argc > 1 && *argv[1] == '-') {
		/* 
		   who cares what flag you pass, if theres one consider it
		   the 'only braileify non-printables' option
		 */
		all = 0;
	}
	brailecat(stdin, stdout, all);
	exit(EXIT_SUCCESS);
}
