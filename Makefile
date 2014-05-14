LEVELDBPATH="./deps/leveldb"

all: $(LEVELDBPATH)/libleveldb.a ldb

$(LEVELDBPATH)/libleveldb.a:
	cd $(LEVELDBPATH) && make

ldb:
	g++ -o ldb ldb.cc $(LEVELDBPATH)/libleveldb.a -lpthread -I $(LEVELDBPATH)/include -std=gnu++11

clean:
	rm -f ldb && cd $(LEVELDBPATH) && make clean

