PROGNAME=gitsuperv
CC=gcc

$(PROGNAME): src/*.c
	     $(CC) src/*.c -o $(PROGNAME)

clean:
	rm -f $(PROGNAME)
