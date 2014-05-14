
CXX ?= g++
BIN ?= ldb
SRC = ldb.cc
PREFIX ?= /usr/local
LEVELDBPATH ?= ./deps/leveldb
CXXFLAGS += -lpthread -I$(LEVELDBPATH)/include -std=gnu++11 -stdlib=libc++

export CXXFLAGS
all: $(LEVELDBPATH)/libleveldb.a $(BIN)

$(LEVELDBPATH)/libleveldb.a:
	$(MAKE) -C $(LEVELDBPATH)

$(BIN):
	$(CXX) -o $(BIN) $(LEVELDBPATH)/libleveldb.a $(SRC) $(CXXFLAGS)

clean:
	rm -f $(BIN)
	$(MAKE) clean -C $(LEVELDBPATH)

install: all
	install $(BIN) $(PREFIX)/bin

uninstall:
	rm -f $(PREFIX)/bin/$(BIN)
