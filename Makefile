
CXX ?= g++
BIN ?= ldb
SRC = ldb.cc
PREFIX ?= /usr/local
LEVELDBPATH ?= ./deps/leveldb
CXXFLAGS += -I$(LEVELDBPATH)/include -std=gnu++11 -stdlib=libc++

export CXXFLAGS
all: $(LEVELDBPATH)/libleveldb.a ./deps/linenoise/linenoise.c $(BIN)

$(LEVELDBPATH)/libleveldb.a:
	$(MAKE) -C $(LEVELDBPATH)

$(BIN):
	$(CC) -c ./deps/linenoise/linenoise.c
	$(CXX) -o $(BIN) ./linenoise.o $(LEVELDBPATH)/libleveldb.a $(SRC) $(CXXFLAGS) -lpthread

clean:
	rm -f $(BIN)
	rm -f linenoise.o
	$(MAKE) clean -C $(LEVELDBPATH)

install: all
	install $(BIN) $(PREFIX)/bin

uninstall:
	rm -f $(PREFIX)/bin/$(BIN)
