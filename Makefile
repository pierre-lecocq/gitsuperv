PROGNAME=gitsuperv

MANPAGE=$(PROGNAME).1
MANPAGE_GZ=$(MANPAGE).gz
MANPAGE_HTML=$(MANPAGE).html

PROGPATH=/usr/bin
MANPATH=/usr/share/man/man1


$(PROGNAME): software man manhtml

software: src/*.c
	gcc -Wall -fstack-protector -O2 src/*.c -o $(PROGNAME) -lgit2

man:
	gzip -9 -n -c doc/$(MANPAGE) > $(MANPAGE_GZ)

manhtml:
	cat doc/$(MANPAGE) | groff -mandoc -Thtml > $(MANPAGE_HTML)

install:
	install -D $(PROGNAME) $(PROGPATH)/$(PROGNAME)
	install -D -m 644 $(MANPAGE_GZ) $(MANPATH)/$(MANPAGE_GZ)

uninstall:
	rm -f $(PROGPATH)/$(PROGNAME)
	rm -f $(MANPATH)/$(MANPAGE_GZ)

test:
	./$(PROGNAME) -d ~/work

.PHONY: clean install uninstall

clean:
	rm -f $(PROGNAME)
	rm -f $(MANPAGE_GZ)
	rm -f $(MANPAGE_HTML)

re: clean $(PROGNAME)
