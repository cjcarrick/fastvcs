CFLAGS += -std=c89 -Wall -Wpedantic \
		  -O2 -march=native -mtune=native

all: fastvcs.c
	$(CC) $(CFLAGS) $^ -o fastvcs

install:
	@install -b ./fastvcs /usr/local/bin/

clean:
	rm -rf fastvcs.dSYM fastvcs
