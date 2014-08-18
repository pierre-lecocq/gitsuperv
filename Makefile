PROGNAME=gitsuperv
MANPAGE=$(PROGNAME).1
MANPAGE_GZ=$(MANPAGE).gz
MANPAGE_HTML=$(MANPAGE).html

$(PROGNAME): src/*.c
	     gcc -Wall -fstack-protector -O2 src/*.c -o $(PROGNAME) -lgit2

man:
	gzip -9 -n -c doc/$(MANPAGE) > $(MANPAGE_GZ)

manhtml:
	cat doc/$(MANPAGE) | groff -mandoc -Thtml > $(MANPAGE_HTML)

install: $(PROGNAME) $(MANPAGE_GZ)
	install -D $(PROGNAME) /usr/bin/$(PROGNAME)
	install -D -m 644 $(MANPAGE_GZ) /usr/share/man/man1/$(MANPAGE_GZ)

uninstall:
	rm -f /usr/bin/$(PROGNAME)
	rm -f /usr/share/man/man1/$(MANPAGE_GZ)

.PHONY: clean install uninstall

clean:
	rm -f $(PROGNAME)
	rm -f $(MANPAGE_GZ)
	rm -f $(MANPAGE_HTML)

all: $(PROGNAME) man manhtml

re: clean all
