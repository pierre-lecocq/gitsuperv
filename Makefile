PROGNAME=gitsuperv
MANPAGE=$(PROGNAME).1.gz

$(PROGNAME): src/*.c
	     gcc src/*.c -o $(PROGNAME) -lgit2

$(MANPAGE): $(PROGNAME).1
	gzip -9 -n -c $(PROGNAME).1 > $(MANPAGE)

install: $(PROGNAME) $(MANPAGE)
	install -D $(PROGNAME) /usr/bin/$(PROGNAME)
	install -D -m 644 $(MANPAGE) /sur/share/man/man1/$(MANPAGE)

uninstall:
	rm -f /usr/bin/$(PROGNAME)
	rm -f /usr/share/man/man1/$(MANPAGE)

.PHONY: clean install uninstall

clean:
	rm -f $(PROGNAME)
	rm -f $(MANPAGE)
