PROGNAME=gitsuperv
CC=gcc

$(PROGNAME): src/*.c
	     $(CC) src/*.c -o $(PROGNAME) -lgit2

clean:
	rm -f $(PROGNAME)
