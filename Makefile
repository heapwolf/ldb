CXX ?= g++
BIN ?= ldb
LIBPATH ?= ./lib
SRC = ldb.cc $(LIBPATH)/*.cc
PREFIX ?= /usr/local
LEVELDBPATH ?= ./deps/leveldb
LIBLEVELDB ?= $(LEVELDBPATH)/libleveldb.a
CXXFLAGS += -I$(LEVELDBPATH)/include -std=gnu++11

DEPPATH ?= ./deps
DEPS += ./linenoise

OS = $(shell uname)

ifeq ($(OS), Darwin)
	CXXFLAGS += -stdlib=libc++
endif

export CXXFLAGS

all: leveldb $(BIN)

leveldb:
	git clone --depth 1 git://github.com/google/leveldb.git $(LEVELDBPATH)
	make -C $(LEVELDBPATH)

$(BIN): $(DEPS) $(LIBPATH)/*.cc
	$(CXX) -o $(BIN) $(SRC) $(CXXFLAGS) -lpthread $(LIBLEVELDB) -lsnappy $(DEPS:=.o)

$(DEPS):
	git clone --depth 1 git://github.com/lukedeo/cmd-parser.git ./deps/cmd-parser
	git clone --depth 1 git://github.com/antirez/linenoise.git ./deps/linenoise
	$(CC) -c $(DEPPATH)/$(@)/*.c

clean:
	rm -rf $(BIN)
	rm -rf $(DEPPATH)

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

