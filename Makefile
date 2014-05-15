
CXX ?= g++
BIN ?= ldb
SRC = ldb.cc
PREFIX ?= /usr/local
LEVELDBPATH ?= ./deps/leveldb
LIBLEVELDB ?= $(LEVELDBPATH)/libleveldb.a
CXXFLAGS += -I$(LEVELDBPATH)/include -std=gnu++11

DEPPATH ?= ./deps
DEPS += linenoise

OS = $(shell uname)

ifeq ($(OS), Darwin)
	CXXFLAGS += -stdlib=libc++
endif

export CXXFLAGS

all: leveldb $(BIN)

leveldb:
	$(MAKE) -C $(LEVELDBPATH)

$(BIN): $(DEPS)
	$(CXX) -o $(BIN) $(SRC) $(CXXFLAGS) -lpthread $(LIBLEVELDB) $(DEPS:=.o)

$(DEPS):
	$(CC) -c $(DEPPATH)/$(@)/*.c

clean:
	rm -f $(BIN)
	rm -f $(DEPPATH)/{$(DEPS)}/*.o
	$(MAKE) clean -C $(LEVELDBPATH)

install: all
	install $(BIN) $(PREFIX)/bin

uninstall:
	rm -f $(PREFIX)/bin/$(BIN)

