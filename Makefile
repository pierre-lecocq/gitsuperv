PROGNAME=gitsuperv
CC=gcc

$(PROGNAME):
	     $(CC) src/*.c -o $(PROGNAME) -lgit2

clean:
	rm -f $(PROGNAME)
