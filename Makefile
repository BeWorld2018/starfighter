CXXFLAGS ?= -O2 -Wall -g
CXXFLAGS += `pkg-config --cflags sdl2 SDL2_image SDL2_mixer` -DLINUX
LIBS = `pkg-config --libs sdl2 SDL2_image SDL2_mixer`
OBJS = aliens.o audio.o bullets.o cargo.o collectable.o comms.o debris.o events.o explosions.o game.o globals.o graphics.o init.o intermission.o loadSave.o messages.o misc.o missions.o player.o resources.o script.o shop.o Starfighter.o title.o weapons.o

VERSION = 1.3-dev
PROG = starfighter
DOCS = docs/*
DATA = data gfx sound
DATAFILES = data/* gfx/* sound/*

PREFIX ?= /usr
BINDIR ?= $(PREFIX)/games/
DATADIR ?= $(PREFIX)/share/games/parallelrealities/
DOCDIR ?= $(PREFIX)/share/doc/$(PROG)/

# top-level rule to create the program.
ALL = $(PROG)

all: $(ALL)

# compiling other source files.
%.o: src/%.cpp src/*.h
	$(CXX) $(CXXFLAGS) -c -DVERSION=\"$(VERSION)\" -DDATADIR=\"$(DATADIR)\" $<

# linking the program.
$(PROG): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) -o $(PROG) $(LIBS)

# cleaning everything that can be automatically recreated with "make".
clean:
	$(RM) $(OBJS) $(ALL)

# install
install: $(ALL)
	mkdir -p $(DESTDIR)$(BINDIR)
	mkdir -p $(DESTDIR)$(DATADIR)
	mkdir -p $(DESTDIR)$(DOCDIR)

	install -m 755 $(PROG) $(DESTDIR)$(BINDIR)$(PROG)
	cp -pr $(DATA) $(DESTDIR)$(DATADIR)
	cp -p $(DOCS) $(DESTDIR)$(DOCDIR)

optimise:
	advpng -z gfx/*.png
	jpegoptim --strip-all gfx/*.jpg

dist:
	rm -rf starfighter-$(VERSION)
	mkdir starfighter-$(VERSION)
	cp --parents -lt starfighter-$(VERSION) `git ls-files`
	git log >starfighter-$(VERSION)/ChangeLog
	tar czf starfighter-$(VERSION).tar.gz starfighter-$(VERSION)
	rm -rf starfighter-$(VERSION)

.PHONY: all clean install optimise dist
