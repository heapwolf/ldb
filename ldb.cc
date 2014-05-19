#include "ldb.h"

struct Options : public leveldb::Options {
  string path;
};

//
//
//
static Options get_options(int argc, char** argv)
{
  Options options;

  if (argc == 2) options.path = argv[1];

  int c;

  while ((c = getopt(argc, argv, "i:ec:d")) != EOF) {
    switch (c) {
      case 'i':
        options.path = optarg;
        break;
      case 'e':
        options.error_if_exists = true;
        break;
      case 'c':
        options.path = optarg;
        options.create_if_missing = true;
        break;
    }
  }

  return options;
}

//
//
//
static leveldb::DB* create_database(int argc, char** argv)
{
  leveldb::DB* db;
  Options options = get_options(argc, argv);
  leveldb::Status status = leveldb::DB::Open(options, options.path, &db);

  if (false == status.ok()) {
    cerr << "Unable to open/create database " << options.path << endl;
    cerr << status.ToString() << endl;
    exit(1);
  }

  return db;
}

//
//
//
int main(int argc, char** argv)
{
  leveldb::DB* db = create_database(argc, argv);
  ldb::start_repl(db);
}
