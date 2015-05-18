CC = gcc
CCFLAGS = -Wall -W -w
CFLAGS = -Wall -W -I. -pedantic -std=gnu99
LIBFLAGS =
PROGRAMS = synescat brailecat
INSTALL_PATH = /usr/local/bin
INDENTFLAGS = -i4 -br -ce -nprs -nbfda -npcs -ncs -sob -brf -nut -bap -bad -npsl -l140

.PHONY: all clean wipe tidy

all: $(PROGRAMS)

synescat: synescat.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)

brailecat: brailecat.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)

install: $(PROGRAMS)
	install $(PROGRAMS) -m755 $(INSTALL_PATH)

clean:
	rm -f *.o *~

wipe: clean
	rm -f $(PROGRAMS)

tidy:
	indent $(INDENTFLAGS) *.c
