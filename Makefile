CXX ?= g++
BIN ?= ldb
LIBPATH ?= ./lib
SRC = ldb.cc $(LIBPATH)/*.cc
PREFIX ?= /usr/local
LEVELDBPATH ?= ./deps/leveldb
SNAPPYPATH ?= ./deps/snappy-1.1.1
LIBSNAPPY ?= $(SNAPPYPATH)/.libs/libsnappy.a
LIBLEVELDB ?= $(LEVELDBPATH)/libleveldb.a
CXXFLAGS += -I$(LEVELDBPATH)/include -std=gnu++11

DEPPATH ?= ./deps
DEPS += ./linenoise

OS = $(shell uname)

ifeq ($(OS), Darwin)
	CXXFLAGS += -stdlib=libc++
endif

export CXXFLAGS

all: leveldb snappy $(BIN)

leveldb:
	$(MAKE) -C $(LEVELDBPATH)

snappy:
	$(MAKE) -C $(SNAPPYPATH)

$(BIN): $(DEPS) $(LIBPATH)/*.cc
	$(CXX) -o $(BIN) $(SRC) $(CXXFLAGS) -lpthread $(LIBLEVELDB) $(LIBSNAPPY) $(DEPS:=.o)

$(DEPS):
	$(CC) -c $(DEPPATH)/$(@)/*.c

clean:
	rm -f $(BIN)
	rm -f $(DEPPATH)/{$(DEPS)}/*.o
	$(MAKE) clean -C $(LEVELDBPATH)
	$(MAKE) clean -C $(SNAPPYPATH)

install: all install-manpages
	install $(BIN) $(PREFIX)/bin

install-manpages:
	cp ./man/ldb.1.roff /usr/share/man/man1/ldb.1

uninstall:
	rm /usr/share/man/man1/ldb.1
	rm -f $(PREFIX)/bin/$(BIN)

docs:
	./node_modules/.bin/ronn --date `date +%Y/%m` --build --man man/ldb.1.md

test-docs:
	make docs && sudo make install-manpages && man ldb
