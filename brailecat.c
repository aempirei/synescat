/* brailecat
 * Braile "cat" Command
 * Use the 8 braile dot positions as bit on/off indicators
 * Copyright(c) 2012 by Christopher Abad | 20 GOTO 10
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
#include <stdarg.h>

wint_t putbrailebyte(int ch, FILE *fp) {
	const wint_t wch = 0x2800;
	if(ch == EOF)
		return WEOF;
	return fputwc(wch + ch, fp);
}

void brailecat(FILE * fpin, FILE * fpout) {

	const char *dfl_locale = "";

	if (setlocale(LC_CTYPE, dfl_locale) == NULL) {
		fprintf(stderr, "failed to set locale LC_CTYPE=\"%s\"\n", dfl_locale);
		exit(EXIT_FAILURE);
	}

	while(putbrailebyte(fgetc(fpin), fpout) != WEOF) { /* do nothing */ }

	if (!feof(fpin)) {
		perror("fgetc()");
		exit(EXIT_FAILURE);
	}
}

int main() {
	brailecat(stdin, stdout);
	exit(EXIT_SUCCESS);
}
