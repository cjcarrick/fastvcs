CFLAGS += -std=c89 -Wall -Wpedantic \
		  -O2 -march=native -mtune=native

fastvcs: fastvcs.c
	$(CC) $(CFLAGS) $^ -o $@

install: fastvcs
	@install -b ./fastvcs /usr/local/bin/

clean:
	rm -rf fastvcs.dSYM fastvcs
