
CXX ?= g++
BIN ?= ldb
SRC = ldb.cc
PREFIX ?= /usr/local
LEVELDBPATH ?= ./deps/leveldb
CXXFLAGS += -lpthread -I$(LEVELDBPATH)/include -std=gnu++11 -stdlib=libc++

all: $(LEVELDBPATH)/libleveldb.a $(BIN)

$(LEVELDBPATH)/libleveldb.a:
	CXXFLAGS+='$(CXXFLAGS)' $(MAKE) -C $(LEVELDBPATH)

$(BIN):
	$(CXX) -o $(BIN) $(LEVELDBPATH)/libleveldb.a $(SRC) $(CXXFLAGS)

clean:
	rm -f $(BIN)
	$(MAKE) clean -C $(LEVELDBPATH)

install:
	install $(BIN) $(PREFIX)/bin

uninstall:
	rm -f $(PREFIX)/bin/$(BIN)
