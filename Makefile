ifndef CFLAGS
	CFLAGS=-O3
endif

all: fastvcs.c
	$(CC) $(CFLAGS) $^ -o fastvcs

install:
	@install -b ./fastvcs /usr/local/bin/

clean:
	rm -rf fastvcs.dSYM fastvcs
