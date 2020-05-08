CC = gcc
CCFLAGS = -Wall -W -w -O3
CFLAGS = -Wall -W -I. -pedantic -std=gnu18 -O3
LIBFLAGS =
PROGRAMS = synescat brailecat precat
INSTALL_PATH = /usr/local/bin
INDENTFLAGS = -i4 -br -ce -nprs -nbfda -npcs -ncs -sob -brf -nut -bap -bad -npsl -l140

.PHONY: all clean tidy

all: $(PROGRAMS)

$(PROGRAMS): %: %.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)

install: $(PROGRAMS)
	install $(PROGRAMS) -m755 $(INSTALL_PATH)

clean:
	rm -f *.o *~ $(PROGRAMS)

tidy:
	indent $(INDENTFLAGS) *.c
