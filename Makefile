PROGNAME=gitsuperv

MANPAGE=$(PROGNAME).1
MANPAGE_GZ=$(MANPAGE).gz

BUILDPATH=./build
PROGPATH=/usr/bin
MANPATH=/usr/share/man/man1

CFLAGS=--pedantic -Wall -fstack-protector -O2
LDFLAGS=-lgit2
OBJECTS=src/gitsuperv.o

$(PROGNAME): software

software: $(OBJECTS)
	mkdir -p $(BUILDPATH)
	gcc $(CFLAGS) $(OBJECTS) -o $(BUILDPATH)/$(PROGNAME) $(LDFLAGS)
	gzip -9 -n -c man/$(MANPAGE) > $(BUILDPATH)/$(MANPAGE_GZ)

install:
	install -D $(BUILDPATH)/$(PROGNAME) $(PROGPATH)/$(PROGNAME)
	install -D -m 644 $(BUILDPATH)/$(MANPAGE_GZ) $(MANPATH)/$(MANPAGE_GZ)

uninstall:
	rm -f $(PROGPATH)/$(PROGNAME)
	rm -f $(MANPATH)/$(MANPAGE_GZ)

test:
	$(BUILDPATH)/$(PROGNAME) -d ~/work/src -c

.PHONY: clean install uninstall

clean:
	rm -rf $(OBJECTS) $(BUILDPATH)

re: clean $(PROGNAME)
