PROGNAME=gitsuperv
MANPAGE=$(PROGNAME).1.gz

$(PROGNAME): src/*.c
	     gcc -Wall -fstack-protector -O2 src/*.c -o $(PROGNAME) -lgit2

man:
	gzip -9 -n -c doc/$(PROGNAME).1 > $(MANPAGE)

manhtml:
	cat doc/$(PROGNAME).1 | groff -mandoc -Thtml > $(PROGNAME).1.html

install: $(PROGNAME) $(MANPAGE)
	install -D $(PROGNAME) /usr/bin/$(PROGNAME)
	install -D -m 644 $(MANPAGE) /usr/share/man/man1/$(MANPAGE)

uninstall:
	rm -f /usr/bin/$(PROGNAME)
	rm -f /usr/share/man/man1/$(MANPAGE)

.PHONY: clean install uninstall

clean:
	rm -f $(PROGNAME)
	rm -f $(MANPAGE)
	rm -f $(PROGNAME).1.html

all: $(PROGNAME) man manhtml

re: clean all
