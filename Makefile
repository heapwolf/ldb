CXX ?= g++
BIN ?= ldb
LIBPATH ?= ./lib
SRC = ldb.cc $(LIBPATH)/*.cc ./deps/docopt/docopt.cpp
PREFIX ?= /usr/local
LEVELDBPATH ?= ./deps/leveldb
LIBLEVELDB ?= $(LEVELDBPATH)/out-static/libleveldb.a
LIBSNAPPY ?= ./deps/snappy
CXXFLAGS += -I$(LEVELDBPATH)/include -std=gnu++11

DEPPATH ?= ./deps
DEPS += linenoise.o

OS = $(shell uname)

ifeq ($(OS), Darwin)
	LIBTOOLIZE=glibtoolize
	CXXFLAGS += -stdlib=libc++
endif

export CXXFLAGS

all: leveldb $(BIN)

$(LEVELDBPATH):
	git clone --depth 1 --single-branch --branch v1.20 https://github.com/google/leveldb $(LEVELDBPATH)
leveldb: $(LEVELDBPATH)
	make -C $(LEVELDBPATH)

$(BIN): $(DEPS) $(LIBPATH)/*.cc deps/docopt
	$(CXX) -o $(BIN) $(SRC) $(CXXFLAGS) -lpthread -L/usr/local/lib -lsnappy $(LIBLEVELDB) $(DEPS)

deps/snappy:
	git clone --depth 1 https://github.com/0x00A/snappy ./deps/snappy

deps/docopt:
	git clone --depth 1 https://github.com/docopt/docopt.cpp ./deps/docopt

deps/linenoise:
	git clone --depth 1 https://github.com/antirez/linenoise ./deps/linenoise

linenoise.o: deps/linenoise
	$(CC) -c deps/linenoise/*.c

./deps/snappy/snappy.o: deps/snappy
	cd $(LIBSNAPPY) && \
		./autogen.sh && \
		./configure && \
		make && \
		make install

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

